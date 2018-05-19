#include <bits/stdc++.h>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

#define STEP 5
#define JITTER 3
#define RAIO 4

int top_slider = 10;
int top_slider_max = 200;

char TrackbarName[50];

Mat image,imgray, border,points;
  int width, height;
  Vec3b colors;
  int x, y;
  vector<int> yrange;
  vector<int> xrange;

void on_trackbar_canny(int, void*){

  Canny(imgray, border, top_slider, 3*top_slider);
  imshow("cannyborders.png", border);
  points = Mat(height, width, CV_8UC3, Scalar(255,255,255));
  random_shuffle(xrange.begin(), xrange.end());
  for(auto i : xrange){
    random_shuffle(yrange.begin(), yrange.end());
    for(auto j : yrange){

      if(border.at<uchar>(j,i) == 255)
      {
          x = i+rand()%(2*JITTER)-JITTER+1;
          y = j+rand()%(2*JITTER)-JITTER+1;
          colors = image.at<Vec3b>(y,x);
          circle(points, Point(x,y),2, CV_RGB(colors[2],colors[1],colors[0]), -1, CV_AA);
      }
      else{
          x = i+rand()%(2*JITTER)-JITTER+1;
          y = j+rand()%(2*JITTER)-JITTER+1;
          colors = image.at<Vec3b>(x,y);
          circle(points,
                 cv::Point(y,x),
                 RAIO,
                 CV_RGB(colors[2],colors[1],colors[0]),
                 -1,
                 CV_AA);
      }
    }
  }
  imshow("canny",points);

}

int main(int argc, char**argv){

  image= imread(argv[1],CV_LOAD_IMAGE_COLOR);
  cvtColor(image,imgray, CV_BGR2GRAY);

  srand(time(0));

  if(!image.data){
  cout << "nao abriu" << argv[1] << endl;
    cout << argv[0] << " imagem.jpg";
    exit(0);
  }

  width=image.size().width;
  height=image.size().height;

  xrange.resize(height/STEP);
  yrange.resize(width/STEP);

  iota(xrange.begin(), xrange.end(), 0);
  iota(yrange.begin(), yrange.end(), 0);

  for(uint i=0; i<xrange.size(); i++){
    xrange[i]= xrange[i]*STEP+STEP/2;
  }

  for(uint i=0; i<yrange.size(); i++){
    yrange[i]= yrange[i]*STEP+STEP/2;
  }

  sprintf( TrackbarName, "Threshold inferior", top_slider_max );

  namedWindow("canny",1);
  createTrackbar( TrackbarName, "canny",
                &top_slider,
                top_slider_max,
                on_trackbar_canny );

  on_trackbar_canny(top_slider, 0 );

  waitKey();
  imwrite("cannypontos.png",points);
  return 0;
}
