param(
	[ValidateSet('Release')]
	[string] $BuildType = 'Release',

	[System.IO.FileInfo] $CmakeBin = "${env:ProgramFiles}\CMake\bin\cmake.exe"
)

$releaseCommit = $(git log -1 --format='%H')
$releaseCommitShort = $(git rev-parse --short=8 HEAD)

New-Item -ItemType Directory -Force build_win

if (! (Test-Path $PSScriptRoot\build_protoc\protoc)) {
	New-Item -ItemType Directory -Force build_protoc
	& $CmakeBin `
		-DCMAKE_BUILD_TYPE=$BuildType `
		-Dprotobuf_BUILD_TESTS=OFF `
		$($PSScriptRoot | Split-Path | Split-Path)\3rdparty\protobuf\cmake

	#TODO: make
}
