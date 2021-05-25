#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const char ENDOFLINE = '.';
const char SEPERATOR = ',';
const char OPENBLOCK = '[';
const char CLOSEBLOCK = ']';

char *KEYWORDS[9] = {"int", "move", "to", "loop", "times", "out", "newline", "add", "sub"};

char IdentifierNameList[100][20] = { "temp" };
int identifierOrder = 0;


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
    bool flag = false;
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

bool isIntConstant(char *karakter){
    int temp_int = atoi(karakter);
    if (temp_int != 0)
    {
        return true;
    }
    else{
        return false;
    }
    
}

bool isInIdentifierList(char *kelime){
    bool flag = false;
    for (int i = 0; i < strlen(*IdentifierNameList); i++)
    {
        if (strcmp(kelime, IdentifierNameList[i])==0)
        {
            flag  = true;
        }
    }
    return flag;
}

bool isIdentifier(char *karakter , char *lastToken){
    bool isInList = isInIdentifierList(karakter);
    bool isValid = ( (strcmp(lastToken,"int")==0 || strcmp(lastToken,"to")==0 || strcmp(lastToken,"from")==0) && (strlen(karakter) <= 20) ) ? true:false;
    if (isInList)
    {
        return true;
    }
    else if (isValid)
    {
        strcpy(IdentifierNameList[identifierOrder],karakter);
        identifierOrder++;
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

// çalışmıyor
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
        for (int j = commaIndex; j < strlen(satir)-commaIndex; j++)
        {
            last[j-commaIndex] = satir[j];
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

    char *SourcefilePath = argv[1];
    char satir[150];
    int satir_sayisi = 0;
    char satirlar[150][150];

    // okunacak dosya
    FILE *sourceFile = fopen(SourcefilePath, "r");
    if (sourceFile == NULL)
    {
        printf("%s", "Dosya dizinde bulunamadi.");
        exit(0);
    }

    // yazılacak dosya
    FILE *destFile = fopen("myscript.lx","w");

    // satırları tek tek alan döngü
    while (!feof(sourceFile))
    {
        fgets(satir, 150, sourceFile);
        deleteComments(satir); //yorum bulunuyorsa satırda siliyor
        splitDot(satir); // sondaki yapışık noktaları da token alması için noktanın öncesine boşluk ekliyor
        splitComma(satir); // seperator olarak tanıması için virgülleri ayırıyor
        strcpy(satirlar[satir_sayisi], satir);
        satir_sayisi++;
    }

    int anlik_satir = 0;
    char lastToken[] = "";

    // satırları kelimelerine ayırıyor
    while (anlik_satir <= satir_sayisi)
    {
        const char ayirici[] = " ";

        char *token = strtok(satirlar[anlik_satir], ayirici);

        while (token != NULL)
        {
            printf("|||%s----%s|||\n",lastToken,token);

            if (isKeyword(token))
            {
                fprintf(destFile,"%s %s\n","Keyword",token);
            }
            else if (isEndofline(*token))
            {
                fprintf(destFile,"%s\n","Endofline");
            }

            else if (isSeperator(*token))
            {
                fprintf(destFile,"%s\n","Seperator");
            }
            
            else if (isOpenBlock(*token))
            {
                fprintf(destFile,"%s\n","OpenBlock");
            }
            
            else if (isCloseBlock(*token))
            {
                fprintf(destFile,"%s\n","CloseBlock");
            }

            else if (isStringConstant(token))
            {
                fprintf(destFile,"%s %s\n","String Constant",token);
            }

            else if (isIntConstant(token))
            {
                fprintf(destFile,"%s %s\n","IntConstant",token);
            }

            else if (isIdentifier(token,lastToken))
            {
                fprintf(destFile,"%s %s\n","Identifier",token);
            }
     
            else
            {
                fprintf(destFile,"%s\n", token);
            }

            strcpy(lastToken,token);
            token = strtok(NULL, ayirici);
        }
        anlik_satir++;
    }
    
    fclose(sourceFile);
    fclose(destFile);
    return 0;
}
