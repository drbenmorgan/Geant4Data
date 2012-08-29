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


Re-using Existing Data Installs
-------------------------------
The current technique for pointing to data libraries assumes either a
full download of everything or no data at all. We only have a single
switch for data installs (all libraries or none) for simplicity. 
To attempt re-use of existing installs of data, we can look for libraries
in the directory pointed to by the `GEANT4_INSTALL_FULL_DATADIR` variable,
but this may only contain some of the required libraries (we always
download/install all libraries for clarity/simplicity).
In this case, we need to download some libraries into the build tree,
as is done already, and point to any preexisting ones in the install tree.

This distinction is needed because we allow the build tree to be fully
functional (that is, you can build applications against it). In the
current system, this potential difference in data library paths cannot
be handled. To rectify this, we allow for a mixture of data paths
for the build tree and have two variables for each library: one which
points to its location for use by the build tree, the other to its
location for use by the install tree.

To implement this, and to help handling of datasets, we implement a small
API to declare datasets and extract information on them.
 
