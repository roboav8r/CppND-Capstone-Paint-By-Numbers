#include <iostream>

#include "watershed_segmentation.h"

// Keys for command-line input values
std::string keys = 
    "{@input | ../data/dog.jpg | input image file}"
    "{@displayIntermediate | 0 | Boolean true/false to show intermediate image processing steps}"
    "{@scale | 0.1 | input image scale}"
    "{@spatialWindow | 21 | Spatial window for Mean Shift filtering}"
    "{@colorWindow | 21 | Color window for Mean Shift filtering}"
    "{@blurWidth | 3 | Width of gaussian blur filter}"
    "{@sharpCoeff | 1.25 | Sharpening coefficient}"
    "{@binThresh | 0 | Binary threshold for grayscale computation}"
    "{@peakThresh | 0.005 | Floating point threshold [0, 1.0) for peak finding}"
    "{@dilateWidth | 1 | Integer width to dilate the peaks }";


// Main program
int main(int argc, char *argv[]) {

    // Process user inputs
    cv::CommandLineParser parser( argc, argv, keys );
    float scale = parser.get<float>( "@scale" );
    bool displayIntermediate = parser.get<bool>( "@displayIntermediate" );
    std::cout << "Input image rescaling factor: " << scale << std::endl;
    displayIntermediate ? std::cout << "Displaying intermediate images" << std::endl : std::cout << "Not displaying intermediate images" << std::endl;


    // Create Watershed image segmenter object with user inputs
    Segmentation::Watershed watershed( 
        displayIntermediate, 
        parser.get<cv::String>( "@input" ), 
        parser.get<float>( "@scale" ),
        parser.get<int>( "@spatialWindow" ), 
        parser.get<int>( "@colorWindow" ),
        parser.get<int>( "@blurWidth" ),
        parser.get<float>( "@sharpCoeff" ),
        parser.get<int>( "@binThresh" ),
        parser.get<float>( "@peakThresh" ),
        parser.get<int>( "@dilateWidth" )
    );

    // Process the image
    watershed.scaleImage();
    watershed.filterImage();
    watershed.smoothImage();
    watershed.sharpenImage();
    watershed.distanceTransform();
    watershed.findPeaks();
    watershed.findMarkers();
    watershed.findWatershedMarkers();
    watershed.getSegmentColors();

    // Generate, save, and display the final "Paint-by-numbers" image
    watershed.finalImage();

    return 0;
}