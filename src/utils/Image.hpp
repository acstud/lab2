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

#include <memory>
#include <cassert>

#include "../lodepng/lodepng.h"

///@brief A pixel.
struct Pixel {
  unsigned char colors[4];

  inline unsigned char &red() { return colors[0]; }

  inline unsigned char &green() { return colors[1]; }

  inline unsigned char &blue() { return colors[2]; }

  inline unsigned char &alpha() { return colors[3]; }
};

///@brief An image.
struct Image {

  Image() = default;

  ///@brief Construct a new image of /p width x /p height.
  Image(unsigned int width, unsigned int height);

  ///@brief Return an image loaded from a PNG file \p file_name.
  static std::shared_ptr<Image> fromPNG(const std::string &file_name);

  ///@brief Write the image to a PNG file \p file_name.
  unsigned int toPNG(const std::string &file_name);

  ///@brief Return a single pixel value
  inline Pixel operator()(int x, int y) const {
    assert((x < width) && (y < height));
    return pixels[y * width + x];
  }

  ///@brief Return a single pixel value
  inline Pixel pixel(int x, int y) const {
    return operator()(x, y);
  }

  ///@brief Access a single pixel
  inline Pixel &operator()(int x, int y) {
    assert((x < width) && (y < height));
    return pixels[y * width + x];
  }

  ///@brief Access a single pixel value
  inline Pixel &pixel(int x, int y) {
    return operator()(x, y);
  }

  ///@brief Test if image is approximately equal to right hand side image.
  bool is_approximately_equal_to(const Image *other) const;

  /// @brief Pixels
  Pixel *pixels = nullptr;

  /// @brief Raw buffer; you probably want to use the contents of this buffer on your CUDA device.
  std::vector<unsigned char> raw;

  /// @brief Width of the image
  unsigned int width = 0;

  /// @brief Heigh of the image
  unsigned int height = 0;

  /// @brief Number of color channels. Currently fixed to 4, for red, green, blue and alpha
  const unsigned int colors = 4;

  /// @brief Error threshold used for approximately equal comparison.
  const double error_threshold = 1;

  /// @brief Print all pixel values on stdout. Not recommended for large images.
  std::string toString();
};