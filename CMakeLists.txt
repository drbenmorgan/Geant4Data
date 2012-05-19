# - Top level build script of Geant4Data
#
#-----------------------------------------------------------------------
# Copyright 2012 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012 University of Warwick
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)
#-----------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Geant4Data)

#-----------------------------------------------------------------------
# Setup installation directories
# TODO : Check which versions of CMake supply this module.
include(GNUInstallDirs)

#-----------------------------------------------------------------------
# CMake options for installing data
# - Choose a directory under which to install the data.
# - Choose which libraries to install.
# - If a requested library already exists, use that.
#
# Use ExternalProject to drive the download, so CMake 2.8 or higher only
if(${CMAKE_VERSION} VERSION_GREATER 2.7)
  option(GEANT4_INSTALL_DATA "Download and install Geant4 Data Libraries" OFF)

  # Provide an option for increasing the download timeout
  # Helps with large datasets over slow connections.
  # Only supported for CMake 2.8.1 and higher.
  if(${CMAKE_VERSION} VERSION_GREATER 2.8.0)
    set(GEANT4_INSTALL_DATA_TIMEOUT 1500 CACHE STRING "Time in seconds for Data Library download")
    set(_geant4data_timeout_cmd TIMEOUT ${GEANT4_INSTALL_DATA_TIMEOUT})
  endif()

endif()

#-----------------------------------------------------------------------
# Build the C++ data location/access API library and test programs
#
add_subdirectory(Source)

