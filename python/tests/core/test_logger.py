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
