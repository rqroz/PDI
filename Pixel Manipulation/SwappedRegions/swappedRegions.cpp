#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void validate(int, char**, String&);
void usage(char*);
Mat initImage(String);
Mat changeImage(Mat&, Vec2i&, Vec2i&);
void swapImagePixels(Mat&, Vec2i&, Vec2i&);

int main(int argc, char* argv[]){
  String choice;
  validate(argc, argv, choice);

  String imgPath = argc > 2 ? argv[2] : "biel.png";
  Mat image = initImage(imgPath);
  Vec2i middle(image.rows/2 - 1, image.cols/2 - 1), max(image.rows - 1, image.cols - 1);

  namedWindow("imgWindow", WINDOW_AUTOSIZE);

  //Displaying original image
  imshow("imgWindow", image);
  waitKey();

  if(choice == "0"){ // OpenCV Manipulation
    printf("Manipulando a imagem a partir de funções do OpenCV\n");
    image = changeImage(image, middle, max);
  }else{ // Pixel-by-Pixel Manipulation
    printf("Manipulando a imagem pixel à pixel...\n");
    swapImagePixels(image, middle, max);
  }

  //Displaying new image
  imshow("imgWindow", image);
  waitKey();

  return 0;
}

void validate(int argc, char** argv, String& choice){
  if(argc < 2) usage(argv[0]);
  choice = argv[1];
  if(choice != "0" && choice != "1") usage(argv[0]);
}

void usage(char *program_name){
  printf("O programa deve ser utilizado segundo a seguinte assinatura de chamada:\n");
  printf("%s #N /optional/path/to/image\n", program_name);
  printf("Onde #N deve ser 0 ou 1 indicando a forma de manipulação de imagem desejada:\n");
  printf("0: Manipulação por construtores do OpenCV\n");
  printf("1: Manipulação pixel a pixel por rotinas de loop\n");
  exit(1);
}

Mat initImage(String img_name){
  Mat img = imread(img_name, CV_LOAD_IMAGE_GRAYSCALE);
  if(!img.data){
    printf("Não foi possível carregar a imagem 'biel.png'...\n");
    exit(1);
  }
  return img;
}

Mat changeImage(Mat& image, Vec2i& middle, Vec2i& max){
  Size imgSize = image.size(), cropSize = imgSize/2;
  Mat newImg(imgSize, CV_8UC1);
  vector<Mat> subImages = {
    image(Rect(middle[0], middle[1], cropSize.width, cropSize.height)),
    image(Rect(middle[0], 0, cropSize.width, cropSize.height)),
    image(Rect(0, middle[1], cropSize.width, cropSize.height)),
    image(Rect(0, 0, cropSize.width, cropSize.height))
  };
  int size = subImages.size();

  for(int i=0; i< size; ++i){
    int y = middle[0] * (i%2); // 0 if i is even, middle value if i is odd
    int x = i >= size/2 ? middle[1] : 0; // 0 if i is less than half the size of subImages, otherwise middle value
    //copy current image to newImg's (x,y) top corner with .cols width and .rows height
    subImages[i].copyTo(newImg(Rect(x, y, subImages[i].cols, subImages[i].rows)));
  }

  return newImg;
}

void swapImagePixels(Mat& image, Vec2i& middle, Vec2i& max){
  int tempPixelLevel = 0;
  for(int x=0; x <= middle[0]; ++x){
    for(int y=0; y <=max[1]; ++y){
      if(y < middle[1]){
        // x < half of width & y < half of height
        tempPixelLevel = image.at<uchar>(x,y);
        image.at<uchar>(x,y) = image.at<uchar>(middle[0] + x, middle[1] + y);
        image.at<uchar>(middle[0] + x, middle[1] + y) = tempPixelLevel;
      }else{
        // x < half of width & y > half of height
        tempPixelLevel = image.at<uchar>(x,y);
        image.at<uchar>(x,y) = image.at<uchar>(middle[0] + x, y - middle[1]);
        image.at<uchar>(middle[0] + x, y - middle[1]) = tempPixelLevel;
      }
    }
  }
}
