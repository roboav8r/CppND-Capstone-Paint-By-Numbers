#ifndef WATERSHED_SEGMENTATION_H
#define WATERSHED_SEGMENTATION_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

namespace Segmentation {

    /*
    WATERSHED SEGMENTER
    This class accepts an input image and a set of processing parameters. 
    Then, it performs a series of transformations in order to generate a "Paint-by-Numbers" styled image from the original.
    
    See documentation/tutorial here: https://docs.opencv.org/4.x/d2/dbd/tutorial_distance_transform.html
    https://anothertechs.com/programming/cpp/opencv/opencv-watershed/
    */
    class Watershed {
        public:
            /* 
            CONSTRUCTORS
            */
            
            Watershed( bool display_int, std::string filename, float scale, int spatial_window, int color_window, int gauss_blur_width, float sharp_coeff, int gray_thresh, float peak_thresh, int dilate_wid) : 
            displayInt_(display_int), 
            scaleFactor_(scale),
            spatialWindow_(spatial_window),
            colorWindow_(color_window),
            gaussianBlurWidth_(gauss_blur_width),
            sharpCoeff_(sharp_coeff),
            grayThreshold_(gray_thresh),
            peakThresh_(peak_thresh),
            dilateWidth_(dilate_wid)
            {
                // Load file into private CV matrix variable
                this->loadImage(filename, srcImg_);
            }

            /* 
            METHODS
            */

            // Display an OpenCV matrix image, and destroy on any key press
            void showImage(cv::Mat img, std::string name) {
                cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
                cv::imshow(name, img); // Show our image inside the created window.

                // Cleanup
                cv::waitKey(0); // Wait for any keystroke in the window
                cv::destroyWindow(name); //destroy the created window
            }

            // Load an image from file into an OpenCV matrix
            void loadImage(const std::string& imgFile, cv::Mat& inputImg)
            {   
                std::cout << "Loading file: " << imgFile << std::endl;
                inputImg = cv::imread( imgFile );
                if( inputImg.empty() )
                {
                    std::cout << "Could not open or find the image!\n" << std::endl;
                };

                // Show the initial image
                if (displayInt_) { showImage(inputImg, "Initial Image");};
            }

            // Scale the input image by a scaling factor
            void scaleImage()
            {
                int heightScaled{(int)(srcImg_.rows*scaleFactor_)};
                int widthScaled{(int)(srcImg_.cols*scaleFactor_)};
                std::cout << "Resizing source image to: " << heightScaled << "x" << widthScaled << " pixels" << std::endl;
                cv::resize(srcImg_, scaledImg_, cv::Size(widthScaled, heightScaled), cv::INTER_LINEAR);
                
                // Show the scaled image
                if (displayInt_) { showImage(scaledImg_, "Scaled Image");};
            }

            // Mean-shift filter the image
            void filterImage()
            {
                std::cout << "Filtering image with spatial window: " << spatialWindow_ << " pixels, color window: " << colorWindow_ << " pixels" << std::endl;
                cv::pyrMeanShiftFiltering(scaledImg_,filteredImg_,spatialWindow_,colorWindow_);
                
                // Display the filtered image
                if (displayInt_) { showImage(filteredImg_, "Mean-Shift Filtered Image");}
            }

            // Gaussian Blur/smooth the image
            void smoothImage()
            {
                std::cout << "Smoothing using a Gaussian blur of " << gaussianBlurWidth_ << " pixels" << std::endl;
                cv::GaussianBlur(filteredImg_,smoothedImg_,cv::Size(gaussianBlurWidth_,gaussianBlurWidth_),0,0);
                
                // Display the smoothed image
                if (displayInt_) { showImage(smoothedImg_, "Smoothed Image");}
            }

            // Sharpen the image
            void sharpenImage()
            {
                cv::Mat laplacianImg, sharpImg;
                cv::Mat kernel = (cv::Mat_<float>(3,3) <<
                            sharpCoeff_,  sharpCoeff_, sharpCoeff_,
                            sharpCoeff_, -8*sharpCoeff_, sharpCoeff_,
                            sharpCoeff_,  sharpCoeff_, sharpCoeff_);
                std::cout << "Sharpening image using kernel: " << std::endl;
                std::cout << kernel << std::endl;
                cv::filter2D(smoothedImg_, laplacianImg, CV_32F, kernel);

                smoothedImg_.convertTo(sharpImg, CV_32F);
                sharperImg_ = sharpImg - laplacianImg;
                sharperImg_.convertTo(sharperImg_, CV_8UC3);
                laplacianImg.convertTo(laplacianImg, CV_8UC3);

                // Display the sharpened image
                if (displayInt_) { showImage(sharperImg_, "Sharpened Image");}
            }

