#include "DetectorConstruction.hh"
#include "SensitiveDetector.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PeriodicBoundaryBuilder.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"

#include <sstream>
using namespace std;

DetectorConstruction::DetectorConstruction(G4String runid, int test_mode) :
  G4VUserDetectorConstruction()
{

  logical_scorer = NULL;
  run_id = runid;
  mode = test_mode;
  world_xy = 2*mm;
  world_size_z = 10*mm;
}

DetectorConstruction::~DetectorConstruction()
{
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{

  G4Material* test_material = G4NistManager::Instance()->FindOrBuildMaterial("G4_SILICON_DIOXIDE");

  double factor = 1000.0;

  if( mode == 0 ) world_xy *= factor;

  G4Box* world = new G4Box("world", world_xy/2, world_xy/2, world_size_z/2);

  G4LogicalVolume* logical_world = new G4LogicalVolume(world, test_material, "logical_world");
  G4VisAttributes * world_vis_att = new G4VisAttributes(G4Color::Grey());
  world_vis_att->SetForceWireframe(true);
  logical_world->SetVisAttributes(world_vis_att);

  G4VPhysicalVolume* physical_world = new G4PVPlacement(0, G4ThreeVector(),
    logical_world, "physical_world", 0, false, 0);

  G4PeriodicBoundaryBuilder* pbb = new G4PeriodicBoundaryBuilder();
  G4LogicalVolume* logical_cyclic_world = pbb->Construct(logical_world);

  double scorer_thick = 1*micrometer;

  G4Box* scorer = new G4Box("scorer", world_xy/2.0, world_xy/2.0,
    scorer_thick/2.0);

  logical_scorer = new G4LogicalVolume(scorer, test_material, "logical_scorer");

  double z_pos = -world_size_z/2.0 + scorer_thick/2.0;

  new G4PVPlacement( 0, G4ThreeVector(0,0,z_pos), logical_scorer, "physical_scorer"
    , logical_cyclic_world, false, 0);

  logical_scorer->SetVisAttributes(G4Color::Red());

  return physical_world;
}

void DetectorConstruction::ConstructSDandField()
{
  G4SDManager* sd_manager = G4SDManager::GetSDMpointer();

  SensitiveDetector* sd = new SensitiveDetector("scorer_" + run_id);
  sd_manager->AddNewDetector(sd);

  logical_scorer->SetSensitiveDetector(sd);
}
