#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

#define RADIUS 20
#define MAX_TRACKBAR_VALUE 100
#define TRACKBAR_WINDOW_NAME "Filtered Image"

void deslocaDFT(Mat&);
void displayFilter(Mat&);
void homomorphic_trackbar_handler(int, void*);

int valueH, valueL, d0, c0, dft_M, dft_N;
Mat filter, img, tmpImg;

int main(int argv, char** argc){
  Mat filteredImg, padded, complexImg, grayImg;
  Mat_<float> realInput, zeros;
  vector<Mat> planes;
  char key;

  namedWindow(TRACKBAR_WINDOW_NAME, CV_WINDOW_AUTOSIZE);
  createTrackbar("Gamma H:", TRACKBAR_WINDOW_NAME, &valueH, MAX_TRACKBAR_VALUE, homomorphic_trackbar_handler);
  createTrackbar("Gamma L:", TRACKBAR_WINDOW_NAME, &valueL, MAX_TRACKBAR_VALUE, homomorphic_trackbar_handler);
  createTrackbar("D0:", TRACKBAR_WINDOW_NAME, &d0, MAX_TRACKBAR_VALUE, homomorphic_trackbar_handler);
  createTrackbar("C0:", TRACKBAR_WINDOW_NAME, &c0, MAX_TRACKBAR_VALUE, homomorphic_trackbar_handler);


  img = imread("pulpfiction.jpg", CV_LOAD_IMAGE_GRAYSCALE);

  // identifica os tamanhos otimos para
  // calculo do FFT
  dft_M = getOptimalDFTSize(img.rows);
  dft_N = getOptimalDFTSize(img.cols);

  // realiza o padding da imagem
  copyMakeBorder(img, padded, 0,
                 dft_M - img.rows, 0,
                 dft_N - img.cols,
                 BORDER_CONSTANT, Scalar::all(0));

   // parte imaginaria da matriz complexa (preenchida com zeros)
   zeros = Mat_<float>::zeros(padded.size());

   // prepara a matriz complexa para ser preenchida
   complexImg = Mat(padded.size(), CV_32FC2, Scalar(0));

   // a função de transferência (filtro frequencial) deve ter o
   // mesmo tamanho e tipo da matriz complexa
   filter = complexImg.clone();

   // cria uma matriz temporária para criar as componentes real
   // e imaginaria do filtro ideal
   tmpImg = Mat(dft_M, dft_N, CV_32F);
   while(1){
     grayImg = img.clone();
     imshow("Original Image", grayImg);
     // realiza o padding da imagem
     copyMakeBorder(grayImg, padded, 0,
                     dft_M - img.rows, 0,
                     dft_N - img.cols,
                     BORDER_CONSTANT, Scalar::all(0));

     // limpa o array de matrizes que vao compor a
     // imagem complexa
     planes.clear();
     // cria a compoente real
     realInput = Mat_<float>(padded);
     realInput += Scalar::all(1);
     log(realInput,realInput);

     // insere as duas componentes no array de matrizes
     planes.push_back(realInput);
     planes.push_back(zeros);

     // combina o array de matrizes em uma unica
     // componente complexa
     merge(planes, complexImg);

     // calcula o dft
     dft(complexImg, complexImg);

     // realiza a troca de quadrantes
     deslocaDFT(complexImg);

     // aplica o filtro frequencial
     mulSpectrums(complexImg, filter, complexImg, 0);

     // troca novamente os quadrantes
     deslocaDFT(complexImg);

     // calcula a DFT inversa
     idft(complexImg, complexImg, DFT_SCALE);

     // limpa o array de planos
     planes.clear();

     // separa as partes real e imaginaria da
     // imagem filtrada
     split(complexImg, planes);

     // calcular expodencial
     exp(planes[0], planes[0]);

     // normaliza a parte real para exibicao
     normalize(planes[0], planes[0], 0, 1, CV_MINMAX);
     imshow(TRACKBAR_WINDOW_NAME, planes[0]);
     planes[0].convertTo(filteredImg, CV_8UC1, 255.0);
     imwrite("filteredImg.png", filteredImg);

     displayFilter(filteredImg);

     key = (char) waitKey(10);
     if( key == 27 ) break; // esc pressed!
   }

   return 0;
}

void displayFilter(Mat& filteredImg){
  Mat result;
  vector<Mat> filterMat;
  split(filteredImg, filterMat);
  normalize(filterMat[0], filterMat[0], 0, 1, CV_MINMAX);
  filterMat[0].convertTo(result, CV_8UC1, 255.0);
  imshow("filter", result);
  imwrite("filter.png", result);
}

void homomorphic_trackbar_handler(int, void*){
  double gammaH = valueH/10.0;
  double gammaL = valueL/10.0;
  double d_val = d0/10.0;
  double c_val = c0/1000.0;

  for(int i = 0; i < dft_M; i++){
    for(int j = 0; j < dft_N; j++){
      double D = pow(i - dft_M/2, 2) + pow(j - dft_N/2, 2);
      tmpImg.at<float> (i,j) = (gammaH - gammaL) * (1 - exp(-c_val*D/pow(d_val, 2))) + gammaL;
    }
  }

  Mat parts[]= {tmpImg, tmpImg};
  merge(parts, 2, filter);
}
// troca os quadrantes da imagem da DFT
void deslocaDFT(Mat& image){
  Mat tmp, A, B, C, D;

  // se a imagem tiver tamanho impar, recorta a regiao para
  // evitar cópias de tamanho desigual
  image = image(Rect(0, 0, image.cols & -2, image.rows & -2));
  int cx = image.cols/2;
  int cy = image.rows/2;

  // reorganiza os quadrantes da transformada
  // A B   ->  D C
  // C D       B A
  A = image(Rect(0, 0, cx, cy));
  B = image(Rect(cx, 0, cx, cy));
  C = image(Rect(0, cy, cx, cy));
  D = image(Rect(cx, cy, cx, cy));

  // A <-> D
  A.copyTo(tmp);  D.copyTo(A);  tmp.copyTo(D);

  // C <-> B
  C.copyTo(tmp);  B.copyTo(C);  tmp.copyTo(B);
}
