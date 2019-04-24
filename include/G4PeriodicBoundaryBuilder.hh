#pragma once

#include "globals.hh"
#include "G4LogicalVolumePeriodic.hh"
#include "G4SystemOfUnits.hh"

#include <map>
#include <string>
using namespace std;

class G4PeriodicBoundaryBuilder
{

public:
  G4PeriodicBoundaryBuilder();

  ~G4PeriodicBoundaryBuilder();

  G4LogicalVolume *Construct(G4LogicalVolume *);

private:
  G4LogicalVolumePeriodic *logical_periodic;
};
