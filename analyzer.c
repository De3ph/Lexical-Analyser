#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

const char SEPERATOR[] = "Seperator";
const char OPENBLOCK = '[';
const char CLOSEBLOCK = ']';

char *KEYWORDS[9] = {"int", "move", "to", "loop", "times", "out", "newline", "add", "sub"};

char IdentifierNameList[100][20] = {"temp"};
int IdentifierNameListLength = 0;

int IdentifierValueList[1000] = {0};
int IdentifierValueOrder = 0;

char satir[500];
int satir_sayisi = 0;
char satirlar[500][500];

char codeBlockLines[500][500];
int codeBlockLineOrder = 0;

typedef struct node
{
    int intVal;
    char *identNameVal;
    char *strVal;
} node;

node create_node()
{
    node temp_node;
    temp_node.intVal = -1000000;
    temp_node.identNameVal = "none";
    temp_node.strVal = "none";
    return temp_node;
}

typedef struct out_list
{
    node list[200];
} out_list;

int out_list_order = 0;
int out_list_start = 0;

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

bool isSeperator(char *karakter)
{
    return strcmp(karakter, SEPERATOR) == 0 ? true : false;
}

//hatalı
bool isStringConstant(char *karakter)
{
    return (karakter[0] == '"' && karakter[strlen(karakter) - 1] == '"') ? true : false;
}

