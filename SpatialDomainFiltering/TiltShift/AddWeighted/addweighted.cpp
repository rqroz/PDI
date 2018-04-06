
#include <iostream>
#include <opencv2/opencv.hpp>

#define MAX_SLIDER_VAL 100

using namespace cv;
using namespace std;

int alphaSlider = 0, topSlider = 0;

/*
* image1: original image
* image2: image after low pass filtering
* imageTop:
* blended: resulting image after merge of image with imageTop
*/
Mat image1, image2, imageTop, blended;

char trackbarName[50];

void onTrackbarLine(int, void*);
void onTrackbarBlend(int, void*);

int main(int argc, char* argv[]){
  image1 = imread("blend1.jpg");
  image2 = imread("blend2.jpg");

  image2.copyTo(imageTop);
  namedWindow("addWeighted", 1);

  sprintf(trackbarName, "Alpha x %d", MAX_SLIDER_VAL);
  createTrackbar(trackbarName, "addWeighted", &alphaSlider, MAX_SLIDER_VAL, onTrackbarBlend);
  onTrackbarBlend(alphaSlider, 0);

  sprintf(trackbarName, "Scanline x %d", MAX_SLIDER_VAL);
  createTrackbar(trackbarName, "addWeighted", &topSlider, MAX_SLIDER_VAL, onTrackbarLine);
  onTrackbarLine(topSlider, 0);

  waitKey(0);

  return 0;
}

void onTrackbarBlend(int, void*){
  double alpha = (double) alphaSlider/MAX_SLIDER_VAL;
  addWeighted(image1, alpha, imageTop, 1 - alpha, 0, blended);
  imshow("addWeighted", blended);
}

void onTrackbarLine(int, void*){
  image1.copyTo(imageTop);
  int limit = topSlider*255/MAX_SLIDER_VAL;
  if(limit > 0){
    Mat tmp = image2(Rect(0, 0, 256, limit));
    tmp.copyTo(imageTop(Rect(0, 0, 256, limit)));
  }

  onTrackbarBlend(alphaSlider, 0);
}
