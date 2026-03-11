"""
Copyright (C) 2024-2026 The VILA Authors.

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
