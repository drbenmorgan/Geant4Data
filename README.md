Geant4Data
==========
The [Geant4 Toolkit](http://geant4.cern.ch) makes use of many data
resources (known as "data libraries") in the implementation of some of its
physics models. Each resource is distributed as compressed tar files
which expand to a directory containing the data as a series of text files
organised into one or more subdirectories. Geant4 physics models needing 
access to these data do so via environment variables, one per data library,
which point to the top level directory of the resource.

Automated download, install and configuration of the eight libraries 
currently available was implemented in the CMake build system for Geant4
starting from version 9.5.0. Whilst this works and eases the task of 
managing and accessing the data resources, several issues have been
highlighted by users and developers:

* Data is installed under the share/ directory along with all other
(non-physics) data, leading to inefficiencies in disk usage and data sharing.
* Existing installs of data cannot be re-used, even though they may not
have been updated between Geant4 releases.
* All data resources are installed even if one or more are not required by 
the use case.
* The 1.5-2GB size of the full data resource leads to a very large binary 
package.

This small (unofficial!) project studies resolutions to these issues,
at both the CMake and C++ levels. 


