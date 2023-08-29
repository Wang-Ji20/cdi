//===--- tracker.hh - Track Copy/Move Nums ----------------------*- C++ -*-===//
// cdi 2023
//
// Identification: include/constructor/tracker.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
// SPDX-License-Identifier: MIT
//===----------------------------------------------------------------------===//
// Copyright 2018 The Abseil Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//===----------------------------------------------------------------------===//

#ifndef CDI_CONSTRUCTOR_TRACKER_HH
#define CDI_CONSTRUCTOR_TRACKER_HH

#include <memory>
#include <utility>

// A class that tracks its copies and moves so that it can be queried in tests.
template <class T>
class Tracked {
public:
  Tracked() = default;

  // NOLINTNEXTLINE(runtime/explicit)
  Tracked(const T &val) : val_(val) {}

  Tracked(const Tracked &that)
      : val_(that.val_),
        num_moves_(that.num_moves_),
        num_copies_(that.num_copies_) {
    ++(*num_copies_);
  }

  Tracked(Tracked &&that) noexcept
      : val_(std::move(that.val_)),
        num_moves_(std::move(that.num_moves_)),
        num_copies_(std::move(that.num_copies_)) {
    ++(*num_moves_);
  }

  auto
  operator=(const Tracked &that) -> Tracked & {
    Tracked(that).swap(*this);
    ++(*num_copies_);
  }

  auto
  operator=(Tracked &&that) noexcept -> Tracked & {
    that.swap(*this);
    ++(*num_moves_);
  }

  auto
  val() const -> const T & {
    return val_;
  }

  friend auto
  operator==(const Tracked &lhs, const Tracked &rhs) -> bool {
    return lhs.val_ == rhs.val_;
  }

  friend auto
  operator!=(const Tracked &lhs, const Tracked &rhs) -> bool {
    return !(lhs == rhs);
  }

  auto
  num_copies() -> size_t {
    return *num_copies_;
  }

  auto
  num_moves() -> size_t {
    return *num_moves_;
  }

private:
  T val_;
  std::shared_ptr<size_t> num_moves_ = std::make_shared<size_t>(0);
  std::shared_ptr<size_t> num_copies_ = std::make_shared<size_t>(0);
};

#endif // CDI_CONSTRUCTOR_TRACKER_HH
