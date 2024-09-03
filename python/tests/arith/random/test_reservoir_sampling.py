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
