#include <stdio.h>
#include <iostream>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

#define COMMASCARA "Usando máscara"
#define SEMMASCARA "Sem máscara!"
#define COMMASCARACOR Scalar(0, 255, 0)
#define SEMMASCARACOR Scalar(0, 0, 255)
#define p Point(50,50)
#define font FONT_HERSHEY_PLAIN
#define fontScale 1 
#define thickness 1

using namespace cv;
using namespace std;

int main()
{
    double scale = 1;
    int bwThreshold = 6; // limiar
    VideoCapture capture; // capturar vídeo
    Mat frame, image, img;
    CascadeClassifier cascadeFace, cascadeEye, cascadeMouth, cascadeUpperBody;
    // carregando os arquivos cascade
    cascadeFace.load("//home//pi//Desktop//t8//haarcascades//haarcascade_frontalface_default.xml");
    cascadeEye.load("//home//pi//Desktop//t8//haarcascades//haarcascade_eye.xml");
    cascadeMouth.load("//home//pi//Desktop//t8//haarcascades//haarcascade_mcs_mouth.xml");
    cascadeUpperBody.load("//home//pi//Desktop//t8//haarcascades//haarcascade_upperbody.xml");
    capture.open(0); // caminho do vídeo
    Mat gray, blackWhite;
    Scalar color = Scalar(255, 255, 255);

    if (capture.isOpened())
    {
        std::cout << "Detecção de rosto começando\nPressione q ou Q para sair" << std::endl;
        while (1)
        {
            capture.read(frame);  // ler a imagem
            capture.read(image);
            flip(image, image, 1); // virar a imagem 1
            cvtColor(image, gray, COLOR_BGR2GRAY); // escala de cinza
            threshold(gray, blackWhite, bwThreshold, 255, THRESH_BINARY); // preto e branco
            std::vector<Rect> faces, facesBW; 
            cascadeFace.detectMultiScale(gray, faces, 1.1, 4); // detecção do rosto
            cascadeFace.detectMultiScale(blackWhite, facesBW, 1.1, 4); // detecção do rosto preto e branco
            
            if (faces.size() == 0 && facesBW.size() == 0)
                putText(image, "Nenhum rosto encontrado\n", p, font, fontScale, SEMMASCARACOR, thickness, LINE_AA);
            
            // uma máscara branca em uma imagem cinza não é vista
            else if (faces.size() == 0 && facesBW.size() == 1)
                putText(image, COMMASCARA, p, font, fontScale, COMMASCARACOR, thickness, LINE_AA);
            else {
                // Com o rosto detectado se analisa os outros pontos
                for (size_t i = 0; i < faces.size(); i ++) {
                    Rect r = faces[i];
                    std::vector<Rect> mouthRect;
                    Point p1, p2;
                    p1.x = r.x;
                    p1.y = r.y;
                    p2.x = r.x + r.width;
                    p2.y = r.y + r.height;
                    // Desenha um retângulo em torno do rosto
                    rectangle(image, p1, p2, color, 2);
                    cascadeMouth.detectMultiScale(gray, mouthRect, 1.5, 5);
                    // A boca não foi detectada mas como o rosto foi a pessoa provavelmente está usando uma máscara
                    if (mouthRect.size() == 0)
                        putText(image, COMMASCARA, p, font, fontScale, COMMASCARACOR, thickness, LINE_8);
                    else {
                        for (size_t j = 0; j < mouthRect.size(); j ++) {
                            Rect m = mouthRect[j];
                            // A boca foi detectada então a pessoa não está usando uma máscara
                            if (r.y < m.y < r.y + r.height){
                                putText(image, SEMMASCARA, p, font, fontScale, SEMMASCARACOR, thickness, LINE_8);
                                break;
                            }
                        }
                    }
                }
            }
            imshow("Detecção de máscara facial", image);
            char c = (char)waitKey(30);
            if (c == 27 || c == 'q' || c == 'Q'){
                std::cout << "Saindo" << std::endl;
                break;
            }
        }
        // fechar
        capture.release();
        destroyAllWindows();
    }
    else 
        printf("Não foi possível abrir a câmera");
return 0;
}