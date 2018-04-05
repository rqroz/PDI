
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char* argv[]){
    /*
    * cap: OpenCV object for capturing video
    */
    VideoCapture cap;
    cap.open(0);

    if(!cap.isOpened()){
      printf("Câmera indisponível...\n");
      exit(1);
    }

    /*
    * nBins: Size of vector used to store histograms
    * histW: Histogram's width
    * histH: Histogram's height
    * range[]: Range of values that will exist in the image from which the histogram will be calculated
    * *histRange: Pointer to the range of values for the histogram
    */
    int nBins = 64, histW = nBins, histH = nBins/2;
    float range[] = {0, 256};
    const float *histRange = {range};

    // The boolean variables bellow are to be used for the function calcHist
    bool uniform = true, accumulate = true;

    /*
    * fullContainer: Will store two of the same size side-by-side
    * images: Will hold two images: a normal grayscale image and an equalized version of it
    * hist: Will hold two histograms: one for the normal grayscale image and the other for the equalized one
    * histImg: Images used to store the histograms
    */
    Mat fullContainer(480, 2*640, CV_8UC1);
    vector<Mat> images = {Mat(), Mat()};
    vector<Mat> hist = {Mat(), Mat()};
    vector<Mat> histImg = {
      Mat(histH, histW, CV_8UC1, Scalar(0)),
      Mat(histH, histW, CV_8UC1, Scalar(0))
    };

    // vSize: Number of images
    int vSize = images.size();

    cout << "Width: " << cap.get(CV_CAP_PROP_FRAME_WIDTH) << endl
          << "Height: " << cap.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;

    while(1){
      // Store the current camera frame into the first element of images vector
      cap >> images[0];
      // Transform that image into a grayscale version of itself
      cvtColor(images[0], images[0], CV_BGR2GRAY);
      // Equalize the grayscale version and store the result into the second element of images vector
      equalizeHist(images[0], images[1]);

      // For each image in images array
      for(int i = 0; i < vSize; ++i){
          // Get it's histogram and store it into the same index of hist vector
          calcHist(&images[i], 1, 0, Mat(), hist[i], 1, &nBins, &histRange, uniform, accumulate);
          // Normalize the histogram
          normalize(hist[i], hist[i], 0, histImg[i].rows, NORM_MINMAX, -1, Mat());
          // Set the same index of histImg to be all black
          histImg[i].setTo(Scalar(0));

          // Draw each one of nBins lines on the current histImg
          for(int j = 0; j < nBins; ++j){
            line(histImg[i], Point(j, histH), Point(j, histH - cvRound(hist[i].at<float>(j))), Scalar(127), 1, 8 , 0);
          }

          //Copy the current histImg into the current image
          histImg[i].copyTo(images[i](Rect(0, 0, nBins, histH)));

          //Copy the current image to its desired position into fullContainer
          images[i].copyTo(fullContainer(Rect(i*640,0,640,480)));
      }

      // Display full container and close the window if the user pressed any button after 30ms
      imshow("fullContainer", fullContainer);
      if(waitKey(30) >= 0) break;
    }

    return 0;
}
