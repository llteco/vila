# Copyright (C) 2026 The VILA Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
from pathlib import Path

import numpy as np
import tvm_ffi


def test_tvm_ffi_tensor():
    """Test that the TVM FFI C++ library handles tensor data correctly.

    This test loads the compiled TVM FFI shared library and verifies that:
    1. The library can be loaded successfully
    2. The add_one_tensor function processes 1D tensors correctly
    3. The add_one_tensor function processes 2D tensors correctly
    """

    # Determine the correct library extension for the platform
    ext = ".dll" if os.name == "nt" else ".so"
    prefix = "" if os.name == "nt" else "lib"
    lib_name = f"{prefix}tvm_ffi_call{ext}"

    file_dir = Path(__file__).resolve().parent
    workspace_root = file_dir.parents[1]

    # Prefer Bazel runfiles, then local workspace build outputs.
    candidates = [
        Path(os.environ.get("TEST_SRCDIR", "")) / f"_main/tests/vila/{lib_name}",
        Path(os.environ.get("RUNFILES_DIR", "")) / f"_main/tests/vila/{lib_name}",
        file_dir / lib_name,
        workspace_root / f"bazel-bin/tests/vila/{lib_name}",
        Path.cwd() / lib_name,
    ]

    lib_path = None
    for candidate in candidates:
        if candidate.exists():
            lib_path = str(candidate)
            break

    if lib_path is None:
        raise FileNotFoundError(
            "Unable to locate TVM FFI shared library. Checked: "
            + ", ".join(str(path) for path in candidates)
        )

    # Load the TVM FFI module
    mod = tvm_ffi.load_module(lib_path)
    func = mod.add_one_tensor

    # Test 1D tensor
    x_1d = np.array([1.0, 2.0, 3.0], dtype=np.float32)
    result_1d = np.from_dlpack(func(x_1d))
    expected_1d = np.array([2.0, 3.0, 4.0], dtype=np.float32)
    assert np.allclose(result_1d, expected_1d), (
        f"1D: Expected {expected_1d}, got {result_1d}"
    )

    # Test 2D tensor
    x_2d = np.array([[1.0, 2.0], [3.0, 4.0]], dtype=np.float32)
    result_2d = np.from_dlpack(func(x_2d))
    expected_2d = np.array([[2.0, 3.0], [4.0, 5.0]], dtype=np.float32)
    assert np.allclose(result_2d, expected_2d), (
        f"2D: Expected {expected_2d}, got {result_2d}"
    )

    print("TVM FFI tensor test passed!")


if __name__ == "__main__":
    test_tvm_ffi_tensor()
