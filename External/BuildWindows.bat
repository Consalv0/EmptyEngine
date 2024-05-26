"%ProgramFiles(x86)%/Microsoft Visual Studio/Installer/vswhere.exe" -nologo -latest -property productPath > vswhere.txt
set /p devenv=<vswhere.txt
"%devenv%" "SDL/VisualC/SDL.sln" /Build "Release|x64"
"%devenv%" "SDL/VisualC/SDL.sln" /Build "Debug|x64"
del vswhere.txt