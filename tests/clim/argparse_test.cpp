/*
 * Copyright (C) 2021-2026 The VILA Authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
  auto args = parser.Parse(3, argv);
  EXPECT_EQ(args["pos1"], "pos1");
  EXPECT_EQ(args["pos2"], "pos2");
  // extra positional values now raise, like python argparse
  EXPECT_THROW(parser.Parse(4, argv), ParseError);
}

TEST(Argparse, DoubleDashSeparator) {
  ArgumentParser parser;
  parser.AddArgument("--foo").WithDefault(1);
  parser.AddArgument("pos1");
  const char* argv[] = {__FUNCTION__, "--foo=2", "--", "-bar"};
  auto args = parser.Parse(4, argv);
  EXPECT_EQ(args["foo"].AsLong(), 2);
  EXPECT_EQ(args["pos1"].AsStr(), "-bar");
}

TEST(Argparse, DoubleDashAlone) {
  // a lone "--" is no longer an unknown option named "-"
  ArgumentParser parser;
  parser.AddArgument("--foo").WithDefault(1);
  const char* argv[] = {__FUNCTION__, "--foo=2", "--"};
  auto args = parser.Parse(3, argv);
  EXPECT_EQ(args["foo"].AsLong(), 2);
  EXPECT_TRUE(std::get<1>(parser.ParseKnown(3, argv)).empty());
}

TEST(Argparse, DoubleDashAfterDoubleDash) {
  // only the first "--" is a separator; later ones are position arguments
  ArgumentParser parser;
  parser.AddArgument("pos1");
  parser.AddArgument("pos2");
  const char* argv[] = {__FUNCTION__, "--", "--", "-bar"};
  auto args = parser.Parse(4, argv);
  EXPECT_EQ(args["pos1"].AsStr(), "--");
  EXPECT_EQ(args["pos2"].AsStr(), "-bar");
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

TEST(Argparse, AppendRepeatable) {
  // repeated occurrences append instead of erroring on a count mismatch
  ArgumentParser parser;
  parser.AddArgument("--param").Append();
  const char* argv[] = {__FUNCTION__, "--param", "a=1", "--param", "b=2"};
  auto args = parser.Parse(5, argv);
  EXPECT_EQ(args["param"].Size(), 2U);
  EXPECT_EQ(args["param"][0].AsStr(), "a=1");
  EXPECT_EQ(args["param"][1].AsStr(), "b=2");
  const char* argv2[] = {__FUNCTION__, "--param=a=1", "--param=b=2"};
  args = parser.Parse(3, argv2);
  EXPECT_EQ(args["param"][1].AsStr(), "b=2");
  // absent is fine when not required
  EXPECT_EQ(parser.Parse(1, argv2)["param"].Size(), 0U);
}

TEST(Argparse, AppendWithPositional) {
  // each occurrence still takes exactly Nargs values, positionals intact
  ArgumentParser parser;
  parser.AddArgument("--param").Append();
  parser.AddArgument("graph");
  const char* argv[] = {__FUNCTION__, "--param", "a=1", "topo.json",
                        "--param", "b=2"};
  auto args = parser.Parse(6, argv);
  EXPECT_EQ(args["param"].Size(), 2U);
  EXPECT_EQ(args["graph"].AsStr(), "topo.json");
}

TEST(Argparse, AppendRequired) {
  ArgumentParser parser;
  parser.AddArgument("--param").Append().Required();
  const char* argv[] = {__FUNCTION__, "--param", "a=1"};
  EXPECT_THROW(parser.Parse(1, argv), ParseError);
  EXPECT_EQ(parser.Parse(3, argv)["param"].Size(), 1U);
}

TEST(Argparse, RepeatWithoutAppend) {
  // without Append the exact-count check stays strict
  ArgumentParser parser;
  parser.AddArgument("--foo");
  const char* argv[] = {__FUNCTION__, "--foo", "1", "--foo", "2"};
  EXPECT_THROW(parser.Parse(5, argv), ParseError);
}

TEST(Argparse, ParseDict) {
  std::map<std::string, std::string> dict = {
      {"foo", "bar"},
      {"baz", "qux"}
  };
  ArgumentParser parser;
  parser.AddArgument("--foo");
  parser.AddArgument("--baz");
  auto args = parser.ParseDict(dict);
  EXPECT_EQ(args["foo"].AsStr(), "bar");
  EXPECT_EQ(args["baz"].AsStr(), "qux");
}

TEST(Argparse, ParseDictPositionArgs) {
  std::map<std::string, std::string> dict = {
      {"", "bar;baz"},
  };
  ArgumentParser parser;
  parser.AddArgument("pos1");
  parser.AddArgument("pos2");
  auto args = parser.ParseDict(dict);
  EXPECT_EQ(args["pos1"].AsStr(), "bar");
  EXPECT_EQ(args["pos2"].AsStr(), "baz");
}

TEST(Argparse, ParseDictWithError) {
  std::map<std::string, std::string> dict = {
      {"foo", "bar"},
      {"baz", "qux"}
  };
  ArgumentParser parser;
  parser.AddArgument("--foo");
  EXPECT_THROW(parser.ParseDict(dict), ParseError);
}

TEST(Argparse, ParseDictPositionArgsWithError) {
  std::map<std::string, std::string> dict = {
      {"__POS__", "bar;baz"},
  };
  ArgumentParser parser;
  parser.AddArgument("pos1");
  // unknown positional args now raise
  EXPECT_THROW(parser.ParseDict(dict), ParseError);
}

TEST(Argparse, ParseInterleaved) {
  // options and position arguments can be interleaved anywhere
  ArgumentParser parser;
  parser.AddArgument("--foo").WithDefault(1);
  parser.AddArgument("--flag").StoreTrue();
  parser.AddArgument("pos1");
  parser.AddArgument("pos2");
  const char* argv[] = {__FUNCTION__, "a", "--foo=2", "b", "--flag"};
  auto args = parser.Parse(5, argv);
  EXPECT_EQ(args["pos1"].AsStr(), "a");
  EXPECT_EQ(args["pos2"].AsStr(), "b");
  EXPECT_EQ(args["foo"].AsLong(), 2);
  EXPECT_TRUE(args["flag"]);
}

TEST(Argparse, OptionValueNotPositional) {
  // the value of "--foo bar" is consumed by the option only
  ArgumentParser parser;
  parser.AddArgument("--foo");
  parser.AddArgument("pos1");
  const char* argv[] = {__FUNCTION__, "--foo", "bar", "baz"};
  auto args = parser.Parse(4, argv);
  EXPECT_EQ(args["foo"].AsStr(), "bar");
  EXPECT_EQ(args["pos1"].AsStr(), "baz");
}

TEST(Argparse, OptionNargsConsumesValues) {
  ArgumentParser parser;
  parser.AddArgument("--foo").Nargs(2ULL);
  parser.AddArgument("pos1");
  const char* argv[] = {__FUNCTION__, "--foo", "a", "b", "c"};
  auto args = parser.Parse(5, argv);
  EXPECT_EQ(args["foo"].Size(), 2U);
  EXPECT_EQ(args["foo"][0].AsStr(), "a");
  EXPECT_EQ(args["foo"][1].AsStr(), "b");
  EXPECT_EQ(args["pos1"].AsStr(), "c");
}

TEST(Argparse, PositionalNargsNumber) {
  ArgumentParser parser;
  parser.AddArgument("pair").Nargs(2ULL);
  parser.AddArgument("single");
  const char* argv[] = {__FUNCTION__, "a", "b", "c"};
  auto args = parser.Parse(4, argv);
  EXPECT_EQ(args["pair"].Size(), 2U);
  EXPECT_EQ(args["pair"][0].AsStr(), "a");
  EXPECT_EQ(args["pair"][1].AsStr(), "b");
  EXPECT_EQ(args["single"].AsStr(), "c");
  // "single" left without a value
  const char* argv2[] = {__FUNCTION__, "a", "b"};
  EXPECT_THROW(parser.Parse(3, argv2), ParseError);
}

TEST(Argparse, PositionalNargsPlus) {
  ArgumentParser parser;
  parser.AddArgument("src").Nargs('+');
  parser.AddArgument("dst");
  const char* argv[] = {__FUNCTION__, "a", "b", "c"};
  auto args = parser.Parse(4, argv);
  EXPECT_EQ(args["src"].Size(), 2U);
  EXPECT_EQ(args["src"][0].AsStr(), "a");
  EXPECT_EQ(args["src"][1].AsStr(), "b");
  EXPECT_EQ(args["dst"].AsStr(), "c");
  // "dst" needs at least one value
  const char* argv2[] = {__FUNCTION__, "a"};
  EXPECT_THROW(parser.Parse(2, argv2), ParseError);
}

TEST(Argparse, PositionalNargsStar) {
  ArgumentParser parser;
  parser.AddArgument("cmd");
  parser.AddArgument("rest").Nargs('*');
  const char* argv[] = {__FUNCTION__, "run", "a", "b"};
  auto args = parser.Parse(4, argv);
  EXPECT_EQ(args["cmd"].AsStr(), "run");
  EXPECT_EQ(args["rest"].Size(), 2U);
  EXPECT_EQ(args["rest"][1].AsStr(), "b");
  // "rest" can be empty
  const char* argv2[] = {__FUNCTION__, "run"};
  args = parser.Parse(2, argv2);
  EXPECT_EQ(args["rest"].Size(), 0U);
}

TEST(Argparse, PositionalNargsOptional) {
  ArgumentParser parser;
  parser.AddArgument("maybe").Nargs('?').WithDefault("none");
  const char* argv[] = {__FUNCTION__, "a"};
  EXPECT_EQ(parser.Parse(2, argv)["maybe"].AsStr(), "a");
  const char* argv2[] = {__FUNCTION__};
  EXPECT_EQ(parser.Parse(1, argv2)["maybe"].AsStr(), "none");
}
