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
#include "clim/path.h"

#include <gtest/gtest.h>

namespace fs = std::filesystem;

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
  GetCurrentModuleDir();
}
#ifdef _WIN32
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST(PathTest, WindowsStyle) {
  Path abs(R"(C:\Users\Vila\do_not_exist.log)");
  Path rel(R"(Users\Vila)");
  EXPECT_TRUE(abs.IsAbsolute());
  EXPECT_FALSE(rel.IsAbsolute());
  EXPECT_FALSE(abs.IsDir());
  EXPECT_FALSE(rel.IsDir());
  EXPECT_FALSE(abs.IsFIFO());
  EXPECT_FALSE(rel.IsFIFO());
  EXPECT_FALSE(abs.IsFile());
  EXPECT_FALSE(rel.IsFile());
  EXPECT_TRUE(abs.IsRelativeTo(Path(R"(C:\Users)")));
  EXPECT_TRUE(abs.IsRelativeTo(abs));
  EXPECT_FALSE(abs.IsRelativeTo(rel));
  EXPECT_TRUE(rel.IsRelativeTo(rel));
  EXPECT_FALSE(abs.IsSocket());
  EXPECT_FALSE(rel.IsSocket());
  EXPECT_FALSE(abs.IsSymlink());
  EXPECT_FALSE(rel.IsSymlink());
  EXPECT_EQ(abs.Absolute(), abs);
  EXPECT_EQ(rel.Absolute(), Path::CWD() / rel);
  EXPECT_EQ(abs.AsPosix(), R"(C:/Users/Vila/do_not_exist.log)");
  EXPECT_EQ(rel.AsPosix(), R"(Users/Vila)");
  EXPECT_EQ(abs.AsUri(), R"(file://C:/Users/Vila/do_not_exist.log)");
  EXPECT_EQ(abs.Drive(), "C:");
  EXPECT_EQ(rel.Drive(), "");
  EXPECT_FALSE(abs.Exists());
  EXPECT_EQ(abs.Parent(), Path(R"(C:\Users\Vila)"));
  EXPECT_EQ(rel.Parent(), Path(R"(Users)"));
  EXPECT_EQ(abs.RelativeTo(rel), Path(""));
  EXPECT_EQ(abs.RelativeTo(Path(R"(C:\Users)")), Path("Vila/do_not_exist.log"));
  EXPECT_EQ(abs.Root(), R"(\)");
  EXPECT_EQ(rel.Root(), "");
  EXPECT_EQ(abs.Stem(), "do_not_exist");
  EXPECT_EQ(rel.Stem(), "Vila");
  EXPECT_EQ(abs.Suffix(), ".log");
  EXPECT_EQ(rel.Suffix(), "");
  EXPECT_EQ(Path(R"(C:\)") / rel / Path("do_not_exist.log"), abs);
  EXPECT_EQ(Path("C:/").Parent(), Path("C:/"));
}
#else   // WIN32
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST(PathTest, PoxisStyle) {
  Path abs(R"(/home/Vila/do_not_exist.log)");
  Path rel(R"(home/Vila)");
  EXPECT_TRUE(abs.IsAbsolute());
  EXPECT_FALSE(rel.IsAbsolute());
  EXPECT_FALSE(abs.IsDir());
  EXPECT_FALSE(rel.IsDir());
  EXPECT_FALSE(abs.IsFIFO());
  EXPECT_FALSE(rel.IsFIFO());
  EXPECT_FALSE(abs.IsFile());
  EXPECT_FALSE(rel.IsFile());
  EXPECT_TRUE(abs.IsRelativeTo(Path(R"(/home/Vila)")));
  EXPECT_TRUE(abs.IsRelativeTo(abs));
  EXPECT_FALSE(abs.IsRelativeTo(rel));
  EXPECT_TRUE(rel.IsRelativeTo(rel));
  EXPECT_FALSE(abs.IsSocket());
  EXPECT_FALSE(rel.IsSocket());
  EXPECT_FALSE(abs.IsSymlink());
  EXPECT_FALSE(rel.IsSymlink());
  EXPECT_EQ(abs.Absolute(), abs);
  EXPECT_EQ(rel.Absolute(), Path::CWD() / rel);
  EXPECT_EQ(abs.AsPosix(), R"(/home/Vila/do_not_exist.log)");
  EXPECT_EQ(rel.AsPosix(), R"(home/Vila)");
  EXPECT_EQ(abs.AsUri(), R"(file:///home/Vila/do_not_exist.log)");
  EXPECT_EQ(abs.Drive(), "");
  EXPECT_EQ(rel.Drive(), "");
  EXPECT_FALSE(abs.Exists());
  EXPECT_EQ(abs.Parent(), Path(R"(/home/Vila)"));
  EXPECT_EQ(rel.Parent(), Path("home"));
  EXPECT_EQ(abs.RelativeTo(rel), Path(""));
  EXPECT_EQ(abs.RelativeTo(Path("/home")), Path("Vila/do_not_exist.log"));
  EXPECT_EQ(abs.Root(), "/");
  EXPECT_EQ(rel.Root(), "");
  EXPECT_EQ(abs.Stem(), "do_not_exist");
  EXPECT_EQ(rel.Stem(), "Vila");
  EXPECT_EQ(abs.Suffix(), ".log");
  EXPECT_EQ(rel.Suffix(), "");
  EXPECT_EQ(Path("/") / rel / Path("do_not_exist.log"), abs);
  EXPECT_EQ(Path("/"), Path("/"));
}
#endif  // WIN32
