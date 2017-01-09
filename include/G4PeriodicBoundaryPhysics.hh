#pragma once

#include "G4VPhysicsConstructor.hh"

class G4PeriodicBoundaryProcess;

class G4PeriodicBoundaryPhysics : public G4VPhysicsConstructor {

public:

  G4PeriodicBoundaryPhysics(const G4String& name = "Periodic", bool per_x = true,
    bool per_y = true, bool per_z = false, bool ref_walls = false);
  virtual ~G4PeriodicBoundaryPhysics();

protected:

  virtual void ConstructParticle();
  virtual void ConstructProcess();

private:
  bool reflecting_walls;
  bool periodic_x, periodic_y, periodic_z;

};
