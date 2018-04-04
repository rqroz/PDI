
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
    * image: The final image to be displayed
    * planes: Will hold R, G and B matricial planes to be used for calculating the histograms
    */
    Mat image;
    vector<Mat> planes;

    while(1){
      cap >> image;
      split(image, planes);

      // TODO

      imshow("image", image);
      if(waitKey(30) >= 0) break;
    }

    return 0;
}
