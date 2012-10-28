# - Top level build script of Geant4Data
# The following functions and macros are defined to help define, install,
# reuse and export datasets and associated variables.
#
# FUNCTIONS AND MACROS
# ====================
# Public API
# ----------
# function geant4_get_datasetnames(<output variable>)
#          Store list of currently known dataset names in output variable
#
# function geant4_tupleize_datasets(<output variable>)
#          Set output variable to list of old-style dataset tuples.
#          A tuple has the format:
#            NAME/VERSION/FILENAME/EXTENSION/ENVVAR/MD5SUM
#
# function geant4_add_dataset(NAME      <id>
#                             VERSION   <ver>
#                             FILENAME  <file>
#                             EXTENSION <ext>
#                             ENVVAR    <varname>
#                             MD5SUM    <md5>)
#          Define a new dataset with the following properties
#            NAME         common name of the dataset
#            VERSION      dataset version
#            FILENAME     name of packaged dataset file
#            EXTENSION    extension of packaged dataset file
#            ENVVAR       environment variable used by Geant4 code
#                         to locate this dataset
#            MD5SUM       MD5 hash of packaged dataset file
#
# function geant4_has_dataset(<name> <output variable>)
#          Set output variable to TRUE if the named dataset exists
#          in the list of known datasets.
#
# function geant4_dataset_property(<name> <property> <output variable>)
#          Set output variable to value of dataset "name"'s property.
#          If property does not exist, then value will be blank.
#
# function geant4_dataset_isinstalled(<name>
#                                     <root directory>
#                                     <output variable>)
#          Check if named dataset is installed under the root directory.
#          Set output variable to TRUE if it is, FALSE otherwise.
#
# function geant4_install_dataset(<name> <destination> <timeout>)
#          Download dataset with name to build directory, timing out the
#          download after timeout seconds, and install it
#          into its directory under the destination directory.
#
# function geant4_reuse_dataset(<name> <destination> <is installed>)
#          Reuse the dataset with name located at destination directory.
#          If it is not installed, warn user that it will need installing
#          manually in destination.
#
# function geant4_configure_datasets(DESTINATION <dir>
#                                    DOWNLOAD    <installmissing>
#                                    TIMEOUT     <seconds>
#                                    )
#          Perform the actual heavy lifting to configure each declared
#          dataset for reuse or download as needed.
#
#
# Private API
# -----------
# function _geant4_dataproject(<name>
#                              PREFIX installdir
#                              SOURCE_DIR wheretounpack
#                              URL whattodownload
#                              URL_MD5 expectedMD5ofdownload
#                              TIMEOUT timeoutafter(seconds))
#          Download, unpack and install a dataset for CMake <= 2.8.1
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
# Properties? Shouldn't clash with the tuplized variable...
define_property(GLOBAL PROPERTY "GEANT4_DATASETS"
  BRIEF_DOCS "List of all defined Geant4 dataset names"
  FULL_DOCS
  "Each element of the list gives the name defined for the dataset.
   This name can be used in other Geant4 Data API functions to
   extract other properties of the dataset"
  )

#-----------------------------------------------------------------------
# function geant4_get_datasetnames(<output variable>)
#          Store list of currently known dataset names in output variable
#
function(geant4_get_datasetnames _output)
  get_property(_tmp GLOBAL PROPERTY GEANT4_DATASETS)
  set(${_output} ${_tmp} PARENT_SCOPE)
endfunction()

#-----------------------------------------------------------------------
# function geant4_tupleize_datasets(<output variable>)
#          Set output variable to list of old-style dataset tuples.
#          A tuple has the format:
#            NAME/VERSION/FILENAME/EXTENSION/ENVVAR/MD5SUM
#
function(geant4_tupleize_datasets _output)
  geant4_get_datasetnames(_names)
  set(_tmplist)

  foreach(_ds ${_names})
    set(_tuple ${_ds})
    foreach(_p VERSION FILENAME EXTENSION ENVVAR MD5SUM)
      get_property(_tmpprop GLOBAL PROPERTY ${_ds}_${_p})
      list(APPEND _tuple ${_tmpprop})
    endforeach()
    string(REPLACE ";" "/" _tuple "${_tuple}")
    list(APPEND _tmplist "${_tuple}")
  endforeach()
  
  set(${_output} ${_tmplist} PARENT_SCOPE)
