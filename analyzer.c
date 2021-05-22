#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char ENDOFLINE = '.';
const char SEPERATOR = ',';
char *KEYWORDS[9] = {"int", "move", "to", "loop", "times", "out", "newline", "add", "sub"};

void isValidEnd(char lastChar)
{
    // eğer en sonda nokta yoksa hatali statement
    if (lastChar != ENDOFLINE)
    {
        printf("%s", "satir '.' ile bitmiyor.");
        exit(0);
    }
}

int isKeyword(char *kelime){
    int flag = 0;
    for (int i = 0; i < 9; i++)
    {
        if (strcmp(kelime,KEYWORDS[i])==0)
        {
            flag = 1;
        }
    }
    return flag;
}

void deleteComments(char *satir){
    
    int index1 = strcspn(satir,"{");
    if (index1+2 > strlen(satir))
    {
        return;
    }
    else
    {
        int index2 = strcspn(satir,"}");
        if (index2+2 > strlen(satir))
        {
            return;
        }
        else
        {
                    
            for (int i = index1; i <= index2; i++)
            {
                satir[i] = '\0';
            }
        }

    }
}

void splitDot(char *satir){
    int dotIndex = strcspn(satir,".");
    if (dotIndex < strlen(satir))
    {
        satir[dotIndex] = '\0';
        strcat(satir," .");
    }
    else
    {
        return;
    }
    
}

int main(int argc, char *argv[]) //içteki şeyler cmd de parametre vermeye yariyor
{

    char *filePath = argv[1];
    char satir[150];
    int satir_sayisi = 0;
    char satirlar[150][150];

    FILE *file = fopen(filePath, "r");
    if (file == NULL)
    {
        printf("%s", "Dosya dizinde bulunamadi.");
        exit(0);
    }

    // satırları tek tek alan döngü
    while (!feof(file))
    {
        fgets(satir, 150, file);
        deleteComments(satir); //yorum bulunuyorsa satırda siliyor
        splitDot(satir); // sondaki yapışık noktaları da token alması için noktanın öncesine boşluk ekliyor
        strcpy(satirlar[satir_sayisi], satir);
        satir_sayisi++;
        printf("%s\n",satir);
    }

    printf("%s\n","------------------------------------");
    int anlik_satir = 0;

    // satırları kelimelerine ayırıyor
    while (anlik_satir <= satir_sayisi)
    {
        const char ayirici[] = " ";

        char *token = strtok(satirlar[anlik_satir], ayirici);

        while (token != NULL)
        {
            if (isKeyword(token)==1)
            {
                printf("%s %s\n","Keyword",token);
            }
            else
            {
                printf("%s\n", token);
            }
            
            token = strtok(NULL, ayirici);
        }
        anlik_satir++;
    }
    
    return 0;
}
