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
from logging import Formatter, StreamHandler, addLevelName, getLevelName
from typing import Literal, Optional, Union, overload

LOG_NAME = "VILA"
addLevelName(1, "TRACE")


def _default_level_from_env(name: str = LOG_NAME, level: str | int = "INFO"):
    match os.environ.get(f"{name}_LOG_LEVEL", default=level):
        case "LOG_TRACE" | "TRACE" | "0":
            return 1
        case "LOG_DEBUG" | "DEBUG" | "1":
            return logging.DEBUG
        case "LOG_INFO" | "INFO" | "2":
            return logging.INFO
        case "LOG_WARNING" | "WARNING" | "3":
            return logging.WARNING
        case "LOG_ERROR" | "ERROR" | "4":
            return logging.ERROR
    return logging.INFO


class VilaLogger:
    """Wrapper class for logging with preferred format and level."""

    def __init__(
        self, name: Optional[str] = None, default_level: Optional[str | int] = None
    ) -> None:
        name = name or LOG_NAME
        if name not in logging.Logger.manager.loggerDict:
            self.logger = logging.getLogger(name)
            if self.logger.parent is not logging.root:
                return
            self.logger.setLevel(_default_level_from_env(name, default_level))
            hdl = StreamHandler()
            # use colorlog to colorize different level messages
            try:
                import colorlog  # pylint:disable=import-outside-toplevel

                formatter = colorlog.ColoredFormatter(
                    "[%(asctime)s]%(log_color)s[%(levelname)s] %(message)s"
                )
            except ImportError:
                formatter = Formatter("[%(asctime)s][%(levelname)s] %(message)s")
            hdl.setFormatter(formatter)
            self.logger.addHandler(hdl)
        else:
            self.logger = logging.getLogger(name)

    def trace(self, msg: str):
        """Logging trace message"""
        self.logger.log(1, msg)

    def debug(self, msg: str):
        """Logging debug message"""
        self.logger.debug(msg)

    def info(self, msg: str):
        """Logging informative message"""
        self.logger.info(msg)

    def warning(self, msg: str):
        """Logging warning message"""
        self.logger.warning(msg)

    def error(self, msg: str):
        """Logging error message"""
        self.logger.error(msg)

    def fatal(self, msg: str):
        """Logging critical(fatal) message"""
        self.logger.critical(msg)

    def set_level(self, level: str):
        """Filter log messages by different level."""
        self.logger.setLevel(level.upper())

    def is_enabled_for(self, level: Union[int, str]) -> bool:
        """Whether current log level is active."""
        if isinstance(level, str):
            from logging import _nameToLevel  # pylint:disable=import-outside-toplevel

            return self.logger.isEnabledFor(_nameToLevel[level.upper()])
        return self.logger.isEnabledFor(level)

    @overload
    def get_level(self) -> str: ...  # noqa: E704

    @overload
    def get_level(self, use_string: Literal[True]) -> str: ...  # noqa: E704

    @overload
    def get_level(self, use_string: Literal[False]) -> int: ...  # noqa: E704

    def get_level(self, use_string: bool = True) -> Union[int, str]:
        """Get current logging level."""
        if use_string:
            return getLevelName(self.logger.level)
        return self.logger.level

    def drop(self):
        """Drop the logger handlers and flush pending events.
        Logging after `drop` will do nothing.
        """
        for hdl in list(self.logger.handlers):
            hdl.flush()
            hdl.close()
            self.logger.removeHandler(hdl)

    def nest(self, nested: str):
        """Create a nested logger with nested name."""
        log = VilaLogger(f"{self.logger.name}.{nested}")
        return log


LOG = VilaLogger()
"""Default VILA logger instance. Default log level is INFO."""
