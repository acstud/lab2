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

#include "imgproc.hpp"

///@brief Check if the dimensions of two images are equal, or throw a domain error.
static inline void checkDimensionsEqualOrThrow(const Image *a, const Image *b) {
  assert(a != nullptr);
  assert(b != nullptr);
  if ((a->width != b->width) || (a->height != b->height)) {
    throw std::domain_error("Source and destination image are not of equal dimensions.");
  }
}

///@brief Check if the color channel is valid, or throw a domain error.
static inline void checkValidColorChannelOrThrow(int channel) {
  if ((channel < 0) || (channel > 3)) {
    throw std::domain_error("Color channel must be 0,1,2 or 3.");
  }
}

void convolute(const Image *src, Image *dest, const Kernel *kernel, int channel) {
  // Check arguments
  assert((src != nullptr) && (dest != nullptr) && (kernel != nullptr));
  checkDimensionsEqualOrThrow(src, dest);
  checkValidColorChannelOrThrow(channel);

  // Loop over every pixel
  for (int y = 0; y < src->height; y++) {
    for (int x = 0; x < src->width; x++) {
      // Convolution result
      auto c = 0.0;
      // Loop over every kernel weight
      for (int ky = -kernel->height / 2; ky <= kernel->height / 2; ky++) {
        for (int kx = -kernel->width / 2; kx <= kernel->width / 2; kx++) {
          // Convolute pixel x
          int cx = x + kx;
          // Convolute pixel y
          int cy = y + ky;
          // Bounds checking
          if ((cx >= 0) && (cy >= 0) && (cx < src->width) && (cy < src->height)) {
            // Pixel value
            auto v = (float) src->pixel(cx, cy).colors[channel];
            // Kernel weight
            auto k = kernel->weight(kx, ky);
            // Multiply and accumulate
            c += v * k;
          }
        }
      }
      // Set the channel to the new color
      dest->pixel(x, y).colors[channel] = (unsigned char) (c * kernel->scale);
    }
  }
}

Histogram getHistogram(const Image *src) {
  // Check arguments
  assert((src != nullptr));

  Histogram hist;

  for (int y = 0; y < src->height; y++) {
    for (int x = 0; x < src->width; x++) {
      for (int c = 0; c < 4; c++) {
        auto intensity = src->pixel(x, y).colors[c];
        hist(intensity, c)++;
      }
    }
  }

  return hist;
}

void enhanceContrastLinearly(const Image *src, const Histogram *src_hist, Image *dest, int low, int high, int channel) {
  // Check arguments
  assert((src != nullptr) && (src_hist != nullptr) && (dest != nullptr));
  checkDimensionsEqualOrThrow(src, dest);
  checkValidColorChannelOrThrow(channel);

  unsigned char first = 0;
  unsigned char last = 255;

  // Obtain the first intensity that is above the threshold.
  for (first = 0; first < src_hist->range; first++) {
    if (src_hist->count(first, channel) > low) {
      break;
    }
  }

  // Obtain the first intensity that is above the threshold.
  for (last = 255; last > first; last--) {
    if (src_hist->count(last, channel) > high) {
      break;
    }
  }

  // Obtain the scaling factor and offset.
  float scale = 255.0f / (last - first);
  auto offset = first;

  // For every pixel
  for (int y = 0; y < src->height; y++) {
    for (int x = 0; x < src->width; x++) {
      // Clamp anything above and below the threshold to 0...255
      if (src->pixel(x, y).colors[channel] < first) {
        dest->pixel(x, y).colors[channel] = 0;
      } else if (src->pixel(x, y).colors[channel] > last) {
        dest->pixel(x, y).colors[channel] = 255;
      } else {
        // Anything else is scaled
        dest->pixel(x, y).colors[channel] = (unsigned char) (scale * (src->pixel(x, y).colors[channel] - offset));
      }
    }
  }

}

void applyRipple(const Image *src, Image *dest, float frequency) {
  // Check arguments
  assert((src != nullptr) && (dest != nullptr));
  checkDimensionsEqualOrThrow(src, dest);

  // For every pixel
  for (int y = 0; y < src->height; y++) {
    for (int x = 0; x < src->width; x++) {

      // Make this pixel black by default
      (*dest)(x, y) = Pixel{0, 0, 0, 0};

      // Normalize x and y to [-1,1]
      float nx = -1.0f + (2.0f * x) / src->width;
      float ny = -1.0f + (2.0f * y) / src->height;

      // Calculate distance to center
      auto dist = std::sqrt(std::pow(ny, 2) + std::pow(nx, 2));

      // Calculate angle
      float angle = std::atan2(ny, nx);

      // Use a funky formula to make a lensing effect.
      auto src_dist = std::pow(std::sin(dist * M_PI / 2.0 * frequency), 2);

      // Check if this pixel lies within the source range, otherwise make this pixel transparent.
      if ((src_dist > 1.0f)) {
        continue;
      }

      // Calculate normalized lensed X and Y
      auto nsx = src_dist * std::cos(angle);
      auto nsy = src_dist * std::sin(angle);

      // Rescale to image size
      auto sx = int((nsx + 1.0) / 2 * src->width);
      auto sy = int((nsy + 1.0) / 2 * src->height);

      // Check bounds on source pixel
      if ((sx >= src->width) || (sy >= src->height)) {
        continue;
      }

      // Set destination pixel from source
      dest->pixel(x, y) = src->pixel(sx, sy);
    }
  }
}

void copyChannel(const Image *src, Image *dest, int channel) {
  // Check arguments
  assert((src != nullptr) && (dest != nullptr));
  checkDimensionsEqualOrThrow(src, dest);
  checkValidColorChannelOrThrow(channel);

  // For every pixel
  for (int y = 0; y < src->height; y++) {
    for (int x = 0; x < src->width; x++) {
      // Just copy the value.
      dest->pixel(x, y).colors[channel] = src->pixel(x, y).colors[channel];
    }
  }
}
