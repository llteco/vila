"""
INTEL CONFIDENTIAL

Copyright (C) 2024 Intel Corporation. All Rights Reserved.

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

from vila.core.action import parse_unknown_args


def test_parse_args_positonal_only():
    args = ["foo", "bar", "1", "3.14", "[1, x, y]", "True", "False", "None"]
    pos, kv = parse_unknown_args(args)

    assert tuple(pos) == ("foo", "bar", 1, 3.14, [1, "x", "y"], True, False, None)
    assert not kv


def test_parse_args_key_words_only():
    args = [
        "--foo",
        "1",
        "--bar=3.14",
        "--baz",
        "True",
        "--qux=False",
        "--quux",
        "None",
    ]
    pos, kv = parse_unknown_args(args)

    assert not pos
    assert kv == dict(foo=1, bar=3.14, baz=True, qux=False, quux=None)


def test_parse_args_mixed():
    args = ["--foo=baz", "1", "--bar", "x", "True", "--qux=False"]
    pos, kv = parse_unknown_args(args)
    assert tuple(pos) == (1, True)
    assert kv == dict(foo="baz", bar="x", qux=False)


def test_parse_args_empty():
    pos, kv = parse_unknown_args([])
    assert not pos and not kv


def test_parse_args_unpaired_key():
    pos, kv = parse_unknown_args(["1", "--foo=None", "-f"])
    assert tuple(pos) == (1,)
    assert kv == dict(foo=None)
