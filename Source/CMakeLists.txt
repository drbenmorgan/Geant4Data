# - Build the Geant4Data library and test programs
#
# Copyright 2012 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012 University of Warwick
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)
#----------------------------------------------------------------------------

add_library(G4Data SHARED G4DataLibrary.hh G4DataLibrary.cc)
add_executable(BasicUsage BasicUsage.cpp)
target_link_libraries(BasicUsage G4Data)

