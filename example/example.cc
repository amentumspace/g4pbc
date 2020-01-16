#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "Shielding.hh"

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

  G4RunManager* run_manager = new G4RunManager();

  run_manager->SetUserInitialization(new DetectorConstruction());

  //PhysicsList* physics_list = new PhysicsList();//an empty modular physics list for geantinos only

  Shielding* physics_list = new Shielding();

  //arguments: physics list name, cycle x, y, z, reflecting walls
  G4PeriodicBoundaryPhysics* pbc = new G4PeriodicBoundaryPhysics("Periodic", true,
    true, false, false);
  pbc->SetVerboseLevel(0);

  physics_list->RegisterPhysics(pbc);

  run_manager->SetUserInitialization(physics_list);

  run_manager->SetUserInitialization(new ActionInitialization());

  run_manager->Initialize();

  G4UImanager* ui_manager = G4UImanager::GetUIpointer();

  if (argc!=1) {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    ui_manager->ApplyCommand(command+fileName);
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
