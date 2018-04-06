
#include <iostream>
#include <opencv2/opencv.hpp>

#define MAX_SLIDER_VAL 100

using namespace cv;
using namespace std;

int heightSlider = 0, decaySlider = 0, vertSlider = 0, alphaSlider = 0;
double heightVal, decayVal, vertVal;
double alpha = 0.5;
/*
* image1: original image
* image2: image after low pass filtering
* imageTop:
* blended: resulting image after merge of image with imageTop
*/
Mat image1, image2, imageTop, blended;
Mat originalImage, lowPassFilteredImage, mergedImage, imagesContainer;

char trackbarName[50];

void initializeContainer();
void addTextToImage(Mat&, String, float);
void mergeImages(Mat&, Mat&, Mat&);

void onTrackbarBlend(int, void*);
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
  originalImage = imread("trainspotting.jpg");

  if(!originalImage.data){
    cout << "Não foi possível carregar a imagem 'trainspotting.jpg'...\n";
    exit(1);
  }

  namedWindow("tiltShift", 1);

  lowPassFilter(originalImage, lowPassFilteredImage);
  initializeContainer();
  mergeImages(originalImage, lowPassFilteredImage, mergedImage);

  imshow("tiltShift", imagesContainer);

  sprintf(trackbarName, "Alpha x %d", MAX_SLIDER_VAL);
  createTrackbar(trackbarName, "tiltShift", &alphaSlider, MAX_SLIDER_VAL, onTrackbarBlend);
  onTrackbarBlend(alphaSlider, 0);

  sprintf(trackbarName, "Region Height x %d", MAX_SLIDER_VAL);
  createTrackbar(trackbarName, "tiltShift", &heightSlider, MAX_SLIDER_VAL, onTrackbarHeight);
  onTrackbarHeight(heightSlider, 0);

  sprintf(trackbarName, "Decay x %d", MAX_SLIDER_VAL);
  createTrackbar(trackbarName, "tiltShift", &decaySlider, MAX_SLIDER_VAL, onTrackbarDecay);
  onTrackbarDecay(decaySlider, 0);

  sprintf(trackbarName, "Vertical Position x %d", MAX_SLIDER_VAL);
  createTrackbar(trackbarName, "tiltShift", &vertSlider, MAX_SLIDER_VAL, onTrackbarVert);
  onTrackbarVert(vertSlider, 0);

  waitKey(0);

  return 0;
}

void initializeContainer(){
  Size imgSize = originalImage.size(), halfSize = imgSize/2;
  Mat smallerImage;

  imagesContainer = Mat(1.5*imgSize.height, imgSize.width, CV_8UC3);

  resize(originalImage, smallerImage, halfSize);
  addTextToImage(smallerImage, "Original", halfSize.width/3);
  smallerImage.copyTo(imagesContainer(Rect(0, 0, halfSize.width, halfSize.height)));
  resize(lowPassFilteredImage, smallerImage, halfSize);
  addTextToImage(smallerImage, "Low Pass Filter", halfSize.width/4);
  smallerImage.copyTo(imagesContainer(Rect(halfSize.width, 0, halfSize.width, halfSize.height)));
}

void addTextToImage(Mat& img, String text, float width){
  double fontScale = 0.75;
  int thickness = 2;
  Size s = img.size();
  Point textOrg(width, s.height/1.1);
  putText(img, text, textOrg, FONT_HERSHEY_TRIPLEX, fontScale, Scalar(0, 0, 255), thickness,8);
}

void onTrackbarBlend(int, void*){
  alpha = (double) alphaSlider/MAX_SLIDER_VAL;
  mergeImages(originalImage, lowPassFilteredImage, mergedImage);
  imshow("tiltShift", mergedImage);
}
//
// void onTrackbarLine(int, void*){
//   image1.copyTo(imageTop);
//   int limit = topSlider*255/MAX_SLIDER_VAL;
//   if(limit > 0){
//     Mat tmp = image2(Rect(0, 0, 256, limit));
//     tmp.copyTo(imageTop(Rect(0, 0, 256, limit)));
//   }
//
//   onTrackbarBlend(alphaSlider, 0);
// }

void mergeImages(Mat& image, Mat& lpfImage, Mat& result){
  addWeighted(image, alpha, lpfImage, 1 - alpha, 0.0, result);

  Size imgSize = image.size();
  result.copyTo(imagesContainer(Rect(0, imgSize.height/2, imgSize.width, imgSize.height)));
}

void onTrackbarHeight(int, void*){
  heightVal = (double) heightSlider/MAX_SLIDER_VAL;
  cout << "Height Slider: " << heightVal << endl;
}

void onTrackbarDecay(int, void*){
  decayVal = (double) decaySlider/MAX_SLIDER_VAL;
  cout << "Weight Slider: " << decayVal << endl;
}

void onTrackbarVert(int, void*){
  vertVal = (double) vertSlider/MAX_SLIDER_VAL;
  cout << "Vert Slider: " << vertVal << endl;
}
