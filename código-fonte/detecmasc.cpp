#include <stdio.h>
#include <time.h>
#include <iostream>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <unistd.h>

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

void arqcsv_masc(int flag)
{

    FILE *fp;

    fp=fopen("dados.csv","a");
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    if(size == 0)
    {
        fprintf(fp,"Data,Uso de Máscara,Temperatura\n");
    }
    
    //data
    int diaatual, mesatual, anoatual, horaatual, minutoatual;
    time_t agora_segundos = time(NULL);
    struct tm *agora = localtime(&agora_segundos);
    diaatual = agora->tm_mday;
    mesatual = agora->tm_mon + 1;
    anoatual = agora->tm_year + 1900;
    horaatual = agora->tm_hour;
    minutoatual = agora->tm_min;
    fprintf(fp,"%i/%i/%i %i:%i,", diaatual, mesatual, anoatual, horaatual, minutoatual);

    if(flag == 1)
    {
        fprintf("Usando máscara,");
    }
    if(flag == 0)
    {
        fprintf("Sem máscara,");
    }
    fclose(fp);
}

int main()
{
    double scale = 1;
    int bwThreshold = 6; // limiar
    VideoCapture capture; // capturar vídeo
    Mat image, img; 
    CascadeClassifier cascadeFace, cascadeMouth;
    // carregando os arquivos cascade
    cascadeFace.load("//home//pi//Projeto-SOE//código-fonte//haarcascades//haarcascade_frontalface_default.xml");
    cascadeMouth.load("//home//pi//Projeto-SOE//código-fonte//haarcascades//haarcascade_mcs_mouth.xml");
    capture.open(0); // caminho do vídeo
    Mat gray, blackWhite;
    Scalar color = Scalar(255, 255, 255);

    int flag = 0;

    if (capture.isOpened())
    {
        std::cout << "Detecção de rosto começando\nPressione q ou Q para sair" << std::endl;
        while (1)
        {
            capture.read(image); // ler a imagem
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
            {
                putText(image, COMMASCARA, p, font, fontScale, COMMASCARACOR, thickness, LINE_AA);
                flag = 1;
            }
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
                    {
                        putText(image, COMMASCARA, p, font, fontScale, COMMASCARACOR, thickness, LINE_8);
                        flag = 1;
                    }
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

            if (flag == 1)
            {
                arqcsv_masc(flag);
                sleep(15);
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
