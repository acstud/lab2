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

#include "Kernel.hpp"

Kernel Kernel::gaussian(int width, int height, float sigma) {
  Kernel g(width, height);
  for (int y = -height / 2; y <= height / 2; y++) {
    for (int x = -width / 2; x <= width / 2; x++) {
      auto sigma_pow2 = (float) std::pow(sigma, 2);
      auto scale = 1 / (2.0 * M_PI * sigma_pow2);
      auto e_pow = std::pow(M_E, -((std::pow(x, 2) + std::pow(y, 2)) / (2 * sigma_pow2)));
      g(x, y) = static_cast<float>(scale * e_pow);
    }
  }
  g.normalize();
  return g;
}

void Kernel::print() {
  for (int y = -height / 2; y <= height / 2; y++) {
    if (y == 0) {
      std::cout << std::setw(12) << (std::to_string(scale) + " x ");
    } else {
      std::cout << std::setw(12) << "";
    }
    for (int x = -width / 2; x <= width / 2; x++) {
      std::cout << std::setw(12) << (std::to_string(this->operator()(x, y)) + (x != width / 2 ? ", " : "\n"));
    }
  }
}

void Kernel::normalize() {
  float sum = 0.0f;
  for (int y = -height / 2; y <= height / 2; y++) {
    for (int x = -width / 2; x <= width / 2; x++) {
      sum += this->operator()(x, y);
    }
  }
  scale = 1 / sum;
}

Kernel::Kernel(int width, int height) : width(width), height(height) {
  if ((width % 2 != 1) || (height % 2 != 1) || (width < 1) || (height < 1)) {
    throw std::runtime_error("Each kernel dimension must be positive uneven.");
  }
  weights = std::vector<float>((size_t) (width * height), 0.0f);
  yoff = height / 2;
  xoff = width / 2;
}
