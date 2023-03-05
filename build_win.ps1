param(
	[ValidateSet('Release')]
	[string] $BuildType = 'Release',

	[System.IO.FileInfo] $CmakeBin = "${env:ProgramFiles}\CMake\bin\cmake.exe",

	[System.IO.DirectoryInfo] $VcpkgDir = "${PSScriptRoot}\vcpkg"
)

$releaseCommit = $(git log -1 --format='%H')
$releaseCommitShort = $(git rev-parse --short=8 HEAD)

# New-Item -ItemType Directory -Force build_win

# if (! (Test-Path $PSScriptRoot\build_protoc\protoc)) {
# 	New-Item -ItemType Directory -Force build_protoc
# 	& $CmakeBin `
# 		-DCMAKE_BUILD_TYPE=$BuildType `
# 		-Dprotobuf_BUILD_TESTS=OFF `
# 		$($PSScriptRoot | Split-Path | Split-Path)\3rdparty\protobuf\cmake

# 	#TODO: make
# }

New-Item -ItemType Directory -Force build_msvc

Push-Location build_msvc

$vcpkgBin = "${VcpkgDir}\vcpkg.exe"

& $vcpkBin install --triplet x64-windows protobuf-c protobuf gamenetworkingsockets physfs

& $CmakeBin .. --toolchain $PSScriptRoot\cmake_modules\msvc_toolchain.cmake

#TODO: make -j10 openjkdf2-64

end {
	Pop-Location
}
