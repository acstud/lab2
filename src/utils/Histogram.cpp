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

#include "Image.hpp"
#include "Histogram.hpp"

int Histogram::max(int channel) const {
  int max = 0;

  int cstart = 0;
  int cend = channels;

  if (channel != -1) {
    cstart = channel;
    cend = channel + 1;
  }

  for (int c = cstart; c < cend; c++) {
    for (int i = 0; i < range; i++) {
      if (max < count((unsigned char) i, c)) {
        max = count((unsigned char) i, c);
      }
    }
  }

  return max;
}

std::shared_ptr<Image> Histogram::toImage(unsigned int height_per_channel) const {
  auto img = std::make_shared<Image>((unsigned int) range, channels * height_per_channel);

  // Create a black background color for the histogram
  Pixel black = {{0, 0, 0, 255}};

  // Loop over all channels
  for (int c = 0; c < channels; c++) {
    // Find the channel maximum value to scale down the bars
    int scale = max(c);
    // Loop over all intensities
    for (int i = 0; i < range; i++) {

      // Create a color for this bar based on intensity position
      auto color = (unsigned char) (128 + i / 2);
      auto inverse = (unsigned char) (128 - i / 2);

      Pixel pixel = {{inverse, inverse, inverse, 255}};

      // If this is the alpha channel, the color should be white and slowly fade out
      if (c == 3) {
        pixel = {{255, 255, 255, color}};
      }

      // Set the actual channel to the color based on intensity
      pixel.colors[c] = color;

      // Figure out the height of the histogram bar, but use dB
      auto bar_height = (int) ((height_per_channel * count((unsigned char) i, c)) / scale);

      // Draw the bar vertically
      for (int y = 0; y < bar_height; y++) {
        int ty = ((c + 1) * height_per_channel - 1) - y;
        img->pixel(i, ty) = pixel;
      }

      // Color the rest of the pixels black
      for (int y = bar_height; y < height_per_channel; y++) {
        int ty = ((c + 1) * height_per_channel - 1) - y;
        img->pixel(i, ty) = black;
      }
    }
  }

  return img;
}

