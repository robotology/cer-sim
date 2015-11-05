@echo OFF
@title MultiMesh Scripting
cls


@set inputFolder=stl
@set inputMeshFormat=stl
@set outputFolder=dae
@set outputMeshFormat=dae

rem The meshlabserver program location:
@set meshlabserverPath="C:\Program Files\VCG\MeshLab\meshlabserver.exe"


rem ------------------------------------------------------
rem       List the Current Input Mesh Format
rem ------------------------------------------------------
echo ----------------------------------------------------------------
echo.
echo Processing meshes with the format:
echo %inputMeshFormat%
echo.

rem ------------------------------------------------------
rem            List the input Meshes
rem ------------------------------------------------------
echo.
echo ----------------------------------------------------------------
echo.
echo Input Folder Mesh List:
for %%X in (%inputFolder%\*.%inputMeshFormat%) do (echo "%%X")
rem To get help on the "for" syntax use: for /?
echo.

rem ------------------------------------------------------
rem   Do a simple mesh conversion on a single file
rem ------------------------------------------------------

rem Example Syntax: 
rem "C:\Program Files\VCG\MeshLab\meshlabserver.exe" -i input\boulder-mini1.ply -o output\boulder-mini1.ply -om vc fq wn

rem Do a simple mesh conversion
rem %meshlabserverPath% -i %inputFolder%\%singleMeshNamePrefix%.%inputMeshFormat% -o %outputFolder%\%singleMeshNamePrefix%.%outputMeshFormat% %outputMeshOptions%

rem ------------------------------------------------------
rem    Do a simple mesh conversion on a folder
rem ------------------------------------------------------

rem Run the "for" loop from inside the input folder
cd %inputFolder%

for %%I in (*.%inputMeshFormat%) do (%meshlabserverPath% -i %%I -o ..\%outputFolder%\%%~nI.%outputMeshFormat%)
rem To get help on the "for" syntax use: for /?

rem Go back down a directory
cd .. 