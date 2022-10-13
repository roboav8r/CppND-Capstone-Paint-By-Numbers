#include <iostream>
#include <filesystem>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

int main() {

    // Define input parameters
    //std::string directory = std::filesystem::current_path();
    std::string image_path = "~/udacity_cpp_developer/CppND-Capstone-Paint-By-Numbers/data/dog1.jpg";
    std::cout << "Checking for file at " << image_path << std::endl;
    //std::cout << "Checking for file at " << std::filesystem::current_path() << std::endl;

    // Import the image
    cv::Mat image = cv::imread("../data/dog1.jpg");

    // Show the initial image
    // std::string initName{"Initial Image"};
    // cv::namedWindow(initName);
    // cv::imshow(initName, image); // Show our image inside the created window.
    
    // // Cleanup
    // cv::waitKey(0); // Wait for any keystroke in the window
    // cv::destroyWindow(initName); //destroy the created window


    // Subtract background
    // https://learnopencv.com/pytorch-for-beginners-semantic-segmentation-using-torchvision
    // Adapted from https://learnopencv.com/applications-of-foreground-background-separation-with-semantic-segmentation/
    // https://docs.openvino.ai/latest/omz_tools_downloader.html#doxid-omz-tools-downloader


    return 0;
}