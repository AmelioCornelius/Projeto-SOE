#include <stdio.h>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio/videoio_c.h>

using namespace cv;
using namespace std;

void main (){
	VideoCapture cap(0); //capturar vídeo
	Mat img;

	CascadeClassifier faceCascade
	faceCascade.load("t3/haarcascade_frontalface_default.xml")

	if (faceCascade.empty()) {
		printf("Não foi possível carregar o arquivo XML\n");
	}

	vector<Rect> faces; //vetor de retangulos
	faceCascade.detectMultiScale(img, faces, 1.1, 10);

	for (int i=0; i< faces.size(); i++){
		rectangle(img, faces[i].tl(), faces[i].br(), Scalar(255, 0, 255), 3);

	}

	while (true) {
		cap.read(img);
		imshow("Image", img); //mostrar a imagem
		waitKey(1);
	}


}

//int main(int argv, char** argc)
//{
//	return 0;
//}