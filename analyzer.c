#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const char ENDOFLINE = '.';
const char SEPERATOR = ',';
const char OPENBLOCK = '[';
const char CLOSEBLOCK = ']';

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

bool isKeyword(char *kelime){
    int flag = false;
    for (int i = 0; i < 9; i++)
    {
        if (strcmp(kelime,KEYWORDS[i])==0)
        {
            flag = true;
        }
    }
    return flag;
}

bool isEndofline(char karakter){
    if (karakter == ENDOFLINE)
    {
        return true;
    }
    else {
        return false;
    }
}

bool isSeperator(char karakter){
    if (karakter == SEPERATOR)
    {
        return true;
    }
    else {
        return false;
    }
}

bool isOpenBlock(char karakter){
    if (karakter == OPENBLOCK)
    {
        return true;
    }
    else {
        return false;
    }
}

bool isCloseBlock(char karakter){
    if (karakter == CLOSEBLOCK)
    {
        return true;
    }
    else {
        return false;
    }
}

bool isStringConstant(char *karakter){
    if (karakter[0]== '"' && karakter[strlen(karakter)-1]=='"')
    {
        return true;
    }
    else{
        return false;
    }
    
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

void splitComma(char *satir){
    int commaIndex = strcspn(satir,",");
    if (commaIndex < strlen(satir))
    {
        char first[commaIndex+1];
        for (int i = 0; i < commaIndex; i++)
        {
            first[i] = satir[i];
        }
        
        char last[strlen(satir)-commaIndex+1];
        for (int j = 0; j < strlen(satir)-commaIndex; j++)
        {
            last[j] = satir[j];
        }
        
        strcat(first," ,");
        strcat(first,last);
        
        satir = first;
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
        splitComma(satir); // seperator olarak tanıması için virgülleri ayırıyor
        strcpy(satirlar[satir_sayisi], satir);
        satir_sayisi++;
    }

    int anlik_satir = 0;

    // satırları kelimelerine ayırıyor
    while (anlik_satir <= satir_sayisi)
    {
        const char ayirici[] = " ";

        char *token = strtok(satirlar[anlik_satir], ayirici);

        while (token != NULL)
        {
            if (isKeyword(token))
            {
                printf("%s %s\n","Keyword",token);
            }
            else if (isEndofline(*token))
            {
                printf("%s\n","Endofline");
            }

            else if (isSeperator(*token))
            {
                printf("%s\n","Seperator");
            }
            
            else if (isOpenBlock(*token))
            {
                printf("%s\n","OpenBlock");
            }
            
            else if (isCloseBlock(*token))
            {
                printf("%s\n","CloseBlock");
            }

            else if (isStringConstant(token))
            {
                printf("%s %s\n","String Constant",token);
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
