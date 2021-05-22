#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char ENDOFLINE = '.';
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

    while (!feof(file))
    {
        fgets(satir, 150, file);
        strcpy(satirlar[satir_sayisi], satir);
        satir_sayisi++;
    }

    int anlik_satir = 0;

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
