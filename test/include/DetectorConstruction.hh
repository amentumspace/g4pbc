#pragma once

#include "G4SystemOfUnits.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4LogicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:

    DetectorConstruction(G4String runid="", int test_mode=0);
   ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    void ConstructSDandField();

    double GetWorldXY(){return world_xy;};
    double GetWorldZ(){return world_size_z;};

  private:
    G4LogicalVolume* logical_scorer;
    G4String run_id;
    G4int mode;
    double world_xy;
    double world_size_z;

};
