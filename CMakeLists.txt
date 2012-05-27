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
# Check report of CMake version
#
message(STATUS "configuring with CMake ${CMAKE_VERSION}")

#-----------------------------------------------------------------------
# Project modules
#
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

#-----------------------------------------------------------------------
# Include needed modules
#
include(GNUInstallDirs)
include(CMakeMacroParseArguments)

#-----------------------------------------------------------------------
# Define the known datasets as a list of tuples, tuple entries being
# colon separated
# Tuple entries:
# 0 : Directory Name
# 1 : Version
# 2 : Filename
# 3 : Filename Extension
# 4 : Environment Variable
# 5 : Expected MD5 sum File
# 6 : (NOTIMPLEMENTEDYET) Marker for detecting existing install (expected file)
#
# This is a mess (make note on rationalizing filename and
# Environment variable, e.g.
# name-version.tar.bz2 => env(G4_NAME_DATADIR)
#
# Use a local URL for development
set(GEANT4_DATASETS_URL "file://$ENV{HOME}/Downloads")
message(STATUS "GEANT4_DATASETS_URL : ${GEANT4_DATASETS_URL}")

set(GEANT4_DATASETS
  G4NDL/4.0/G4NDL/tar.gz/G4NEUTRONHPDATA/6d23aff98e5706af1dd7fff397969f37
  G4EMLOW/6.23/G4EMLOW/tar.gz/G4LEDATA/98cdca1024bf168c8f8ca1deb40f43b9
  PhotonEvaporation/2.2/G4PhotonEvaporation/tar.gz/G4LEVELGAMMADATA/8010e7ce8a92564e38dd3418e6040563
  RadioactiveDecay/3.4/G4RadioactiveDecay/tar.gz/G4RADIOACTIVEDATA/a5b681048584631608ab0965b33c7959
  G4ABLA/3.0/G4ABLA/tar.gz/G4ABLADATA/d7049166ef74a592cb97df0ed4b757bd
  G4NEUTRONXS/1.1/G4NEUTRONXS/tar.gz/G4NEUTRONXSDATA/61ef3a05b56525db04e11820e3f603f1
  G4PII/1.3/G4PII/tar.gz/G4PIIDATA/05f2471dbcdf1a2b17cbff84e8e83b37
  RealSurface/1.0/RealSurface/tar.gz/G4REALSURFACEDATA/0dde95e00fcd3bcd745804f870bb6884
  )

