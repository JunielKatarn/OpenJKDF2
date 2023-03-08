#!/bin/zsh

export PATH="${HOMEBREW_PREFIX}/opt/llvm/bin:$PATH"
export MACOSX_DEPLOYMENT_TARGET=10.15
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

cd /Users/$USER/workspace/OpenJKDF2/

export OPENJKDF2_RELEASE_COMMIT=$(git log -1 --format="%H")
export OPENJKDF2_RELEASE_COMMIT_SHORT=$(git rev-parse --short=8 HEAD)

#make flex/flex
#make byacc/yacc

#cp DF2/player_bak/Max/Max.plr ~/.local/share/openjkdf2/player/Max/Max.plr
#OPENJKDF2_NO_ASAN=1 DEBUG_QOL_CHEATS=1 make -f Makefile.macos -j10 &&
mkdir -p build_darwin64 && cd build_darwin64

if [ ! -f build_protoc/protoc ]; then
    mkdir -p build_protoc && cd build_protoc
    cmake -DCMAKE_BUILD_TYPE=Release -Dprotobuf_BUILD_TESTS=OFF ../../3rdparty/protobuf/cmake
    make -j10 protoc
    cd ..
fi

if [ ! -f build_protobuf/libprotobuf.dylib ]; then
    mkdir -p build_protobuf && cd build_protobuf
    cmake -DCMAKE_INSTALL_PREFIX=. -DCMAKE_BUILD_TYPE=Release -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_SHARED_LIBS=ON ../../3rdparty/protobuf/cmake
    make -j10 install
    cd ..
fi

if [ ! -f build_gns/bin/libGameNetworkingSockets.dylib ]; then
    mkdir -p build_gns && cd build_gns
    GNS_BUILD=$(pwd)
    export PKG_CONFIG_PATH_OLD=$PKG_CONFIG_PATH
    export PKG_CONFIG_PATH=$PKG_CONFIG_PATH_OLD:/opt/homebrew/opt/openssl@1.1/lib/pkgconfig
    cmake -DCMAKE_BUILD_TYPE=Release -DProtobuf_USE_STATIC_LIBS=ON -DProtobuf_LIBRARIES="-L$GNS_BUILD/../build_protobuf/lib" -DProtobuf_LIBRARIES_PATH="$GNS_BUILD/../build_protobuf/lib" -DProtobuf_INCLUDE_DIR=$GNS_BUILD/../../3rdparty/protobuf/src -DProtobuf_INCLUDE_DIR2=$GNS_BUILD/../build_protobuf/include -DProtobuf_PROTOC_EXECUTABLE=$GNS_BUILD/../build_protoc/protoc $GNS_BUILD/../../3rdparty/GameNetworkingSockets
    make -j10
    export PKG_CONFIG_PATH=$PKG_CONFIG_PATH_OLD
    cd ..
fi

if [ ! -f build_physfs/libphysfs.a ]; then
    mkdir -p build_physfs && cd build_physfs
    PHYSFS_BUILD=$(pwd)
    cmake -DCMAKE_BUILD_TYPE=Release -DPHYSFS_ARCHIVE_GRP=FALSE -DPHYSFS_ARCHIVE_WAD=FALSE -DPHYSFS_ARCHIVE_HOG=FALSE -DPHYSFS_ARCHIVE_MVL=FALSE -DPHYSFS_ARCHIVE_QPAK=FALSE -DPHYSFS_ARCHIVE_SLB=FALSE -DPHYSFS_ARCHIVE_VDF=FALSE $PHYSFS_BUILD/../../3rdparty/physfs
    make -j10
    cd ..
fi

export PKG_CONFIG_PATH_OLD=$PKG_CONFIG_PATH
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH_OLD:/usr/local/opt/openssl@1.1/lib/pkgconfig
DEBUG_QOL_CHEATS=1 OPENJKDF2_NO_ASAN=0 cmake .. &&
DEBUG_QOL_CHEATS=1 OPENJKDF2_NO_ASAN=0 make -j10 &&
cd .. &&
#cp resource/shaders/* DF2/resource/shaders/ &&
mkdir -p "/Users/$USER/Library/Application Support/OpenJKDF2/openjkdf2/resource/shaders/" &&
cp resource/shaders/* "/Users/$USER/Library/Application Support/OpenJKDF2/openjkdf2/resource/shaders/" &&
echo "Running..." &&
#codesign -s - openjkdf2-64 &&
#OPENJKDF2_ROOT="/Users/$USER/.local/share/openjkdf2_todoa" LSAN_OPTIONS="suppressions=/Users/$USER/workspace/OpenJKDF2/suppr.txt" ASAN_OPTIONS="log_path=/Users/$USER/workspace/OpenJKDF2/asan.log"  lldb -o run ./OpenJKDF2.app/Contents/MacOS/openjkdf2-64 -- -enhancedCogVerbs #-- -verboseNetworking
#OPENJKDF2_ROOT="/Users/$USER/.local/share/openjkdf2_todoa2012" LSAN_OPTIONS="suppressions=/Users/$USER/workspace/OpenJKDF2/suppr.txt" ASAN_OPTIONS="log_path=/Users/$USER/workspace/OpenJKDF2/asan.log"  lldb -o run ./OpenJKDF2.app/Contents/MacOS/openjkdf2-64 -- -enhancedCogVerbs #-- -verboseNetworking
#OPENJKDF2_ROOT="/Users/$USER/.local/share/openjkdf2-cds" LSAN_OPTIONS="suppressions=/Users/$USER/workspace/OpenJKDF2/suppr.txt" ASAN_OPTIONS="log_path=/Users/$USER/workspace/OpenJKDF2/asan.log"  lldb -o run ./OpenJKDF2.app/Contents/MacOS/openjkdf2-64 -- -enhancedCogVerbs #-- -verboseNetworking
LSAN_OPTIONS="suppressions=/Users/$USER/workspace/OpenJKDF2/suppr.txt" ASAN_OPTIONS="log_path=/Users/$USER/workspace/OpenJKDF2/asan.log detect_leaks=0"  lldb -o run ./OpenJKDF2.app/Contents/MacOS/openjkdf2-64 #-- -verboseNetworking
#lldb -o run ./OpenJKDF2.app/Contents/MacOS/openjkdf2-64
#open OpenJKDF2.app


#LSAN_OPTIONS="suppressions=/Users/$USER/workspace/OpenJKDF2/suppr.txt" ASAN_OPTIONS="detect_leaks=1:log_path=/Users/$USER/workspace/OpenJKDF2/asan.log" ./OpenJKDF2.app/Contents/MacOS/openjkdf2-64
