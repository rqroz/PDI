
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char* argv[]){
    VideoCapture cap;
    vector<Mat> planes; // Will hold R, G and B matricial planes to be used for calculating the histograms
    // vector<Mat> hist; // Will hold three histograms, each responsible for one color spectrum (R, G and B)
    Mat image, histR, histG, histB;

    int nBins = 64; // Size of vector used to store histograms
    float range[] = {0, 256}; // Range of values that will exist in the image from which the histogram will be calculated
    const float *histRange = {range}; // pointer to the range of values for the histogram

    // The boolean variables bellow are to be used for the function calcHist
    bool uniform = true;
    bool accumulate = true;

    cap.open(0);

    if(!cap.isOpened()){
      printf("Câmera indisponível...\n");
      exit(1);
    }

    cout << "Largura: " << cap.get(CV_CAP_PROP_FRAME_WIDTH) << endl
          << "Altura: " << cap.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;

    int histw = nBins, histh = nBins/2;
    Mat histImgR(histh, histw, CV_8UC3, Scalar(0, 0, 0));
    Mat histImgG(histh, histw, CV_8UC3, Scalar(0, 0, 0));
    Mat histImgB(histh, histw, CV_8UC3, Scalar(0, 0, 0));

    while(1){
      cap >> image;
      split(image, planes);
      // calcHist(images, numberOfImages, channels, inputArrayMask, outputArrayHist, dimensions, *histSize, **range, uniform, accumulate);
      calcHist(&planes[0], 1, 0, Mat(), histR, 1, &nBins, &histRange, uniform, accumulate);
      calcHist(&planes[1], 1, 0, Mat(), histG, 1, &nBins, &histRange, uniform, accumulate);
      calcHist(&planes[2], 1, 0, Mat(), histB, 1, &nBins, &histRange, uniform, accumulate);

      normalize(histR, histR, 0, histImgR.rows, NORM_MINMAX, -1, Mat());
      normalize(histG, histG, 0, histImgG.rows, NORM_MINMAX, -1, Mat());
      normalize(histB, histB, 0, histImgB.rows, NORM_MINMAX, -1, Mat());

      histImgR.setTo(Scalar(0));
      histImgG.setTo(Scalar(0));
      histImgB.setTo(Scalar(0));

      for(int i = 0; i < nBins; ++i){
        line(histImgR, Point(i, histh), Point(i, histh - cvRound(histR.at<float>(i))), Scalar(0, 0, 255), 1, 8 , 0);
        line(histImgG, Point(i, histh), Point(i, histh - cvRound(histG.at<float>(i))), Scalar(0, 255, 0), 1, 8 , 0);
        line(histImgB, Point(i, histh), Point(i, histh - cvRound(histB.at<float>(i))), Scalar(255, 0, 0), 1, 8 , 0);
      }

      histImgR.copyTo(image(Rect(0, 0, nBins, histh)));
      histImgG.copyTo(image(Rect(0, histh, nBins, histh)));
      histImgB.copyTo(image(Rect(0, 2*histh, nBins, histh)));

      imshow("image", image);
      if(waitKey(30) >= 0) break;
    }

    return 0;
}
