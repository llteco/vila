#!/usr/bin/python3
# -*- coding: utf-8 -*-

import ctypes
import importlib
import os
import unittest
from pathlib import Path


class TestPybind11(unittest.TestCase):
    def test_so(self):
        if os.name == "nt":
            so_file = Path(__file__).parent / "pybind_test.pyd"
        else:
            so_file = Path(__file__).parent / "pybind_test.so"
        self.assertTrue(so_file.exists(), so_file.as_posix())
        hdl = ctypes.CDLL(so_file)
        del hdl

    def test_import(self):
        lib = importlib.import_module("tests.vila.pybind11.pybind_test")
        lib.hello()
        self.assertEqual(lib.add(2, 3), 5)


if __name__ == "__main__":
    unittest.main()