endfunction()

#-----------------------------------------------------------------------
# function geant4_add_dataset(NAME      <id>
#                             VERSION   <ver>
#                             FILENAME  <file>
#                             EXTENSION <ext>
#                             ENVVAR    <varname>
#                             MD5SUM    <md5>)
#          Define a new dataset with the following properties
#            NAME         common name of the dataset
#            VERSION      dataset version
#            FILENAME     name of packaged dataset file
#            EXTENSION    extension of packaged dataset file
#            ENVVAR       environment variable used by Geant4 code
#                         to locate this dataset
#            MD5SUM       MD5 hash of packaged dataset file
#
function(geant4_add_dataset)
  # - Parse arguments and create variables
  set(oneValueArgs NAME VERSION FILENAME EXTENSION ENVVAR MD5SUM)
  cmake_parse_arguments(_G4ADDDATA "" "${oneValueArgs}" "" ${ARGN})

  # - Fail if already defined
  geant4_has_dataset(${_G4ADDDATA_NAME} _dsexists)
  if(_dsexists)
    message(FATAL_ERROR "Dataset ${_G4ADDDATA_NAME} is already defined")
  endif()

  # - Set properties as global props <NAME>_<PROP>
  # Simple properties...
  foreach(_prop VERSION FILENAME EXTENSION ENVVAR MD5SUM)
    set_property(GLOBAL PROPERTY ${_G4ADDDATA_NAME}_${_prop} ${_G4ADDDATA_${_prop}})
  endforeach()

  # Derived properties...
  # FILE : the full filename of the packed dataset
  set_property(GLOBAL PROPERTY ${_G4ADDDATA_NAME}_FILE 
    "${_G4ADDDATA_FILENAME}.${_G4ADDDATA_VERSION}.${_G4ADDDATA_EXTENSION}"
    )
  # DIRECTORY : the name of the directory that results from unpacking
  #             the packed dataset file.
  set_property(GLOBAL PROPERTY ${_G4ADDDATA_NAME}_DIRECTORY 
    "${_G4ADDDATA_NAME}${_G4ADDDATA_VERSION}"
    )
  # URL : remote and full URL to the packed dataset file
  set_property(GLOBAL PROPERTY ${_G4ADDDATA_NAME}_URL
    "${GEANT4_DATASETS_URL}/${_G4ADDDATA_FILENAME}.${_G4ADDDATA_VERSION}.${_G4ADDDATA_EXTENSION}"
    )

  # - add it to the list of defined datasets
  set_property(GLOBAL APPEND PROPERTY GEANT4_DATASETS ${_G4ADDDATA_NAME})
endfunction()

#-----------------------------------------------------------------------
# function geant4_has_dataset(<name> <output variable>)
#          Set output variable to TRUE if the named dataset exists
#          in the list of known datasets.
#
function(geant4_has_dataset _name _output)
  get_property(_dslist GLOBAL PROPERTY GEANT4_DATASETS)
  list(FIND _dslist ${_name} _index)
  if(_index GREATER -1)
    set(${_output} TRUE PARENT_SCOPE)
  else()
    set(${_output} FALSE PARENT_SCOPE)
  endif()
endfunction()

