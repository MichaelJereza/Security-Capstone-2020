typedef struct IUnknown IUnknown;

#include <iostream>
#include <Windows.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>
//#include "Release/hooktest2.dll"

int main()
{


    /* get handle to dll */
    HINSTANCE hGetProcIDDLL = LoadLibrary(L"C:\\Document and Settings\\Owner\\Desktop\\test\\WindowsHookExample.dll");

    /* get pointer to the function in the dll*/
    FARPROC lpfnGetProcessID = GetProcAddress(HMODULE(hGetProcIDDLL), "main");

    /*
       Define the Function in the DLL for reuse. This is just prototyping the dll's function.
       A mock of it. Use "stdcall" for maximum compatibility.
    */
    typedef int(__stdcall* pICFUNC)();

    pICFUNC MyFunction;
    MyFunction = pICFUNC(lpfnGetProcessID);

    /* The actual call to the function contained in the dll */
    int intMyReturnVal = main();

    /* Release the Dll */
    FreeLibrary(hGetProcIDDLL);

    
    /* The return val from the dll */
   // returnintMyReturnVal;
    return 0;
}
