#pragma once 

#include "globals.hh"
#include "G4VUserParallelWorld.hh"

class G4LogicalVolume;

class ParallelWorld : public G4VUserParallelWorld {

    public: 
        ParallelWorld(G4String worldName, int test_mode, G4String runid, 
            G4double world_xy_, G4double world_z_);
        virtual ~ParallelWorld();

        G4LogicalVolume * logical_scorer;

    public: 
        virtual void Construct();
        virtual void ConstructSD();
        G4String run_id;
        G4double world_xy;
        G4double world_z;
        G4int mode;
};

