"""
Copyright (C) 2026 The VILA Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def workspace(skylib = True, rules_cc = True, rules_foreign_cc = True, rules_python = True, rules_shell = True):
    """Declare basic workspace dependency

    Bazel rules

    Args:
        skylib: Whether to include Bazel Skylib.
        rules_cc: Whether to include Bazel Rules for C/C++.
        rules_foreign_cc: Whether to include Bazel Rules for Foreign C/C++.
        rules_python: Whether to include Bazel Rules for Python.
        rules_shell: Whether to include Bazel Rules for Shell.
    """

    # https://github.com/bazelbuild/bazel-skylib/releases
    skylib_version = "1.9.0"
    if skylib:
        http_archive(
            name = "bazel_skylib",
            sha256 = "3b5b49006181f5f8ff626ef8ddceaa95e9bb8ad294f7b5d7b11ea9f7ddaf8c59",
            urls = [
                "https://github.com/bazelbuild/bazel-skylib/releases/download/{0}/bazel-skylib-{0}.tar.gz".format(skylib_version),
            ],
        )

    # https://github.com/bazel-contrib/bazel_features
    bazel_features_version = "1.36.0"
    http_archive(
        name = "bazel_features",
        integrity = "sha256-k5CzkaaNOySu95ZrzoVW0oAD/j8CKlAI78eAforKrxo=",
        strip_prefix = "bazel_features-%s" % bazel_features_version,
        url = "https://github.com/bazel-contrib/bazel_features/archive/refs/tags/v%s.tar.gz" % bazel_features_version,
    )

    # https://github.com/bazelbuild/rules_cc/releases
    rules_cc_version = "0.0.14"  # this is the last working version for legacy workspace
    if rules_cc:
        http_archive(
            name = "rules_cc",
            integrity = "sha256-kG6JKGrMZ8IIGcPIizKD3g1YaK/aM2NdcKyuDel3e7c=",
            strip_prefix = "rules_cc-%s" % rules_cc_version,
            url = "https://github.com/bazelbuild/rules_cc/archive/refs/tags/%s.tar.gz" % rules_cc_version,
        )

    # https://github.com/bazelbuild/rules_foreign_cc/releases
    foreign_cc_version = "0.14.0"
    if rules_foreign_cc:
        http_archive(
            name = "rules_foreign_cc",
            integrity = "sha256-4PDrsaIiPJmpBKVl5iqihb8dGortoi0Q6iEnWRYkhmw=",
            strip_prefix = "rules_foreign_cc-%s" % foreign_cc_version,
            url = "https://github.com/bazelbuild/rules_foreign_cc/archive/refs/tags/%s.tar.gz" % foreign_cc_version,
        )

    # https://github.com/bazelbuild/rules_python/releases
    rules_python_version = "1.8.1"
    if rules_python:
        http_archive(
            name = "rules_python",
            integrity = "sha256-euJcDTtSEk//4ZmjRSD0PklvQCfVlFLfcBhOztI7lu8=",
            strip_prefix = "rules_python-%s" % rules_python_version,
            url = "https://github.com/bazelbuild/rules_python/archive/refs/tags/%s.tar.gz" % rules_python_version,
        )

    # https://github.com/bazelbuild/rules_shell/releases
    rules_shell_version = "0.8.0"
    if rules_shell:
        http_archive(
            name = "rules_shell",
            sha256 = "20721f63908879c083f94869e618ea8d4ff5edb91ff9a72a2ebee357fdbc352d",
            strip_prefix = "rules_shell-%s" % rules_shell_version,
            url = "https://github.com/bazelbuild/rules_shell/archive/refs/tags/v%s.tar.gz" % rules_shell_version,
        )
