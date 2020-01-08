#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "ParallelWorld.hh"
#include "Shielding.hh"

#include "G4ParallelWorldPhysics.hh"
#include "G4PeriodicBoundaryPhysics.hh"
#include "G4RunManager.hh"

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "G4UImanager.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

int main(int argc, char** argv)
{

  G4String particle_name = "geantino";
  if (argc >= 2 ) particle_name = argv[1];
  G4cout << "Primary particle " << particle_name << G4endl;

  G4int test_mode = 2;
  if (argc >= 3) test_mode = atoi(argv[2]);
  G4cout << "Running in test mode " << test_mode << G4endl;

  G4int number_of_primaries = 1;
  if (argc >= 4) number_of_primaries = atoi(argv[3]);
  G4cout << "Number of primary particles " << number_of_primaries << G4endl;

  G4int job_id = 0;
  if (argc >= 5) job_id = atoi(argv[4]);
  G4cout << "Job id / RNG seed " << job_id << G4endl;

  G4RunManager* run_manager = new G4RunManager();

  //seed random number generator according to cmd line argument
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  CLHEP::HepRandom::setTheSeed(job_id);

  //construct run id and pass to DC to name the HDF5 file
  G4String run_id = particle_name + "_" + std::to_string(test_mode) + "_" +
    std::to_string(job_id);

  G4double world_xy = 2*mm;
  G4double world_z = 10*mm;
  DetectorConstruction* dc = new DetectorConstruction(
    test_mode, world_xy, world_z);
  G4String parallelWorldName = "ParallelWorld";
  dc->RegisterParallelWorld(new ParallelWorld(
    parallelWorldName, test_mode, run_id, world_xy, world_z));

  run_manager->SetUserInitialization(dc);

  Shielding* physics_list = new Shielding();

  G4PeriodicBoundaryPhysics* PBC = new G4PeriodicBoundaryPhysics("Cyclic", true,
    true, false);
  PBC->SetVerboseLevel(0);

  if (test_mode == 2) physics_list->RegisterPhysics(PBC);

  physics_list->RegisterPhysics(new G4ParallelWorldPhysics(parallelWorldName));

  run_manager->SetUserInitialization(physics_list);

  run_manager->SetUserInitialization(new ActionInitialization());

  run_manager->Initialize();

  G4UImanager* ui_manager = G4UImanager::GetUIpointer();

  //the macro file will configure the default source and range cuts
  ui_manager->ApplyCommand("/control/execute config.mac");

  ui_manager->ApplyCommand("/gps/pos/centre 0. 0. " + std::to_string(world_z/2.0)
  + " mm");

  bool use_planar_source = false;
  if(use_planar_source){
    ui_manager->ApplyCommand("/gps/pos/halfx " + std::to_string(world_xy/2.0)
    + " mm");
    ui_manager->ApplyCommand("/gps/pos/halfy " + std::to_string(world_xy/2.0)
    + " mm");
  }

  ui_manager->ApplyCommand("/gps/particle "+particle_name);

  if (argc > 1) {
    ui_manager->ApplyCommand("/run/beamOn "+std::to_string(number_of_primaries));
  } else {

#ifdef G4VIS_USE
  G4VisManager* vis_manager = new G4VisExecutive;
  vis_manager->Initialize();
#endif

#ifdef G4UI_USE
    G4UIExecutive * ui_executive = new G4UIExecutive(argc,argv);
#ifdef G4VIS_USE
    ui_manager->ApplyCommand("/control/execute vis.mac");
#endif
    ui_executive->SessionStart();
    delete ui_executive;
#endif

#ifdef G4VIS_USE
  delete vis_manager;
#endif
  }

  delete run_manager;

  return 0;

}
