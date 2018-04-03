#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void init_image(Mat&);
void read_location(const int, const Mat&, Vec2i&);

int main(int argc, char* argv[]){
  Mat image;
  Vec2i starting_loc, finishing_loc;

  init_image(image);
  read_location(1, image, starting_loc);
  read_location(2, image, finishing_loc);

  for(int i=starting_loc[0]; i < finishing_loc[0]; ++i){
    for(int j=starting_loc[1]; j < finishing_loc[1]; ++j){
      image.at<uchar>(i,j) = 255 - image.at<uchar>(i,j);
    }
  }

  namedWindow("janelaBiel", WINDOW_AUTOSIZE);
  imshow("janelaBiel", image);
  waitKey();

  return 0;
}

void read_location(const int loc_index, const Mat& img, Vec2i &loc){
  int rows = img.rows - 1, cols = img.cols - 1;
  do{
    printf("Informe as duas coordenadas do ponto %d:\n", loc_index);
    printf("Nota: as coordenadas devem ser inteiros e estar entre (0,0) e (%d, %d).\n->", rows, cols);
    cin >> loc[0] >> loc[1];
  }while(loc[0] < 0 || loc[1] < 0 || loc[0] > rows || loc[1] > cols);
}

void init_image(Mat& img){
  img = imread("biel.png", CV_LOAD_IMAGE_GRAYSCALE);
  if(!img.data){
    printf("Não foi possível carregar a imagem 'biel.png'...\n");
    exit(1);
  }
}
