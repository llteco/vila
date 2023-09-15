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
#ifndef CLIM_QUAT_H_
#define CLIM_QUAT_H_
#include <array>
#include <cmath>
#include <limits>
#include <tuple>

#include "clim/numerical.h"

/** @brief The order of Euler angles. */
enum class EulerType : int {
  xyz = 12,
  xzy = 21,
  yxz = 102,
  yzx = 120,
  zyx = 210,
  zxy = 201
};

inline constexpr std::tuple<int, int, int> EulerAxis(const EulerType& order) {
  int iorder = static_cast<int>(order);
  int k = iorder % 10;
  int j = (iorder / 10) % 10;
  int i = (iorder / 100) % 10;
  return {i, j, k};
}

template <class T>
struct Degree {
  explicit Degree(T value) : deg(value) {}

  T Rad() const { return deg2rad(deg); }

  static Degree FromRad(T value) { return Degree(rad2deg(value)); }

  T deg;
};

/** @class Quaternion
 * @brief A quaternion represents a rotation in 3D space.
 *
 * This class provides calculation and transformation with matrix and Euler
 * angles.
 * For more details, see https://en.wikipedia.org/wiki/Quaternion
 */
template <class T>
class Quaternion {
 public:
  /**
   * @brief Default construction. Create an identity quaternion.
   */
  Quaternion() : Quaternion(Identity()) {}

  /**
   * @brief Construct a quaternion directly from 4 numbers.
   *
   * @param [in] x:
   * @param [in] y:
   * @param [in] z:
   * @param [in] w:
   */
  Quaternion(T x, T y, T z, T w) {
    x_ = x;
    y_ = y;
    z_ = z;
    w_ = w;
  }

  /**
   * @brief Construct a quaternion from euler angle
   *
   * @param [in] pitch: angle around x-axis
   * @param [in] yaw: angle around y-axis
   * @param [in] roll: angle around z-axis
   * @param [in] order: order to apply rotation angles
   */
  Quaternion(T pitch, T yaw, T roll, EulerType order = EulerType::xyz) {
#if __cplusplus >= 201703L
    auto&& [i, j, k] = EulerAxis(order);
#else
    int i = std::get<0>(EulerAxis(order));
    int j = std::get<1>(EulerAxis(order));
    int k = std::get<2>(EulerAxis(order));
#endif
    std::array<T, 3> angles{pitch, yaw, roll};
    *this = elementaryQuat(i, angles[i]) * elementaryQuat(j, angles[j]) *
            elementaryQuat(k, angles[k]);
  }

  Quaternion(
      Degree<T> pitch,
      Degree<T> yaw,
      Degree<T> roll,
      EulerType order = EulerType::xyz
  )
      : Quaternion(pitch.Rad(), yaw.Rad(), roll.Rad(), order) {}

  T W() const { return w_; }

  T X() const { return x_; }

  T Y() const { return y_; }

  T Z() const { return z_; }

  /**
   * @brief Apply 3-D rotation to a 3D vector.
   *
   * @param xyz: a tuple of vector coordinate (x, y, z)
   * @return a new vector with rotated coordinate (x', y', z')
   */
  std::tuple<T, T, T> Apply(std::tuple<T, T, T> xyz) const {
#if __cplusplus >= 201703L
    auto&& [x, y, z] = xyz;
#else
    auto x = std::get<0>(xyz);
    auto y = std::get<1>(xyz);
    auto z = std::get<2>(xyz);
#endif
    auto vec = Quaternion(x, y, z, 0);
    auto rot = *this * vec * this->Conj();
    return std::tie(rot.x_, rot.y_, rot.z_);
  }

  /**
   * @brief Apply 3-D rotation to a 3D vector.
   *
   * @param xyz: a tuple of vector coordinate (x, y, z)
   * @return a new vector with rotated coordinate (x', y', z')
   */
  std::tuple<T, T, T> Apply(T x, T y, T z) const {
    return Apply(std::tie(x, y, z));
  }

  /**
   * @brief Conjugation of the quaternion
   *
   * @note The mark of conjugation for quaternion Q is usually Q^* or Q^\dagger.
   * @note A rotation to vector X can be represented as QXQ^*.
   *
   * @return Quaternion
   */
  Quaternion Conj() const { return Quaternion(-x_, -y_, -z_, w_); }

  T Dot(const Quaternion& rhs) const {
    return x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_ + w_ * rhs.w_;
  }

  Quaternion Add(const Quaternion& rhs) const {
    return Quaternion(x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_, w_ + rhs.w_);
  }

  Quaternion Mul(const Quaternion& rhs) const {
    return Quaternion(
        w_ * rhs.x_ + x_ * rhs.w_ + y_ * rhs.z_ - z_ * rhs.y_,
        w_ * rhs.y_ - x_ * rhs.z_ + y_ * rhs.w_ + z_ * rhs.x_,
        w_ * rhs.z_ + x_ * rhs.y_ - y_ * rhs.x_ + z_ * rhs.w_,
        w_ * rhs.w_ - x_ * rhs.x_ - y_ * rhs.y_ - z_ * rhs.z_
    );
  }

