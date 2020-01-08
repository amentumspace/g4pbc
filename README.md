# Description

Periodic boundary conditions (PBC) are boundary conditions that approximate an
infinite volume using a small unit cell. Two dimensional PBCs can model infinite planar geometries;
the PBCs are applied to two cartesian coordinates and the third extends to infinity.
This document assumes that you are familiar with developing applications based on the Geant4 Monte Carlo toolkit under linux.

The G4PBC module adds a PBC capability to the [Geant4 Monte Carlo toolkit](http://geant4.web.cern.ch/geant4/).

The following occurs when G4PBC is used in a Geant4 simulation:

 - a periodic world volume is defined with dimensions matching those of the world volume
 - the world is resized to avoid sharing a surface with the periodic world
 - daughter volumes use the periodic world volume as their mother volume
 - the cartesian coordinates that are periodic are selected
 - the periodic boundary process is applied to all particles
 - when a particle steps onto the surface of the periodic world volume,
 and if the normal to the surface is in a direction that is periodic:
    - the position of the particle is translated to the opposite face of the periodic world

This package contains the G4PBC library, an example application to
demonstrate usage, and a test application to assure correct operation for
several particle types in a material of interest.

# Dependencies

  The Geant4 Monte Carlo toolkit and dependencies thereof

  The git version control system

# Obtaining source code

Clone this repository with the following command:

    git clone https://github.com/amentumspace/g4pbc.git

# Installation

Configure the run-time environment by sourcing the configuration script
of your geant4 installation, then perform an out-of-source build:

    source <path_to_geant4_install>/bin/geant4.sh
    mkdir build
    cd build/
    cmake ..
    make -j<number_processors>
    make install

This will install the package including libraries, header files, and cmake
configuration file to /usr/local

You may install the package in a different location with:

    cmake -DCMAKE_INSTALL_PREFIX=<path_to_g4pbc_install> ..

# Usage

## Register the process

The first step is to instantiate the G4PeriodicBoundaryPhysics list and to
register it with the run manager in your main program. You
must be using a physics list that inherits from the
G4VModularPhysicsList base class.

    #include "G4PeriodicBoundaryPhysics.hh"

    :

    PhysicsList* physics_list = new PhysicsList();
    G4PeriodicBoundaryPhysics* pbc = \
      new G4PeriodicBoundaryPhysics("PBC", true, true, false);
    pbc->SetVerboseLevel(0);

    physics_list->RegisterPhysics(pbc);

Arguments 1-3 of the constructor determine which cartesian axes are periodic. The default settings are periodic in x and y directions, and normal boundary conditions in the z direction; ie., an infinite planar geometry. 

## Construct the geometry

The second step is to define a periodic world volume in your detector construction
class. This has been abstracted into the G4PeriodicBoundaryBuilder class which
contains a single method, Construct, that creates a box with half width
equal to that of the world volume and of the same material.

The world volume is then resized to include a buffer to avoid sharing a surface
with the periodic world volume.

    #include "G4PeriodicBoundaryBuilder.hh"

    :

    G4PeriodicBoundaryBuilder* pbb = new G4PeriodicBoundaryBuilder();
    G4LogicalVolume* logical_periodic_world = pbb->Construct(logical_world);

The logical periodic world must be used as the mother logical for the rest
of the user defined geometry.

## Configure the CMakeLists file  

The third step is to modify the CMakeLists.txt file to link to the libraries.

    find_package(g4pbc REQUIRED NO_MODULE)
    ...
    include_directories(${g4pbc_INCLUDE_DIRS})
    ...
    target_link_libraries(example g4pbc::g4pbc)

If you have installed the package in a different location, specify the
path to the cmake config files with the g4pbc_DIR cmake flag (see the
next section).

# Example application

A simple example application is provided. The geometry is an air-filled box
with dimensions 1cm x 1cm x 10cm. The primary beam is defined in the macro file
run.mac - a point source of mono-energetic geantinos with isotropic angular
distribution.  

## Build

    source <path_to_geant4_install>/bin/geant4.sh
    cd example/
    mkdir build
    cd build
    cmake -Dg4pbc_DIR=<path_to_g4pbc_install>/lib/cmake/g4pbc/ ..
    make

NB. the path is to the directory containing the g4pbcConfig.cmake file.

## Run

Either interactively with visualisation

    $ ./example
    $> /control/execute run.mac

or in batch mode without visualisation

    $ ./example run.mac


[Cyclic Boundary Example](./example/g4cbp_example1.png)

Figure 1: Expected output of the application when running in
interactive mode with visualisation. A geantino is tracked through the geometry
accounting for cyclic boundary conditions.

# Test application

## Dependencies

  The Geant4 Monte Carlo toolkit

  G4PBC libraries

  libhdf5-dev - Hierarchical Data Format 5 (HDF5) - development files

  parallel - build and execute shell command lines from standard input in parallel

### Analysis

    Python

    The following Python modules:

    - scipy
    - tables

## Description

A second Geant4 application runs tests that assure accurate
modelling of an infinite planar geometry, accounting for physics processes relevant
to the particle of interest in the material of interest.

The simulation runs in a number of modes:

  - 0 - geometry with a semi-infinite lateral extent
  - 1 - geometry with a finite lateral extent with normal boundary conditions
  - 2 - same as 1, with cyclic boundary conditions 

## Build

The build process is the same as for the test application above.

## Usage

Interactive mode with visualisation and default settings:

    ./test

Batch mode:

    ./test <particle_type> <test_mode> <number_of_primaries> <jobid>

particle_type is a string that must match that used by Geant4; for example, 'e-' for the electron.

test_mode is an integer between 0 and 2 (see Description above). The default value is 0.

number_of_primaries is self-explanatory. The default value is 1.

jobid is an integer unique to the job that is used for file naming and to seed
the random number generator. The default value is 0.

### Geometry

The world volume is composed of silicon dioxide with z-dimension of 10 mm.
The lateral exent in X and Y directions is 2 m for mode 0, and 2 mm for
modes 1-3.

### Primary Beam

The primary beam, defined in the config.mac macro file, is a point source
located at (0,0,5 mm) with an isotropic angular distribution. It is mono-energetic
with energy of 1 MeV. The default particle is the geantino.

The type of particle and the number of primary particles are determined by
command line arguments at run-time.

### Physics

The Shielding physics list with range cuts of 0.1 mm.

### Scoring

The scorer is a box of height 1 micron with lateral extent matching that of the world volume.
It is positioned at the base of the world volume. Details such as particle type,
kinetic energy, position, and direction cosines, of particles stepping onto the
scorer are stored as a tuple to a binary file in HDF5 format.

## Analysis

Analyse the resulting HDF5 files with python:

    python ./analysis.py <particle_type> <number_of_primaries> <jobid>

Results from all jobs are collated, and histograms of kinetic energy
and z-direction cosines are created for a particular particle type and for all
simulation modes.

Histograms are saved to PNG format.

## Automation

Run the automated tests from the build directory:

    bash ../run_test.sh

The script defines a list of particle types, number of primaries
per simulation, and the number of jobs to run in parallel (assuming you are
running on a multi-core machine).

The parallel command launches the test app across available
cores, cycling through all particle types and modes. It then runs the
analysis.py script.

## Acknowledgements

This package was created by [Amentum Pty Ltd](http://www.amentum.com.au) under contract to the
[Australian Department of Defence](http://www.dst.defence.gov.au/).

This package includes code developed by Members of the
[Geant4 Collaboration](http://cern.ch/geant4)
([Geant4 License](http://geant4.web.cern.ch/geant4/license/LICENSE.html))

## License

Copyright (c) 2020 Amentum Pty Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
