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
#ifndef CLIM_PATH_H_
#define CLIM_PATH_H_
#include <ostream>
#include <utility>
#include <vector>

#include "clim/os_path.h"

/**
 * @brief A helper class ease the use of std::filesystem.
 *
 * Similar with Python's pathlib.Path
 */
class Path {
 public:
  explicit Path(std::filesystem::path path) : path_(std::move(path)) {}

  ~Path() = default;

  /**
   * @brief Return an absolute version of this path by prepending the current
   * working directory. No normalization or symlink resolution is performed.
   *
   * Use Resolve() to get the canonical path to a file.
   */
  Path Absolute() const { return Path(std::filesystem::absolute(path_)); }

  /**
   * @brief Return the string representation of the path with forward (/)
   * slashes.
   */
  std::string AsPosix() const { return path_.generic_string(); }

  /**
   * @brief Return the path as a 'file' URI.
   */
  std::string AsUri() const { return std::string("file://") + AsPosix(); }

  /**
   * @brief Return a new path pointing to the current working directory.
   */
  static Path CWD() { return Path(std::filesystem::current_path()); }

  /**
   * @brief The drive prefix (letter or UNC path), if any.
   */
  std::string Drive() const { return path_.root_name().string(); }

  /**
   * @brief Whether this path exists.
   */
  bool Exists() const { return std::filesystem::exists(path_); }

  /**
   * @brief Return a new path with expanded ~ and ~user constructs.
   */
  Path ExpandUser() const;

  /**
   * @brief Iterate over this subtree and yield all existing files (of any
   * kind, including directories) matching the given relative pattern.
   */
  std::vector<std::filesystem::path> Glob(std::string_view pattern) const {
    return ::Glob(pattern);
  }

  /**
   * @brief True if the path is absolute (has both a root and, if applicable,
   * a drive).
   */
  bool IsAbsolute() const {
    // On Windows `is_absolute` is os related behaviour, poxis path like
    // "/root/a/b/c" is translated to "C:/root/a/b/c" so is not regarded
    // as absolute path.
    // Here we make this function behave os agnostic, same as Python pathlib.
    if (path_.string()[0] == '/') {
      return true;
    }
    return path_.is_absolute();
  }

  /**
   * @brief Whether this path is a directory.
   */
  bool IsDir() const { return std::filesystem::is_directory(path_); }

  /**
   * @brief Whether this path is a FIFO.
   */
  bool IsFIFO() const { return std::filesystem::is_fifo(path_); }

  /**
   * @brief Whether this path is a regular file (also True for symlinks pointing
   * to regular files).
   */
  bool IsFile() const { return std::filesystem::is_regular_file(path_); }

  /**
   * @brief Return True if the path is relative to another path or False.
   */
  bool IsRelativeTo(const Path& other) const {
    if (*this == other) {
      return true;
    }
    return !RelativeTo(other).Str().empty();
  }

  /**
   * @brief Whether this path is a socket.
   */
  bool IsSocket() const { return std::filesystem::is_socket(path_); }

  /**
   * @brief Whether this path is a symbolic link.
   */
  bool IsSymlink() const { return std::filesystem::is_symlink(path_); }

  /**
   * @brief The final path component, if any.
   */
  std::string Name() const { return path_.filename().string(); }

  /**
   * @brief The logical parent of the path.
   */
  Path Parent() const { return Path(path_.parent_path()); }

  /**
   * @brief Return the relative path to another path identified by the passed
   * arguments.
   * If the operation is not possible (because this is not a subpath of the
   * other path), raise ValueError.
   */
  Path RelativeTo(const Path& rel) const {
    Path path = Absolute();
    std::vector<std::string> rev;
    while (path.Parent() != path) {
      if (path == rel.Absolute()) {
        break;
      }
      rev.push_back(path.Name());
      path = path.Parent();
    }
    path = Path("");
    for (auto i = rev.rbegin(); i != rev.rend(); ++i) {
      path /= Path(*i);
    }
    if (rel / path == *this) {
      return path;
    }
    return Path("");
  }

  /**
   * @brief Make the path absolute, resolving all symlinks on the way and also
   * normalizing it.
   *
   * @param strict
   */
  Path Resolve(bool strict = false) const {
    Path path(std::filesystem::read_symlink(path_));
    if (IsAbsolute()) {
      return path;
    }
    return CWD() / path;
  }

  /**
   * @brief Recursively yield all existing files (of any kind, including
   * directories) matching the given relative pattern, anywhere in
   * this subtree.
   */
  std::vector<std::filesystem::path> RGlob(std::string_view pattern) const {
    return ::Glob(pattern, true);
  }

  /**
   * @brief The root of the path, if any.
   */
  std::string Root() const { return path_.root_directory().string(); }

  /**
   * @brief The final path component, minus its last suffix.
   */
  std::string Stem() const { return path_.stem().string(); }

  /**
   * @brief Return the raw text of the path string.
   */
  std::string Str() const { return path_.string(); }

  /**
   * @brief The final component's last suffix, if any.
   *
   * This includes the leading period. For example: '.txt'
   */
  std::string Suffix() const { return path_.extension().string(); }

  /**
   * @brief Return a new path with the file name changed.
   */
  Path WithName(std::string_view name) const {
    std::filesystem::path new_path(path_);
    return Path(new_path.replace_filename(name));
  }

  /**
   * @brief Return a new path with the stem changed.
   */
  Path WithStem(std::string_view stem) const {
    std::filesystem::path new_path(path_);
    new_path.remove_filename();
    return Path(new_path / (std::string(stem) + Suffix()));
  }

  /**
   * @brief Return a new path with the file suffix changed.
   *
   * If the path has no suffix, add given suffix.
   * If the given suffix is an empty string, remove the suffix from the path.
   */
  Path WithSuffix(std::string_view suffix) const {
    std::filesystem::path new_path(path_);
    return Path(new_path.replace_extension(suffix));
  }

  Path operator/(const Path& path) const { return Path(path_ / path.path_); }

  Path& operator/=(const Path& path) {
    path_ /= path.path_;
    return *this;
  }

  Path& operator+=(const Path& path) {
    path_ += path.path_;
    return *this;
  }

  bool operator==(const Path& other) const { return path_ == other.path_; }

  bool operator!=(const Path& other) const { return path_ != other.path_; }

  friend std::ostream& operator<<(std::ostream& os, const Path& path) {
    return os << path.path_;
    return os;
  }

 private:
  std::filesystem::path path_;
};

#endif /* CLIM_PATH_H_ */