#-----------------------------------------------------------------------
# function geant4_dataset_property(<name> <property> <output variable>)
#          Set output variable to value of dataset "name"'s property.
#          If property does not exist, then value will be blank.
#
function(geant4_dataset_property _name _prop _output)
  geant4_has_dataset(${_name} _dsexists)
  if(NOT _dsexists)
    message(FATAL_ERROR "non-existent dataset ${_name}")
  endif()

  get_property(_tmp GLOBAL PROPERTY ${_name}_${_prop})
  set(${_output} ${_tmp} PARENT_SCOPE)
endfunction()

#-----------------------------------------------------------------------
# function geant4_dataset_isinstalled(<name>
#                                     <root directory>
#                                     <output variable>)
#          Check if named dataset is installed under the root directory.
#          Set output variable to TRUE if it is, FALSE otherwise.
#
function(geant4_dataset_isinstalled _name _rdirectory _output)
  geant4_dataset_property(${_name} DIRECTORY _dsdir)
  set(_expectedpath ${_rdirectory}/${_dsdir})

  if(IS_DIRECTORY ${_expectedpath})
    set(${_output} TRUE PARENT_SCOPE)
  else()
    set(${_output} FALSE PARENT_SCOPE)
  endif()
endfunction()

#-----------------------------------------------------------------------
# function geant4_install_dataset(<name> <destination> <timeout>)
#          Download dataset with name to build directory, timing out the
#          download after timeout seconds, and install it
#          into its directory under the destination directory.
#
function(geant4_install_dataset _name _destination _timeout)
  message(STATUS "Configuring download of missing dataset ${_name} (${_dsver})")
  # - Extract needed dataset properties
  geant4_dataset_property(${_name} DIRECTORY _ds_dir)
  geant4_dataset_property(${_name} VERSION _ds_version)
  geant4_dataset_property(${_name} URL _ds_url)
  geant4_dataset_property(${_name} MD5SUM _ds_md5sum)

  # - Dispatch to ExternalProject or our own implementation.
  # Use of URL_MD5 *and* TIMEOUT require CMake 2.8.2 or higher.
  if(${CMAKE_VERSION} VERSION_GREATER "2.8.1")
    include(ExternalProject)
    ExternalProject_Add(${_name}
      PREFIX Externals/${_name}-${_ds_version}
      SOURCE_DIR ${GEANT4_BUILD_FULL_DATADIR}/${_ds_dir}
      URL ${_ds_url}
      URL_MD5 ${_ds_md5sum}
      TIMEOUT ${_timeout}
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
      )
  else()
    _geant4_dataproject(${_name}
      PREFIX Externals/${_name}-${_ds_version}
      SOURCE_DIR ${GEANT4_BUILD_FULL_DATADIR}
      URL ${_ds_url}
      URL_MD5 ${_ds_md5sum}
      TIMEOUT ${_timeout}
      )
  endif()

  # - Add install target, and report back paths...
  install(DIRECTORY ${PROJECT_BINARY_DIR}/data/${_ds_dir}
    DESTINATION ${_destination}
    COMPONENT Data
    )
endfunction()

#-----------------------------------------------------------------------
# function geant4_reuse_dataset(<name> <destination> <is installed>)
#          Reuse the dataset with name located at destination directory.
#          If it is not installed, warn user that it will need installing
#          manually in destination.
#
function(geant4_reuse_dataset _name _destination _ispresent)
  geant4_dataset_property(${_name} VERSION _dsver)
  if(_ispresent)
    message(STATUS "Reusing dataset ${_name} (${_dsver})")
  else()
    message(STATUS "Configuring expected dataset ${_name} (${_dsver})")
  endif()
endfunction()

