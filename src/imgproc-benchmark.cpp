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

#include <iostream>
#include <getopt.h>

#include "utils/Image.hpp"
#include "utils/Kernel.hpp"
#include "utils/Timer.hpp"

#include "baseline/water.hpp"

#ifdef USE_CUDA
#include "students/water_cuda.hpp"
#endif

/// @brief Structure to pass program options
struct ProgramOptions {
  std::string input_file = "";
  bool cuda = false;
  bool test = false;
  WaterEffectOptions water_opts;

  /// @brief Print usage information
  static void usage(char *argv[]) {
    std::cerr << "Usage: " << argv[0] << "-hanmeifc -g G -r R <image.png>\n"
              << "Options:\n"
                 "  -h    Show help.\n"
                 "\n"
                 "Image processing function selection: \n"
                 "  -g G  Gaussian blur with kernel size GxG.\n"
                 "  -m    Histogram.\n"
                 "  -e    Contrast enhancement (enables histogram).\n"
                 "  -n    Output contrast enhanced histogram as image (unaffected by -i).\n"
                 "  -r R  Ripple effect with frequency R.\n"
                 "\n"
                 "  -i    Save intermediate images.\n"
                 "  -f    Run full baseline pipeline.\n"
                 "\n"
                 "  -c    Run full pipeline using CUDA.\n"
                 "  -a    Run full baseline & CUDA pipeline, compare output.\n";

    std::cerr.flush();
    exit(0);
  }

  /// @brief Run everything selected through the options.
  void run() {
    // Load the image.
    auto img = Image::fromPNG(input_file);
    std::shared_ptr<Image> img_baseline_result;

    Timer tt;

    // Start the total pipeline measurement.
    tt.start();
    img_baseline_result = runWaterEffect(img.get(), &water_opts);
    // Stop the timer for the baseline pipeline.
    tt.stop();
    std::cout << "Full pipeline (baseline): " << tt.seconds() << " s." << std::endl;


    // Save the final result if any image was produced
    if (water_opts.enhance || water_opts.blur || water_opts.ripple) {
      img_baseline_result->toPNG("output/" + water_opts.img_name + "_result.png");
    }

    // Run the whole pipeline using CUDA
    if (cuda) {
#ifdef USE_CUDA
      tt.start();
      auto img_cuda_result = runWaterEffectCUDA(img.get(), &water_opts);
      tt.stop();
      std::cout << "Full pipeline (CUDA):     " << tt.seconds() << " s." << std::endl;

      if (img_cuda_result != nullptr) {
        img_cuda_result->toPNG("output/" + water_opts.img_name + "_result.png");
      } else {
        std::cerr << "CUDA water effect returned nullptr. Cannot output PNG." << std::endl;
      }

      // Compare the CUDA implementation to the baseline if testing is enabled
      if (test) {
        if (img_cuda_result != nullptr) {
          if (img_cuda_result->is_approximately_equal_to(img_baseline_result.get())) {
            std::cout << "Test passed." << std::endl;
          } else {
            std::cout << "Test failed." << std::endl;
          }
        } else {
          std::cerr << "CUDA water effect returned nullptr. Cannot test approximate equality." << std::endl;
        }
      }
#else
      std::cout << "Project was built without CUDA support.\n"
                   "Skipping CUDA pipeline and test." << std::endl;
#endif
    }
  }
};

int main(int argc, char *argv[]) {

  ProgramOptions po;

  if (argc == 1) {
    ProgramOptions::usage(argv);
  }

  // Use GNU getopt to parse command line options
  int opt;
  while ((opt = getopt(argc, argv, "hg:menfir:ac")) != -1) {
    switch (opt) {

      case 'h': {
        ProgramOptions::usage(argv);
        break;
      }

      case 'c': {
        po.cuda = true;
        break;
      }

      case 'g': {
        char *end;
        po.water_opts.blur_size = (unsigned int) std::strtol(optarg, &end, 10);
        po.water_opts.blur = true;
        break;
      }

      case 'm':po.water_opts.histogram = true;
        break;

      case 'e': {
        po.water_opts.enhance = true;
        po.water_opts.histogram = true;
        break;
      }

      case 'n':po.water_opts.enhance_hist = true;
        break;

      case 'f': {
        po.water_opts.blur = true;
        po.water_opts.histogram = true;
        po.water_opts.enhance = true;
        po.water_opts.ripple = true;
        break;
      }

      case 'i':po.water_opts.save_intermediate = true;
        break;

      case 'r': {
        char *end;
        po.water_opts.ripple_frequency = (unsigned int) std::strtof(optarg, &end);
        po.water_opts.ripple = true;
        break;
      }

      case 'a': {
        po.water_opts.blur = true;
        po.water_opts.histogram = true;
        po.water_opts.enhance = true;
        po.water_opts.ripple = true;
        po.test = true;
        po.cuda = true;
        break;
      }

      case '?':
        if ((optopt == 'g') || (optopt == 'r')) {
          std::cerr << "Options -g and -r require an argument." << std::endl;
          ProgramOptions::usage(argv);
        }
        break;

      default:ProgramOptions::usage(argv);
        break;
    }
  }

  // Check if last argument is a filename.
  if (argv[argc - 1][0] != '-') {
    // File name is last argument.
    po.input_file = std::string(argv[argc - 1]);
    // Strip any path
    po.water_opts.img_name = po.input_file.substr(po.input_file.find_last_of("\\/") + 1);
    // Strip the extension
    po.water_opts.img_name = po.water_opts.img_name.substr(0, po.water_opts.img_name.find_last_of('.'));
  } else {
    ProgramOptions::usage(argv);
  }

  // Create an output directory, if it doesn't already exist.
  // This only works for cool operating systems.
  if (system("mkdir -p output") != 0) {
    throw std::runtime_error("Could not create output directory.");
  }

  // Run everything
  po.run();

  return 0;
}