            // Compute distance transform
            void distanceTransform()
            {
                cv::Mat grayImg;
                cv::cvtColor(sharperImg_, grayImg, cv::COLOR_BGR2GRAY);
                std::cout << "Converting to binary image with 8-bit threshold " << grayThreshold_ << std::endl;
                cv::threshold(grayImg, grayImg, grayThreshold_, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
                if (displayInt_) { showImage(grayImg, "Grayscale Image");}

                // Compute distance image
                cv::distanceTransform(grayImg,distImg_,cv::DIST_L2, 3); // Potential improvement: the final mask parameter can be an input variable
                cv::normalize(distImg_,distImg_, 0, 1.0, cv::NORM_MINMAX);
                if (displayInt_) { showImage(distImg_, "Distance Transform Image");}
            }

            // Find peaks of the distance-transformed image
            void findPeaks()
            {
                std::cout << "Finding peaks in Distance Image using Peak Threshold: " << peakThresh_ << ", Dilation width: " << dilateWidth_ << std::endl;
                cv::threshold(distImg_, distImg_, peakThresh_, 1.0, cv::THRESH_BINARY);
                cv::Mat kernelImg = cv::Mat::ones(dilateWidth_, dilateWidth_, CV_8U);
                cv::dilate(distImg_, peakImg_, kernelImg);
                if (displayInt_) { showImage(peakImg_, "Peaks");}
            }

            // Find markers of each individual segment
            void findMarkers()
            {
                // Create the CV_8U version of the distance image
                // It is needed for findContours()
                cv::Mat distImg_8u;
                distImg_.convertTo(distImg_8u, CV_8U);
                
                // Find total markers
                cv::findContours(distImg_8u, contours_, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
                markers_ = cv::Mat::zeros(distImg_.size(), CV_32S);
                // Draw the foreground markers
                for (size_t i = 0; i < contours_.size(); i++)
                {
                    drawContours(markers_, contours_, static_cast<int>(i), cv::Scalar(static_cast<int>(i)+1), -1);
                }
                // Draw the background marker
                cv::circle(markers_, cv::Point(5,5), 3, cv::Scalar(255), -1);
                //cv::Mat markers8u;
                markers_.convertTo(markerImg_, CV_8U, 10);
                if (displayInt_) { showImage(markerImg_, "Markers");}
            }

            // Run the watershed algorithm
            void findWatershedMarkers()
            {
                cv::watershed(sharperImg_, markers_);
                //cv::Mat outputImg;
                markers_.convertTo(watershedMarkerImg_, CV_8U);
                bitwise_not(watershedMarkerImg_, watershedMarkerImg_);

                if (displayInt_) { showImage(watershedMarkerImg_, "Watershed Markers");}
            }

            // Select random colors for each segment
            void getRandomColors()
            {

                for (size_t i = 0; i < contours_.size(); i++)
                {
                    int b = cv::theRNG().uniform(0, 256);
                    int g = cv::theRNG().uniform(0, 256);
                    int r = cv::theRNG().uniform(0, 256);
                    colors_.push_back(cv::Vec3b((uchar)b, (uchar)g, (uchar)r));
                }
            }

            // Select mean/average colors for each segment
            void getSegmentColors()
            {
                int numSegments = contours_.size();
                std::vector<int> numSegmentPixels(numSegments,1);
                std::vector<std::vector<long unsigned int>> segmentRgbSums(numSegments,std::vector<long unsigned int>(3,0));

                // Iterate through the input image
                for (int ii = 0; ii < smoothedImg_.rows; ii++)
                {
                    for (int jj = 0; jj < smoothedImg_.cols; jj++)
                    {
                        int index = markers_.at<int>(ii,jj);
                        if (index > 0 && index <= static_cast<int>(contours_.size()))
                        {
                            segmentRgbSums[index-1][0] = segmentRgbSums[index-1][0] + (smoothedImg_.at<cv::Vec3b>(ii,jj)[0]);
                            segmentRgbSums[index-1][1] = segmentRgbSums[index-1][1] + (smoothedImg_.at<cv::Vec3b>(ii,jj)[1]);
                            segmentRgbSums[index-1][2] = segmentRgbSums[index-1][2] + (smoothedImg_.at<cv::Vec3b>(ii,jj)[2]);
                            numSegmentPixels[index-1] +=1; //increment segment pixel count 
                        }
                    }

                    // Now, compute average of each segment's RGB values and assign to color vector
                    for (int kk = 0; kk < numSegments; kk++) {
                        colors_[kk][0] = segmentRgbSums[kk][0] / numSegmentPixels[kk];
                        colors_[kk][1] = segmentRgbSums[kk][1] / numSegmentPixels[kk];
                        colors_[kk][2] = segmentRgbSums[kk][2] / numSegmentPixels[kk];
                    }
                }
            }

            // Generate, save, and display the final image
            void finalImage()
            {
                finalImg_ = cv::Mat::zeros(markers_.size(), CV_8UC3);
                // Fill labeled segments with colors
                for (int i = 0; i < markers_.rows; i++)
                {
                    for (int j = 0; j < markers_.cols; j++)
                    {
                        int index = markers_.at<int>(i,j);
                        if (index > 0 && index <= static_cast<int>(contours_.size()))
                        {
                            finalImg_.at<cv::Vec3b>(i,j) = colors_[index-1];
                        }
                    }
                }
                // Save and visualize the final image
                cv::imwrite("../results/out.jpg", finalImg_);
                showImage(finalImg_, "Final Image");
            }

        private:
            // Image at various stages of processing
            cv::Mat srcImg_, scaledImg_, filteredImg_, smoothedImg_, sharperImg_, distImg_, 
            peakImg_, markers_, markerImg_, watershedMarkerImg_, finalImg_;
            std::vector<std::vector<cv::Point>> contours_; // contour lines
            std::vector<cv::Vec3b> colors_; // segment colors
            const bool displayInt_; // Display intermediate image processing steps
            const float scaleFactor_; // How much to scale the input image
            const int spatialWindow_; // Spatial window to use during mean-shift filtering
            const int colorWindow_; // Color window to use during mean-shift filtering
            const int gaussianBlurWidth_; // Size of gaussian blur filter for smoothing
            const float sharpCoeff_; // Laplacian coefficient (i.e. strength) for sharpening
            const int grayThreshold_; // Binary gray threshold for making black/white mask
            const float peakThresh_; // Peak threshold for peak finding
            const int dilateWidth_; // Dilating width for peak finding


        protected:
            // No protected variables, since this is not intended to be a parent class

    }; // Watershed class

} //Segmentation namespace

#endif // WATERSHED_SEGMENTATION_H