bool isIntConstant(char *karakter)
{
    int len = strlen(karakter);

    if (len > 100)
    {
        return false;
    }

    for (int i = 0; i < len; i++)
    {
        if (i > 0)
        {
            if (karakter[i] == '-' || len == 1)
            {
                return false;
            }
        }
        if (!(isdigit(karakter[i]) || karakter[i] == '-'))
        {
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

bool isIdentifier(char *karakter)
{
    int len = strlen(karakter);

    if (len > 20)
    {
        return false;
    }

    for (int i = 0; i < len; i++)
    {
        if (karakter[0] == '_')
        {
            return false;
        }
        if (!((karakter[i] >= 'a' && karakter[i] <= 'z') || (karakter[i] >= 'A' && karakter[i] <= 'Z') || (karakter[i] == '_')))
        {
            return false;
        }
    }

    return true;
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

void renameDot(char *satir)
{
    int dotIndex = strcspn(satir, ".");
    while (dotIndex < strlen(satir))
    {
        char first[dotIndex];
        char second[strlen(satir) - dotIndex];
        substring(satir, 0, dotIndex, first);
        substring(satir, dotIndex + 1, strlen(satir) - 1, second);
        strcat(first, " Endofline");
        strcat(first, second);
        strcpy(satir, first);
        dotIndex = strcspn(satir, ".");
    }
}

void renameComma(char *satir)
{
    int commaIndex = strcspn(satir, ",");
    //char first[commaIndex];
    //char second[strlen(satir) - commaIndex];

    char first[500];
    char second[500];

    while (commaIndex < strlen(satir))
    {
        substring(satir, 0, commaIndex, first);
        substring(satir, commaIndex + 1, strlen(satir) - 1, second);
        strcat(first, " Seperator");
        strcat(first, second);
        strcpy(satir, first);
        commaIndex = strcspn(satir, ",");
        strcpy(first, "");
        strcpy(second, "");
    }
}

int insertTo(char *line, int pos)
{
    int len = satir_sayisi;
    for (int i = len; i >= pos; i--)
    {
        strcpy(satirlar[i + 1], satirlar[i]);
    }
    strcpy(satirlar[pos], line);
    satir_sayisi++;
}

int main() //icteki seyler cmd de parametre vermeye yariyor
{
    char fileName[] = "myscript.ba";
    /*
    printf("Enter the filename: \n");
	scanf("%s", &fileName);
    strcmp(fileName,".ba");
    */

    // okunacak dosya
    FILE *sourceFile = fopen(fileName, "r");
    if (sourceFile == NULL)
    {
        printf("%s", "File not found. Please check the file.\n");
    }

    // satirlari tek tek alan dongu
    while (!feof(sourceFile))
    {
        fgets(satir, 500, sourceFile);
        deleteComments(satir); //yorum bulunuyorsa satirda siliyor
        renameComma(satir);    // seperator olarak tanimasi icin virgulleri ayiriyor
        renameDot(satir);      // sondaki yapisik noktalari da token almasi icin noktanin oncesine bosluk ekliyor
        strcpy(satirlar[satir_sayisi], satir);
        satir_sayisi++;
    }

    int anlik_satir = 0;
    const char ayirici[] = " \n";
    char *token;

    out_list print_list;

    // satirlari kelimelerine ayiriyor
    while (anlik_satir <= satir_sayisi)
    {

        token = strtok(satirlar[anlik_satir], ayirici);

        while (token != NULL)
        {

            //int state kontrolu
            if (strcmp(token, "int") == 0)
            {
                char identName[20];

                token = strtok(NULL, ayirici);

                if (isIdentifier(token))
                {
                    strcpy(identName, token);
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("Error found at %i. . %s is not valid Identifier.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (strcmp(token, "Endofline") == 0)
                {
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("Error found at %i. . %s is not Endofline.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (token == NULL)
                {
                    bool isDeclared = isInIdentifierList(identName);

                    if (isDeclared)
                    {
                        printf("Variable already declared.");
                        exit(0);
                    }
                    else
                    {
                        strcpy(IdentifierNameList[IdentifierNameListLength], identName);
                        IdentifierNameListLength++;

                        IdentifierValueList[IdentifierValueOrder] = 0;
                        IdentifierValueOrder++;
                        break;
                    }
                }
                else
                {
                    printf("Error found at %i, character found after Endofline.", anlik_satir + 1);
                    exit(0);
                }
            }

            //move state kontrolu
            if (strcmp(token, "move") == 0)
            {
                int srcInt = 0;
                char destVal[20];

                token = strtok(NULL, ayirici);

                if (isIntConstant(token) || isIdentifier(token))
                {
                    if (isIntConstant(token))
                    {
                        srcInt = atoi(token);
                        token = strtok(NULL, ayirici);
                    }
                    else
                    {
                        for (int i = 0; i < IdentifierValueOrder; i++)
                        {
                            if (!strcmp(token, IdentifierNameList[i]))
                            {
                                srcInt = IdentifierValueList[i];
                                break;
                            }
                        }

                        token = strtok(NULL, ayirici);
                    }
                }
                else
                {
                    printf("Error found at %i. . %s is not IntConstant.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (strcmp(token, "to") == 0)
                {
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("Error found at %i. . %s is not equal to 'to' keyword.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (isIdentifier(token))
                {
                    strcpy(destVal, token);
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("Error found at %i. . %s is not Identifier.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (strcmp(token, "Endofline") == 0)
                {
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("Error found at %i. . %s is not Endofline.\n", anlik_satir + 1, token);
                    exit(0);
                }
                if (token == NULL)
                {
                    for (int i = 0; i < IdentifierValueOrder; i++)
                    {
                        if (!strcmp(destVal, IdentifierNameList[i]))
                        {
                            IdentifierValueList[i] = srcInt;
                            break;
                        }
                    }
                    break;
                }
                else
                {
                    printf("Error found at %i, character found after Endofline.", anlik_satir + 1);
                    exit(0);
                }
            }

            //add state kontrolu
            if (strcmp(token, "add") == 0)
            {
                int srcInt = 0;
                char destVal[20];

                token = strtok(NULL, ayirici);

                if (isIntConstant(token) || isIdentifier(token))
                {
                    if (isIntConstant(token))
                    {
                        srcInt = atoi(token);
                        token = strtok(NULL, ayirici);
                    }
                    else
                    {
                        for (int i = 0; i < IdentifierValueOrder; i++)
                        {
                            if (!strcmp(token, IdentifierNameList[i]))
                            {
                                srcInt = IdentifierValueList[i];
                                break;
                            }
                        }

                        token = strtok(NULL, ayirici);
                    }
                }
                else
                {
                    printf("Error found at %i. . %s is not IntConstant.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (strcmp(token, "to") == 0)
                {
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("Error found at %i. . %s is not equal to 'to' keyword.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (isIdentifier(token))
                {
                    strcpy(destVal, token);
                    token = strtok(NULL, ayirici);
                }

                else
                {
                    printf("Error found at %i. . %s is not Identifier.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (strcmp(token, "Endofline") == 0)
                {
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("Error found at %i. . %s is not Endofline.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (token == NULL)
                {
                    for (int i = 0; i < IdentifierValueOrder; i++)
                    {
                        if (!strcmp(destVal, IdentifierNameList[i]))
                        {
                            IdentifierValueList[i] += srcInt;
                            break;
                        }
                    }
                    break;
                }
                else
                {
                    printf("Error found at %i, character found after Endofline.", anlik_satir + 1);
                    exit(0);
                }
            }

            //sub state kontrolu
            if (strcmp(token, "sub") == 0)
            {
                int srcInt = 0;
                char destVal[20];

                if (isIntConstant(token) || isIdentifier(token))
                {
                    if (isIntConstant(token))
                    {
                        srcInt = atoi(token);
                        token = strtok(NULL, ayirici);
                    }
                    else
                    {
                        for (int i = 0; i < IdentifierValueOrder; i++)
                        {
                            if (!strcmp(token, IdentifierNameList[i]))
                            {
                                srcInt = IdentifierValueList[i];
                                break;
                            }
                        }

                        token = strtok(NULL, ayirici);
                    }
                }
                else
                {
                    printf("Error found at %i. . %s is not IntConstant.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (strcmp(token, "from") == 0)
                {
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("Error found at %i. . %s is not equal to 'from' keyword.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (isIdentifier(token))
                {
                    strcpy(destVal, token);
                    token = strtok(NULL, ayirici);
                }

                else
                {
                    printf("Error found at %i. . %s is not Identifier.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (strcmp(token, "Endofline") == 0)
                {
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("Error found at %i. . %s is not Endofline.\n", anlik_satir + 1, token);
                    exit(0);
                }
                if (token == NULL)
                {
                    for (int i = 0; i < IdentifierValueOrder; i++)
                    {
                        if (!strcmp(destVal, IdentifierNameList[i]))
                        {
                            IdentifierValueList[i] -= srcInt;
                            break;
                        }
                    }

                    break;
                }
                else
                {
                    printf("Error found at %i, character found after Endofline.", anlik_satir + 1);
                    exit(0);
                }
            }

            //out state kontrolu

            if (strcmp(token, "out") == 0)
            {
                token = strtok(NULL, ayirici);

                while (!(strcmp(token, "Endofline") == 0))
                {
                    node temp_node = create_node();

                    if (isIntConstant(token) || isIdentifier(token) || isStringConstant(token) || strcmp(token, "newline") == 0 || isSeperator(token))
                    {
                        if (isIntConstant(token))
                        {
                            temp_node.intVal = atoi(token);
                            print_list.list[out_list_order] = temp_node;
                            out_list_order++;
                            token = strtok(NULL, ayirici);
                        }
                        else if (isSeperator(token))
                        {
                            token = strtok(NULL, ayirici);
                        }
                        else if (strcmp(token, "newline") == 0)
                        {
                            temp_node.strVal = "\n";
                            print_list.list[out_list_order] = temp_node;
                            out_list_order++;
                            token = strtok(NULL, ayirici);
                        }
                        else if (isIdentifier(token))
                        {
                            temp_node.identNameVal = token;
                            print_list.list[out_list_order] = temp_node;
                            out_list_order++;
                            token = strtok(NULL, ayirici);
                        }
                        else if (isStringConstant(token))
                        {
                            temp_node.strVal = token;
                            print_list.list[out_list_order] = temp_node;
                            out_list_order++;
                            token = strtok(NULL, ayirici);
                        }
                        else
                        {
                            printf("Error found at %i. . %s is against the grammer of Out State.\n", anlik_satir + 1, token);
                            exit(0);
                        }
                    }
                }

                if (strcmp(token, "Endofline") == 0)
                {
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("Error found at %i. . %s is not Endofline.\n", anlik_satir + 1, token);
                    exit(0);
                }
                if (token == NULL)
                {
                    for (int i = out_list_start; i < out_list_order; i++)
                    {
                        if (!(strcmp(print_list.list[i].identNameVal, "none") == 0))
                        {
                            for (int j = 0; j < IdentifierNameListLength; j++)
                            {
                                if (strcmp(print_list.list[i].identNameVal, IdentifierNameList[j]) == 0)
                                {
                                    printf("%i", IdentifierValueList[j]);
                                    break;
                                }
                            }
                        }
                        else if (!(strcmp(print_list.list[i].strVal, "none") == 0))
                        {
                            printf("%s", print_list.list[i].strVal);
                        }
                        else if (print_list.list[i].intVal != -1000000)
                        {
                            printf("%i", print_list.list[i].intVal);
                        }
                    }
                    out_list_start = out_list_order;
                    break;
                }
                else
                {
                    printf("Error found at %i, character found after Endofline.", anlik_satir + 1);
                    exit(0);
                }
            }

            //loop state kontrolu

            if (strcmp(token, "loop") == 0)
            {
                int loopTimes = 0;
                char *loopTimesIdentifier;
                char newCode[500];
                strcpy(newCode, "");

                token = strtok(NULL, ayirici);

                if (isIntConstant(token) || isIdentifier(token))
                {
                    if (isIntConstant(token))
                    {
                        loopTimes = atoi(token);
                        token = strtok(NULL, ayirici);
                    }
                    else
                    {
                        loopTimesIdentifier = token;
                        char *currentIdentName;

                        for (int i = 0; i < IdentifierNameListLength; i++)
                        {
                            currentIdentName = IdentifierNameList[i];
                            if (strcmp(loopTimesIdentifier, currentIdentName) == 0)
                            {
                                loopTimes = IdentifierValueList[i];
                                break;
                            }
                        }
                        token = strtok(NULL, ayirici);
                    }
                }
                else
                {
                    printf("Error found at %i. . %s is against the State of Loop.\n", anlik_satir + 1, token);
                    exit(0);
                }

                if (strcmp(token, "times") == 0 || strcmp(token, "times\n") == 0)
                {
                    token = strtok(NULL, ayirici);
                }
                else
                {
                    printf("Error found at %i. . %s is not the 'times' keyword.\n", anlik_satir + 1, token);
                    exit(0);
                }
                bool dotControl = false;
                char *checkCodeBlock;
                substring(satirlar[anlik_satir + 1], 0, 1, checkCodeBlock);

                // kod bloğu açılmadan tek satırlı loop (myscript.ba 2. satırdaki gibi.)
                if (token != NULL && !(strcmp(checkCodeBlock, "[\n") == 0 || strcmp(checkCodeBlock, "[") == 0))
                {
                    while (token != NULL)
                    {
                        if (strcmp(token, "Endofline") == 0)
                        {
                            dotControl = true;
                        }
                        strcat(newCode, token);
                        if (!dotControl)
                        {
                            strcat(newCode, " ");
                        }
                        token = strtok(NULL, ayirici);
                    }

                    if (dotControl)
                    {
                        for (int i = 0; i < loopTimes; i++)
                        {
                            insertTo(newCode, anlik_satir + 1);
                        }
                        break;
                    }
                    else
                    {
                        printf("Error found at %i. . %s is not Endofline.\n", anlik_satir + 1, token);
                        exit(0);
                    }
                    strcpy(newCode, "");
                }

                // code block ise
                else if (strcmp(checkCodeBlock, "[\n") == 0 || strcmp(checkCodeBlock, "[") == 0)
                {
                    anlik_satir++;
                    token = strtok(satirlar[anlik_satir], ayirici);
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
                        token = strtok(NULL, ayirici);

                        while (!strcmp(token, "]") == 0)
                        {

                            while (token != NULL)
                            {
                                if (strcmp(token, "Endofline") == 0)
                                {
                                    dotControl = true;
                                }
                                strcat(newCode, token);
                                if (!dotControl)
                                {
                                    strcat(newCode, " ");
                                }
                                token = strtok(NULL, ayirici);
                            }
                            strcpy(codeBlockLines[codeBlockLineOrder], newCode);
                            codeBlockLineOrder++;
                            anlik_satir++;
                            token = strtok(satirlar[anlik_satir], ayirici);
                            strcpy(newCode, "");
                            dotControl = false;
                        }
                        for (int i = 0; i < codeBlockLineOrder; i++)
                        {
                            for (int j = 0; j < loopTimes; j++)
                            {
                                insertTo(codeBlockLines[i],
                                         (
                                             anlik_satir + ((loopTimes * i) + j + 1)));
                            }
                        }
                        break;
                    }
                    else
                    {
                        printf("Unproper loop state.");
                        exit(0);
                    }
                }
                else
                {
                    printf("Unproper loop state.");
                    exit(0);
                }
            }

            else
            {
                //hatali
                printf("Error found at %i. . %s is not valid keyword.\n", anlik_satir, token);
                exit(0);
            }
        }
        anlik_satir++;
    }

    fclose(sourceFile);
    return 0;
}