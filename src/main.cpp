#include <iostream>

#include <opencv2/core.hpp>
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
cv::Mat srcImg, scaledImg, filteredImg, smoothedImg, sharperImg, distImg, peakImg, markers, markerImg, watershedMarkerImg, finalImg;
std::vector<std::vector<cv::Point>> colorContours;
std::vector<cv::Vec3b> colors;

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

// Mean-shift filter the image
void filterImage(const cv::Mat& inputImg, const int sw, const int cw, cv::Mat& outputImg, const bool display)
{
    std::cout << "Filtering image with spatial window: " << sw << " pixels, color window: " << cw << " pixels" << std::endl;
    cv::pyrMeanShiftFiltering(inputImg,outputImg,sw,cw);
    
    // Display the filtered image
    if (display) { showImage(outputImg, "Mean-Shift Filtered Image");}
}

// Gaussian Blur/smooth the image
void smoothImage(const cv::Mat& inputImg, const int blurDim, cv::Mat& outputImg, const bool display)
{
    std::cout << "Smoothing using a Gaussian blur of " << blurDim << " pixels" << std::endl;
    cv::GaussianBlur(inputImg,outputImg,cv::Size(blurDim,blurDim),0,0);
    
    // Display the smoothed image
    if (display) { showImage(outputImg, "Smoothed Image");}
}

// Sharpen the image
void sharpenImage(const cv::Mat& inputImg, const float sc, cv::Mat& outputImg, const bool display)
{
    cv::Mat laplacianImg, sharpImg;
    cv::Mat kernel = (cv::Mat_<float>(3,3) <<
                  sc,  sc, sc,
                  sc, -8*sc, sc,
                  sc,  sc, sc);
    std::cout << "Sharpening image using kernel: " << std::endl;
    std::cout << kernel << std::endl;
    cv::filter2D(inputImg, laplacianImg, CV_32F, kernel);

    inputImg.convertTo(sharpImg, CV_32F);
    outputImg = sharpImg - laplacianImg;
    outputImg.convertTo(outputImg, CV_8UC3);
    laplacianImg.convertTo(laplacianImg, CV_8UC3);

    // Display the sharpened image
    if (display) { showImage(outputImg, "Sharpened Image");}
}

