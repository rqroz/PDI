
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
    * vSize: Size of histImg vector
    * range[]: Range of values that will exist in the image from which the histogram will be calculated
    * *histRange: Pointer to the range of values for the histogram
    */
    int nBins = 64, histW = nBins, histH = nBins/2, vSize;
    float range[] = {0, 256};
    const float *histRange = {range};

    // The boolean variables bellow are to be used for the function calcHist
    bool uniform = true;
    bool accumulate = true;

    /*
    * image: The final image to be displayed
    * planes: Will hold R, G and B matricial planes to be used for calculating the histograms
    * hist: Will hold three histograms, each responsible for one color spectrum (R, G and B)
    * histImg: Images used to store the histograms
    * histScalar: Scalar vector for each RGB component
    */
    Mat image;
    vector<Mat> planes;
    vector<Mat> hist = {Mat(), Mat(), Mat()};
    vector<Mat> histImg = {
      Mat(histH, histW, CV_8UC3, Scalar(0, 0, 0)),
      Mat(histH, histW, CV_8UC3, Scalar(0, 0, 0)),
      Mat(histH, histW, CV_8UC3, Scalar(0, 0, 0))
    };
    vector<Scalar> histScalar = {
      Scalar(0, 0, 255),
      Scalar(0, 255, 0),
      Scalar(255, 0, 0)
    };

    vSize = histImg.size();

    cout << "Width: " << cap.get(CV_CAP_PROP_FRAME_WIDTH) << endl
          << "Height: " << cap.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;

    while(1){
      cap >> image;
      split(image, planes);
      for(int i = 0; i < vSize; ++i){
          calcHist(&planes[i], 1, 0, Mat(), hist[i], 1, &nBins, &histRange, uniform, accumulate);
          normalize(hist[i], hist[i], 0, histImg[i].rows, NORM_MINMAX, -1, Mat());
          histImg[i].setTo(Scalar(0));

          for(int j = 0; j < nBins; ++j){
            line(histImg[i], Point(j, histH), Point(j, histH - cvRound(hist[i].at<float>(j))), histScalar[i], 1, 8 , 0);
          }

          histImg[i].copyTo(image(Rect(0, i*histH, nBins, histH)));
      }

      imshow("image", image);
      if(waitKey(30) >= 0) break;
    }

    return 0;
}
