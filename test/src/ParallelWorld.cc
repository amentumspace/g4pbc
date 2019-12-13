#include "ParallelWorld.hh"

#include "SensitiveDetector.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"

ParallelWorld::ParallelWorld(G4String worldName, int test_mode, G4String runid, 
    G4double world_xy_, G4double world_z_) 
    : G4VUserParallelWorld(worldName){
    logical_scorer = nullptr;
    run_id = runid;

    world_xy = world_xy_;
    world_z = world_z_;

    mode = test_mode;
}

ParallelWorld::~ParallelWorld(){;}

void ParallelWorld::Construct(){

    G4VPhysicalVolume* ghostWorld = GetWorld();

    G4LogicalVolume* worldLogical = ghostWorld->GetLogicalVolume();

    double scorer_thick = 1*micrometer;

    double factor = 1000.0;

    if( mode == 0 ) world_xy *= factor;
    
    G4Box* scorer = new G4Box("scorer", 
        world_xy/2.0, world_xy/2.0,scorer_thick/2.0);

    logical_scorer = new G4LogicalVolume(scorer, nullptr, "logical_scorer");

    double z_pos = -world_z/2.0 + scorer_thick/2.0;

    new G4PVPlacement(
        0, G4ThreeVector(0,0,z_pos), logical_scorer, "physical_scorer"
        , worldLogical, false, 0);

}

void ParallelWorld::ConstructSD()
{
  G4SDManager* sd_manager = G4SDManager::GetSDMpointer();

  SensitiveDetector* sd = new SensitiveDetector("scorer_" + run_id);
  sd_manager->AddNewDetector(sd);

  logical_scorer->SetSensitiveDetector(sd);
}
