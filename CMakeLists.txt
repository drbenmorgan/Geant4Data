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
set(${PROJECT_NAME}_VERSION "1.0.0")

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
# GEANT4 PHYSICS DATA - API AND GLOBAL CMAKE VARIABLES
#
#-----------------------------------------------------------------------
# URLs, directories and dataset entries
# We may want these as properties so we can have a small API for
# retrieving them globally
#-----------------------------------------------------------------------
# Geant4 Data Repository
set(GEANT4_DATASETS_URL "http://geant4.cern.ch/support/source")

# Where to install data in the build tree
set(GEANT4_BUILD_FULL_DATADIR ${PROJECT_BINARY_DIR}/data)

# Where to install data in the install tree (a Default)
set(GEANT4_INSTALL_DATADIR_DEFAULT "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}-${${PROJECT_NAME}_VERSION}/data")

message(STATUS "GEANT4_DATASETS_URL : ${GEANT4_DATASETS_URL}")

# Define the known datasets as a list of tuples, tuple entries being
# forward slash separated. Messy.
# Tuple entries:
# 0 : Directory Name
# 1 : Version
# 2 : Filename
# 3 : Filename Extension
# 4 : Environment Variable
# 5 : Expected MD5 sum File
# 6 : (NOTIMPLEMENTEDYET) Marker for detecting existing install
#
set(GEANT4_DATASETS
  G4NDL/4.0/G4NDL/tar.gz/G4NEUTRONHPDATA/6d23aff98e5706af1dd7fff397969f37
  G4EMLOW/6.26/G4EMLOW/tar.gz/G4LEDATA/637f440a38b136f2e4df51f1cf0a02f9
  PhotonEvaporation/2.2/G4PhotonEvaporation/tar.gz/G4LEVELGAMMADATA/8010e7ce8a92564e38dd3418e6040563
  RadioactiveDecay/3.4/G4RadioactiveDecay/tar.gz/G4RADIOACTIVEDATA/a5b681048584631608ab0965b33c7959
  G4ABLA/3.0/G4ABLA/tar.gz/G4ABLADATA/d7049166ef74a592cb97df0ed4b757bd
  G4NEUTRONXS/1.1/G4NEUTRONXS/tar.gz/G4NEUTRONXSDATA/61ef3a05b56525db04e11820e3f603f1
  G4PII/1.3/G4PII/tar.gz/G4PIIDATA/05f2471dbcdf1a2b17cbff84e8e83b37
  RealSurface/1.0/RealSurface/tar.gz/G4REALSURFACEDATA/0dde95e00fcd3bcd745804f870bb6884
  )

#-----------------------------------------------------------------------
# API for downloading and installing data
#-----------------------------------------------------------------------
# Properties?
define_property(GLOBAL PROPERTY "GEANT4_DATASETS"
  BRIEF_DOCS "List of all defined Geant4 dataset names"
  FULL_DOCS
  "Each element of the list gives the name defined for the dataset"
  "This name can be used in other Geant4 Data API functions to"
  "extract other properties of the dataset"
  )

#-----------------------------------------------------------------------
# function geant4_add_dataset(NAME id
#                             VERSION ver
#                             FILENAME file
#                             EXTENSION ext
#                             ENVVAR varname
#                             MD5SUM md5)
function(geant4_add_dataset)
  # - Parse arguments and create variables
  set(oneValueArgs NAME VERSION FILENAME EXTENSION ENVVAR MD5SUM)
  cmake_parse_arguments(_G4ADDDATA "" "${oneValueArgs}" "" ${ARGN})

  set(_fullfilename "${_G4ADDDATA_FILENAME}.${_G4ADDDATA_VERSION}.${_G4ADDDATA_EXTENSION}")
  set(_basedirectory "${_G4ADDDATA_NAME}${_G4ADDDATA_VERSION}")  
endfunction()

