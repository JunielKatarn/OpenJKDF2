set(PHYSFS_ROOT ${CMAKE_BINARY_DIR}/physfs CACHE PATH PHYSFS_ROOT)
if(TARGET_WIN32) # Due to PhysFS’ incompleteness
    set(OPTIONAL_LIBRARY_LIBS -DOPTIONAL_LIBRARY_LIBS:FILE=advapi32;shell32;user32)
endif()
ExternalProject_Add(
    PHYSFS
    SOURCE_DIR          ${CMAKE_SOURCE_DIR}/lib/physfs
    BINARY_DIR          ${PHYSFS_ROOT}
    INSTALL_DIR         ${PHYSFS_ROOT}
    UPDATE_DISCONNECTED TRUE
    CMAKE_ARGS          --toolchain ${CMAKE_TOOLCHAIN_FILE}
                        --install-prefix ${PHYSFS_ROOT}
                        -DCMAKE_BUILD_TYPE:STRING=Release
                        -DPHYSFS_BUILD_STATIC:BOOL=TRUE
                        -DPHYSFS_BUILD_SHARED:BOOL=TRUE
                        -DPHYSFS_ARCHIVE_7Z:BOOL=FALSE
                        -DPHYSFS_ARCHIVE_GRP:BOOL=FALSE
                        -DPHYSFS_ARCHIVE_WAD:BOOL=FALSE
                        -DPHYSFS_ARCHIVE_HOG:BOOL=FALSE
                        -DPHYSFS_ARCHIVE_MVL:BOOL=FALSE
                        -DPHYSFS_ARCHIVE_QPAK:BOOL=FALSE
                        -DPHYSFS_ARCHIVE_SLB:BOOL=FALSE
                        -DPHYSFS_ARCHIVE_VDF:BOOL=FALSE
                        -DPHYSFS_BUILD_TEST:BOOL=FALSE
                        -DPHYSFS_BUILD_DOCS:BOOL=FALSE
    PATCH_COMMAND git apply --reverse --check ${PROJECT_SOURCE_DIR}/lib/physfs_macos.patch
    CMAKE_CACHE_ARGS    ${OPTIONAL_LIBRARY_LIBS}
)
# *Replicate* variables generated by `FindPhysFS`
set(PHYSFS_FOUND TRUE)
set(PHYSFS_INCLUDE_DIR ${PHYSFS_ROOT}/include)
set(PHYSFS_LIBRARY physfs)

# Create physfs imported targets (😒 actually, this is FindPhysFS’ job)
add_library(PhysFS::PhysFS_s STATIC IMPORTED)
add_dependencies(PhysFS::PhysFS_s PHYSFS)
file(MAKE_DIRECTORY ${PHYSFS_INCLUDE_DIR})
set_property(
    TARGET PhysFS::PhysFS_s
    PROPERTY IMPORTED_LOCATION
    ${PHYSFS_ROOT}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${PHYSFS_LIBRARY}${CMAKE_STATIC_LIBRARY_SUFFIX}
)
target_include_directories(PhysFS::PhysFS_s INTERFACE ${PHYSFS_INCLUDE_DIR})
target_link_directories(PhysFS::PhysFS_s INTERFACE ${PHYSFS_INCLUDE_DIR}/../lib)
if(TARGET_WIN32)
    target_link_libraries(PhysFS::PhysFS_s INTERFACE advapi32 shell32 user32)
endif()

add_library(PhysFS::PhysFS SHARED IMPORTED)
add_dependencies(PhysFS::PhysFS PHYSFS)
set_target_properties(
    PhysFS::PhysFS PROPERTIES
    IMPORTED_LOCATION
    ${PHYSFS_ROOT}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}${PHYSFS_LIBRARY}${CMAKE_SHARED_LIBRARY_SUFFIX}
    IMPORTED_IMPLIB
    ${PHYSFS_ROOT}/lib/${CMAKE_IMPORT_LIBRARY_PREFIX}${PHYSFS_LIBRARY}${CMAKE_IMPORT_LIBRARY_SUFFIX}
)
target_include_directories(PhysFS::PhysFS INTERFACE ${PHYSFS_INCLUDE_DIR})
target_link_directories(PhysFS::PhysFS INTERFACE ${PHYSFS_ROOT}/lib)
