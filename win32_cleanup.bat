@echo off

FOR /d /r %%F IN (build?) DO (
    echo %%F
    @IF EXIST %%F RMDIR /S /Q "%%F"
)

FOR /d /r %%F IN (debug?) DO (
    echo %%F
    @IF EXIST %%F RMDIR /S /Q "%%F"
)

FOR /d /r %%F IN (release?) DO (
    echo %%F
    @IF EXIST %%F RMDIR /S /Q "%%F"
)

FOR /d /r %%F IN (.vs?) DO (
    echo %%F
    @IF EXIST %%F RMDIR /S /Q "%%F"
)