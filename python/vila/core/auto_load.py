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

from importlib import import_module
from pathlib import Path
from typing import Optional, Set

from .logger import LOG


def auto_load(cwd: str | Path, top: Path, filters: Optional[Set[str]] = None):
    """Search and load python modules in `cwd`.

    Args:
        cwd (str | Path): module directory, glob recurrsively
        top (Path, optional): module top directory.
        filters (Set[str], optional): patterns to filter out modules. Defaults to None.
    """
    models_dir = Path(cwd).resolve()
    package = models_dir.relative_to(top).as_posix().replace("/", ".")

    def _filter(p: Path) -> bool:
        pattern = ("__",)
        if filters:
            pattern += tuple(filters)
        return all(i not in p.stem for i in pattern)

    for src in filter(_filter, models_dir.rglob("*.py")):
        module_url = src.relative_to(models_dir).with_suffix("").as_posix()
        try:
            import_module(f".{module_url.replace('/', '.')}", package)
        except ImportError as ex:
            LOG.warning(f"failed to load {module_url}: {ex}")
