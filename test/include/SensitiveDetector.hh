#pragma once

// GEANT4 //
#include "globals.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4VSensitiveDetector.hh"
#include "G4Event.hh"
#include "G4TouchableHistory.hh"

// HDF5 //
#include "H5Cpp.h"
#include "H5PacketTable.h"

// STL //
#include <vector>


struct Packet
{
    unsigned int event;
    int id;
    int parent_id;

    int particle_type;

    double kinetic_energy;

    double position_x;
    double position_y;
    double position_z;

    double direction_x;
    double direction_y;
    double direction_z;

};


class SensitiveDetector : public G4VSensitiveDetector
{
  public:
    SensitiveDetector( std::string name);
    ~SensitiveDetector();

  public:
    bool ProcessHits(G4Step*, G4TouchableHistory*);

  private:
    void SetupPacketTable();

  private:
    hid_t file_;
    FL_PacketTable* table_;

    std::string filename;
};
