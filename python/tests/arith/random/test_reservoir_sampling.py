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

import time

import numpy as np
from vila.arith.random.reservoir_sampling import ReservoirSampler


def test_reservoir_sampling_functionality():
    np.random.seed(42)
    sampler = ReservoirSampler(1000)
    sampler += np.random.standard_normal([100])
    assert sampler.sample.size == 100

    sampler += np.random.standard_normal([100000])
    assert sampler.sample.size == 1000

    assert np.abs(sampler.sample.mean()) < 0.05
    assert np.abs(1 - sampler.sample.std()) < 0.05
    np.random.seed(None)


def test_reservoir_sampling_performance():
    np.random.seed(42)
    sampler = ReservoirSampler(100000)
    data = np.random.standard_normal([10000000])
    beg = time.time()
    sampler.add(data)
    end = time.time()
    np.random.seed(None)
    print("Time elapsed: ", end - beg)
