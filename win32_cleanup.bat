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

DEL /A /F /Q /S "*.bak"
DEL /A /F /Q /S "*.exe*"
DEL /A /F /Q /S "*.pdb*"
DEL /A /F /Q /S "*.ilk*"
DEL /A /F /Q /S "*.spv"
DEL /A /F /Q /S "*.vert.d"
DEL /A /F /Q /S "*.frag.d"