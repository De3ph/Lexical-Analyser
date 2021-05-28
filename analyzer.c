#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

const char ENDOFLINE = '.';
const char SEPERATOR[] = "Seperator";
const char OPENBLOCK = '[';
const char CLOSEBLOCK = ']';

char *KEYWORDS[9] = {"int", "move", "to", "loop", "times", "out", "newline", "add", "sub"};

char IdentifierNameList[100][20] = {"temp"};
int identifierOrder = 0;

int substring(char *source, int from, int n, char *target)
{
    int length, i;
    for (length = 0; source[length] != '\0'; length++)
        ;
    if (from > length)
    {
        printf("Starting index is invalid.\n");
        return 1;
    }
    if ((from + n) > length)
    {
        n = (length - from);
    }
    for (i = 0; i < n; i++)
    {
        target[i] = source[from + i];
    }
    target[i] = '\0';
    return 0;
}

void isValidEnd(char lastChar)
{
    // eğer en sonda nokta yoksa hatali statement
    if (lastChar != ENDOFLINE)
    {
        printf("%s", "satir '.' ile bitmiyor.");
        exit(1);
    }
}

bool isKeyword(char *kelime)
{
    bool flag = false;
    for (int i = 0; i < 9; i++)
    {
        if (strcmp(kelime, KEYWORDS[i]) == 0)
        {
            flag = true;
        }
    }
    return flag;
}

bool isEndofline(char karakter)
{
    return karakter == ENDOFLINE ? true : false;
}

bool isSeperator(char *karakter)
{
    return strcmp(karakter, SEPERATOR) == 0 ? true : false;
}

bool isOpenBlock(char karakter)
{
    return karakter == OPENBLOCK ? true : false;
}

bool isCloseBlock(char karakter)
{
    return karakter == CLOSEBLOCK ? true : false;
}

bool isValidCodeBlock(char *satir,int anlik_satir,char *satirlar){
    
}

bool isStringConstant(char *karakter)
{
    return (karakter[0] == '"' && karakter[strlen(karakter) - 1] == '"') ? true : false;
}

//tam doğru değil,
bool isIntConstant(char *karakter)
{
    int temp_int = *karakter;
    return isdigit(temp_int) != 0 ? true : false;
}

bool isInIdentifierList(char *kelime)
{
    bool flag = false;
    for (int i = 0; i < strlen(*IdentifierNameList); i++)
    {
        if (strcmp(kelime, IdentifierNameList[i]) == 0)
        {
            flag = true;
        }
    }
    return flag;
}

bool isIdentifier(char *karakter, char *lastToken)
{
    bool isInList = isInIdentifierList(karakter);
    int first_letter = karakter[0];
    bool isValid = ((strcmp(lastToken, "int") == 0 || strcmp(lastToken, "to") == 0 || strcmp(lastToken, "from") == 0 || strcmp(lastToken, "loop") == 0 || strcmp(lastToken, "add") == 0 || strcmp(lastToken, "out") == 0 || strcmp(lastToken, ",") == 0) && (strlen(karakter) <= 20) && (isalpha(first_letter) != 0)) ? true : false;
    if (isInList)
    {
        return true;
    }
    else if (isValid)
    {
        strcpy(IdentifierNameList[identifierOrder], karakter);
        identifierOrder++;
        return true;
    }

    else
    {
        return false;
    }
}

