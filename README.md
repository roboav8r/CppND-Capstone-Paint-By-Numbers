# Udacity C++ Nanodegree Capstone Repo: "Paint By Numbers"

This program segments an image into a smaller subset of colors in a paint-by-number style using OpenCV in C++. This is based off of the starter repo for the Capstone project in the [Udacity C++ Nanodegree Program](https://www.udacity.com/course/c-plus-plus-nanodegree--nd213).

## Dependencies for Running Locally
* cmake >= 3.7
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./PaintByNumbers `.

Also, try these combinations!
`./PaintByNumbers ../data/apache.jpg True 0.3 15 35 3 0.25 0 0.01 0`
`./PaintByNumbers ../data/blackhawk.jpg True 1.5 15 35 3 1.5 50 0.0275 0`
`./PaintByNumbers ../data/dog.jpg True 0.1 21 21 3 1.25 0 0.005 0`
`./PaintByNumbers ../data/f14.jpg True 1.0 21 15 1 1.5 0 0.01 1`
`./PaintByNumbers ../data/yoda.jpeg True 0.75 30 30 3 1.75 0 0.03 1`




# Rubric 
https://review.udacity.com/#!/rubrics/2533/view

# Credit
Based off of the image segmentation tutorial here:
https://docs.openvino.ai/latest/omz_demos_segmentation_demo_cpp.html#doxid-omz-demos-segmentation-demo-cpp

And OpenCV tutorials here:

Semantic Segmentation:
https://docs.opencv.org/4.2.0/d4/d88/samples_2dnn_2segmentation_8cpp-example.html
    // Subtract background
    // https://learnopencv.com/pytorch-for-beginners-semantic-segmentation-using-torchvision
    // Adapted from https://learnopencv.com/applications-of-foreground-background-separation-with-semantic-segmentation/
    // https://docs.openvino.ai/latest/omz_tools_downloader.html#doxid-omz-tools-downloader