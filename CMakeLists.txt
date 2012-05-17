# - Top level build script of Geant4Data
#
#----------------------------------------------------------------------------
# Copyright 2012 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012 University of Warwick
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)
#----------------------------------------------------------------------------
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


