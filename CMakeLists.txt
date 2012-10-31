# - Main CMake script for "Geant4Data" project
# This is a prototype for improvements to the management and installation
# of datasets used by Geant4.
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
include(Geant4InstallData)

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
# Shell scripts are slightly awkward on Geant4 because we need to
# create a code block - each line of the block setting an environment
# variable
# - SH only...
geant4_get_datasetnames(_datasets)
set(G4DATA_EXPORTS )
foreach(_ds ${_datasets})
  geant4_get_dataset_property(${_ds} ENVVAR _envvar)
  geant4_get_dataset_property(${_ds} BUILD_DIR _buildloc)
  list(APPEND G4DATA_EXPORTS "export ${_envvar}=\"${_buildloc}\"")
endforeach()
string(REPLACE ";" "\n" G4DATA_EXPORTS "${G4DATA_EXPORTS}")

#-----------------------------------------------------------------------
# macro geant4_data_exports()
#       Very simple macro to create the set of variables to be exported
#       to the scripts via @VAR@ expansion.
#       TODO : option to select between build and install tree variables?
function(geant4_data_create_exports _loc)
  geant4_get_datasetnames(_datasets)
  foreach(_ds ${_datasets})
    message(STATUS "exporting ${_ds}")
    geant4_get_dataset_property(${_ds} ENVVAR _envvar)
    geant4_get_dataset_property(${_ds} ${_loc} _buildloc)
    set(${_envvar}_DIR ${_buildloc} PARENT_SCOPE)
  endforeach()
endfunction()

# - Build tree...
geant4_data_create_exports(BUILD_DIR)

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

# - Install tree...
geant4_data_create_exports(INSTALL_DIR)

configure_file(cmake/geant4data.sh.in 
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/geant4data.sh 
  @ONLY
  )

configure_file(cmake/geant4data.csh.in 
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/geant4data.csh 
  @ONLY
  )

configure_file(cmake/geant4data.gmk.in
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/geant4data.gmk
  @ONLY
  )


# - And we should be done...

