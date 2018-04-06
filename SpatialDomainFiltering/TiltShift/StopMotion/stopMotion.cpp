
#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>

#define MAX_SLIDER_VAL 100

using namespace cv;
using namespace std;

void mergeImages(Mat&, Mat&, Mat&);
void populateWeightMats(Mat&, Mat&, float, float, float);

void lowPassFilter(Mat &src, Mat &dst){
  float lowPassFilter[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  Mat mask, tempMask, src32f, filtered;

  mask = Mat(3, 3, CV_32F, lowPassFilter);
  scaleAdd(mask, 1/9.0, Mat::zeros(3, 3, CV_32F), tempMask);
  mask = tempMask;

  src.convertTo(src32f, CV_32F);
  filter2D(src32f, filtered, src32f.depth(), mask, Point(1,1), 0);
  filtered.convertTo(dst, CV_8U);
}

int main(int argc, char* argv[]){
  String filename = "cat.mp4";
  VideoCapture cap(filename);
  VideoWriter stopMotionVideo;

  Mat frame, lpfFrame, mergedFrame;
  Size frameSize;
  float sizeProportion;

  if(!cap.isOpened()){
      cout << "The video '" << filename << "' could not be opened..." << endl;
      exit(1);
  }

  cap >> frame;
  frameSize = frame.size();
  sizeProportion = (float) frame.size().height/frame.size().width;
  frameSize = Size(640, (int) 640*sizeProportion);

  stopMotionVideo = VideoWriter("stopmotion.avi", VideoWriter::fourcc('X','V','I','D'), 4, frameSize, 1);
  if (!stopMotionVideo.isOpened()){
    cout << "Could not open the file to write the video in..." <<  endl;
    exit(1);
  }

  namedWindow("stopMotion", 1);

  for(int i = 0;; ++i){
      cap >> frame;
      if(frame.empty()) break;
      resize(frame, frame, frameSize);

      lowPassFilter(frame, lpfFrame);
      mergeImages(frame, lpfFrame, mergedFrame);

      imshow("stopMotion", mergedFrame);
      if(i%20==0) stopMotionVideo.write(mergedFrame);

      if(waitKey(30) >= 0) break;
  }

  return 0;
}

void populateWeightMats(Mat& alphaMat, Mat& betaMat, float heightVal, float vertVal, float decayVal){
  Size matSize = alphaMat.size();
  float alpha = 0.5, beta = 1 - alpha;
  int x, upperBound, lowerBound;
  upperBound = -heightVal/2;
  lowerBound = -upperBound;

  for(int i = 0; i < matSize.height; ++i){
    x = i - (vertVal + heightVal/2);
    alpha = 0.5*(tanh((x - upperBound)/decayVal) - tanh((x - lowerBound)/decayVal));
    beta = 1 - alpha;

    Vec3f alphaV(alpha, alpha, alpha);
    Vec3f betaV(beta, beta, beta);

    for(int j = 0; j < matSize.width; ++j){
      alphaMat.at<Vec3f>(i, j) = alphaV;
      betaMat.at<Vec3f>(i, j) = betaV;
    }
  }
}

void mergeImages(Mat& img, Mat& lpfImg, Mat& mergedImage){
  Size imgSize = img.size();
  Mat alphaMat(imgSize, CV_32FC3), betaMat(imgSize, CV_32FC3), outputOriginal, outputLPF;
  float decay = 0.5, height = 0.5*imgSize.height, vert = 0.3*imgSize.height;
  populateWeightMats(alphaMat, betaMat, height, vert, decay);

  img.convertTo(outputOriginal, CV_32FC3);
  lpfImg.convertTo(outputLPF, CV_32FC3);

  multiply(outputOriginal, alphaMat, outputOriginal);
  multiply(outputLPF, betaMat, outputLPF);

  add(outputOriginal, outputLPF, mergedImage);

  mergedImage.convertTo(mergedImage, CV_8UC3);
}
