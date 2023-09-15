"""
INTEL CONFIDENTIAL

Copyright (C) 2023 Intel Corporation. All Rights Reserved.

The source code contained or described herein and all documents
related to the source code ("Material") are owned by Intel Corporation
or licensors. Title to the Material remains with Intel
Corporation or its licensors. The Material contains trade
secrets and proprietary and confidential information of Intel or its
licensors. The Material is protected by worldwide copyright
and trade secret laws and treaty provisions. No part of the Material may
be used, copied, reproduced, modified, published, uploaded, posted,
transmitted, distributed, or disclosed in any way without Intel's prior
express written permission.

No License under any patent, copyright, trade secret or other intellectual
property right is granted to or conferred upon you by disclosure or
delivery of the Materials, either expressly, by implication, inducement,
estoppel or otherwise. Any license under such intellectual property rights
must be express and approved by Intel in writing.
"""

filegroup(
    name = "api_v2",
    srcs = glob(
        include = [
            "src/**/include/openvino/**/*.h",
            "src/**/include/openvino/**/*.hpp",
        ],
    ),
    visibility = ["//visibility:public"],
)

cc_import(
    name = "openvino_dll",
    interface_library = ":bin/intel64/Release/openvino.lib",
    shared_library = ":bin/intel64/Release/openvino.dll",
    system_provided = False,
)

cc_import(
    name = "openvino_ir_frontend_dll",
    interface_library = ":bin/intel64/Release/openvino_ir_frontend.lib",
    shared_library = ":bin/intel64/Release/openvino_ir_frontend.dll",
    system_provided = False,
)

cc_import(
    name = "openvino_onnx_frontend_dll",
    interface_library = ":bin/intel64/Release/openvino_onnx_frontend.lib",
    shared_library = ":bin/intel64/Release/openvino_onnx_frontend.dll",
    system_provided = False,
)

cc_import(
    name = "openvino_intel_vpux_plugin_dll",
    interface_library = ":bin/intel64/Release/openvino_intel_vpux_plugin.lib",
    shared_library = ":bin/intel64/Release/openvino_intel_vpux_plugin.dll",
    system_provided = False,
)

cc_import(
    name = "vpux_level_zero_backend_dll",
    interface_library = ":bin/intel64/Release/vpux_level_zero_backend.lib",
    shared_library = ":bin/intel64/Release/vpux_level_zero_backend.dll",
    system_provided = False,
)

cc_import(
    name = "vpux_driver_compiler_adapter_dll",
    interface_library = ":bin/intel64/Release/vpux_driver_compiler_adapter.lib",
    shared_library = ":bin/intel64/Release/vpux_driver_compiler_adapter.dll",
    system_provided = False,
)

cc_import(
    name = "vpux_mlir_compiler_dll",
    interface_library = ":bin/intel64/Release/vpux_mlir_compiler.lib",
    shared_library = ":bin/intel64/Release/vpux_mlir_compiler.dll",
    system_provided = False,
)

cc_library(
    name = "openvino",
    hdrs = [":api_v2"],
    data = [
        "@vpull//:patches/plugins/debug/plugins.xml",
        "@vpull//:patches/plugins/release/plugins.xml",
    ],
    includes = [
        "src/common/conditional_compilation/include",
        "src/common/itt/include",
        "src/common/util/include",
        "src/core/dev_api/include",
        "src/core/include",
        "src/frontends/common/include",
        "src/frontends/ir/include",
        "src/inference/include",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":openvino_dll",
        ":openvino_intel_vpux_plugin_dll",
        ":openvino_ir_frontend_dll",
        ":openvino_onnx_frontend_dll",
        ":vpux_driver_compiler_adapter_dll",
        ":vpux_level_zero_backend_dll",
        ":vpux_mlir_compiler_dll",
    ],
)
