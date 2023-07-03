::environment install for Windows x64
@ECHO OFF
echo ___________________________________________________________________________________
echo Running environment install for Windows x64.
echo Expected runtime: 30 minutes
echo Authors: Gavin yu
echo ___________________________________________________________________________________
echo.

echo Administrative permissions required. Detecting permissions...
echo.
net session >nul 2>&1
if %errorLevel% == 0 (
    echo Success: Administrative permissions confirmed.
) else (
    echo Failure: Current permissions inadequate.
    exit 1
)

cd C:/

echo Looking for "C:/local"...
if exist local ( 
    cd local
) else ( 
    mkdir local
    cd local
)

echo Checking if prerequisites are met...
echo.
echo Looking for VsDevCmd...
 if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" (
    echo Found VsDevCmd.
    echo.
) else (
    echo VsDevCmd was not found. This means that you did not install visual studio 2022
    echo.
    echo Installation was not initialized.
    exit 2
)
echo Looking for git...
git --version >nul 2>&1 && (
    echo Found git.
    echo.
) || (
    echo git was not found.
    echo.
    echo Installation was not initialized.
    exit 2
)
echo Looking for CMake...
"C:\Program Files\CMake\bin\cmake.exe" --version >nul 2>&1 && (
    echo Found cmake.
    echo.
) || (
    echo cmake was not found.
    echo.
    echo Installation was not initialized.
    exit 2
)

echo Looking for nasm...
"C:\Program Files\NASM\nasm.exe" -v >nul 2>&1 && (
    echo Found nasm.
    echo.
) || (
    echo nasm was not found.
    echo.
    echo Installation was not initialized.
    exit 2
)

echo determinating if nasm was added to path...
nasm -v >nul 2>&1 && (
    echo Found nasm.
    echo.
) || (
    echo nasm was not initalized to path.
    SETX path "%PATH%;C:/Program Files/NASM" /m
    echo nasm was added to path.
    echo Please run this file again for the installation to continue
    exit 0
)
echo Looking for perl...
"C:\Strawberry\perl\bin\perl.exe" -v >nul 2>&1 && (
    echo Found Perl.
    echo.
) || (
    echo Perl was not found.
    echo.
    echo Installation was not initialized.
    exit 2
)

::check if existing libraries exist
if not exist lib_existing ( 
    mkdir lib_existing
)

if not exist lib_problems (
    mkdir lib_problems
)

::INSTALL BOOST ________________________________________________
::check if cleanup is needed for boost
cd "C:\local"
if exist lib_problems/boost.txt (
    ::clean up boost
    echo Boost had issues installing last time.
)
::check if boost is installed
if exist lib_existing/boost.txt (
    ::skip installation
    echo Boost has been installed.
    echo.
) else (
    ::install boost and build boost
    echo Fetching boost...
    powershell -Command "$ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest https://boostorg.jfrog.io/artifactory/main/release/1.79.0/source/boost_1_79_0.zip -OutFile boost_1_79_0.zip"
    echo Extracting boost...
    powershell -Command "$ProgressPreference = 'SilentlyContinue'; Expand-Archive -Force C:/local/boost_1_79_0.zip -DestinationPath C:/local/"
    echo Bootstrapping...
    cd "C:\local\boost_1_79_0"
    @ECHO ON
    call bootstrap.bat
    echo Building...
    b2
    @ECHO OFF
    echo.
    echo Cleaning up...
    del "C:\local\boost_1_79_0.zip" /q
    echo.
    echo Built Boost libraries.
    echo.>"C:/local/lib_existing/boost.txt"
    echo.
)

 ::INSTALL MONGOC DRIVER
 cd "C:\local"
 ::check if cleanup is needed for mongoc
