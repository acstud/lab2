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

#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream>

#include "Image.hpp"

Image::Image(unsigned int width, unsigned int height) : width(width), height(height) {
  // Zero-initialize the image.
  raw = std::vector<unsigned char>(width * height * 4, 255);
  // Set the pixel pointer to the start of the raw buffer
  pixels = reinterpret_cast<Pixel *>(raw.data());
}

std::shared_ptr<Image> Image::fromPNG(const std::string &file_name) {

  auto img = std::make_shared<Image>();

  // Use lodepng to decode a PNG file
  unsigned int err = lodepng::decode(img->raw, img->width, img->height, file_name, LCT_RGBA);

  // Set the pixel pointer to the start of the raw buffer
  img->pixels = reinterpret_cast<Pixel *>(img->raw.data());

  if (err) {
    throw std::runtime_error("Could not load image.");
  }

  return img;
}

unsigned int Image::toPNG(const std::string &file_name) {
  unsigned int ret = lodepng::encode(file_name, raw, width, height, LCT_RGBA);
  return ret;
}

std::string Image::toString() {
  std::stringstream ss;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      for (int c = 0; c < colors; c++) {
        ss << std::setw(3) << (unsigned int) pixels[(y * width + x)].colors[c]
           << (c == colors - 1 ? "|" : ",");
      }
    }
    ss << std::endl;
  }
  return ss.str();
}

bool Image::is_approximately_equal_to(const Image *other) const{
  if ((width != other->width) || (height != other->height)) {
    std::cerr << "Not approximately equal because dimensions are different." << std::endl;
    return false;
  }

  // Calculate the error.
  double err[4] = {0.0};
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      for (int c = 0; c < colors; c++) {
        auto this_val = (double) pixel(x, y).colors[c];
        auto that_val = (double) other->pixel(x, y).colors[c];

        err[c] += std::sqrt(std::pow(this_val - that_val, 2));
      }
    }
  }

  int total_pixels = width * height;

  for (int c = 0; c < colors; c++) {
    auto channel_err = err[c] / total_pixels;
    std::cerr << "Channel " << c << " error: " << channel_err << std::endl;
    if (channel_err > error_threshold) {
      return false;
    }
  }
  return true;
}

