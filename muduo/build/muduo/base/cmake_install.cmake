# Install script for directory: /home/skeleton/桌面/muduo/muduo/muduo/base

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/skeleton/桌面/muduo/muduo/build/lib/libmuduo_base.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  include("/home/skeleton/桌面/muduo/muduo/build/muduo/base/CMakeFiles/muduo_base.dir/install-cxx-module-bmi-Release.cmake" OPTIONAL)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/muduo/base" TYPE FILE FILES
    "/home/skeleton/桌面/muduo/muduo/muduo/base/AsyncLogging.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/Atomic.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/BlockingQueue.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/BoundedBlockingQueue.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/Condition.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/CountDownLatch.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/CurrentThread.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/Date.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/Exception.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/FileUtil.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/GzipFile.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/LogFile.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/LogStream.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/Logging.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/Mutex.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/ProcessInfo.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/Singleton.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/StringPiece.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/Thread.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/ThreadLocal.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/ThreadLocalSingleton.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/ThreadPool.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/TimeZone.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/Timestamp.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/Types.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/WeakCallback.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/copyable.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/base/noncopyable.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/skeleton/桌面/muduo/muduo/build/muduo/base/tests/cmake_install.cmake")

endif()

