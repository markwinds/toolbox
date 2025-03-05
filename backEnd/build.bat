:: 不输出脚本执行的命令
@echo off

:: 获取当前脚本所在路径 并设置为工作目录
set WORK_PATH=%~dp0
cd /d %WORK_PATH%

:: 定义依赖库生成路径 inc和lib
set INC_PATH=%WORK_PATH%\third\inc
set LIB_PATH=%WORK_PATH%\third\lib
:: 如果目录不存在则创建
if not exist %INC_PATH% mkdir %INC_PATH%
if not exist %LIB_PATH% mkdir %LIB_PATH%

:: 如果子仓库没有检出 则检出子仓库
if not exist %WORK_PATH%\third\src\drogon\.gitignore (
    git submodule update --init --recursive
)

:: 编译依赖库
call :build_zlib
call :build_libzip
call :copy_json
call :build_jsoncpp
call :build_drogon
call :build_openssl
@REM call :build_libcurl

:: 编译项目
cd %WORK_PATH%
if not exist build mkdir build
cd build
:: 只在build目录不存在或CMake缓存不存在时运行CMake配置
if not exist CMakeCache.txt (
    cmake .. -G Ninja ^
        -DCMAKE_C_COMPILER=clang ^
        -DCMAKE_CXX_COMPILER=clang++ ^
        -DCMAKE_BUILD_TYPE=Release ^
        -DCMAKE_INSTALL_PREFIX=install
)
ninja
echo build toolbox success
:: 创建bin目录
if not exist %WORK_PATH%\bin mkdir %WORK_PATH%\bin

:: 如果正在运行则杀死进程 如果没找到不要报错
taskkill /IM toolbox.exe /F

:: 拷贝生成的exe到bin目录
cp %WORK_PATH%\build\toolbox.exe %WORK_PATH%\bin\
:: 根据cp命令返回值输出
if %errorlevel% equ 0 (
    echo create %WORK_PATH%\bin\toolbox.exe success
) else (
    echo create %WORK_PATH%\bin\toolbox.exe failed
)

:: 脚本结束
echo Script finished
goto :eof

:: 编译zlib
:build_zlib
:: 查看lib中是否存在zlibstatic.lib 如果不存在则编译zlib
if exist %LIB_PATH%\zlibstatic.lib (
    echo zlibstatic.lib exists, skip build
) else (
    cd %WORK_PATH%
    echo zlibstatic.lib does not exist
    cd third/src/zlib
    mkdir build
    cd build
    cmake .. -G Ninja ^
        -DCMAKE_C_COMPILER=clang ^
        -DCMAKE_CXX_COMPILER=clang++ ^
        -DCMAKE_BUILD_TYPE=Release ^
        -DCMAKE_INSTALL_PREFIX=install
    ninja
    ninja install
    :: 拷贝生成的库和头文件
    :: 为zlib的头文件单独创建一个文件夹 并拷贝头文件
    mkdir %INC_PATH%\zlib
    xcopy /E /Y install\include\* %INC_PATH%\zlib\
    xcopy /E /Y install\lib\zlibstatic.lib %LIB_PATH%\
    echo zlib ok
    :: 删除编译过程中生成的文件
    cd ..
    git checkout -- .
    cd %WORK_PATH%
)
goto :eof

:: 编译libzip
:build_libzip
:: 查看lib中是否存在zip.lib 如果不存在则编译libzip
if exist %LIB_PATH%\zip.lib (
    echo zip.lib exists, skip build
) else (
    cd %WORK_PATH%
    echo zip.lib does not exist
    cd third/src/libzip
    mkdir build
    cd build
    cmake .. -G Ninja ^
        -DCMAKE_BUILD_TYPE=Release ^
        -DCMAKE_C_COMPILER=clang ^
        -DCMAKE_CXX_COMPILER=clang++ ^
        -DBUILD_SHARED_LIBS=OFF ^
        -DZLIB_INCLUDE_DIR=%INC_PATH%\zlib ^
        -DZLIB_LIBRARY=%LIB_PATH%\zlibstatic.lib ^
        -DCMAKE_INSTALL_PREFIX=install
    ninja
    ninja install
    :: 拷贝生成的库和头文件
    :: 为libzip的头文件单独创建一个文件夹 并拷贝头文件
    mkdir %INC_PATH%\libzip
    xcopy /E /Y install\include\* %INC_PATH%\libzip\
    copy /Y install\lib\zip.lib %LIB_PATH%\
    echo libzip ok
    :: 删除编译过程中生成的文件
    cd ..
    git checkout -- .
    cd %WORK_PATH%
)
goto :eof

:: 拷贝json头文件
:copy_json
:: 查看inc中是否存在json头文件 如果不存在则拷贝
if exist %INC_PATH%\json\json.hpp (
    echo json.hpp exists, skip copy
) else (
    echo json.hpp does not exist
    mkdir %INC_PATH%\json
    xcopy /E /Y third\src\json\json.hpp %INC_PATH%\json\
    echo json ok
)
goto :eof

