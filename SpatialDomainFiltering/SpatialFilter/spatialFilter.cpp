
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void menu();
void printMask(Mat&);
void proccessImage(Mat &mask, Mat temp, float base[], float propotion, bool replaceMask);
void proccessImage(Mat &mask, Mat temp, float base[], float =0, bool =false);

int main(int argc, char* argv[]){
  VideoCapture video;
  Mat cap, frame, frame32f, frameFiltered;
  Mat mask, mask1;
  Mat result, result1;

  float media[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  float gauss[] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
  float horizontal[]={-1, 0, 1, -2, 0, 2, -1, 0, 1};
  float vertical[]={-1, -2, -1, 0, 0, 0, 1, 2, 1};
  float laplacian[]={0, -1, 0, -1, 4, -1, 0, -1, 0};
  int absolut;
  char key;


  // Start

  video.open(0);
  if(!video.isOpened()){
    printf("Câmera indisponível...\n");
    exit(1);
  }

  cout << "Width: " << video.get(CV_CAP_PROP_FRAME_WIDTH) << endl
        << "Height: " << video.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;

  namedWindow("spatialFilter", 1);

  mask = Mat(3, 3, CV_32F, media);
  scaleAdd(mask, 1/9.0, Mat::zeros(3,3,CV_32F), mask1);
  swap(mask, mask1);
  absolut = 1;

  menu();
  while(1){
    video >> cap;
    cvtColor(cap, frame, CV_BGR2GRAY);
    flip(frame, frame, 1);
    imshow("original", frame);
    frame.convertTo(frame32f, CV_32F);
    filter2D(frame32f, frameFiltered, frame32f.depth(), mask, Point(1, 1), 0);

    if(absolut) frameFiltered = abs(frameFiltered);

    frameFiltered.convertTo(result, CV_8U);
    imshow("spatialFilter", result);
    key = (char) waitKey(10);
    if(key == 27) break; // Esc was pressed

    switch (key) {
      case 'a':
        menu();
        absolut = !absolut;
        break;
      case 'm':
        proccessImage(mask, mask1, media, 1/9.0, true);
        break;
      case 'g':
        proccessImage(mask, mask1, gauss, 1/16.0, true);
        break;
      case 'v':
        proccessImage(mask, Mat(), horizontal, 0, true);
        break;
      case 'h':
        proccessImage(mask, Mat(), vertical);
        break;
      case 'l':
        proccessImage(mask, Mat(), laplacian);
        break;
      default:
        break;
    }
  }

  return 0;
}

void menu(){
  cout << "\nEscolha o filtro que deseja ativar: \n"
            "a - calcular modulo\n"
            "m - media\n"
            "g - gauss\n"
            "v - vertical\n"
            "h - horizontal\n"
            "l - laplaciano\n"
            "esc - sair\n";
}

void printMask(Mat& m){
  Size s = m.size();
  for(int i = 0; i < s.width; ++i){
    for(int j=0; j < s.height; ++j){
      cout << m.at<float>(i, j) << ", ";
    }
    cout << endl;
  }
}

void proccessImage(Mat &mask, Mat temp, float base[], float propotion, bool replaceMask){
  mask = Mat(3, 3, CV_32F, base);
  if(replaceMask && propotion > 0){
    scaleAdd(mask, propotion, Mat::zeros(3,3,CV_32F), temp);
    mask = temp;
  }

  printMask(mask);
  menu();
}
