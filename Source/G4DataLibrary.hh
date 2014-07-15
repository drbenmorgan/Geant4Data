// G4DataLibrary - API for accessing Geant4 data library paths
//
// Copyright (c) 2012 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2012 by The University of Warwick
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//! typedef to help future porting into Geant4
#include <string>
typedef std::string G4String;

//! namespace holding functions for data access
namespace G4DataLibrary {
//! return fully qualified path to root directory holding data libraries
G4String GetDataRootPath();

//! return fully qualified path to named dataset
G4String GetPathForDataset(const G4String& id);
}

