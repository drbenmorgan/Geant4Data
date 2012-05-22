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

  # The Physics Data Install Dir
  # Try to follow as far as possible what's done in GNUInstallDirs
  # *Should* default to CMAKE_INSTALL_DATAROOTDIR?
  if(NOT GEANT4_INSTALL_DATADIR)
    set(GEANT4_INSTALL_DATADIR "" CACHE PATH "read-only architecture independent Geant4 physics data (DATAROOTDIR/Geant4Data)")
    set(GEANT4_INSTALL_DATADIR "${CMAKE_INSTALL_DATAROOTDIR}/Geant4Data")
  endif()

  mark_as_advanced(GEANT4_INSTALL_DATADIR)

  if(NOT IS_ABSOLUTE ${GEANT4_INSTALL_DATADIR})
    set(GEANT4_INSTALL_FULL_DATADIR "${CMAKE_INSTALL_PREFIX}/${GEANT4_INSTALL_DATADIR}")
  else()
    set(GEANT4_INSTALL_FULL_DATADIR "${GEANT4_INSTALL_DATADIR}")
  endif()

  # Provide an option for increasing the download timeout
  # Helps with large datasets over slow connections.
  # Only supported for CMake 2.8.1 and higher.
  if(${CMAKE_VERSION} VERSION_GREATER 2.8.0)
    set(GEANT4_INSTALL_DATA_TIMEOUT 1500 CACHE STRING "Time in seconds for Data Library download")
    set(_geant4data_timeout_cmd TIMEOUT ${GEANT4_INSTALL_DATA_TIMEOUT})
    mark_as_advanced(GEANT4_INSTALL_DATA_TIMEOUT)
  endif()

endif()

#-----------------------------------------------------------------------
# Build the C++ data location/access API library and test programs
#
add_subdirectory(Source)

#-----------------------------------------------------------------------
# Final messaging of results (because we don't generate scripts for now)
#
message(STATUS "GEANT4_INSTALL_DATADIR : ${GEANT4_INSTALL_DATADIR}")
message(STATUS "GEANT4_INSTALL_FULL_DATADIR : ${GEANT4_INSTALL_FULL_DATADIR}")

