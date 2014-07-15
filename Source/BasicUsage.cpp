// BasisUsage - just a smoke test of running.
//
// Copyright (c) 2012 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2012 by The University of Warwick
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>

#include "G4DataLibrary.hh"

int main() {
  std::cout << "[BasicUsage] G4DataLibrary::GetDataRootPath() = '"
            << G4DataLibrary::GetDataRootPath()
            << "'"
            << std::endl;

  std::cout << "[BasicUsage] G4DataLibrary::GetPathForDataset(\"NEUTRONHP\") = '"
            << G4DataLibrary::GetPathForDataset("NEUTRONHP")
            << "'"
            << std::endl;


  return 0;
}

