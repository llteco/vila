/******************************************************************************
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2023 Intel Corporation. All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel Corporation
 * or licensors. Title to the Material remains with Intel
 * Corporation or its licensors. The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * licensors. The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No License under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials, either expressly, by implication, inducement,
 * estoppel or otherwise. Any license under such intellectual property rights
 * must be express and approved by Intel in writing.
 ******************************************************************************/
#include "clim/os.h"

#include <gtest/gtest.h>

#include <fstream>

#include "clim/os_path.h"
#include "clim/str_replace.h"

namespace fs = std::filesystem;

TEST(Os, GetEnv) {
  for (const auto& [k, v] : Environ()) {
    std::cout << k << "=" << v << std::endl;
  }
}

TEST(OsPath, Glob) {
  // make test file
  auto dir = fs::temp_directory_path() / "clim_test";
  fs::create_directory(dir);
  fs::create_directory(dir / "subdir");
  std::ofstream(dir / "a.foo").write("a", 1);
  std::ofstream(dir / "subdir/b.foo").write("a", 1);
  std::ofstream(dir / "c.bar").write("a", 1);
  std::ofstream(dir / "subdir/d.bar").write("a", 1);
  std::ofstream(dir / "e.foobar").write("a", 1);
  std::ofstream(dir / "subdir/h").write("a", 1);
  auto ans = Glob(dir / "*");
  EXPECT_EQ(ans.size(), 3);
  ans = Glob(dir / "*", true);
  EXPECT_EQ(ans.size(), 6);
  ans = Glob(dir / "*.foo", true);
  EXPECT_EQ(ans.size(), 2);
  ans = Glob(dir / "*.b??");
  EXPECT_EQ(ans.size(), 1);
  ans = Glob(dir / "**/*.bar");
  EXPECT_EQ(ans.size(), 2);
  fs::remove_all(dir);
}

TEST(OsPath, GetCurrentModuleDir) {
  // Get current UT directory from GetCurrentModuleDir
  fs::path module_dir = GetCurrentModuleDir();
  std::string module_str = StrReplace(module_dir.string(), "\\", "/");
  if (Environ().count("RUNFILES_DIR") == 0) {
    GTEST_SKIP() << "not run from bazel test";
  }
  // Get current UT directory from environment
  std::string env_dir = Environ()["RUNFILES_DIR"];
#ifdef _WIN32
  env_dir = StrReplace(env_dir + "/vila/tests/clim/", "\\", ",");
#else
  env_dir += "/vila/";
#endif
  // Compare
  EXPECT_EQ(env_dir, module_str);
}
