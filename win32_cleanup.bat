@echo off

FOR /d %%d IN (build, debug, release) DO (
    @IF EXIST %%d RMDIR /S /Q "%%d"
)

FOR %%f IN (*.bak, *.exe*, *.pdb, *.ilk, *.vert.d, *.frag.d, *.spv) DO (
	@IF EXIST %%f DEL /F /Q "%%f"
)