#-----------------------------------------------------------------------
# function geant4_configure_datasets(DESTINATION <dir>
#                                    DOWNLOAD    <installmissing>
#                                    TIMEOUT     <seconds>
#                                    )
#          Perform the actual heavy lifting to configure each declared
#          dataset for reuse or download as needed.
#
function(geant4_configure_datasets)
  # - Parse arguments and create variables
  set(oneValueArgs DESTINATION DOWNLOAD TIMEOUT)
  cmake_parse_arguments(_G4CFGDSS "" "${oneValueArgs}" "" ${ARGN})

  geant4_get_datasetnames(_dsnames)

  foreach(_ds ${_dsnames})
    geant4_dataset_isinstalled(${_ds} "${_G4CFGDSS_DESTINATION}" _installed)
    if(NOT _installed AND _G4CFGDSS_DOWNLOAD)
      geant4_install_dataset(${_ds} "${_G4CFGDSS_DESTINATION}" ${_G4CFGDSS_TIMEOUT}) 
    else()
      geant4_reuse_dataset(${_ds} "${_G4CFGDSS_DESTINATION}" ${_installed})
    endif()
  endforeach()
endfunction()


#-----------------------------------------------------------------------
# OLD STYLE FUNCTIONS - SOME MAY BE REUSED
#-----------------------------------------------------------------------
# function _geant4_dataproject(<name>
#                              PREFIX installdir
#                              SOURCE_DIR wheretounpack
#                              URL whattodownload
#                              URL_MD5 expectedMD5ofdownload
#                              TIMEOUT timeoutafter(seconds))
#          Download, unpack and install a dataset for CMake <= 2.8.1
#
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
# function geant4_do_install_data(<repourl>
#                                 <dataset_tuple>
#                                 <prefix>
#                                 <download_timeout>)
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
# DEFINE DATASETS - could put this into another file which we can include
#-----------------------------------------------------------------------
geant4_add_dataset(
  NAME      G4NDL
  VERSION   4.0
  FILENAME  G4NDL
  EXTENSION tar.gz
  ENVVAR    G4NEUTRONHPDATA
  MD5SUM    6d23aff98e5706af1dd7fff397969f37
  )

geant4_add_dataset(
  NAME      G4EMLOW
  VERSION   6.26
  FILENAME  G4EMLOW
  EXTENSION tar.gz
  ENVVAR    G4LEDATA
  MD5SUM    637f440a38b136f2e4df51f1cf0a02f9
  )

geant4_add_dataset(
  NAME      PhotonEvaporation
  VERSION   2.2
  FILENAME  G4PhotonEvaporation
  EXTENSION tar.gz
  ENVVAR    G4LEVELGAMMADATA
  MD5SUM    8010e7ce8a92564e38dd3418e6040563
  )

geant4_add_dataset(
  NAME      RadioactiveDecay
  VERSION   3.4
  FILENAME  G4RadioactiveDecay
  EXTENSION tar.gz
  ENVVAR    G4RADIOACTIVEDATA
  MD5SUM    a5b681048584631608ab0965b33c7959
  )

geant4_add_dataset(
  NAME      G4ABLA
  VERSION   3.0
  FILENAME  G4ABLA
  EXTENSION tar.gz
  ENVVAR    G4ABLADATA
  MD5SUM    d7049166ef74a592cb97df0ed4b757bd
  )

geant4_add_dataset(
  NAME      G4NEUTRONXS
  VERSION   1.1
  FILENAME  G4NEUTRONXS
  EXTENSION tar.gz
  ENVVAR    G4NEUTRONXSDATA
  MD5SUM    61ef3a05b56525db04e11820e3f603f1
  )

geant4_add_dataset(
  NAME      G4PII
  VERSION   1.3
  FILENAME  G4PII
  EXTENSION tar.gz
  ENVVAR    G4PIIDATA
  MD5SUM    05f2471dbcdf1a2b17cbff84e8e83b37
  )

geant4_add_dataset(
  NAME      RealSurface
  VERSION   1.0
  FILENAME  RealSurface
  EXTENSION tar.gz
  ENVVAR    G4REALSURFACEDATA
  MD5SUM    0dde95e00fcd3bcd745804f870bb6884
  )

#-----------------------------------------------------------------------
# GEANT4 PHYSICS DATA - USER INTERFACE AND PROCESSING
#-----------------------------------------------------------------------
# User options for installing data
# - Choose a directory under which to install the data.
# - Choose whether to download and install missing datasets.
# - Change download timeout for problematic connections
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