if exist lib_problems/mongoc.txt (
    ::clean up mongoc
    echo Cleaning up Mongo-C-Driver...
)
::check if mongoc is installed
if exist lib_existing/mongoc.txt (
    ::skip installation
    echo Mongo-C-Driver has been installed.
    echo.
) else (
    ::install mongoc and build mongoc
    echo Fetching Mongo-C-Driver...
    powershell -Command "$ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest https://github.com/mongodb/mongo-c-driver/releases/download/1.22.0/mongo-c-driver-1.22.0.tar.gz -OutFile mongo-c-driver-1.22.0.tar.gz"
    echo Extracting Mongo-C-Driver...
    tar -xvzf "C:/local/mongo-c-driver-1.22.0.tar.gz" -C "C:/local"
    echo Generating makefiles...
    @ECHO ON
    cd "C:/local/mongo-c-driver-1.22.0"
    mkdir cmake-build
    cd "cmake-build"
    "C:\Program Files\CMake\bin\cmake.exe" -G "Visual Studio 17 2022" -DCMAKE_INSTALL_PREFIX=C:/local -DCMAKE_PREFIX_PATH=C:/local ..
    echo Building files 1/2...
    "C:\Program Files\CMake\bin\cmake.exe" --build .
    echo Building files 2/2...
    "C:\Program Files\CMake\bin\cmake.exe" --build . --target install
    @ECHO OFF
    echo.
    echo Cleaning up...
    del "C:\local\mongo-c-driver-1.22.0.tar.gz" /q
    echo.
    echo Built Mongo-C-Driver.
    echo.>"C:/local/lib_existing/mongoc.txt"
    echo.
)

 ::INSTALL MONGOCXX DRIVER
 cd "C:\local"
 ::check if cleanup is needed for mongoc
if exist lib_problems/mongocxx.txt (
    ::clean up mongoc
    echo Cleaning up Mongo-Cxx-Driver...
)

::check if mongocxx is installed
if exist lib_existing/mongocxx.txt (
    echo Mongo-Cxx-Driver has already been installed.
    echo.
    goto opensslinstall
) 

if exist lib_existing/mongoc.txt (
        ::install boost and build boost
        echo Fetching Mongo-Cxx-Driver...
        powershell -Command "$ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.7/mongo-cxx-driver-r3.6.7.tar.gz -OutFile mongo-cxx-driver-r3.6.7.tar.gz"
        echo Extracting Mongo-Cxx-Driver...
        tar -xvzf "C:/local/mongo-cxx-driver-r3.6.7.tar.gz" -C "C:/local"
        echo Generating makefiles...
        @ECHO ON
        cd "C:/local/mongo-cxx-driver-r3.6.7/build"
        "C:\Program Files\CMake\bin\cmake.exe" .. -G "Visual Studio 17 2022" -DBOOST_ROOT=C:/local/boost_1_79_0 -DCMAKE_PREFIX_PATH=C:/local -DCMAKE_INSTALL_PREFIX=C:/local
        echo Building files 1/2...
        "C:\Program Files\CMake\bin\cmake.exe" --build .
        echo Building files 2/2...
        "C:\Program Files\CMake\bin\cmake.exe" --build . --target install
        @ECHO OFF
        echo.
        echo Cleaning up...
        del "C:\local\mongo-cxx-driver-r3.6.7.tar.gz" /q
        echo.
        echo Built Mongo-Cxx-Driver.
        echo.>"C:/local/lib_existing/mongocxx.txt"
        echo.
    ) else (
        echo Mongo-C-Driver was not found. Aborting!
        echo.
        exit 1
    )

:opensslinstall

 ::INSTALL OPENSSL
 cd "C:\local"
 ::check if cleanup is needed for openssl
if exist lib_problems/openssl.txt (
    ::clean up openssl
    echo Cleaning up openssl...
)
::check if openssl is installed
if exist lib_existing/openssl.txt (
    echo openssl has been installed.
    echo.
    goto finish
)

echo Fetching openssl...
git clone git://git.openssl.org/openssl.git "C:/local/openssl"
echo Building openssl...
call "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/Tools/VsDevCmd.bat" -arch=x64 -host_arch=x64 & cd "C:/local/openssl" & perl Configure --prefix=C:\local & nmake & nmake test & nmake install
echo Built openssl.
echo Cleaning up buildfiles...
cd ..
rmdir "C:\local\openssl" /s /q

echo.> "C:/local/lib_existing/openssl.txt"
echo.

::clean mongoc and cxx
if exist "C:\local\mongo-c-driver-1.22.0" (
rmdir "C:\local\mongo-c-driver-1.22.0" /s /q
)
if exist "C:\local\mongo-cxx-driver-r3.6.7" (
rmdir "C:\local\mongo-cxx-driver-r3.6.7" /s /q
)


:finish

if exist "C:\local\lib_problems" (
    rmdir "C:\local\lib_problems" /s /q
)

echo Finished.
echo Please Add local and bin to path. 
PAUSE