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

DetectorConstruction::DetectorConstruction(int test_mode, 
  G4double world_xy_, G4double world_z_) :
  G4VUserDetectorConstruction()
{

  logical_scorer = NULL;
  mode = test_mode;
  world_xy = world_xy_;
  world_size_z = world_z_;
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

  G4LogicalVolume* logical_world = new G4LogicalVolume(
    world, test_material, "logical_world");
  G4VisAttributes * world_vis_att = new G4VisAttributes(G4Color::Grey());
  world_vis_att->SetForceWireframe(true);
  logical_world->SetVisAttributes(world_vis_att);

  G4VPhysicalVolume* physical_world = new G4PVPlacement(0, G4ThreeVector(),
    logical_world, "physical_world", 0, false, 0);

  G4PeriodicBoundaryBuilder* pbb = new G4PeriodicBoundaryBuilder();
  G4LogicalVolume* logical_cyclic_world = pbb->Construct(logical_world);

  return physical_world;
}

void DetectorConstruction::ConstructSDandField(){;}