//alt satıra sarkan yorum satırları sıkıntılı
void deleteComments(char *satir)
{

    int index1 = strcspn(satir, "{");
    if (index1 + 2 > strlen(satir))
    {
        return;
    }
    else
    {
        int index2 = strcspn(satir, "}");
        if (index2 + 2 > strlen(satir))
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

// hatalı, içte nokta varsa onu ayırıyor
void splitDot(char *satir)
{
    int dotIndex = strcspn(satir, ".");
    if (dotIndex == strlen(satir) - 2 || dotIndex == strlen(satir) - 1)
    {
        satir[dotIndex] = '\0';
        strcat(satir, " .");
    }
}

void splitComma(char *satir)
{
    int commaIndex = strcspn(satir, ",");
    while (commaIndex < strlen(satir))
    {
        char first[commaIndex];
        char second[strlen(satir) - commaIndex];
        substring(satir, 0, commaIndex, first);
        substring(satir, commaIndex + 1, strlen(satir) - 1, second);
        strcat(first, " Seperator");
        strcat(first, second);
        strcpy(satir, first);
        commaIndex = strcspn(satir, ",");
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
        printf("%s", "Dosya dizinde bulunamadi. Lütfen dizini kontrol edin.");
        exit(1);
    }

    // yazılacak dosya
    FILE *destFile = fopen("myscript.lx", "w");

    // satırları tek tek alan döngü
    while (!feof(sourceFile))
    {
        fgets(satir, 150, sourceFile);
        deleteComments(satir); //yorum bulunuyorsa satırda siliyor
        splitDot(satir);       // sondaki yapışık noktaları da token alması için noktanın öncesine boşluk ekliyor
        splitComma(satir);     // seperator olarak tanıması için virgülleri ayırıyor
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
            //int state kontrolü
            if (strcmp(token, "int") == 0)
            {
                fprintf(destFile, "Keyword %s\n", token);
                strcpy(lastToken, token);
                token = strtok(NULL, ayirici);

                if (isIdentifier(token, lastToken))
                {
                    fprintf(destFile, "%s %s\n", "Identifier", token);
                    strcpy(lastToken, token);
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("%s Identifier degil.", token);
                    exit(1);
                }

                if (isEndofline(*token))
                {
                    fprintf(destFile, "%s\n", "Endofline");
                    break;
                }
                else
                {
                    printf("%s Endofline degil.", token);
                }
            }

            //move state kontrolü
            else if (strcmp(token, "move") == 0)
            {
                fprintf(destFile, "Keyword %s\n", token);
                strcpy(lastToken, token);
                token = strtok(NULL, ayirici);

                if (isIntConstant(token) || isIdentifier(token, lastToken))
                {
                    if (isIntConstant(token))
                    {
                        fprintf(destFile, "IntConstant %s\n", token);
                        strcpy(lastToken, token);
                        token = strtok(NULL, ayirici);
                    }
                    else
                    {
                        fprintf(destFile, "Identifier %s\n", token);
                        strcpy(lastToken, token);
                        token = strtok(NULL, ayirici);
                    }
                }
                else
                {
                    printf("%s IntConstant degil.", token);
                    exit(1);
                }

                if (strcmp(token, "to") == 0)
                {
                    fprintf(destFile, "Keyword %s\n", token);
                    strcpy(lastToken, token);
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("%s 'to' anahtar kelimesine eşit değil.", token);
                    exit(1);
                }

                if (isIdentifier(token, lastToken))
                {
                    fprintf(destFile, "Identifier %s\n", token);
                    strcpy(lastToken, token);
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("%s Identifier degil.", token);
                    exit(1);
                }
                if (isEndofline(*token))
                {
                    fprintf(destFile, "%s\n", "Endofline");
                    break;
                }
                else
                {
                    printf("%s Endofline degil.", token);
                }
            }

            //add state kontrolü
            else if (strcmp(token, "add") == 0)
            {
                fprintf(destFile, "Keyword %s\n", token);
                strcpy(lastToken, token);
                token = strtok(NULL, ayirici);

                if (isIntConstant(token) || isIdentifier(token, lastToken))
                {
                    if (isIntConstant(token))
                    {
                        fprintf(destFile, "IntConstant %s\n", token);
                        strcpy(lastToken, token);
                        token = strtok(NULL, ayirici);
                    }
                    else
                    {
                        fprintf(destFile, "Identifier %s\n", token);
                        strcpy(lastToken, token);
                        token = strtok(NULL, ayirici);
                    }
                }
                else
                {
                    printf("%s IntConstant degil.", token);
                    exit(1);
                }

                if (strcmp(token, "to") == 0)
                {
                    fprintf(destFile, "Keyword %s\n", token);
                    strcpy(lastToken, token);
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("%s 'to' anahtar kelimesine eşit değil.", token);
                    exit(1);
                }

                if (isIdentifier(token, lastToken))
                {
                    fprintf(destFile, "Identifier %s\n", token);
                    strcpy(lastToken, token);
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("%s Identifier degil.", token);
                    exit(1);
                }
                if (isEndofline(*token))
                {
                    fprintf(destFile, "%s\n", "Endofline");
                    break;
                }
                else
                {
                    printf("%s Endofline degil.", token);
                }
            }

            //sub state kontrolü
            else if (strcmp(token, "sub") == 0)
            {
                fprintf(destFile, "Keyword %s\n", "sub");
                strcpy(lastToken, token);
                token = strtok(NULL, ayirici);

                if (isIntConstant(token) || isIdentifier(token, lastToken))
                {
                    if (isIntConstant(token))
                    {
                        fprintf(destFile, "IntConstant %s\n", token);
                        strcpy(lastToken, token);
                        token = strtok(NULL, ayirici);
                    }
                    else
                    {
                        fprintf(destFile, "Identifier %s\n", token);
                        strcpy(lastToken, token);
                        token = strtok(NULL, ayirici);
                    }
                }
                else
                {
                    printf("%s IntConstant degil.", token);
                    exit(1);
                }

                if (strcmp(token, "from") == 0)
                {
                    fprintf(destFile, "Keyword %s\n", "from");
                    strcpy(lastToken, token);
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("%s 'from' anahtar kelimesi değil.", token);
                    exit(1);
                }

                if (isIdentifier(token, lastToken))
                {
                    fprintf(destFile, "Identifier %s\n", token);
                    strcpy(lastToken, token);
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("%s Identifier degil.", token);
                    exit(1);
                }
                if (isEndofline(*token))
                {
                    fprintf(destFile, "%s\n", "Endofline");
                    break;
                }
                else
                {
                    printf("%s Endofline degil.", token);
                }
            }

            //out state kontrolü
            else if (strcmp(token, "out") == 0)
            {

                fprintf(destFile, "Keyword %s\n", "out");
                strcpy(lastToken, token);
                token = strtok(NULL, ayirici);

                while (strcmp(token, ".") != 0)
                {

                    if (isIntConstant(token) || isIdentifier(token, lastToken))
                    {
                        if (isIntConstant(token))
                        {
                            fprintf(destFile, "IntConstant %s\n", token);
                            strcpy(lastToken, token);
                            token = strtok(NULL, ayirici);
                        }
                        else
                        {
                            fprintf(destFile, "Identifier %s\n", token);
                            strcpy(lastToken, token);
                            token = strtok(NULL, ayirici);
                        }
                    }

                    else if (isStringConstant(token))
                    {
                        fprintf(destFile, "StringConstant %s\n", token);
                        strcpy(lastToken, token);
                        token = strtok(NULL, ayirici);
                    }
                    else if (strcmp(token, "newline") == 0)
                    {
                        fprintf(destFile, "Keyword %s\n", token);
                        strcpy(lastToken, token);
                        token = strtok(NULL, ayirici);
                    }
                    else if (isSeperator(token))
                    {
                        fprintf(destFile, "Seperator\n");
                        strcpy(lastToken, token);
                        token = strtok(NULL, ayirici);
                    }

                    else
                    {
                        printf("Out gramerine aykiri girdi. %s", token);
                        exit(1);
                    }
                }

                if (isEndofline(*token))
                {
                    fprintf(destFile, "%s\n", "Endofline");
                    break;
                }
                else
                {
                    printf("%s Endofline degil.", token);
                    exit(1);
                }
            }

            //loop state kontrolü
            else if (strcmp(token, "loop") == 0)
            {
                fprintf(destFile, "Keyword %s\n", "loop");
                strcpy(lastToken, token);
                token = strtok(NULL, ayirici);

                if (isIntConstant(token) || isIdentifier(token, lastToken))
                {
                    if (isIntConstant(token))
                    {
                        fprintf(destFile, "IntConstant %s\n", token);
                        strcpy(lastToken, token);
                        token = strtok(NULL, ayirici);
                    }
                    else
                    {
                        fprintf(destFile, "Identifier %s\n", token);
                        strcpy(lastToken, token);
                        token = strtok(NULL, ayirici);
                    }
                }
                else
                {
                    printf("Loop gramerine aykiri girdi. %s", token);
                    exit(1);
                }

                if (strcmp(token, "times"))
                {
                    fprintf(destFile, "Keyword %s\n", "loop");
                    strcpy(lastToken, token);
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("%s, times anahtar kelimesi degil.",token);
                    exit(1);
                }

                // line or code block
                



            }
            else
            {
                //hatalı
                printf("%s Geçerli bir keyword degil.", token);
                exit(1);
            }

            // strcpy(lastToken, token);
            // token = strtok(NULL, ayirici);
        }

        /*
        while (token != NULL)
        {
            
            if (isKeyword(token))
            {
                fprintf(destFile, "%s %s\n", "Keyword", token);
            }
            else if (isEndofline(*token))
            {
                fprintf(destFile, "%s\n", "Endofline");
            }

            else if (isSeperator(*token))
            {
                fprintf(destFile, "%s\n", "Seperator");
            }

            else if (isOpenBlock(*token))
            {
                fprintf(destFile, "%s\n", "OpenBlock");
            }

            else if (isCloseBlock(*token))
            {
                fprintf(destFile, "%s\n", "CloseBlock");
            }

            else if (isStringConstant(token))
            {
                fprintf(destFile, "%s %s\n", "String Constant", token);
            }

            else if (isIntConstant(token))
            {
                fprintf(destFile, "%s %s\n", "IntConstant", token);
            }

            else if (isIdentifier(token, lastToken))
            {
                fprintf(destFile, "%s %s\n", "Identifier", token);
            }

            else
            {
                fprintf(destFile, "%s\n", token);
            }
            
            strcpy(lastToken, token);
            token = strtok(NULL, ayirici);
        }
        */
        anlik_satir++;
    }

    fclose(sourceFile);
    fclose(destFile);
    return 0;
}
