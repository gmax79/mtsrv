@echo off
set zip="poco-1.7.9"

if exist poco goto end

set filename="%zip%.zip"
echo %filename%
build\7za.exe x -tzip %filename%

move %zip% poco

copy build\components poco\ /Y
copy build\buildwin.cmd poco\ /Y

echo end
:end
