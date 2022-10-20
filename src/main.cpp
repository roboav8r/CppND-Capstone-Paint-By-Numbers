#include <iostream>
#include <filesystem>

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


int main(int argc, char *argv[]) {


    // Load the image
    cv::CommandLineParser parser( argc, argv, "{@input | ../data/dog1.jpg | input image}" );
    cv::Mat src = cv::imread( parser.get<cv::String>( "@input" ) );
    if( src.empty() )
    {
        std::cout << "Could not open or find the image!\n" << std::endl;
        std::cout << "Usage: " << argv[0] << " <Input image>" << std::endl;
        return -1;
    }

    // Show the initial image
    std::string srcName{"Initial Image"};
    cv::namedWindow(srcName);
    cv::imshow(srcName, src); // Show our image inside the created window.

    // Cleanup
    cv::waitKey(0); // Wait for any keystroke in the window
    cv::destroyWindow(srcName); //destroy the created window


    // SCALE the image for easier visibility
    int heightScaled{400}; // TODO make this an input
    int widthScaled{heightScaled*src.cols/src.rows};
    cv::Mat scaled;
    cv::resize(src, scaled, cv::Size(widthScaled, heightScaled), cv::INTER_LINEAR);
    
    // Show the scaled image
    std::string scaledName{"Scaled Image"};
    cv::namedWindow(scaledName);
    cv::imshow(scaledName, scaled); // Show our image inside the created window.

    // Cleanup
    cv::waitKey(0); // Wait for any keystroke in the window
    cv::destroyWindow(scaledName); //destroy the created window


    // SMOOTHING the image
    cv::Mat kernel = (cv::Mat_<float>(3,3) <<
                  1,  2, 1,
                  2, -12, 2,
                  1,  2, 1); 
    cv::Mat imgLaplacian;
    cv::filter2D(scaled, imgLaplacian, CV_32F, kernel);
    
    // Display the laplacian
    std::string laplaceName{"Laplacian"};
    cv::namedWindow(laplaceName);
    cv::imshow(laplaceName, imgLaplacian); // Show our image inside the created window.
    cv::waitKey(0); // Wait for any keystroke in the window
    cv::destroyWindow(laplaceName); //destroy the created window
    
    // Apply laplace filter to source
    cv::Mat smooth;
    scaled.convertTo(smooth, CV_32F);
    cv::Mat smoothed = smooth - imgLaplacian;
    
    // Display the smoothed image
    std::string smoothedName{"Smoothed"};
    cv::namedWindow(smoothedName);
    cv::imshow(smoothedName, smoothed); // Show our image inside the created window.
    cv::waitKey(0); // Wait for any keystroke in the window
    cv::destroyWindow(smoothedName); //destroy the created window


    // Subtract background
    // https://learnopencv.com/pytorch-for-beginners-semantic-segmentation-using-torchvision
    // Adapted from https://learnopencv.com/applications-of-foreground-background-separation-with-semantic-segmentation/
    // https://docs.openvino.ai/latest/omz_tools_downloader.html#doxid-omz-tools-downloader


    return 0;
}