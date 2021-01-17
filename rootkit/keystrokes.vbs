'Focus on svchost.exe window
set WshShell = WScript.CreateObject("WScript.Shell")
WshShell.AppActivate("svchost.exe")

'Give some buffer time
WScript.Sleep 100

'Send keystrokes to launch Explorer
WshShell.SendKeys "explorer.exe"
WshShell.SendKeys "{ENTER}"