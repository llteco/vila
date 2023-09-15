/*
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2021-2023 Intel Corporation
 *
 * This software and the related documents are Intel copyrighted materials,
 * and your use of them is governed by the express license under which they
 * were provided to you ("License"). Unless the License provides otherwise,
 * you may not use, modify, copy, publish, distribute, disclose or transmit
 * this software or the related documents without Intel's prior written
 * permission. This software and the related documents are provided as is, with
 * no express or implied warranties, other than those that are expressly stated
 * in the License.
 */
#include "clim/argparse.h"

#include <gtest/gtest.h>

TEST(Argparse, ParseKnownArgs) {
  ArgumentParser parser;
  parser.AddArgument("--name").WithDefault("foo").WithHelp("set name");
  parser.AddArgument("--foo").WithDefault(1);
  const char* argv[] = {"", "--name=Andy", "--bar=2"};
  auto&& [args, _] = parser.ParseKnown(3, argv);
  std::string name = args["name"];
  int64_t foo = args["foo"];
  EXPECT_EQ(name, "Andy");
  EXPECT_EQ(foo, 1);
}

TEST(Argparse, ParseArgs) {
  ArgumentParser parser;
  parser.AddArgument("--name").WithDefault("foo").WithHelp("set name");
  parser.AddArgument("--foo").WithDefault(1.5);
  const char* argv[] = {__FUNCTION__, "--name=Andy", "--bar=2"};
  EXPECT_THROW(parser.Parse(3, argv), ParseError);
}

TEST(Argparse, ParsePosition) {
  ArgumentParser parser;
  parser.AddArgument("pos1");
  parser.AddArgument("pos2");
  const char* argv[] = {__FUNCTION__, "pos1", "pos2", "pos3", "--pos1=1"};
  // missing pos2
  EXPECT_THROW(parser.Parse(2, argv), ParseError);
  // duplicate --pos1
  EXPECT_THROW(parser.ParseKnown(5, argv), ParseError);
  auto args = parser.Parse(4, argv);
  EXPECT_EQ(args["pos1"], "pos1");
  EXPECT_EQ(args["pos2"], "pos2");
  // pos3 just ignored
  EXPECT_EQ(args.count("pos3"), 0);
}

TEST(Argparse, ParseRequired) {
  ArgumentParser parser;
  parser.AddArgument("--foo").Required();
  const char* argv[] = {__FUNCTION__, "--foo", "bar"};
  EXPECT_THROW(parser.ParseKnown(1, argv), ParseError);
  EXPECT_THROW(parser.ParseKnown(2, argv), ParseError);
  EXPECT_EQ(parser.Parse(3, argv)["foo"].AsStr(), "bar");
}

TEST(Argparse, DefaultValue) {
  ArgumentParser parser;
  parser.AddArgument("--foo").WithDefault(1);
  // defaults to default value is ""
  parser.AddArgument("--bar");
  const char* argv[] = {__FUNCTION__, "--foo", "2"};
  EXPECT_EQ(parser.Parse(1, argv)["bar"].AsStr(), "");
  EXPECT_EQ(parser.Parse(1, argv)["foo"].AsLong(), 1);
  EXPECT_EQ(parser.Parse(2, argv)["foo"].AsLong(), 1);
  EXPECT_EQ(parser.Parse(3, argv)["foo"].AsLong(), 2);
}

TEST(Argparse, StoreTrueOrFalse) {
  ArgumentParser parser;
  parser.AddArgument("--foo").StoreTrue();
  parser.AddArgument("--bar").StoreFalse();
  const char* argv[] = {__FUNCTION__, "--foo", "--bar"};
  EXPECT_FALSE(parser.Parse(1, argv)["foo"]);
  EXPECT_TRUE(parser.Parse(2, argv)["foo"]);
  EXPECT_TRUE(parser.Parse(2, argv)["bar"]);
  EXPECT_FALSE(parser.Parse(3, argv)["bar"]);
}

TEST(Argparse, AddDuplicate) {
  ArgumentParser parser;
  parser.AddArgument("--foo");
  EXPECT_THROW(parser.AddArgument("--foo"), ParseError);
}

TEST(Argparse, NargsStar) {
  ArgumentParser parser;
  parser.AddArgument("--foo").Nargs('*').Required();
  const char* argv[] = {__FUNCTION__, "--foo=1", "--foo=2"};
  EXPECT_EQ(parser.Parse(1, argv)["foo"].AsStr(), "");
  EXPECT_EQ(parser.Parse(2, argv)["foo"].AsLong(), 1);
  EXPECT_EQ(parser.Parse(2, argv)["foo"][0].AsLong(), 1);
  EXPECT_EQ(parser.Parse(3, argv)["foo"][0].AsLong(), 1);
  EXPECT_EQ(parser.Parse(3, argv)["foo"][1].AsLong(), 2);
}

TEST(Argparse, NargsPlus) {
  ArgumentParser parser;
  parser.AddArgument("--foo").Nargs('+').Required();
  const char* argv[] = {__FUNCTION__, "--foo=1", "--foo=2"};
  EXPECT_THROW(parser.Parse(1, argv)["foo"], ParseError);
  EXPECT_EQ(parser.Parse(2, argv)["foo"].AsLong(), 1);
  EXPECT_EQ(parser.Parse(2, argv)["foo"][0].AsLong(), 1);
  EXPECT_EQ(parser.Parse(3, argv)["foo"][0].AsLong(), 1);
  EXPECT_EQ(parser.Parse(3, argv)["foo"][1].AsLong(), 2);
}

TEST(Argparse, NargsNumber) {
  ArgumentParser parser;
  parser.AddArgument("--foo").Nargs(2ULL);
  const char* argv[] = {__FUNCTION__, "--foo=1", "--foo=2"};
  EXPECT_EQ(parser.Parse(1, argv)["foo"].AsStr(), "");
  EXPECT_THROW(parser.Parse(2, argv), ParseError);
  EXPECT_EQ(parser.Parse(3, argv)["foo"][0].AsLong(), 1);
  EXPECT_EQ(parser.Parse(3, argv)["foo"][1].AsLong(), 2);
}
