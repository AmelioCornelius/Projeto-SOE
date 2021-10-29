//gcc mlx90614.c -o temp -l bcm2835
//sudo ./temp
#include <stdio.h>
#include <time.h>
#include <bcm2835.h> // biblioteca necessária para o sensor
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#define AVG 1   //média das amostras
 

void arqcsv_temp(float ctemp)
{
    //dados=strcat(dados,".csv");

    FILE *fp;

    fp=fopen("dados.csv","a");
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    if(size == 0)
    {
        fprintf(fp,"Data,Temperatura\n");
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

    fprintf(fp,"%.2f\n", ctemp);
    fclose(fp);
}
 
 
int main(int argc, char **argv)
{
    unsigned char buf[6];
    unsigned char i, reg;
    double temp = 0,calc = 0, ctemp, atemp;
    bcm2835_init();
    bcm2835_i2c_begin();
    bcm2835_i2c_set_baudrate(25000); // taxa de transmissão
    // endereço
    bcm2835_i2c_setSlaveAddress(0x5a);
 
    printf("\nMedindo!!\n");
 
    calc = 0; // variável auxiliar
    reg = 7;
 
    for(i=0;i<AVG;i++)
	{
        bcm2835_i2c_begin(); // início da comunicação i2c
        bcm2835_i2c_write (&reg, 1);
        bcm2835_i2c_read_register_rs(&reg,&buf[0],3);
        temp = (double) (((buf[1]) << 8) + buf[0]);
        temp = (temp * 0.02)-0.01;
        temp = temp - 268.1; // conversão para celsius e ajustes
        calc+=temp;
        sleep(1);
    }
 
    ctemp = calc/AVG; // temperatura corporal média
    arqcsv_temp(ctemp);
    calc = 0;
    reg = 6;
 
    for(i = 0; i < AVG; i++){
        bcm2835_i2c_begin();
        bcm2835_i2c_write (&reg, 1);
        bcm2835_i2c_read_register_rs(&reg,&buf[0],3);
        temp = (double) (((buf[1]) << 8) + buf[0]);
        temp = (temp * 0.02)-0.01;
        temp = temp - 273.15;
        calc += temp;
        sleep(1);
    }
 
    atemp = calc/AVG; // temperatura ambiente média
 
    printf("Temperatura Ambiente = %04.2f\n", atemp);
    printf("Temperatura Corporal = %04.2f\n", ctemp);
    
    // diagnostico conforme o valor da temperatura corporal 
    if(35.0 <ctemp< 37.7)
        printf("Temperatura normal\n");
    else if (ctemp <35.0)
        printf("Hipotermia\n");
    else if (ctemp > 37.7)
        printf("Febre\n");


    return 0;
}
