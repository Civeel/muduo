# Install script for directory: /home/skeleton/桌面/muduo/muduo/muduo/net

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/skeleton/桌面/muduo/muduo/build/lib/libmuduo_net.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  include("/home/skeleton/桌面/muduo/muduo/build/muduo/net/CMakeFiles/muduo_net.dir/install-cxx-module-bmi-Release.cmake" OPTIONAL)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/muduo/net" TYPE FILE FILES
    "/home/skeleton/桌面/muduo/muduo/muduo/net/Buffer.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/net/Callbacks.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/net/Channel.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/net/Endian.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/net/EventLoop.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/net/EventLoopThread.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/net/EventLoopThreadPool.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/net/InetAddress.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/net/TcpClient.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/net/TcpConnection.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/net/TcpServer.h"
    "/home/skeleton/桌面/muduo/muduo/muduo/net/TimerId.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/skeleton/桌面/muduo/muduo/build/muduo/net/http/cmake_install.cmake")
  include("/home/skeleton/桌面/muduo/muduo/build/muduo/net/inspect/cmake_install.cmake")
  include("/home/skeleton/桌面/muduo/muduo/build/muduo/net/tests/cmake_install.cmake")

endif()

