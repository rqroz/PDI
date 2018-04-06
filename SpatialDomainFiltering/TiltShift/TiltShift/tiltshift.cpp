
#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>

#define MAX_SLIDER_VAL 100

using namespace cv;
using namespace std;

int heightSlider = 0, decaySlider = 0, vertSlider = 0;
double heightVal, decayVal, vertVal;
/*
* image1: original image
* image2: image after low pass filtering
* imageTop:
* blended: resulting image after merge of image with imageTop
*/
Mat originalImage, lowPassFilteredImage, mergedImage, imagesContainer;
Size imgSize;

void initializeContainer();
void addTextToImage(Mat&, String, float);
void populateWeightMats(Mat&, Mat&);
void mergeImages();

void onTrackbarHeight(int, void*);
void onTrackbarDecay(int, void*);
void onTrackbarVert(int, void*);

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
  char trackbarName[50];

  originalImage = imread("trainspotting.jpg");

  if(!originalImage.data){
    cout << "Não foi possível carregar a imagem 'trainspotting.jpg'...\n";
    exit(1);
  }

  resize(originalImage, originalImage, Size(480, 320));

  namedWindow("tiltShift", 1);
  
  lowPassFilter(originalImage, lowPassFilteredImage);
  initializeContainer();
  mergeImages();

  sprintf(trackbarName, "Region Height x %d", MAX_SLIDER_VAL);
  createTrackbar(trackbarName, "tiltShift", &heightSlider, MAX_SLIDER_VAL, onTrackbarHeight);

  sprintf(trackbarName, "Vertical Position x %d", MAX_SLIDER_VAL);
  createTrackbar(trackbarName, "tiltShift", &vertSlider, MAX_SLIDER_VAL, onTrackbarVert);

  sprintf(trackbarName, "Decay");
  createTrackbar(trackbarName, "tiltShift", &decaySlider, MAX_SLIDER_VAL, onTrackbarDecay);

  waitKey(0);
  imwrite("result.png", imagesContainer);
  return 0;
}

void initializeContainer(){
  imgSize = originalImage.size();
  Size halfSize = imgSize/2;
  Mat smallerImage;

  imagesContainer = Mat(1.5*imgSize.height, imgSize.width, CV_8UC3);

  resize(originalImage, smallerImage, halfSize);
  addTextToImage(smallerImage, "Original", 5);
  smallerImage.copyTo(imagesContainer(Rect(0, 0, halfSize.width, halfSize.height)));
  resize(lowPassFilteredImage, smallerImage, halfSize);
  addTextToImage(smallerImage, "Low Pass Filter", 14);
  smallerImage.copyTo(imagesContainer(Rect(halfSize.width, 0, halfSize.width, halfSize.height)));
}

void addTextToImage(Mat& img, String text, float margin){
  float fontScale = 0.75, thickness = 1.5;
  Size s = img.size();
  Point textOrg(s.width/margin, s.height/1.1);
  putText(img, text, textOrg, FONT_HERSHEY_TRIPLEX, fontScale, Scalar(0, 127, 255), thickness, 8);
}

void populateWeightMats(Mat& alphaMat, Mat& betaMat){
  float alpha = 0.5, beta = 1 - alpha;
  int x, upperBound, lowerBound;
  upperBound = -heightVal/2;
  lowerBound = -upperBound;

  for(int i = 0; i < imgSize.height; ++i){
    x = i - (vertVal + heightVal/2);
    alpha = 0.5*(tanh((x - upperBound)/decayVal) - tanh((x - lowerBound)/decayVal));
    beta = 1 - alpha;

    Vec3f alphaV(alpha, alpha, alpha);
    Vec3f betaV(beta, beta, beta);

    for(int j = 0; j < imgSize.width; ++j){
      alphaMat.at<Vec3f>(i, j) = alphaV;
      betaMat.at<Vec3f>(i, j) = betaV;
    }
  }
}

void mergeImages(){
  imgSize = originalImage.size();
  Mat alphaMat(imgSize, CV_32FC3), betaMat(imgSize, CV_32FC3), outputOriginal, outputLPF;

  populateWeightMats(alphaMat, betaMat);

  originalImage.convertTo(outputOriginal, CV_32FC3);
  lowPassFilteredImage.convertTo(outputLPF, CV_32FC3);

  multiply(outputOriginal, alphaMat, outputOriginal);
  multiply(outputLPF, betaMat, outputLPF);

  add(outputOriginal, outputLPF, mergedImage);

  mergedImage.convertTo(mergedImage, CV_8UC3);
  mergedImage.copyTo(imagesContainer(Rect(0, imgSize.height/2, imgSize.width, imgSize.height)));
  imshow("tiltShift", imagesContainer);
}

void onTrackbarHeight(int, void*){
  heightVal = (double) heightSlider*imgSize.height/MAX_SLIDER_VAL;
  mergeImages();
}

void onTrackbarVert(int, void*){
  vertVal = (double) vertSlider*imgSize.height/MAX_SLIDER_VAL;
  mergeImages();
}

void onTrackbarDecay(int, void*){
  decayVal = decaySlider == 0 ? 0.01 : (double) decaySlider;
  mergeImages();
}
