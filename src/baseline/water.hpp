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

#include <string>
#include <memory>

#include "../utils/Image.hpp"

/// @brief structure to pass pipeline options
struct WaterEffectOptions {
  std::string img_name;
  bool blur = false;
  int blur_size = 11;
  bool histogram = false;
  bool enhance = false;
  bool enhance_hist = false;
  bool ripple = false;
  float ripple_frequency = 2 * 1.337f;
  bool save_intermediate = false;
};

/**
 * @brief Return an image on which a water effect was applied.
 *
 * @param src       The source image .
 * @param options   The options for the water effect.
 * @return          A smart pointer to a new image.
 */
std::shared_ptr<Image> runWaterEffect(const Image *src, const WaterEffectOptions *options);
