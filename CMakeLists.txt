cmake_minimum_required(VERSION 3.29 FATAL_ERROR)
set(ARCH "x86_64")
set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

#Base flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wformat-security -Wno-unused -Wno-array-bounds -Wno-stringop-overflow")

if(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES MinSizeRel OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
  set(REL_BUILD "true")
endif()

#Platform-specific flags
if(WIN32 OR WIN64)
  add_definitions(-DWINDOWS)
  if(${REL_BUILD} MATCHES "true")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--high-entropy-va -Wl,--dynamicbase -Wl,--nxcompat")
  endif()
elseif(UNIX)
  add_definitions(-DLINUX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  if(${REL_BUILD} MATCHES "true")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE")
  endif()
elseif(APPLE)
  add_definitions(-DOSX)
endif()

if(${REL_BUILD} MATCHES "true")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")
  add_definitions(-DRELEASE)
elseif(CMAKE_BUILD_TYPE MATCHES Debug)
  set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0")
  add_definitions(-DDEBUG)
endif()

FetchContent_Declare(libcommons
                     GIT_REPOSITORY "https://github.com/izzyaxel/libcommons.git"
                     GIT_TAG "origin/master"
                     SOURCE_DIR "modules/libcommons")
FetchContent_MakeAvailable(libcommons)

include_directories("${CMAKE_BINARY_DIR}/modules/libcommons/src/")

set(SRC
    src/gl.cc
    src/export.hh
    src/glrRenderer.cc src/glrRenderer.hh
    src/glrFramebuffer.cc src/glrFramebuffer.hh
    src/glrPostProcessing.cc src/glrPostProcessing.hh
    src/glrTexture.cc src/glrTexture.hh
    src/glrShader.cc src/glrShader.hh
    src/glrAtlas.cc src/glrAtlas.hh
    src/glrImage.cc src/glrImage.hh
    src/glrColor.cc src/glrColor.hh
    src/glrMesh.cc src/glrMesh.hh
    src/glrExternal.cc src/glrExternal.hh
    src/glrRenderable.hh
    src/glrRenderList.cc
    src/glrRenderList.hh
    src/glrShaderPipeline.cc
    src/glrShaderPipeline.hh
    src/glrUtil.cc
    src/glrUtil.hh)

project(GLRender)
include_directories(include)
add_library(${PROJECT_NAME} SHARED ${SRC})

add_dependencies(${PROJECT_NAME} commons)
target_link_libraries(${PROJECT_NAME} commons)
