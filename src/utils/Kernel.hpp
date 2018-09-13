// Copyright 2018 Delft University of Technology
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdexcept>
#include <vector>
#include <cmath>
#include <iostream>
#include <iomanip>
/**
 * @brief A convolution kernel
 */
struct Kernel {

  ///@brief Construct a new kernel with a specific width and height.
  Kernel(int width, int height);

  ///@brief Return a gaussian kernel.
  static Kernel gaussian(int width, int height, float sigma);

  ///@brief Return weight at x,y
  inline float operator()(int x, int y) const {
    return weights[(y + yoff) * width + (x + xoff)];
  }

  inline float weight(int x, int y) const {
    return weights[(y + yoff) * width + (x + xoff)];
  }

  ///@brief Access weight at x,y
  inline float &operator()(int x, int y) {
    return weights[(y + yoff) * width + (x + xoff)];
  }

  /// @brief Set scaling factor to average of all weights.
  void normalize();

  /// @brief Print the kernel on stdout
  void print();

  int width = 0;
  int height = 0;

  int xoff = 0;
  int yoff = 0;

  std::vector<float> weights = {};

  float scale = 1.0f;
};
