#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char* argv[]){
  Mat image, mask;
  CvPoint p(0,0);

  int width, height, nObjects = 0;

  image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
  if(!image.data){
    std::cout << "imagem nao carregou corretamente\n";
    return(-1);
  }

  width = image.size().width;
  height = image.size().height;

  for(int i=0; i < height; ++i){
    for(int j=0; j < width; ++j){
      if(image.at<uchar>(i,j) == 255){
        nObjects++;
        p.x=j;
        p.y=i;
        floodFill(image, p, nObjects%256);
      }
    }
  }

  imshow("image", image);
  imwrite("labelling.png", image);
  waitKey();

  return 0;
}
