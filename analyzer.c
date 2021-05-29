#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

const char ENDOFLINE = '.';
const char SEPERATOR[] = "Seperator";
const char OPENBLOCK = '[';
const char CLOSEBLOCK = ']';

char *KEYWORDS[11] = {"int", "move", "to", "loop", "times", "out", "newline", "add", "sub", "[", "]"};

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
    // eger en sonda nokta yoksa hatali statement
    if (lastChar != ENDOFLINE)
    {
        printf("%s", "satir '.' ile bitmiyor.");
        exit(0);
    }
}

bool isKeyword(char *kelime)
{
    bool flag = false;
    for (int i = 0; i < 11; i++)
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

bool isStringConstant(char *karakter)
{
    return (karakter[0] == '"' && karakter[strlen(karakter) - 1] == '"') ? true : false;
}

//tam dogru degil,
bool isIntConstant(char *karakter)
{
    /*
    int temp_int = *karakter;
    return isdigit(temp_int) != 0 ? true : false;
    */

    int len = strlen(karakter);

	if (len > 100) {
		return false;
	}

	for (int i = 0; i < len; i++) {
		if (i > 0) {
			if (karakter[i] == '-' || len == 1) {
				return false;
			}
		}
		if (!(isdigit(karakter[i]) || karakter[i] == '-')) {
			return false;
		}
	}

	return true;
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
    bool isValid = ((strcmp(lastToken, "int") == 0 || strcmp(lastToken, "to") == 0 || strcmp(lastToken, "from") == 0 || strcmp(lastToken, "loop") == 0 || strcmp(lastToken, "add") == 0 || strcmp(lastToken, "out") == 0 || strcmp(lastToken, ",") == 0) && (strlen(karakter) <= 20) && (isalpha(first_letter) != 0) && (isKeyword(karakter) == false) ) ? true : false;
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

//alt satira sarkan yorum satirlari sikintili
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

// hatali, icte nokta varsa onu ayiriyor
void splitDot(char *satir)
{
    static int satir_no = 1;
    int dotIndex = strcspn(satir, ".");
    char *p = strchr(satir,'.');

    if (dotIndex == strlen(satir) - 2 || dotIndex == strlen(satir) - 1)
    {
        satir[dotIndex] = '\0';
        strcat(satir, " .");
    }
    else
    {
        if (p == NULL)
        {
            return;
        }
        printf("%i. satirda hata. \'.\' dan sonra girdi olamaz.",satir_no);
        exit(0);
    }
    satir_no++;
    
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

int main(int argc, char *argv[]) //icteki seyler cmd de parametre vermeye yariyor
{

    //char *SourcefilePath = argv[1];
    char *SourcefilePath = "./try.ba";

    char satir[150];
    int satir_sayisi = 0;
    char satirlar[150][150];

    // okunacak dosya
    FILE *sourceFile = fopen(SourcefilePath, "r");
    if (sourceFile == NULL)
    {
        printf("%s", "Dosya dizinde bulunamadi. Lutfen dizini kontrol edin.");
        exit(0);
    }

    // yazilacak dosya
    FILE *destFile = fopen("myscript.lx", "w");

    // satirlari tek tek alan dongu
    while (!feof(sourceFile))
    {
        fgets(satir, 150, sourceFile);
        deleteComments(satir); //yorum bulunuyorsa satirda siliyor
        splitDot(satir);       // sondaki yapisik noktalari da token almasi icin noktanin oncesine bosluk ekliyor
        splitComma(satir);     // seperator olarak tanimasi icin virgulleri ayiriyor
        strcpy(satirlar[satir_sayisi], satir);
        satir_sayisi++;
    }

    int anlik_satir = 0;
    char lastToken[] = "";

    // satirlari kelimelerine ayiriyor
    while (anlik_satir <= satir_sayisi)
    {
        const char ayirici[] = " ";

        char *token = strtok(satirlar[anlik_satir], ayirici);

        while (token != NULL)
        {

            //int state kontrolu

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
                    printf("%i. satirda hata. %s Identifier degil.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (isEndofline(*token))
                {
                    fprintf(destFile, "%s\n", "Endofline");
                    break;
                }
                else
                {
                    printf("%i. satirda hata. %s Endofline degil.\n", anlik_satir + 1, token);
                }
            }

            //move state kontrolu

            if (strcmp(token, "move") == 0)
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
                    printf("%i. satirda hata. %s IntConstant degil.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (strcmp(token, "to") == 0)
                {
                    fprintf(destFile, "Keyword %s\n", token);
                    strcpy(lastToken, token);
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("%i. satirda hata. %s 'to' anahtar kelimesine esit degil.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (isIdentifier(token, lastToken))
                {
                    fprintf(destFile, "Identifier %s\n", token);
                    strcpy(lastToken, token);
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("%i. satirda hata. %s Identifier degil.\n", anlik_satir + 1, token);
                    exit(0);
                }
                if (isEndofline(*token))
                {
                    fprintf(destFile, "%s\n", "Endofline");
                    break;
                }
                else
                {
                    printf("%i. satirda hata. %s Endofline degil.\n", anlik_satir + 1, token);
                }
            }

            //add state kontrolu

            if (strcmp(token, "add") == 0)
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
                    printf("%i. satirda hata. %s IntConstant degil.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (strcmp(token, "to") == 0)
                {
                    fprintf(destFile, "Keyword %s\n", token);
                    strcpy(lastToken, token);
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("%i. satirda hata. %s 'to' anahtar kelimesine esit degil.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (isIdentifier(token, lastToken))
                {
                    fprintf(destFile, "Identifier %s\n", token);
                    strcpy(lastToken, token);
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("%i. satirda hata. %s Identifier degil.\n", anlik_satir + 1, token);
                    exit(0);
                }
                if (isEndofline(*token))
                {
                    fprintf(destFile, "%s\n", "Endofline");
                    break;
                }
                else
                {
                    printf("%i. satirda hata. %s Endofline degil.\n", anlik_satir + 1, token);
                }
            }

            //sub state kontrolu

            if (strcmp(token, "sub") == 0)
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
                    printf("%i. satirda hata. %s IntConstant degil.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (strcmp(token, "from") == 0)
                {
                    fprintf(destFile, "Keyword %s\n", "from");
                    strcpy(lastToken, token);
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("%i. satirda hata. %s 'from' anahtar kelimesi degil.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (isIdentifier(token, lastToken))
                {
                    fprintf(destFile, "Identifier %s\n", token);
                    strcpy(lastToken, token);
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("%i. satirda hata. %s Identifier degil.\n", anlik_satir + 1, token);
                    exit(0);
                }
                if (isEndofline(*token))
                {
                    fprintf(destFile, "%s\n", "Endofline");
                    break;
                }
                else
                {
                    printf("%i. satirda hata. %s Endofline degil.\n", anlik_satir + 1, token);
                }
            }

            //out state kontrolu

            if (strcmp(token, "out") == 0)
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
                        printf("%i. satirda hata. Out gramerine aykiri girdi. %s\n", anlik_satir + 1, token);
                        exit(0);
                    }
                }

                if (isEndofline(*token))
                {
                    fprintf(destFile, "%s\n", "Endofline");
                    break;
                }
                else
                {
                    printf("%i. satirda hata. %s Endofline degil.\n", anlik_satir + 1, token);
                    exit(0);
                }
            }

            //loop state kontrolu

            if (strcmp(token, "loop") == 0)
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
                    printf("%i. satirda hata. Loop gramerine aykiri girdi. %s\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (strcmp(token, "times") == 0 || strcmp(token, "times\n") == 0)
                {
                    fprintf(destFile, "Keyword %s\n", "times");
                    strcpy(lastToken, token);
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("%i. satirda hata. %s, times anahtar kelimesi degil.\n", anlik_satir + 1, token);
                    exit(0);
                }
                continue;
            }

            if (strcmp(token, "[\n") == 0 || strcmp(token, "[") == 0)
            {
                bool isFound = false;
                while (isFound != true)
                {
                    for (int i = anlik_satir; i < satir_sayisi; i++)
                    {
                        if (strchr(satirlar[i], ']') != NULL)
                        {
                            isFound = true;
                            break;
                        }
                    }
                }
                if (isFound)
                {
                    fprintf(destFile, "Open Block\n");
                    strcpy(lastToken, token);
                    token = strtok(NULL, ayirici);
                }
                continue;
            }
            if (strcmp(token, "]") == 0 || strcmp(token, "]\n") == 0)
            {
                fprintf(destFile, "CloseBlock\n");
                strcpy(lastToken, token);
                token = strtok(NULL, ayirici);
            }

            else
            {
                //hatali
                printf("%i. satirda hata. %s Gecerli bir keyword degil.\n", anlik_satir + 1, token);
                exit(0);
            }

            // strcpy(lastToken, token);
            // token = strtok(NULL, ayirici);
        }
        anlik_satir++;
    }

    fclose(sourceFile);
    fclose(destFile);
    return 0;
}