:: 编译jsoncpp
:build_jsoncpp
:: 查看lib中是否存在jsoncpp.lib 如果不存在则编译jsoncpp
if exist %LIB_PATH%\jsoncpp.lib (
    echo jsoncpp.lib exists, skip build
) else (
    cd %WORK_PATH%
    echo jsoncpp.lib does not exist
    cd third/src/jsoncpp
    mkdir build
    cd build
    cmake .. -G Ninja ^
        -DCMAKE_C_COMPILER=clang ^
        -DCMAKE_CXX_COMPILER=clang++ ^
        -DJSONCPP_WITH_TESTS=OFF ^
        -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF ^
        -DBUILD_SHARED_LIBS=OFF ^
        -DBUILD_STATIC_LIBS=ON ^
        -DCMAKE_BUILD_TYPE=Release ^
        -DCMAKE_INSTALL_PREFIX=install
    ninja
    ninja install
    :: 拷贝生成的库和头文件
    :: 为jsoncpp的头文件单独创建一个文件夹 并拷贝头文件
    mkdir %INC_PATH%\jsoncpp
    xcopy /E /Y install\include\* %INC_PATH%\jsoncpp\
    copy /Y install\lib\jsoncpp.lib %LIB_PATH%\
    echo jsoncpp ok
    cd %WORK_PATH%
)
goto :eof

:: 编译drogon
:build_drogon
:: 查看lib中是否存在drogon.lib 如果不存在则编译drogon
if exist %LIB_PATH%\drogon.lib (
    echo drogon.lib exists, skip build
) else (
    echo drogon.lib does not exist
    cd %WORK_PATH%
    cd third/src/drogon
    mkdir build
    cd build
    cmake .. -G Ninja ^
        -DCMAKE_BUILD_TYPE=Release ^
        -DCMAKE_C_COMPILER=clang ^
        -DCMAKE_CXX_COMPILER=clang++ ^
        -DBUILD_EXAMPLES=OFF ^
        -DBUILD_CTL=OFF ^
        -DBUILD_DOC=OFF ^
        -DBUILD_ORM=OFF ^
        -DBUILD_YAML_CONFIG=OFF ^
        -DBUILD_BROTLI=OFF ^
        -DJSONCPP_INCLUDE_DIRS=%INC_PATH%\jsoncpp ^
        -DJSONCPP_LIBRARIES=%LIB_PATH%\jsoncpp.lib ^
        -DZLIB_LIBRARY=%LIB_PATH%\zlibstatic.lib ^
        -DZLIB_INCLUDE_DIR=%INC_PATH%\zlib ^
        -DCMAKE_INSTALL_PREFIX=install
    ninja
    ninja install
    :: 拷贝生成的库和头文件
    xcopy /E /Y install\include\* %INC_PATH%\
    xcopy /Y install\lib\*.lib %LIB_PATH%\
    echo drogon ok
    cd %WORK_PATH%
)
goto :eof

:: 编译openssl
:build_openssl
:: 查看lib中是否存在libssl.lib 如果不存在则编译openssl
if exist %LIB_PATH%\libssl.lib (
    echo libssl.lib exists, skip build
) else (
    :: 载入 Visual Studio 环境
    for /f "tokens=*" %%i in ('"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath') do (
        call "%%i\VC\Auxiliary\Build\vcvars64.bat"
    )

    where perl >nul 2>&1
    if %ERRORLEVEL% neq 0 (
        echo 未找到 Perl。请安装 Strawberry Perl
        exit /b 1
    )

    where nasm >nul 2>&1
    if %ERRORLEVEL% neq 0 (
        echo 警告: 未找到 NASM。某些优化的汇编代码可能无法编译
        exit /b 1
    )

    cd %WORK_PATH%
    cd third/src/openssl
    ls
    :: clang-cl（MSVC 兼容 Clang 编译器）
    perl Configure VC-WIN64A no-shared no-apps no-tests --prefix=%WORK_PATH%\third\src\openssl\install CC=clang-cl
    nmake clean
    nmake
    nmake install

    xcopy /E /Y %WORK_PATH%\third\src\openssl\install\include\* %INC_PATH%\
    xcopy /Y %WORK_PATH%\third\src\openssl\install\lib\libssl.lib %LIB_PATH%\
    xcopy /Y %WORK_PATH%\third\src\openssl\install\lib\libcrypto.lib %LIB_PATH%\

    echo openssl ok
    git checkout -- .
    cd %WORK_PATH%
)
goto :eof

:: 编译libcurl
:build_libcurl
:: 查看lib中是否存在libcurl.lib 如果不存在则编译libcurl
if exist %LIB_PATH%\libcurl.lib (
    echo libcurl.lib exists, skip build
) else (
    cd %WORK_PATH%
    echo libcurl.lib does not exist
    cd third/src/libcurl
    mkdir build
    cd build
    cmake .. -G Ninja ^
        -DCMAKE_BUILD_TYPE=Release ^
        -DCMAKE_C_COMPILER=clang ^
        -DCMAKE_CXX_COMPILER=clang++ ^
        -DOPENSSL_ROOT_DIR=%INC_PATH%\openssl ^ 
        -DOPENSSL_LIBRARIES=%LIB_PATH%\libssl.lib;%LIB_PATH%\libcrypto.lib ^ 
        -DCMAKE_INSTALL_PREFIX=install
    ninja
    ninja install
    :: 拷贝生成的库和头文件
    :: 为libcurl的头文件单独创建一个文件夹 并拷贝头文件
    mkdir %INC_PATH%\libcurl
    xcopy /E /Y install\include\* %INC_PATH%\libcurl\
    xcopy /E /Y install\lib\libcurl.lib %LIB_PATH%\
    echo libcurl ok
    cd %WORK_PATH%
)
goto :eof







