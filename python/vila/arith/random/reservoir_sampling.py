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
