#include<stdio.h>
#include <stdlib.h>
#include<string.h>


char ENDOFLINE = '.';
char *KEYWORDS[9] ={"int","move","to","loop","times","out","newline","add","sub"}; 


void isValidEnd(char lastChar){
    // eğer en sonda nokta yoksa hatali statement
    if (lastChar != ENDOFLINE)
    {   
        printf("%s","satir '.' ile bitmiyor.");
        exit(0);
    }
    else{
        printf("%s","ez");
    }
}


int main(int argc, char *argv[]) //içteki şeyler cmd de parametre vermeye yariyor
{

    char* filePath = argv[1];
    char satir[150];
    int satir_sayisi = 0;
    char satirlar[150][150];
    
    FILE *file = fopen(filePath , "r");
    if (file == NULL)
    {
        printf("%s","Dosya dizinde bulunamadi.");
        exit(0);
    }

    while (!feof(file))
    {
        fgets(satir,150,file);
        strcpy(satirlar[satir_sayisi],satir);
        satir_sayisi++;
    }
    int anlik_satir = 0;

    while (anlik_satir < strlen(*satirlar))
    {
        isValidEnd(satirlar[anlik_satir][strlen(satirlar[anlik_satir])-2]);
        exit(0);
    }
    
    
    

    return 0;
}

/*
int isValidEnd(char lastChar){
    // eğer en sonda nokta yoksa hatali statement
    if (lastChar == ENDOFLINE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
    
}
*/





