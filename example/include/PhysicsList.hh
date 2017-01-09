#pragma once

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

class PhysicsList: public G4VModularPhysicsList
{
  public:
    PhysicsList();
   ~PhysicsList();

  protected:
    virtual void ConstructParticle();
    virtual void ConstructProcess();
    virtual void SetCuts();
};
