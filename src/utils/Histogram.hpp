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

#include <vector>
#include "Image.hpp"

struct Histogram {

  const int channels = 4;
  const int range = 256;

  Histogram() : values(channels * range, 0) {}

  std::vector<int> values;

  /// @brief Return count of pixels of intensity on a channel
  inline int operator()(unsigned char intensity, int channel) const {
    assert(channel >= 0 && channel < 4);
    return values[channel * range + intensity];
  }

  /// @brief Return count of pixels of intensity on a channel
  inline int count(unsigned char intensity, int channel) const {
    return operator()(intensity, channel);
  }

  /// @brief Access count of pixels of intensity on a channel
  inline int &operator()(unsigned char intensity, int channel) {
    return values[channel * range + intensity];
  }

  /// @brief Access count of pixels of intensity on a channel
  inline int &count(unsigned char intensity, int channel) {
    return operator()(intensity, channel);
  }

  /**
   * @brief Return the maximum value in the histogram.
   * @param channel The color channel to obtain the maximum value from. If this is set to -1, obtain the maximum value
   *                for all channels.
   */
  int max(int channel = -1) const;

  /**
   * @brief Convert the histogram to an image.
   * @param height_per_channel The number of pixels for the histogram height per channel.
   * @return An image containing the histogram.
   */
  std::shared_ptr<Image> toImage(unsigned int height_per_channel = 128) const;

};