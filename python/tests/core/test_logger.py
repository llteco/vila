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

import logging
import os

import pytest
from vila.core import LOG, VilaLogger


def test_logger_get_default():
    logger = VilaLogger()
    assert logger.logger is LOG.logger
    assert logger.get_level() == "INFO"


def test_logger_set_level_from_env(caplog: pytest.LogCaptureFixture):
    os.environ["FOO_LOG_LEVEL"] = "LOG_DEBUG"

    logger = VilaLogger("FOO")
    assert logger.get_level() == "DEBUG"
    logger.debug("debug")
    logger.trace("trace")

    os.environ["BAR_LOG_LEVEL"] = "ERROR"

    logger = VilaLogger("BAR")
    assert logger.get_level() == "ERROR"
    logger.info("info")
    logger.warning("warning")
    logger.error("error")

    os.environ["QUZ_LOG_LEVEL"] = "TRACE"
    assert VilaLogger("QUZ").get_level() == "TRACE"

    os.environ["QUX_LOG_LEVEL"] = "INFO"
    assert VilaLogger("QUX").get_level() == "INFO"

    assert tuple(caplog.record_tuples) == (
        ("FOO", logging.DEBUG, "debug"),
        ("BAR", logging.ERROR, "error"),
    )


def test_logger_set_level(caplog: pytest.LogCaptureFixture):
    logger = VilaLogger("BAZ", default_level="WARNING")
    assert logger.get_level() == "WARNING"
    logger.info("info")
    assert logger.is_enabled_for("fatal")
    assert logger.is_enabled_for("error")
    assert logger.is_enabled_for("warning")
    assert not logger.is_enabled_for(logging.INFO)
    assert not logger.is_enabled_for(logging.DEBUG)
    assert not logger.is_enabled_for("trace")
    assert not caplog.record_tuples

    logger.set_level("INFO")
    assert logger.get_level(use_string=False) == logging.INFO
    logger.info("info")
    assert caplog.records[0].message == "info"


def test_logger_drop():
    logger = VilaLogger("DROPTEST")
    logger.drop()

    logger.info("info")
    assert not logger.logger.handlers


def test_logger_nest(caplog: pytest.LogCaptureFixture):
    logger2 = VilaLogger("NESTTEST").nest("L1").nest("L2")
    assert logger2.logger.name == "NESTTEST.L1.L2"

    logger1 = VilaLogger("NESTTEST.L1")
    assert logger1.logger is logger2.logger.parent

    logger1.set_level("WARNING")
    logger2.info("info")
    assert not caplog.records

    logger2.fatal("fatal")
    assert caplog.records[0].message == "fatal"
