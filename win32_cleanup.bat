@echo off

FOR /d /r %%F IN (build?) DO (
    @IF EXIST %%F RMDIR /S /Q "%%F"
)

FOR /d /r %%F IN (debug?) DO (
    @IF EXIST %%F RMDIR /S /Q "%%F"
)

FOR /d /r %%F IN (release?) DO (
    @IF EXIST %%F RMDIR /S /Q "%%F"
)

FOR /d /r %%F IN (.vs?) DO (
    @IF EXIST %%F RMDIR /S /Q "%%F"
)

DEL /A /F /Q /S "*.exe*"
DEL /A /F /Q /S "*.bak*"