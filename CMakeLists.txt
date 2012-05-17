# - Top level build script of Geant4Data
#
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Geant4Data)

#----------------------------------------------------------------------------
# CMake options for installing data
# - Choose a directory under which to install the data.
# - Choose which libraries to install.
# - If a requested library already exists, use that.
#

#----------------------------------------------------------------------------
# Build the C++ data location/access API library and test programs
#
add_subdirectory(Source)


