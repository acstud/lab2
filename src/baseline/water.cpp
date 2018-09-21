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

#include "../utils/Timer.hpp"
#include "../utils/Histogram.hpp"

#include "imgproc.hpp"

#include "water.hpp"

/// @brief Run the histogram stage.
std::shared_ptr<Histogram> runHistogramStage(const Image *previous, const WaterEffectOptions *options) {
  // Obtain the histogram
  auto hist = std::make_shared<Histogram>(getHistogram(previous));

  // Optionally save the intermediate histogram as an image
  if (options->save_intermediate) {
    auto hist_img = hist->toImage();
    hist_img->toPNG("output/" + options->img_name + "_histogram.png");
  }
  return hist;
}

/// @brief Run the contrast enhancement stage.
std::shared_ptr<Image> runEnhanceStage(const Image *previous,
                                       const Histogram *hist,
                                       const WaterEffectOptions *options) {
  // Check if histogram stage has been properly executed.
  if ((!options->histogram) || (hist == nullptr)) {
    throw std::runtime_error("Contrast enhancement is only possible when histogram stage has been performed.");
  }

  // Determine the threshold from the histogram, by taking 10% of the maximum value in the histogram.
  auto threshold = (int) (hist->max(0) * 0.1);

  // Create a new image to store the result
  auto img_enhanced = std::make_shared<Image>(previous->width, previous->height);

  // Enhance the contrast on the color channels
  enhanceContrastLinearly(previous, hist, img_enhanced.get(), threshold, threshold, 0);
  enhanceContrastLinearly(previous, hist, img_enhanced.get(), threshold, threshold, 1);
  enhanceContrastLinearly(previous, hist, img_enhanced.get(), threshold, threshold, 2);

  // Copy over the alpha channel
  copyChannel(previous, img_enhanced.get(), 3);

  // Save the resulting image
  if (options->save_intermediate)
    img_enhanced->toPNG("output/" + options->img_name + "_enhanced.png");

  // Set the previous image to point to the enhanced image for next stages.
  previous = img_enhanced.get();

  // Create and save the enhanced histogram (if enabled).
  if (options->enhance_hist) {
    auto enhanced_hist = getHistogram(img_enhanced.get());
    auto enhanced_hist_img = enhanced_hist.toImage();
    enhanced_hist_img->toPNG("output/" + options->img_name + "_enhanced_histogram.png");
  }

  return img_enhanced;
}

/// @brief Run the ripple effect stage.
std::shared_ptr<Image> runRippleStage(const Image *previous, const WaterEffectOptions *options) {
  // Create a new image to store the result
  auto img_rippled = std::make_shared<Image>(previous->width, previous->height);

  // Apply the ripple effect
  applyRipple(previous, img_rippled.get(), options->ripple_frequency);

  // Save the resulting image
  if (options->save_intermediate)
    img_rippled->toPNG("output/" + options->img_name + "_rippled.png");

  return img_rippled;
}

/// @brief Run the blur stage.
std::shared_ptr<Image> runBlurStage(const Image *previous, const WaterEffectOptions *options) {
  // Create a Gaussian convolution kernel
  Kernel gaussian = Kernel::gaussian(options->blur_size, options->blur_size, 1.0);

  // Create a new image to store the result
  auto img_blurred = std::make_shared<Image>(previous->width, previous->height);

  // Blur every channel using the gaussian kernel
  convolute(previous, img_blurred.get(), &gaussian, 0);
  convolute(previous, img_blurred.get(), &gaussian, 1);
  convolute(previous, img_blurred.get(), &gaussian, 2);
  convolute(previous, img_blurred.get(), &gaussian, 3);

  // Save the resulting image
  if (options->save_intermediate)
    img_blurred->toPNG("output/" + options->img_name + "_blurred.png");

  return img_blurred;
}

std::shared_ptr<Image> runWaterEffect(const Image *src, const WaterEffectOptions *options) {
  // Stage timer
  Timer ts;

  // Smart pointers to intermediate images:
  std::shared_ptr<Histogram> hist;
  std::shared_ptr<Image> img_result;

  // Histogram stage
  if (options->histogram) {
    ts.start();
    hist = runHistogramStage(src, options);
    ts.stop();
    std::cout << "Stage: Histogram:        " << ts.seconds() << " s." << std::endl;
  }

  // Contrast enhancement stage
  if (options->enhance) {
    ts.start();
    if (hist == nullptr) {
      throw std::runtime_error("Cannot run enhance stage without histogram.");
    }
    img_result = runEnhanceStage(src, hist.get(), options);
    ts.stop();
    std::cout << "Stage: Contrast enhance: " << ts.seconds() << " s." << std::endl;
  }

  // Ripple effect stage
  if (options->ripple) {
    ts.start();
    if (img_result == nullptr) {
      img_result = runRippleStage(src, options);
    } else {
      img_result = runRippleStage(img_result.get(), options);
    }
    ts.stop();
    std::cout << "Stage: Ripple effect:    " << ts.seconds() << " s." << std::endl;
  }

  // Gaussian blur stage
  if (options->blur) {
    ts.start();
    if (img_result == nullptr) {
      img_result = runBlurStage(src, options);
    } else {
      img_result = runBlurStage(img_result.get(), options);
    }
    ts.stop();
    std::cout << "Stage: Blur:             " << ts.seconds() << " s." << std::endl;
  }

  return img_result;
}
