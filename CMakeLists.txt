# - Top level build script of Geant4Data
#
#-----------------------------------------------------------------------
# Copyright 2012 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012 University of Warwick
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)
#-----------------------------------------------------------------------
cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)
project(Geant4Data)

#-----------------------------------------------------------------------
# Setup installation directories
# TODO : Check which versions of CMake supply this module.
include(GNUInstallDirs OPTIONAL)

#-----------------------------------------------------------------------
# Define the known datasets as a list of tuples, tuple entries being
# colon separated
# Tuple entries:
# 0 : Directory Name
# 1 : Version
# 2 : Filename
# 3 : Filename Extension
# 4 : Environment Variable
# 5 : Marker for detecting existing install (expected file)
# 6 : Expected MD5 sum File (support only on CMake > ?)
#
# This is a mess (make note on rationalizing filename and
# Environment variable, e.g.
# name-version.tar.bz2 => env(G4_NAME_DATADIR)
#
# Could also supply this list as a text file to be read in
set(GEANT4_DATASETS_URL $ENV{HOME}/Downloads)
message(STATUS "GEANT4_DATASETS_URL : ${GEANT4_DATASETS_URL}")

set(GEANT4_DATASETS
  G4NDL:4.0:G4NDL:tar.gz:G4NEUTRONHPDATA:foobar:frob
  G4EMLOW:6.26:G4EMLOW:tar.gz:G4LEDATA:foobar:frob
  PhotonEvaporation:2.2:PhotonEvaporation:tar.gz:G4LEVELGAMMADATA:foobar:frob
  RadioactiveDecay:3.4:G4RadioactiveDecay:tar.gz:G4RADIOACTIVEDATA:foobar:frob
  G4ABLA:3.0:G4ABLA:tar.gz:G4ABLADATA:foobar:frob
  G4NEUTRONXS:1.1:G4NEUTRONXS:tar.gz:G4NEUTRONXSDATA:foobar:frob
  G4PII:1.3:G4PII:tar.gz:G4PIIDATA:foobar:frob
  RealSurface:1.0:RealSurface:tar.gz:G4REALSURFACEDATA:foobar:frob)

set(GEANT4_DATASETS_NEWSTYLE
  6d23aff98e5706af1dd7fff397969f37:G4NDL.4.0.tar.gz
  98cdca1024bf168c8f8ca1deb40f43b9:G4EMLOW.6.23.tar.gz
  8010e7ce8a92564e38dd3418e6040563:G4PhotonEvaporation.2.2.tar.gz
  a5b681048584631608ab0965b33c7959:G4RadioactiveDecay.3.4.tar.gz
  d7049166ef74a592cb97df0ed4b757bd:G4ABLA.3.0.tar.gz
  61ef3a05b56525db04e11820e3f603f1:G4NEUTRONXS.1.1.tar.gz
  05f2471dbcdf1a2b17cbff84e8e83b37:G4PII.1.3.tar.gz
  0dde95e00fcd3bcd745804f870bb6884:RealSurface.1.0.tar.gz
  )

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


  # Now we set up the install, if required
  if(GEANT4_INSTALL_DATA)
    # Drive download and unpacking via ExternalProject
    include(ExternalProject)
    
    foreach(_dataset ${GEANT4_DATASETS})
      # Listify entry and extract parameters
      string(REPLACE ":" ";" _tuple ${_dataset})
      list(GET _tuple 0 _dataset_name)
      list(GET _tuple 1 _dataset_version)
      list(GET _tuple 2 _dataset_filename)
      list(GET _tuple 3 _dataset_extension)
      list(GET _tuple 4 _dataset_envvar)
      list(GET _tuple 5 _dataset_marker)
      list(GET _tuple 6 _dataset_md5sum)

      message(STATUS "Checking for prior install of ${_dataset_name}")
    endforeach()
  endif()
endif()

#-----------------------------------------------------------------------
# Try download scheme for CMake 2.6.4...
#
set(GEANT4_DATA_URL "file://$ENV{HOME}/Downloads")
set(GEANT4_DATA_ARCHIVE "G4ABLA.3.0.tar.gz")
set(GEANT4_DATA_DOWNLOAD_DIR Data/Downloads)
set(TIMEOUT 1500)
set(GEANT4_DATA_STAMPDIR ${PROJECT_BINARY_DIR}/Data)

# - the target
# Sort of works like EP, but that does something fancier with
# ordering  - see the '${name}-complete' stamp
add_custom_target(Data ALL 
  COMMENT "Data target"
  DEPENDS unpack.stamp
  VERBATIM)


# - Download
configure_file(DataDownload.cmake.in download.cmake @ONLY)
add_custom_command(
  OUTPUT download.stamp
  COMMENT "test download"
  COMMAND ${CMAKE_COMMAND} -P download.cmake
  COMMAND ${CMAKE_COMMAND} -E touch download.stamp
  VERBATIM
  )

# - Checksum
configure_file(DataChecksum.cmake.in checksum.cmake @ONLY)
add_custom_command(
  OUTPUT checksum.stamp
  COMMENT "test checksum"
  COMMAND ${CMAKE_COMMAND} -P checksum.cmake
  COMMAND ${CMAKE_COMMAND} -E touch checksum.stamp
  DEPENDS download.stamp
  VERBATIM
  )

# - Unpack
add_custom_command(
  OUTPUT unpack.stamp
  COMMENT "test unpack"
  COMMAND ${CMAKE_COMMAND} -E tar xz Downloads/${GEANT4_DATA_ARCHIVE}
  COMMAND ${CMAKE_COMMAND} -E touch ${PROJECT_BINARY_DIR}/unpack.stamp
  DEPENDS checksum.stamp
  WORKING_DIRECTORY Data
  VERBATIM
  )
#-----------------------------------------------------------------------
# Function to setup downloads....
# It has as arguments:
#  Remote file
#  Local file
#  Timeout for download
#  Expected MD5 of file 
#  Which directory to unpack in
function(geant4_add_data_download)
  message(STATUS "geant4_add_data_download(${ARGN})")
endfunction()


geant4_add_data_download()
geant4_add_data_download(remote local timout md5 dir)


#-----------------------------------------------------------------------
# Build the C++ data location/access API library and test programs
#
add_subdirectory(Source)

#-----------------------------------------------------------------------
# Final messaging of results (because we don't generate scripts for now)
#
message(STATUS "GEANT4_INSTALL_DATADIR : ${GEANT4_INSTALL_DATADIR}")
message(STATUS "GEANT4_INSTALL_FULL_DATADIR : ${GEANT4_INSTALL_FULL_DATADIR}")

