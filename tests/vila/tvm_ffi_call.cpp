/**
 * Copyright (C) 2026 The VILA Authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// This file demonstrates how to use TVM FFI to export C++ functions
// that can be called from Python via tvm_ffi.load_module().
//
// TVM FFI provides a simple mechanism to create shared libraries (.dll on Windows,
// .so on Linux) that can be loaded dynamically from Python. Functions exported
// using TVM_FFI_DLL_EXPORT_TYPED_FUNC are automatically discoverable.
//
// Key concepts:
// 1. TVM_FFI_DLL_EXPORT_TYPED_FUNC(ExportName, FunctionPtr)
//    - ExportName: the name exposed to Python (e.g., "add_one_tensor")
//    - FunctionPtr: a C++ function pointer with matching signature
// 2. The C++ function takes a tvm::ffi::Tensor and returns a tvm::ffi::Tensor
// 3. TVM FFI automatically converts numpy arrays to Tensor via DLPack
//
// Example usage in Python:
//   import tvm_ffi
//   import numpy as np
//   mod = tvm_ffi.load_module("path/to/tvm_ffi_call.dll")
//   func = mod.add_one_tensor
//   x = np.array([1.0, 2.0, 3.0], dtype=np.float32)
//   result = func(x)  # Returns array([2.0, 3.0, 4.0], dtype=np.float32)

#include <tvm/ffi/tvm_ffi.h>

#include <cstring>
#include <cstdint>

// CPU allocator for creating output tensors
struct CPUAlloc {
  void AllocData(DLTensor* tensor) {
    size_t size = tvm::ffi::GetDataSize(*tensor);
    tensor->data = malloc(size);
  }

  void FreeData(DLTensor* tensor) {
    if (tensor->data != nullptr) {
      free(tensor->data);
      tensor->data = nullptr;
    }
  }
};

// AddOneTensor_ takes an input tensor and returns a new tensor with each
// element incremented by 1. This demonstrates:
// 1. Receiving a Tensor from Python (via DLPack conversion)
// 2. Creating a new output tensor using TVM FFI's tensor API
// 3. Performing element-wise operations on tensor data
//
// @param input The input tensor (float32)
// @return A new tensor with each element = input element + 1
tvm::ffi::Tensor AddOneTensor_(const tvm::ffi::Tensor& input) {
  // Get a view of the input tensor for reading
  tvm::ffi::TensorView view(input);

  // Verify the input is float32 for this simple example
  if (view.dtype().code != kDLFloat || view.dtype().bits != 32 || view.dtype().lanes != 1) {
    TVM_FFI_THROW(ValueError) << "Expected float32 tensor, got dtype with code="
                               << static_cast<int>(view.dtype().code)
                               << ", bits=" << static_cast<int>(view.dtype().bits);
  }

  // Create output tensor with the same shape
  tvm::ffi::Tensor output = tvm::ffi::Tensor::FromNDAlloc(
      CPUAlloc(),  // allocator
      view.shape(),  // shape
      view.dtype(),  // dtype (float32)
      input.device()  // device (cpu)
  );

  // Copy data and add 1 to each element
  const float* in_data = static_cast<const float*>(view.data_ptr());
  float* out_data = static_cast<float*>(output.data_ptr());

  int64_t numel = view.numel();
  for (int64_t i = 0; i < numel; ++i) {
    out_data[i] = in_data[i] + 1.0f;
  }

  return output;
}

// TVM_FFI_DLL_EXPORT_TYPED_FUNC registers the C++ function AddOneTensor_ as "add_one_tensor"
// in the shared library's exports. When Python loads this library via tvm_ffi,
// it can access the function as mod.add_one_tensor.
//
// The macro handles all the boilerplate for exporting a typed function to Python,
// including function signature registration and ABI compatibility.
TVM_FFI_DLL_EXPORT_TYPED_FUNC(add_one_tensor, AddOneTensor_)
