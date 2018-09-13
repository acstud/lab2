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

#include "../utils/Image.hpp"
#include "../utils/Kernel.hpp"
#include "../utils/Histogram.hpp"

/**
 * @brief Convolute the image \p img with the kernel \p kernel on channel \p channel.
 *
 * The result is stored in \p dest.
 *
 * @param src       The source image to convolute the kernel with.
 * @param dest      The destination image.
 * @param kernel    The convolution kernel.
 * @param channel   The color channel.
 */
void convolute(const Image *src, Image *dest, const Kernel *kernel, int channel);

/**
 * @brief Obtain a histogram from \p img of a specific color \p channel.
 *
 * @param src       The source image to obtain the histogram from.
 * @return          The histogram
 */
Histogram getHistogram(const Image *src);

/**
 * @brief Enhance the contrast of an image.
 *
 * This function linearly stretches the histogram in the horizontal direction, where the intensities between
 * the threshold..255-threshold are stretched to 0..255.
 * The result is stored in \p dest.
 *
 * @param src       The source image to enhance contrast for.
 * @param src_hist  The source image histogram.
 * @param dest      The destination image.
 * @param low       Threshold for lower intensities.
 * @param high      Threshold for higher intensities.
 * @param channel   Color channel
 */
void enhanceContrastLinearly(const Image *src, const Histogram *src_hist, Image *dest, int low, int high, int channel);

/**
 * @brief Apply a ripple effect to \p img.
 *
 * The result is stored in \p dest.
 *
 * @param src       The source image to convolute the kernel with.
 * @param dest      The destination image.
 * @param intensity The intensity of the lens effect.
 */
void applyRipple(const Image *src, Image *dest, float intensity);

/**
 * Copy a color channel from one image to the other.
 * @param src       The source image.
 * @param dest      The destination image.
 * @param channel   The color channel.
 */
void copyChannel(const Image *src, Image *dest, int channel);