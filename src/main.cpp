#include <iostream>
// #include <filesystem>

#include <opencv2/core.hpp>
// #include <opencv2/core/utility.hpp>
// #include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


// Keys for command-line input values
std::string keys = 
    "{@input | ../data/dog1.jpg | input image file}"
    "{@displayIntermediate | 1 | Boolean true/false to show intermediate image processing steps}"
    "{@scale | 0.1 | input image scale}"
    "{@spatialWindow | 21 | Spatial window for Mean Shift filtering}"
    "{@colorWindow | 51 | Color window for Mean Shift filtering}"
    "{@blurWidth | 5 | Width of gaussian blur filter}"
    "{@sharpCoeff | 1.0 | Sharpening coefficient}"
    "{@binThresh | 40 | Binary threshold for grayscale computation}"
    "{@peakThresh | 0.4 | Floating point threshold [0, 1.0) for peak finding}"
    "{@dilateWidth | 3 | Integer width to dilate the peaks }";

// Member variables
cv::Mat srcImg, scaledImg;

// Methods

// Display an OpenCV matrix image, and destroy on any key press
void showImage(cv::Mat img, std::string name) {
    cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
    cv::imshow(name, img); // Show our image inside the created window.

    // Cleanup
    cv::waitKey(0); // Wait for any keystroke in the window
    cv::destroyWindow(name); //destroy the created window
}

// Load an image from file into an OpenCV matrix
void loadImage(const std::string& imgFile, cv::Mat& inputImg, const bool display)
{   
    std::cout << "Loading file: " << imgFile << std::endl;
    inputImg = cv::imread( imgFile );
    if( inputImg.empty() )
    {
        std::cout << "Could not open or find the image!\n" << std::endl;
    };

    // Show the initial image
    if (display) { showImage(inputImg, "Initial Image");};
}

// Scale the input image by a scaling factor
void scaleImage(const cv::Mat& inputImg, const float scaleFactor, cv::Mat& outputImg, const bool display)
{
    int heightScaled{(int)(inputImg.rows*scaleFactor)};
    int widthScaled{(int)(inputImg.cols*scaleFactor)};
    std::cout << "Resizing source image to: " << heightScaled << "x" << widthScaled << " pixels" << std::endl;
    cv::resize(inputImg, outputImg, cv::Size(widthScaled, heightScaled), cv::INTER_LINEAR);
    
    // Show the scaled image
    if (display) { showImage(outputImg, "Scaled Image");};
}