#-----------------------------------------------------------------------
# function _geant4_data_project(<name>
#                               PREFIX installdir
#                               SOURCE_DIR wheretounpack
#                               URL whattodownload
#                               URL_MD5 expectedMD5ofdownload
#                               TIMEOUT timeoutafter(seconds)
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
    STATUS _status)
    # LOG _log) - LOG with TIMEOUT fails due to curl issue, prefer TIMEOUT
  list(GET _status 0 _status_code)
  list(GET _status 1 _status_msg)
  if(NOT _status_code EQUAL 0)
    message(FATAL_ERROR \"error: downloading ${_G4DATA_URL} failed
      status_code : \${_status_code}
      status_msg  : \${_status_msg}
      #log : \${_log}\"
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


#-----------------------------------------------------------------------
# - Dispatch download task according to CMake version, and install data
# function geant4_do_install_data(<repourl>,
#                                 <dataset_tuple>,
#                                 <prefix>,
#                                 <download_timeout>
function(geant4_do_install_data _url _dataset _prefix _timeout)
  # Listify tuple and extract parameters
  string(REPLACE "/" ";" _tuple ${_dataset})
  list(GET _tuple 0 _name)
  list(GET _tuple 1 _version)
  list(GET _tuple 2 _filename)
  list(GET _tuple 3 _extension)
  list(GET _tuple 4 _envvar)
  list(GET _tuple 5 _md5sum)

  # - Dispatch to ExternalProject or our own implementation.
  # Use of URL_MD5 *and* TIMEOUT require CMake 2.8.2 or higher.
  if(${CMAKE_VERSION} VERSION_GREATER "2.8.1")
    include(ExternalProject)
    ExternalProject_Add(${_name}
      PREFIX Externals/${_filename}-${_version}
      SOURCE_DIR ${GEANT4_BUILD_FULL_DATADIR}/${_name}${_version}
      URL ${_url}/${_filename}.${_version}.${_extension}
      URL_MD5 ${_md5sum}
      TIMEOUT ${_timeout}
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
      )
  else()
    _geant4_dataproject(${_name}
      PREFIX Externals/${_filename}-${_version}
      SOURCE_DIR ${GEANT4_BUILD_FULL_DATADIR}
      URL ${_url}/${_filename}.${_version}.${_extension}
      URL_MD5 ${_md5sum}
      TIMEOUT ${_timeout}
      )
  endif()

  # - Add install target, and report back paths...
  install(DIRECTORY ${PROJECT_BINARY_DIR}/data/${_name}${_version}
    DESTINATION ${_prefix}
    COMPONENT Data
    )
endfunction()


#-----------------------------------------------------------------------
# GEANT4 PHYSICS DATA - USER INTERFACE AND PROCESSING
#
#-----------------------------------------------------------------------
# User options for installing data
# - Choose whether to install data, which both files and environment
# - Choose a directory under which to install the data.
# - Change download timeout for problematic connections
#
option(GEANT4_INSTALL_DATA "Download and install Geant4 Data Libraries" OFF)

#-----------------------------------------------------------------------
# Choose Physics Data Install Dir
# This follows the pattern for interface and setting as in GNUInstallDirs
if(NOT GEANT4_INSTALL_DATADIR)
  set(GEANT4_INSTALL_DATADIR "" CACHE PATH "read-only architecture independent Geant4 physics data (DATAROOTDIR/${GEANT4_INSTALL_DATADIR_DEFAULT}")
  set(GEANT4_INSTALL_DATADIR "${GEANT4_INSTALL_DATADIR_DEFAULT}")
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
message(STATUS "GEANT4_BUILD_FULL_DATADIR   : ${GEANT4_BUILD_FULL_DATADIR}")
message(STATUS "GEANT4_INSTALL_DATADIR      : ${GEANT4_INSTALL_DATADIR}")
message(STATUS "GEANT4_INSTALL_FULL_DATADIR : ${GEANT4_INSTALL_FULL_DATADIR}")

#-----------------------------------------------------------------------
# For scripts, we have two places where the build and install paths
# need to be added:
# 
# 1) geant4.(c)sh
# 2) geant4make.(c)sh
#
# and these have to be configured for both the build and install trees.
# As the scripts derive paths from their own location, we need to
# output absolute paths so relative paths can be calculated later.
# They are also needed for testing - this assumes, like the above, that
# all datasets will be installed if we choose to install data at all.
# This means that we can set a variable, say GEANT4_BUILD_FULL_DATADIR, 
# that will point to the build tree directory containing the data files
# In the above it's ${PROJECT_BINARY_DIR}/data
#
# This isn't future proof against the case of a mix of new and preexisting
# installs, but gives consistent behaviour in the short term.
# In the future, we will need a pair of variables for each dataset, e.g.
#
#  GEANT4_INSTALL_FULL_DATADIR_${DATASET_NAME}
#  GEANT4_BUILD_FULL_DATADIR_${DATASET_NAME}
#