# - API for downloading and installing data
# - Download, verify and unpack data on CMake versions with unsupported
# ExternalProject
function(_geant4_dataproject _name)
  # - Parse arguments and create any extra needed variables
  set(oneValueArgs PREFIX SOURCE_DIR URL URL_MD5 TIMEOUT)
  cmake_parse_arguments(_G4DATA "" "${oneValueArgs}" "" ${ARGN})
  get_filename_component(_G4DATA_FILE ${_G4DATA_URL} NAME)

  # - Write Download script
  file(WRITE ${PROJECT_BINARY_DIR}/${_G4DATA_PREFIX}/${_name}-Download.cmake "
  message(STATUS \"downloading ${_G4DATA_URL}...\")
  file(DOWNLOAD
    ${_G4DATA_URL}
    \"${_G4DATA_PREFIX}/${_G4DATA_FILE}\"
    TIMEOUT ${_G4DATA_TIMEOUT}
    STATUS _status
    LOG _log)
  list(GET _status 0 _status_code)
  list(GET _status 1 _status_msg)
  if(NOT _status_code EQUAL 0)
    message(FATAL_ERROR \"error: downloading ${_G4DATA_URL} failed
      status_code : \${_status_code}
      status_msg  : \${_status_msg}
      log : ${_log}\"
      )
  endif()
  ")

  # - Write Verify script
  file(WRITE ${PROJECT_BINARY_DIR}/${_G4DATA_PREFIX}/${_name}-Verify.cmake "
  message(STATUS \"verifying ${_G4DATA_FILE} ...\")
  execute_process(
    COMMAND \"${CMAKE_COMMAND}\" -E md5sum \"${PROJECT_BINARY_DIR}/${_G4DATA_PREFIX}/${_G4DATA_FILE}\"
    OUTPUT_VARIABLE ov
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE _result_code
    )
  if(NOT _result_code EQUAL 0)
    message(FATAL_ERROR \"error: computing md5sum of ${_G4DATA_FILE} failed\")
  endif()
  string(REGEX MATCH \"^([0-9A-Fa-f]+)\" md5_actual \"\${ov}\")
  string(TOLOWER \"\${md5_actual}\" md5_actual)
  string(TOLOWER \"${_G4DATA_URL_MD5}\" md5)
  if(NOT \"\${md5}\" STREQUAL \"\${md5_actual}\")
    message(FATAL_ERROR \"error: md5sum of '${_G4DATA_FILE}' does not match expected value
  md5_expected: \${md5}
    md5_actual: \${md5_actual}
\")
  endif()
  message(STATUS \"verifying ${_G4DATA_FILE} ... done\")
  ")

  # - Write Unpack script
  if(${_G4DATA_FILE} MATCHES "(\\.|=)(tar\\.gz|zip)$")
  else()
    message(FATAL_ERROR "error: do not know how to extract '${_G4DATA_FILE}' -- file needs to be .tar.gz or zip")
  endif()
  file(WRITE ${PROJECT_BINARY_DIR}/${_G4DATA_PREFIX}/${_name}-Unpack.cmake "
  message(STATUS \"unpacking ${_G4DATA_FILE} ...\")
  file(MAKE_DIRECTORY ${_G4DATA_SOURCE_DIR})
  execute_process(COMMAND
    \${CMAKE_COMMAND} -E tar xfz \"${PROJECT_BINARY_DIR}/${_G4DATA_PREFIX}/${_G4DATA_FILE}\"
    WORKING_DIRECTORY ${_G4DATA_SOURCE_DIR}
    RESULT_VARIABLE rv)
  if(NOT rv EQUAL 0)
    message(STATUS \"extracting... [error clean up]\")
    file(REMOVE_RECURSE \"${_G4DATA_SOURCE_DIR}\")
    message(FATAL_ERROR \"error: extract of '${_G4DATA_FILE}' failed\")
  endif()
  message(STATUS \"unpacking ${_G4DATA_FILE} ... done\")
  ")

  # - Add custom commands for each step, each depending on the last
  foreach(_step Download Verify Unpack)
    if(_last_step)
      set(_deps DEPENDS ${PROJECT_BINARY_DIR}/${_G4DATA_PREFIX}/${_name}-${_last_step}.stamp)
    endif()

    add_custom_command(
      OUTPUT ${PROJECT_BINARY_DIR}/${_G4DATA_PREFIX}/${_name}-${_step}.stamp
      COMMENT "${_step} ${_name}"
      COMMAND "${CMAKE_COMMAND}" -P ${PROJECT_BINARY_DIR}/${_G4DATA_PREFIX}/${_name}-${_step}.cmake
      COMMAND "${CMAKE_COMMAND}" -E touch ${PROJECT_BINARY_DIR}/${_G4DATA_PREFIX}/${_name}-${_step}.stamp
      VERBATIM
      ${_deps}
      )

    set(_last_step ${_step})
  endforeach()

  # - Add the main target which will run all the above steps
  add_custom_target(${_name} ALL 
    COMMENT "Completed ${_name}"
    DEPENDS ${PROJECT_BINARY_DIR}/${_G4DATA_PREFIX}/${_name}-${_last_step}.stamp
    VERBATIM
    )
endfunction()


# - Front end for all datasets
function(geant4_do_install_data _url _dataset _prefix _timeout)
  # Listify entry and extract parameters
  string(REPLACE "/" ";" _tuple ${_dataset})
  list(GET _tuple 0 _name)
  list(GET _tuple 1 _version)
  list(GET _tuple 2 _filename)
  list(GET _tuple 3 _extension)
  list(GET _tuple 4 _envvar)
  list(GET _tuple 5 _md5sum)

  # - Dispatch to ExternalProject or our own implementation depending
  # on CMake version.
  if(${CMAKE_VERSION} VERSION_GREATER "2.8.1")
    include(ExternalProject)
    ExternalProject_Add(${_name}
      PREFIX Externals/${_filename}-${_version}
      SOURCE_DIR data/${_name}${_version}
      URL ${_url}/${_filename}.${_version}.${_extension}
      URL_MD5 ${_md5sum}
      TIMEOUT 1500
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
      )
  else()
    _geant4_dataproject(${_name}
      PREFIX Externals/${_filename}-${_version}
      SOURCE_DIR data
      URL ${_url}/${_filename}.${_version}.${_extension}
      URL_MD5 ${_md5sum}
      TIMEOUT 1500
      )
  endif()

  # - Add install target, and report back paths...
  install(DIRECTORY ${PROJECT_BINARY_DIR}/data/${_name}${_version}
    DESTINATION ${_prefix}
    COMPONENT Data
    )
endfunction()


#-----------------------------------------------------------------------
# USER INTERFACE AND PROCESSING
#
#-----------------------------------------------------------------------
# User options for installing data
# - Choose whether to install data, which both files and environment
# - Choose a directory under which to install the data.
# - Change download timeout for problematic connections
#
option(GEANT4_INSTALL_DATA "Download and install Geant4 Data Libraries" OFF)

#-----------------------------------------------------------------------
# The Physics Data Install Dir
# Try to follow as far as possible what's done in GNUInstallDirs
# TODO : select an appropriate default.
if(NOT GEANT4_INSTALL_DATADIR)
  set(GEANT4_INSTALL_DATADIR "" CACHE PATH "read-only architecture independent Geant4 physics data (DATAROOTDIR/Geant4Data)")
  set(GEANT4_INSTALL_DATADIR "${CMAKE_INSTALL_DATAROOTDIR}/Geant4Data")
endif()

if(NOT IS_ABSOLUTE ${GEANT4_INSTALL_DATADIR})
  set(GEANT4_INSTALL_FULL_DATADIR "${CMAKE_INSTALL_PREFIX}/${GEANT4_INSTALL_DATADIR}")
else()
  set(GEANT4_INSTALL_FULL_DATADIR "${GEANT4_INSTALL_DATADIR}")
endif()

mark_as_advanced(GEANT4_INSTALL_DATADIR)

#-----------------------------------------------------------------------
# Provide an option for increasing the download timeout
# Helps with large datasets over slow connections.
set(GEANT4_INSTALL_DATA_TIMEOUT 1500 CACHE STRING "Timeout for Data Library download")
mark_as_advanced(GEANT4_INSTALL_DATA_TIMEOUT)

#-----------------------------------------------------------------------
# Set up check, download and install of needed data
#
if(GEANT4_INSTALL_DATA)
  foreach(_dataset ${GEANT4_DATASETS})
    # - DO INSTALL
    # Input is url, filename, wheretodownload, where to unpack
    # Output is path to unpack location
    # Final install locations will always be under GEANT4_INSTALL_DATADIR
    # Build locations may be a mix of in build and prior installs...
    # For now, just assume we download and install everything
    geant4_do_install_data(${GEANT4_DATASETS_URL} ${_dataset} ${GEANT4_INSTALL_FULL_DATADIR} ${GEANT4_INSTALL_DATA_TIMEOUT})
  endforeach()
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