// Main program
int main(int argc, char *argv[]) {

    // Process user inputs
    cv::CommandLineParser parser( argc, argv, keys );
    float scale = parser.get<float>( "@scale" );
    bool displayIntermediate = parser.get<bool>( "@displayIntermediate" );
    std::cout << "Input image rescaling factor: " << scale << std::endl;
    displayIntermediate ? std::cout << "Displaying intermediate images" << std::endl : std::cout << "Not displaying intermediate images" << std::endl;

    // Load the image from file
    loadImage(parser.get<cv::String>( "@input" ), srcImg, displayIntermediate);

    // SCALE the image for easier visibility
    scaleImage(srcImg, scale, scaledImg, displayIntermediate);
    // int heightScaled{(int)(srcImg.rows*scale)};
    // int widthScaled{(int)(srcImg.cols*scale)};
    // std::cout << "Resizing source image to: " << heightScaled << "x" << widthScaled << std::endl;
    // cv::Mat scaledImg;
    // cv::resize(srcImg, scaledImg, cv::Size(widthScaled, heightScaled), cv::INTER_LINEAR);
    
    // // Show the scaled image
    // std::string scaledName{"Scaled Image"};
    // cv::namedWindow(scaledName);
    // cv::imshow(scaledName, scaledImg); // Show our image inside the created window.

    // // Cleanup
    // cv::waitKey(0); // Wait for any keystroke in the window
    // cv::destroyWindow(scaledName); //destroy the created window


    // FILTERING the image
    cv::Mat filteredImg;
    int spatialWindow = parser.get<int>( "@spatialWindow" );
    int colorWindow = parser.get<int>( "@colorWindow" );
    std::cout << "Filtering the scaled image with spatial window: " << spatialWindow << " pixels, color window: " << colorWindow << " pixels" << std::endl;
    cv::pyrMeanShiftFiltering(scaledImg,filteredImg,spatialWindow,colorWindow);
    
    // Display the filtered image
    std::string filteredName{"Mean Shift Filtered"};
    cv::namedWindow(filteredName);
    cv::imshow(filteredName, filteredImg); // Show our image inside the created window.
    cv::waitKey(0); // Wait for any keystroke in the window
    cv::destroyWindow(filteredName); //destroy the created window


    // SMOOTHING the image
    int smoothingKernelDim = parser.get<int>( "@blurWidth" );
    std::cout << "Smoothing using a Gaussian blur of " << smoothingKernelDim << " pixels" << std::endl;
    cv::Mat smoothedImg;
    cv::GaussianBlur(filteredImg,smoothedImg,cv::Size(smoothingKernelDim,smoothingKernelDim),0,0);
    
    // Display the smoothed image
    std::string smoothedName{"Smoothed"};
    cv::namedWindow(smoothedName);
    cv::imshow(smoothedName, smoothedImg); // Show our image inside the created window.
    cv::waitKey(0); // Wait for any keystroke in the window
    cv::destroyWindow(smoothedName); //destroy the created window


    // SHARPEN the image
    float sharpCoeff = parser.get<float>( "@sharpCoeff" );
    cv::Mat kernel = (cv::Mat_<float>(3,3) <<
                  sharpCoeff,  sharpCoeff, sharpCoeff,
                  sharpCoeff, -8*sharpCoeff, sharpCoeff,
                  sharpCoeff,  sharpCoeff, sharpCoeff);
    std::cout << "Sharpening with kernel: " << std::endl;
    std::cout << kernel << std::endl;
    cv::Mat laplacianImg;
    cv::filter2D(smoothedImg, laplacianImg, CV_32F, kernel);
    cv::Mat sharpImg;
    smoothedImg.convertTo(sharpImg, CV_32F);
    cv::Mat resultImg = sharpImg - laplacianImg;
    resultImg.convertTo(resultImg, CV_8UC3);
    laplacianImg.convertTo(laplacianImg, CV_8UC3);

    // Display the sharpened image
    std::string sharpName{"Sharpened Image"};
    cv::namedWindow(sharpName);
    cv::imshow(sharpName, resultImg); // Show our image inside the created window.
    cv::waitKey(0); // Wait for any keystroke in the window
    cv::destroyWindow(sharpName); //destroy the created window


    // Generate grayscale from filtered image
    cv::Mat grayImg;
    cv::cvtColor(resultImg, grayImg, cv::COLOR_BGR2GRAY);
    int binaryThreshold = parser.get<int>( "@binThresh" );
    std::cout << "Converting to binary image with 8-bit threshold " << binaryThreshold << std::endl;
    cv::threshold(grayImg, grayImg, binaryThreshold, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    imshow("Grayscale Image", grayImg);
    cv::waitKey(0); // Wait for any keystroke in the window
    cv::destroyWindow("Grayscale Image"); //destroy the created window


    // Compute distance image
    cv::Mat distImg;
    cv::distanceTransform(grayImg,distImg,cv::DIST_L2,5);
    cv::normalize(distImg, distImg, 0, 1.0, cv::NORM_MINMAX);
    imshow("Distance Transform Image", distImg);
    cv::waitKey(0); // Wait for any keystroke in the window
    cv::destroyWindow("Distance Transform Image"); //destroy the created window


    // Find peaks of the distance image via thresholding and dilation
    float peakThresh = parser.get<float>( "@peakThresh" );
    int dilateWidth = parser.get<int>( "@dilateWidth" );
    std::cout << "Finding peaks in Distance Image using Peak Threshold: " << peakThresh << ", Dilation width: " << dilateWidth << std::endl;
    cv::threshold(distImg, distImg, peakThresh, 1.0, cv::THRESH_BINARY);
    cv::Mat kernelImg = cv::Mat::ones(dilateWidth, dilateWidth, CV_8U);
    cv::dilate(distImg, distImg, kernelImg);
    imshow("Peaks", distImg);
    cv::waitKey(0); // Wait for any keystroke in the window
    cv::destroyWindow("Peaks"); //destroy the created window

    // Find markers
    // Create the CV_8U version of the distance image
    // It is needed for findContours()
    cv::Mat distImg_8u;
    distImg.convertTo(distImg_8u, CV_8U);
    // Find total markers
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(distImg_8u, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    // Create the marker image for the watershed algorithm
    cv::Mat markers = cv::Mat::zeros(distImg.size(), CV_32S);
    // Draw the foreground markers
    for (size_t i = 0; i < contours.size(); i++)
    {
        drawContours(markers, contours, static_cast<int>(i), cv::Scalar(static_cast<int>(i)+1), -1);
    }
    // Draw the background marker
    cv::circle(markers, cv::Point(5,5), 3, cv::Scalar(255), -1);
    cv::Mat markers8u;
    markers.convertTo(markers8u, CV_8U, 10);
    imshow("Markers", markers8u);
    cv::waitKey(0); // Wait for any keystroke in the window
    cv::destroyWindow("Markers"); //destroy the created window


    // Perform the watershed algorithm
    cv::watershed(filteredImg, markers);
    cv::Mat mark;
    markers.convertTo(mark, CV_8U);
    bitwise_not(mark, mark);
    imshow("Markers_v2", mark); // uncomment this if you want to see how the mark image looks like at that point
    cv::waitKey(0); // Wait for any keystroke in the window
    cv::destroyWindow("Markers_v2"); //destroy the created window

    // Generate random colors
    std::vector<cv::Vec3b> colors;
    for (size_t i = 0; i < contours.size(); i++)
    {
        int b = cv::theRNG().uniform(0, 256);
        int g = cv::theRNG().uniform(0, 256);
        int r = cv::theRNG().uniform(0, 256);
        colors.push_back(cv::Vec3b((uchar)b, (uchar)g, (uchar)r));
    }
    // Create the final image
    cv::Mat dst = cv::Mat::zeros(markers.size(), CV_8UC3);
    // Fill labeled objects with random colors
    for (int i = 0; i < markers.rows; i++)
    {
        for (int j = 0; j < markers.cols; j++)
        {
            int index = markers.at<int>(i,j);
            if (index > 0 && index <= static_cast<int>(contours.size()))
            {
                dst.at<cv::Vec3b>(i,j) = colors[index-1];
            }
        }
    }
    // Visualize the final image
    imshow("Final Result", dst);
    cv::waitKey(0); // Wait for any keystroke in the window
    cv::destroyWindow("Final Result"); //destroy the created window

    return 0;
}