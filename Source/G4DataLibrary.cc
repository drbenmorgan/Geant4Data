// G4DataLibrary - implementation
//
// Copyright (c) 2012 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2012 by The University of Warwick
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "G4DataLibrary.hh"

// Standard Library
#include <stdlib.h>

// Third Party
// - A

// This Project

namespace G4DataLibrary {
//! Internal namespace for implementation details.
//! May need things like directory existence/readability checks
namespace Detail {
//! return fully qualified root directory for data
G4String GetDataRootPathImpl() {
  char* dataDirFromEnv = getenv("G4DATADIR");
  return dataDirFromEnv ? G4String(dataDirFromEnv) : G4String();
}
} // namespace Detail

G4String GetDataRootPath() {
  return Detail::GetDataRootPathImpl();
}

G4String GetPathForDataset(const G4String& id) {
  // intent is that "id" replaces the role of the env vars, so
  // we might supply, e.g. id="NEUTRONHP".
  // Internally, we then map the id to the actual directory name
  // and version valid for this binary.
  // Developers then simply replace their own getenv calls with
  // path = G4DataLibrary::GetPathForDataset("NEUTRONHP");
  return G4String();
}
} // namespace G4DataLibrary

