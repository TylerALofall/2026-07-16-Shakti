@echo off
setlocal
set ROOT=%~dp0
set OUT=%TEMP%\shakti_mcp_test.exe
clang -std=c17 -Wall -Wextra -Werror -pedantic -I"%ROOT%include" -I"%ROOT%core" ^
 "%ROOT%core\cycle_controller.c" "%ROOT%core\route_table.c" ^
 "%ROOT%core\loopback_builder.c" "%ROOT%core\event_id.c" ^
 "%ROOT%routes\route01_epoch.c" "%ROOT%routes\route02_heartbeat.c" ^
 "%ROOT%routes\route03_goal.c" "%ROOT%routes\route04_notebook.c" ^
 "%ROOT%routes\route05_menu.c" "%ROOT%routes\route06_shell.c" ^
 "%ROOT%routes\route07_message_out.c" "%ROOT%routes\route08_message_in.c" ^
 "%ROOT%routes\route09_reflect.c" "%ROOT%tests\test_router.c" -o "%OUT%"
if errorlevel 1 exit /b 1
"%OUT%"
