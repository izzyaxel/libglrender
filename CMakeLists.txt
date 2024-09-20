cmake_minimum_required(VERSION 3.29 FATAL_ERROR)
set(ARCH "x86_64")
set(CMAKE_CXX_STANDARD 23)

#Base flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wformat-security -Wno-unused -Wno-array-bounds -Wno-stringop-overflow")

if(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES MinSizeRel OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
  set(REL_BUILD "true")
endif()

set(BUILD_TYPE "GLRender: ")

#Platform-specific flags
if(WIN32 OR WIN64)
  set(BUILD_TYPE ${BUILD_TYPE} "Windows ")
  add_definitions(-DWINDOWS)
  if(${REL_BUILD} MATCHES "true")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--high-entropy-va -Wl,--dynamicbase -Wl,--nxcompat")
  endif()
elseif(UNIX)
  set(BUILD_TYPE ${BUILD_TYPE} "Unix ")
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

add_subdirectory("libcommons" "${CMAKE_CURRENT_BINARY_DIR}/libcommons/build/")
include_directories("libcommons/src/")

set(SRC
    src/gl.cc
    src/export.hh
    src/glrEnums.hh
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
    src/glrRenderList.hh)

project(GLRender)
include_directories(include)
add_library(${PROJECT_NAME} SHARED ${SRC})

add_dependencies(${PROJECT_NAME} commons)
target_link_libraries(${PROJECT_NAME} commons)
target_precompile_headers(${PROJECT_NAME} PUBLIC src/precompile.hh)

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(ARCHITECTURE "32")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCHITECTURE "64")
endif()

if(${REL_BUILD} MATCHES "true")
  set(BUILD_TYPE ${BUILD_TYPE} ${ARCHITECTURE} "-bit release build")
elseif(CMAKE_BUILD_TYPE MATCHES Debug)
  set(BUILD_TYPE ${BUILD_TYPE} ${ARCHITECTURE} "-bit debug build (hardening is disabled for debug builds)")
endif()
message(${BUILD_TYPE})
