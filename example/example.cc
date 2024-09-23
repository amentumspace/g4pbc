#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "Shielding.hh"

#include "G4PeriodicBoundaryPhysics.hh"
#include "G4RunManager.hh"

#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"

int main(int argc, char** argv)
{

  G4UIExecutive* ui = nullptr;
  if (argc == 1) {
    ui = new G4UIExecutive(argc, argv);
  }

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


  // Visualization
  G4VisExecutive* visManager = nullptr;
  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  if (nullptr == ui) {
    // Batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  }
  else {
    visManager = new G4VisExecutive;
    visManager->Initialize();
    UImanager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
    delete ui;
    delete visManager;
  }


  delete run_manager;

  return 0;

}
