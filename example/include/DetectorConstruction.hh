#pragma once

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:

    DetectorConstruction();
   ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    void ConstructSDandField();
  
};
