
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void menu();
void printMask(Mat&);
void proccessMask(Mat &, int, float[], float, bool);
void proccessMask(Mat &, int, float[], float =0, bool =false);

int main(int argc, char* argv[]){
  VideoCapture video;
  Mat cap, frame, frame32f, frameFiltered;
  Mat mask;
  Mat result, result1;

  float media[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  float gauss[] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
  float horizontal[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
  float vertical[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
  float laplacian[] = {0, -1, 0, -1, 4, -1, 0, -1, 0};
  float lapgauss[] = {0, 0, 1, 0, 0, 0, 1, 2, 1, 0, 1, 2, -16, 2, 1, 0, 1, 2, 1, 0, 0, 0, 1, 0, 0};

  bool absolut = true;
  char key;

  video.open(0);
  if(!video.isOpened()){
    printf("Camera not found...\n");
    exit(1);
  }

  cout << "Width: " << video.get(CV_CAP_PROP_FRAME_WIDTH) << endl
        << "Height: " << video.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;

  namedWindow("spatialFilter", 1);
  proccessMask(mask, 3, media, 1/9.0, true);

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
        cout << endl << "-> Absolut " << (absolut ? "on" : "off") << endl;
        break;
      case 'm':
        proccessMask(mask, 3, media, 1/9.0, true);
        break;
      case 'g':
        proccessMask(mask, 3, gauss, 1/16.0, true);
        break;
      case 'v':
        proccessMask(mask, 3, horizontal, 0, true);
        break;
      case 'h':
        proccessMask(mask, 3, vertical);
        break;
      case 'l':
        proccessMask(mask, 3, laplacian);
        break;
      case 'u':
        proccessMask(mask, 5, lapgauss);
        break;
      default:
        break;
    }
  }

  return 0;
}

void menu(){
  cout << "\nEscolha o filtro que deseja ativar: \n"
            "a - Toggle Absolut\n"
            "m - Mean\n"
            "g - Gauss\n"
            "v - Vertical\n"
            "h - Horizontal\n"
            "l - Laplacian\n"
            "u - Laplacian of Gaussian\n"
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

void proccessMask(Mat &mask, int maskDimension, float base[], float propotion, bool replaceMask){
  Mat tempMask;
  mask = Mat(maskDimension, maskDimension, CV_32F, base);
  if(replaceMask && propotion > 0){
    scaleAdd(mask, propotion, Mat::zeros(3,3,CV_32F), tempMask);
    mask = tempMask;
  }

  printMask(mask);
  menu();
}
