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

from typing import Optional

import numpy as np


class ReservoirSampler:
    def __init__(self, size: int) -> None:
        assert isinstance(size, int) and size > 0
        self.size = size
        self.buffer: Optional[np.ndarray] = None
        self.count = 0

    def add(self, new_sample) -> "ReservoirSampler":
        new_sample = np.asarray(new_sample)
        if self.buffer is None:
            self.buffer = np.empty([self.size], dtype=new_sample.dtype)
        if self.count + new_sample.size <= self.size:
            beg = self.count
            end = self.count + new_sample.size
            self.buffer[beg:end] = new_sample.flatten()
        else:
            mask = []
            indices = []
            for i in range(new_sample.size):
                if self.count + i < self.size:
                    mask.append(True)
                    indices.append(self.count + i)
                else:
                    if (index := np.random.randint(0, self.count + i)) < self.size:
                        mask.append(True)
                        indices.append(index)
                    else:
                        mask.append(False)
            selected_sample = new_sample[mask]
            self.buffer[indices] = selected_sample
        self.count += new_sample.size
        return self

    def __iadd__(self, new_sample) -> "ReservoirSampler":
        return self.add(new_sample)

    @property
    def sample(self) -> np.ndarray:
        if self.buffer is None:
            raise RuntimeError("No sample added yet.")
        return self.buffer[: self.count]