#-----------------------------------------------------------------------
# Select whether to download and install missing datasets
option(GEANT4_INSTALL_DATA "Download/Install datasets missing from GEANT4_INSTALL_DATADIR" OFF)

#-----------------------------------------------------------------------
# Provide an option for increasing the download timeout
# Helps with large datasets over slow connections.
set(GEANT4_INSTALL_DATA_TIMEOUT 1500 CACHE STRING "Timeout for Data Library downloads")
mark_as_advanced(GEANT4_INSTALL_DATA_TIMEOUT)

#-----------------------------------------------------------------------
# Set up check, download and install of needed data
# - New style, based on re-using existing datasets, if they exist
geant4_configure_datasets(
  DESTINATION ${GEANT4_INSTALL_FULL_DATADIR}
  DOWNLOAD    ${GEANT4_INSTALL_DATA}
  TIMEOUT     ${GEANT4_INSTALL_DATA_TIMEOUT}
  )

# - Old style...
#if(GEANT4_INSTALL_DATA)
#  foreach(_dataset ${GEANT4_DATASETS})
    #geant4_do_install_data(${GEANT4_DATASETS_URL} ${_dataset} ${GEANT4_INSTALL_FULL_DATADIR} ${GEANT4_INSTALL_DATA_TIMEOUT})
    #  endforeach()
    #endif()

#-----------------------------------------------------------------------
# C++ DATA API
#-----------------------------------------------------------------------
# Build the C++ data location/access API library and test programs
# TODO!
add_subdirectory(Source)

#-----------------------------------------------------------------------
# Final messaging of results (because we don't generate scripts for now)
#
message(STATUS "GEANT4_BUILD_FULL_DATADIR   : ${GEANT4_BUILD_FULL_DATADIR}")
message(STATUS "GEANT4_INSTALL_DATADIR      : ${GEANT4_INSTALL_DATADIR}")
message(STATUS "GEANT4_INSTALL_FULL_DATADIR : ${GEANT4_INSTALL_FULL_DATADIR}")

#-----------------------------------------------------------------------
# CONFIGURE SHELL/MAKE/CMAKE SCRIPTS
#-----------------------------------------------------------------------
# For scripts, we have up to 5 places where the build and install paths
# need to be added:
# 
# 1) geant4.(c)sh
# 2) geant4make.(c)sh
# 3) geant4make.gmk
# 4) geant4-config
# 5) Geant4Config.cmake
#
#-----------------------------------------------------------------------
# macro geant4_data_exports()
#       Very simple macro to create the set of variables to be exported
#       to the scripts via @VAR@ expansion.
#       TODO : option to select between build and install tree variables?
macro(geant4_data_create_exports)
  set(_g4dev_data_dir "${GEANT4_BUILD_FULL_DATADIR}")
  foreach(_dataset ${GEANT4_DATASETS})
    string(REPLACE "/" ";" _tuple ${_dataset})
    list(GET _tuple 0 _name)
    list(GET _tuple 1 _version)
    list(GET _tuple 4 _envvar)
    set(${_envvar}_DIR "${_g4dev_data_dir}/${_name}${_version}")
  endforeach()
endmacro()

if(GEANT4_INSTALL_DATA)
  # - Build tree only fo now...
  geant4_data_create_exports()

  configure_file(cmake/geant4data.sh.in 
    ${PROJECT_BINARY_DIR}/geant4data.sh 
    @ONLY
    )

  configure_file(cmake/geant4data.csh.in 
    ${PROJECT_BINARY_DIR}/geant4data.csh 
    @ONLY
    )

  configure_file(cmake/geant4data.gmk.in
    ${PROJECT_BINARY_DIR}/geant4data.gmk
    @ONLY
    )
endif()

# - And we should be done...

