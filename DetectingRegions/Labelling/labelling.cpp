#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat initImage(String);
void displayImage(Mat&);
void preProccessImage(Mat&);
int countObjects(Mat&, int, int);

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("To execute the program use: %s /path/to/image\n\n", argv[0]);
    exit(1);
  }

  Mat image = initImage(argv[1]);
  int totalObjects = 0, leakedObjects = 0;
  const int minGrayScale = 0, midGrayScale = 127, maxGrayScale = 255;

  displayImage(image);
  preProccessImage(image);
  totalObjects = countObjects(image, maxGrayScale, midGrayScale);
  displayImage(image);
  leakedObjects = countObjects(image, minGrayScale, midGrayScale);
  displayImage(image);

  printf("The total amount of objects found is: %d\n", totalObjects);
  printf("From those, %d objects were found to be leaked.\n", leakedObjects);
  imwrite("labelling.png", image);

  return 0;
}

void displayImage(Mat& img){
  imshow("image", img);
  waitKey();
}

Mat initImage(String img_name){
  Mat img = imread(img_name, CV_LOAD_IMAGE_GRAYSCALE);
  if(!img.data){
    cout << "Não foi possível carregar a imagem '" << img_name << "'...\n";
    exit(1);
  }
  return img;
}

void preProccessImage(Mat& img){
  /*
  * This function adds a 1px white border around the image and then perform floodFill() at the
  * starting point of (0,0) so it can remove the bubbles
  */
  Size s = img.size();
  CvPoint startingPoint(0,0);

  for (int j = 0; j < s.width; ++j){
    img.at<uchar>(0, j) = 255;
    img.at<uchar>(s.height-1, j) = 255;
  }
  for (int i = 0; i < s.height; ++i){
    img.at<uchar>(i, 0) = 255;
    img.at<uchar>(i, s.width - 1) = 255;
  }

  floodFill(img, startingPoint, 0); // Removing the bubbles touching the image's border
  displayImage(img);
  // Changing the color of the outside of bubbles so we can later search for holes using
  // 0 as the grayScale parameter
  floodFill(img, startingPoint, 50);
  displayImage(img);
}

int countObjects(Mat& img, int compareGrayScale, int setGrayScale){
  /*
  * This function counts how many objects are in an image.
  * The comparison is performed by checking a given gray scale (compareGrayScale) of a certain pixel and
  * identifying the object it belongs to with the floodFill() function.
  * Moreover, setGrayScale is used to set the new value for the pixels of the found objects.
  */
  CvPoint p(0,0);
  Size s = img.size();
  int n = 0;

  for(int i=0; i < s.height; ++i){
    for(int j=0; j < s.width; ++j){
      if(img.at<uchar>(i, j) == compareGrayScale){
        n++;
        p.x=j;
        p.y=i;
        floodFill(img, p, setGrayScale);
      }
    }
  }

  return n;
}
