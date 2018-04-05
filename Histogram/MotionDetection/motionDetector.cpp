
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

bool compareHistograms(const Mat&, const Mat&);

struct ImageInfo {
  /*
  * image: Image to be stored
  * hist: Histogram to be histore
  * histImg: The image craeted from the histogram (bar graph)
  */
  Mat image, hist, histImg;

  void copyTo(ImageInfo& dst){
    image.copyTo(dst.image);
    hist.copyTo(dst.hist);
    histImg.copyTo(dst.histImg);
  }
};

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

    // fullContainer: Will store two of the same size side-by-side
    Mat fullContainer(480, 2*640, CV_8UC1), tempImage;

    ImageInfo current, past;
    current.histImg = Mat(histH, histW, CV_8UC1, Scalar(0));

    cout << "Width: " << cap.get(CV_CAP_PROP_FRAME_WIDTH) << endl
          << "Height: " << cap.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;

    while(1){
      // Store the current camera frame into the first element of images vector
      cap >> tempImage;
      // Transform that image into a grayscale version of itself
      cvtColor(tempImage, current.image, CV_BGR2GRAY);

      // Get it's histogram and store it into the same index of hist vector
      calcHist(&current.image, 1, 0, Mat(), current.hist, 1, &nBins, &histRange, uniform, accumulate);
      // Normalize the histogram
      normalize(current.hist, current.hist, 0, current.histImg.rows, NORM_MINMAX, -1, Mat());
      // Set the same index of histImg to be all black
      current.histImg.setTo(Scalar(0));

      // Draw each one of nBins lines on the current histImg
      for(int j = 0; j < nBins; ++j){
        line(current.histImg, Point(j, histH), Point(j, histH - cvRound(current.hist.at<float>(j))), Scalar(127), 1, 8 , 0);
      }

      // Comparing histograms
      if(compareHistograms(current.hist, past.hist)){
        double fontScale = 1.5;
        int thickness = 2;
        Size s = current.image.size();
        Point textOrg(s.width/5, s.height/1.2);
        putText(current.image, "A moviment was detected!", textOrg, FONT_HERSHEY_COMPLEX_SMALL, fontScale, Scalar::all(200), thickness,8);
        imwrite("someone.png", current.image);
        cout << "A moviment was detected!" << endl;
      }

      // Replacing past histogram with current one
      current.copyTo(past);

      //Copy the current histImg into the current image
      current.histImg.copyTo(current.image(Rect(0, 0, nBins, histH)));
      past.histImg.copyTo(past.image(Rect(0, 0, nBins, histH)));

      //Copy the current image to its desired position into fullContainer
      current.image.copyTo(fullContainer(Rect(0,0,640,480)));
      past.image.copyTo(fullContainer(Rect(640,0,640,480)));

      // Display full container and close the window if the user pressed any button after 30ms
      imshow("fullContainer", fullContainer);
      if(waitKey(30) >= 0) break;
    }

    return 0;
}


bool compareHistograms(const Mat& current, const Mat& past){
  if(countNonZero(past) > 1){
    return compareHist(current, past, CV_COMP_CORREL) < 0.998;
  }

  return false;
}
