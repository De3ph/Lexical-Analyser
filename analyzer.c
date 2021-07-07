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
    // out' ta yazılacakları tutmamızı sağlayan struct
    int intVal;
    char *identNameVal;
    char *strVal;
} node;

node create_node()
{
    // node oluşturucu
    node temp_node;
    temp_node.intVal = -1000000;
    temp_node.identNameVal = "none";
    temp_node.strVal = "none";
    return temp_node;
}

typedef struct out_list
{
    // out state için yazdırılacak nodeları tutan yapı
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
    if ((from + n) > length) // nereye kadar alınacağına bakıyor
    {
        n = (length - from);
    }
    for (i = 0; i < n; i++) // istenen başlangıçtan istenilen sona kadar substring alıyor, verilen hedefte depoluyor
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

bool isStringConstant(char *karakter)
{
    return (karakter[0] == '"' && karakter[strlen(karakter) - 1] == '"') ? true : false;
}

bool isIntConstant(char *karakter)
{
    int len = strlen(karakter);

    // karakterin integer limitleri içinde olup olmadığını kontrol ediyor, (limit dışındaysa double vs oluyor)
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
// değişkenin tanımlanıp tanımlanmadığını kontrol ediyor
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
    // değişken adının uzunluğunu kontrol ediyor
    if (len > 20)
    {
        return false;
    }

    //karakterlerin alphanumeric olup olmadığına bakıyor
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

void deleteComments(char *satir)
{
    int index1 = strcspn(satir, "{"); // { karakterden
    if (index1 + 2 > strlen(satir))
    {
        return;
    }
    else
    {
        int index2 = strcspn(satir, "}"); // } bu karaktere kadar olan şeyleri siliyor
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
        strcat(first, " Endofline"); // kontrol kolaylığı için noktaları değiştiriyor
        strcat(first, second);
        strcpy(satir, first);
        dotIndex = strcspn(satir, ".");
    }
}

void renameComma(char *satir)
{
    int commaIndex = strcspn(satir, ",");

    char first[500];
    char second[500];

    while (commaIndex < strlen(satir))
    {
        substring(satir, 0, commaIndex, first);
        substring(satir, commaIndex + 1, strlen(satir) - 1, second);
        strcat(first, " Seperator"); // kontrol kolaylığı için virgülleri değiştiriyor
        strcat(first, second);
        strcpy(satir, first);
        commaIndex = strcspn(satir, ",");
        strcpy(first, "");
        strcpy(second, "");
    }
}
//satırların tutulduğu listeye, verilen indexine verilen satırı ekliyor
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

int main()
{
    char fileName[50];
    
    printf("Enter the filename: \n");
	scanf("%s", &fileName);
    

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
                        /*
                            IdentifierNameList 'e identifier ismini ekliyor, IdentifierValueList 'e de 
                            aynı index değerinin bulunduğu konuma 0 atamasını yapıyor
                        */
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
                // atanacak değerin int mi variable mı olduğuna bakıyor
                if (isIntConstant(token) || isIdentifier(token))
                {
                    if (isIntConstant(token))
                    {
                        // int ise değeri direk srcInt e atıyor
                        srcInt = atoi(token);
                        token = strtok(NULL, ayirici);
                    }
                    else
                    {
                        for (int i = 0; i < IdentifierValueOrder; i++)
                        {
                            if (!strcmp(token, IdentifierNameList[i]))
                            {
                                //değişken ise değerini IdentifierValueList ten bulup srcInt e atıyor
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
                    // eğer noktadan sonra bir karakter yoksa state doğru oluyor

                    for (int i = 0; i < IdentifierValueOrder; i++)
                    {
                        if (!strcmp(destVal, IdentifierNameList[i]))
                        {
                            /*
                                IdentifierNameList den değişkenin index değerini bulup IdentifierValueList ' te değerini değiştiriyor
                            */
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
                // atanacak değerin int mi variable mı olduğuna bakıyor
                if (isIntConstant(token) || isIdentifier(token))
                {
                    if (isIntConstant(token))
                    {
                        //int ise değeri direk alıyor
                        srcInt = atoi(token);
                        token = strtok(NULL, ayirici);
                    }
                    else
                    {
                        for (int i = 0; i < IdentifierValueOrder; i++)
                        {
                            //değişken ise değerini IdentifierValueList ten bulup srcInt e atıyor
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
                            /*
                                IdentifierNameList den değişkenin index değerini bulup IdentifierValueList ' te değerini srcInt kadar arttırıyor
                            */
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

            //add state ile aynı işlemler yapılıyor, sadece srcInt toplanmak yerine çıkartılıyor
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
                    // ekrana yazılacak bilgileri tutması için node oluşturuyor
                    node temp_node = create_node();

                    if (isIntConstant(token) || isIdentifier(token) || isStringConstant(token) || strcmp(token, "newline") == 0 || isSeperator(token))
                    {
                        // yazdırılacak şeyin türünün tespiti yapılıyor
                        if (isIntConstant(token))
                        {
                            // int ise node un int tutan intVal özelliğini verilen int yapıyor
                            temp_node.intVal = atoi(token);
                            print_list.list[out_list_order] = temp_node; //yazılacaklar listesine ekliyor
                            out_list_order++;
                            token = strtok(NULL, ayirici);
                        }
                        //virgül ise devam ediyor
                        else if (isSeperator(token))
                        {
                            token = strtok(NULL, ayirici);
                        }

                        else if (strcmp(token, "newline") == 0)
                        {
                            //newline ise node un strVal özelliğine "\n" atıyor
                            temp_node.strVal = "\n";
                            print_list.list[out_list_order] = temp_node; //yazılacaklar listesine ekliyor
                            out_list_order++;
                            token = strtok(NULL, ayirici);
                        }
                        else if (isIdentifier(token))
                        {
                            // identifier yazılacak ise yazılacak identifier ın ismini identNameVal e atıyor
                            temp_node.identNameVal = token;
                            print_list.list[out_list_order] = temp_node; //yazılacaklar listesine ekliyor
                            out_list_order++;
                            token = strtok(NULL, ayirici);
                        }
                        else if (isStringConstant(token))
                        {
                            //StringConstant yazılacak ise strVal değerine StringConstant ı atıyor
                            temp_node.strVal = token;
                            print_list.list[out_list_order] = temp_node; //yazılacaklar listesine ekliyor
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
                    // yazılacaklar listesinde (print_list.list) dönüyor
                    for (int i = out_list_start; i < out_list_order; i++)
                    {
                        //identNameVal özelliği değişmiş ise (identifier yazdırılacaksa)
                        if (!(strcmp(print_list.list[i].identNameVal, "none") == 0))
                        {
                            //IdentifierNameList e identifier isminin indexini buluyor
                            for (int j = 0; j < IdentifierNameListLength; j++)
                            {
                                // indexi bulunca IdentifierValueList ten değerini yazıyor
                                if (strcmp(print_list.list[i].identNameVal, IdentifierNameList[j]) == 0)
                                {
                                    printf("%i", IdentifierValueList[j]);
                                    break;
                                }
                            }
                        }
                        //strVal özelliği değişmiş ise (StrConstant yazdırılacaksa)
                        else if (!(strcmp(print_list.list[i].strVal, "none") == 0))
                        {
                            printf("%s", print_list.list[i].strVal);
                        }
                        //intVal özelliği değişmiş ise (int yazdırılacaksa)
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

                //loop un kaç kere döneceğini alıyor
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
                        // identifier değeri kadar dönecekse bulup loopTimes değişkenine atıyor
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
                    //times dan sonra bir şey gelmiş ise
                    while (token != NULL)
                    {
                        // "." ya kadarki gerçekleştirikecek kodu newCode değişkenine kopyalıyor
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
                        //loop un dönüşü kadar dönecek kodu loop state in hemen sonrasına loopTimes kadar ekliyor
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
                    // kod bloğunun kapanıp kapanmadığını kontrol ediyor
                    if (isFound)
                    {
                        token = strtok(NULL, ayirici);

                        while (!strcmp(token, "]") == 0)
                        {
                            // kod satırı kapanana kadar [ ] arasındaki tüm satırları codeBlockLines listesine ekliyor
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
                            //codeBlockLines a newCode ekleniyor
                            strcpy(codeBlockLines[codeBlockLineOrder], newCode);
                            codeBlockLineOrder++;
                            anlik_satir++;
                            token = strtok(satirlar[anlik_satir], ayirici);
                            strcpy(newCode, "");
                            dotControl = false;
                        }
                        //eklenecek satır sayısı kadar dönüyor
                        for (int i = 0; i < codeBlockLineOrder; i++)
                        {
                            //loopTimes kadar eklenecek satırları ekliyor satırlar listesine
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
                printf("Error found at %i. . %s is not valid keyword.\n", anlik_satir, token);
                exit(0);
            }
        }
        anlik_satir++;
    }

    fclose(sourceFile);
    return 0;
}