// Compute distance transform
void distanceTransform(const cv::Mat& inputImg, const int threshold, cv::Mat& outputImg, const bool display)
{
    cv::Mat grayImg;
    cv::cvtColor(inputImg, grayImg, cv::COLOR_BGR2GRAY);
    std::cout << "Converting to binary image with 8-bit threshold " << threshold << std::endl;
    cv::threshold(grayImg, grayImg, threshold, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    if (display) { showImage(grayImg, "Grayscale Image");}

    // Compute distance image
    cv::distanceTransform(grayImg,outputImg,cv::DIST_L2, 3); // Potential improvement: the final mask parameter can be an input variable
    cv::normalize(outputImg, outputImg, 0, 1.0, cv::NORM_MINMAX);
    if (display) { showImage(outputImg, "Distance Transform Image");}
}

// Find peaks of the transformed image
void findPeaks(const cv::Mat& inputImg, const float thresh, const int dilWid, cv::Mat& outputImg, const bool display)
{
    std::cout << "Finding peaks in Distance Image using Peak Threshold: " << thresh << ", Dilation width: " << dilWid << std::endl;
    cv::threshold(inputImg, inputImg, thresh, 1.0, cv::THRESH_BINARY);
    cv::Mat kernelImg = cv::Mat::ones(dilWid, dilWid, CV_8U);
    cv::dilate(inputImg, outputImg, kernelImg);
    if (display) { showImage(outputImg, "Peaks");}
}

// Find markers of each individual segment
void findMarkers(const cv::Mat& inputImg, std::vector<std::vector<cv::Point>>& contours, cv::Mat& markers, cv::Mat& outputImg, const bool display)
{
    // Create the CV_8U version of the distance image
    // It is needed for findContours()
    cv::Mat distImg_8u;
    inputImg.convertTo(distImg_8u, CV_8U);
    
    // Find total markers
    cv::findContours(distImg_8u, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    markers = cv::Mat::zeros(distImg.size(), CV_32S);
    // Draw the foreground markers
    for (size_t i = 0; i < contours.size(); i++)
    {
        drawContours(markers, contours, static_cast<int>(i), cv::Scalar(static_cast<int>(i)+1), -1);
    }
    // Draw the background marker
    cv::circle(markers, cv::Point(5,5), 3, cv::Scalar(255), -1);
    //cv::Mat markers8u;
    markers.convertTo(outputImg, CV_8U, 10);
    if (display) { showImage(outputImg, "Markers");}
}

void findWatershedMarkers(const cv::Mat& inputImg, cv::Mat& markers, cv::Mat& outputImg, const bool display)
{
    cv::watershed(inputImg, markers);
    //cv::Mat outputImg;
    markers.convertTo(outputImg, CV_8U);
    bitwise_not(outputImg, outputImg);

    if (display) { showImage(outputImg, "Watershed Markers");}
}

void getRandomColors(const std::vector<std::vector<cv::Point>> contours, std::vector<cv::Vec3b>& colors)
{

    for (size_t i = 0; i < contours.size(); i++)
    {
        int b = cv::theRNG().uniform(0, 256);
        int g = cv::theRNG().uniform(0, 256);
        int r = cv::theRNG().uniform(0, 256);
        colors.push_back(cv::Vec3b((uchar)b, (uchar)g, (uchar)r));
    }
}

void getSegmentColors(const cv::Mat& inputImg, const cv::Mat& markers, const std::vector<std::vector<cv::Point>> contours, std::vector<cv::Vec3b>& colors)
{
    int numSegments = contours.size();
    std::vector<int> numSegmentPixels(numSegments,1);
    std::vector<std::vector<long unsigned int>> segmentRgbSums(numSegments,std::vector<long unsigned int>(3,0));

    // Iterate through the input image
    for (int ii = 0; ii < inputImg.rows; ii++)
    {
        for (int jj = 0; jj < inputImg.cols; jj++)
        {
            int index = markers.at<int>(ii,jj);
            if (index > 0 && index <= static_cast<int>(colorContours.size()))
            {
                segmentRgbSums[index-1][0] = segmentRgbSums[index-1][0] + (inputImg.at<cv::Vec3b>(ii,jj)[0]);
                segmentRgbSums[index-1][1] = segmentRgbSums[index-1][1] + (inputImg.at<cv::Vec3b>(ii,jj)[1]);
                segmentRgbSums[index-1][2] = segmentRgbSums[index-1][2] + (inputImg.at<cv::Vec3b>(ii,jj)[2]);
                numSegmentPixels[index-1] +=1; //increment segment pixel count 
            }
        }

        // Now, compute average of each segment's RGB values and assign to color vector
        for (int kk = 0; kk < numSegments; kk++) {
            colors[kk][0] = segmentRgbSums[kk][0] / numSegmentPixels[kk];
            colors[kk][1] = segmentRgbSums[kk][1] / numSegmentPixels[kk];
            colors[kk][2] = segmentRgbSums[kk][2] / numSegmentPixels[kk];
        }
    }
}

// Find peaks of the transformed image
void finalImage( const cv::Mat& markers, const std::vector<std::vector<cv::Point>> contours, std::vector<cv::Vec3b>& colors, cv::Mat& outputImg)
{
    outputImg = cv::Mat::zeros(markers.size(), CV_8UC3);
    // Fill labeled segments with colors
    for (int i = 0; i < markers.rows; i++)
    {
        for (int j = 0; j < markers.cols; j++)
        {
            int index = markers.at<int>(i,j);
            if (index > 0 && index <= static_cast<int>(contours.size()))
            {
                outputImg.at<cv::Vec3b>(i,j) = colors[index-1];
            }
        }
    }
    // Save and visualize the final image
    cv::imwrite("../results/out.jpg", outputImg);
    showImage(outputImg, "Final Image");
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

    // Scale the image
    scaleImage(srcImg, scale, scaledImg, displayIntermediate);

    // Filter the image
    filterImage(scaledImg, parser.get<int>( "@spatialWindow" ), parser.get<int>( "@colorWindow" ), filteredImg, displayIntermediate);

    // Smooth/blur the image
    smoothImage(filteredImg, parser.get<int>( "@blurWidth" ), smoothedImg, displayIntermediate);

    // SHARPEN the image
    sharpenImage(smoothedImg, parser.get<float>( "@sharpCoeff" ), sharperImg, displayIntermediate);

    // Generate distance transform
    distanceTransform(sharperImg, parser.get<int>( "@binThresh" ), distImg, displayIntermediate);

    // Find peaks of the distance image via thresholding and dilation
    findPeaks(distImg, parser.get<float>( "@peakThresh" ), parser.get<int>( "@dilateWidth" ), peakImg, displayIntermediate);

    // Find markers
    findMarkers(distImg, colorContours, markers, markerImg, displayIntermediate);

    // Perform the watershed algorithm
    findWatershedMarkers(sharperImg, markers, watershedMarkerImg, displayIntermediate);

    // Generate random colors
    getRandomColors(colorContours, colors);

    // Generate true colors
    getSegmentColors(sharperImg, markers, colorContours, colors);

    // Create the final image
    finalImage(markers, colorContours, colors, finalImg);

    return 0;
}