  Quaternion Reciprocal() const {
    return Conj() / (x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_);
  }

  Quaternion Normalize() const {
    T n = Norm();
    return Quaternion(x_ / n, y_ / n, z_ / n, w_ / n);
  }

  bool Normalized() const {
    return std::abs(Norm() - static_cast<T>(1.0)) <
           std::numeric_limits<T>::epsilon();
  }

  T Magnitude() const {
    return 2 * std::atan2(std::sqrt(x_ * x_ + y_ * y_ + z_ * z_), std::abs(w_));
  }

  T Norm() const { return std::sqrt(x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_); }

  Quaternion Scale(T s) const {
    return Quaternion(x_ * s, y_ * s, z_ * s, w_ * s);
  }

  Quaternion operator-() const { return Quaternion(-x_, -y_, -z_, -w_); }

  Quaternion operator+(const Quaternion& rhs) const { return Add(rhs); }

  Quaternion operator-(const Quaternion& rhs) const { return Add(-rhs); }

  Quaternion operator*(const Quaternion& rhs) const { return Mul(rhs); }

  Quaternion operator*(const T& scalar) const { return Scale(scalar); }

  Quaternion operator/(const T& scalar) const {
    return Scale(static_cast<T>(1) / scalar);
  }

  bool operator==(const Quaternion& other) const noexcept {
    Quaternion lhs = this->Normalize();
    Quaternion rhs = other.Normalize();
    return lhs.x_ - rhs.x_ <= std::numeric_limits<T>::epsilon() &&
           lhs.y_ - rhs.y_ <= std::numeric_limits<T>::epsilon() &&
           lhs.z_ - rhs.z_ <= std::numeric_limits<T>::epsilon() &&
           lhs.w_ - rhs.w_ <= std::numeric_limits<T>::epsilon();
  }

  static Quaternion Identity() {
    return Quaternion(
        static_cast<T>(0), static_cast<T>(0), static_cast<T>(0),
        static_cast<T>(1)
    );
  }

  std::tuple<T, T, T> AsEuler(EulerType order = EulerType::xyz) const {
    T pitch = 0, yaw = 0, roll = 0;
    int i = 0, j = 0, k = 0;
    switch (order) {
      case EulerType::xyz:
        i = 0;
        j = 1;
        k = 2;
        break;
      case EulerType::xzy:
        i = 0;
        j = 2;
        k = 1;
        break;
      case EulerType::yxz:
        i = 1;
        j = 0;
        k = 2;
        break;
      case EulerType::yzx:
        i = 1;
        j = 2;
        k = 0;
        break;
      case EulerType::zxy:
        i = 2;
        j = 0;
        k = 1;
        break;
      case EulerType::zyx:
        i = 2;
        j = 1;
        k = 0;
        break;
    }
    // Step 0
    // Check if permutation is even (+1) or odd (-1)
    int sign = (i - j) * (j - k) * (k - i) / 2;
    constexpr T kEps = static_cast<T>(1e-7);
    constexpr T kPi = numbers::pi_v<T>;
    constexpr T k2Pi = kPi * 2;
    std::array<T, 4> quat{x_, y_, z_, w_};
    T a = w_ - quat[j];
    T b = quat[i] + quat[k] * static_cast<T>(sign);
    T c = quat[j] + w_;
    T d = quat[k] * static_cast<T>(sign) - quat[i];
    // Step 2
    // Compute second angle...
    yaw = 2 * std::atan2(std::sqrt(c * c + d * d), std::sqrt(a * a + b * b));
    // Step 3
    // compute first and third angles, according to case
    T half_sum = std::atan2(b, a);
    T half_diff = std::atan2(d, c);
    // ... and check if equal to is 0 or pi, causing a singularity
    if (std::abs(yaw) <= kEps) {
      pitch = 2 * half_sum;
    } else if (std::abs(yaw - kPi) <= kEps) {
      pitch = -2 * half_diff;
    } else {
      // normal case, no singularities
      pitch = half_sum - half_diff;
      roll = half_sum + half_diff;
    }
    roll *= static_cast<T>(sign);
    yaw -= kPi / 2;
    pitch += pitch < -kPi ? k2Pi : pitch > kPi ? -k2Pi : 0;
    yaw += yaw < -kPi ? k2Pi : yaw > kPi ? -k2Pi : 0;
    roll += roll < -kPi ? k2Pi : roll > kPi ? -k2Pi : 0;
    return std::tie(pitch, yaw, roll);
  }

 private:
  Quaternion elementaryQuat(int axis, T angle) {
    Quaternion q;
    q.w_ = std::cos(angle / static_cast<T>(2));
    T k = std::sin(angle / static_cast<T>(2));
    switch (axis) {
      case 0:
        q.x_ = k;
        break;
      case 1:
        q.y_ = k;
        break;
      case 2:
        q.z_ = k;
        break;
      default:
        break;
    }
    return q;
  }

  T x_, y_, z_, w_;
};

using Quaternion_d = Quaternion<double>;
using Quaternion_f = Quaternion<float>;
#endif  // CLIM_QUAT_H_
