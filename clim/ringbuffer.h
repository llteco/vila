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
/****************************************
 * Description: a ring buffer fifo template
 ****************************************/
#ifndef CLIM_RINGBUFFER_H_
#define CLIM_RINGBUFFER_H_
#include <memory>
#include <type_traits>

/**
 * @brief A simple ring buffer object, acts like a FIFO.
 *
 * This ring buffer is not thread-safe.
 * @tparam T: the object type contains in the ring buffer.
 */
template <class T>
class RingBuffer {
  static_assert(
      std::is_nothrow_default_constructible<T>::value,
      "Element type must be nothrow default constructible"
  );

 public:
  using value_type = T;

  /**
   * @brief Construct a new Ring Buffer object with a fixed size.
   *
   * @param size: capacity of the ring buffer, default to 2.
   */
  explicit RingBuffer(size_t size = 2) : capacity_(size) {
    buffer_.reset(new T[size]);
  }

  ~RingBuffer() = default;

  /// Get the ringbuffer capacity.
  size_t Capacity() const { return capacity_; }

  /**
   * @brief Add an element to the ring buffer.
   *
   * If the ring buffer is full, the oldest element will be removed (roll-over).
   * @param data: the element to be added.
   */
  void Push(const T& data) noexcept {
    buffer_[head_ % capacity_] = data;
    head_++;
    if (head_ == tail_ + capacity_ + 1) {
      // new data overflow, override the oldest data.
      tail_++;
    }
  }

  /**
   * @brief Read the oldest element from the ring buffer.
   *
   * The method is an UB if the ring buffer is empty.
   * @return T: the oldest element.
   */
  T Peak() noexcept {
    T data = buffer_[tail_ % capacity_];
    return data;
  }

  /**
   * @brief Read the oldest element from the ring buffer and remove it.
   *
   * @return T: the element read from.
   */
  T Pop() noexcept {
    T data = Peak();
    if (!Empty()) {
      tail_++;
    }
    return data;
  }

  /// Check if the ring buffer is empty.
  bool Empty() const { return head_ == tail_; }

 private:
  std::unique_ptr<T[]> buffer_;
  const size_t capacity_;
  size_t head_ = 0;
  size_t tail_ = 0;
};
#endif  // CLIM_RINGBUFFER_H_
