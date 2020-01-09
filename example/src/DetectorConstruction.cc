#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PeriodicBoundaryBuilder.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction()
{
}

DetectorConstruction::~DetectorConstruction()
{
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{

  G4Material* air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  double world_size_xy = 1.0 * cm;
  double world_size_z = 10.0 * cm;

  G4Box* world = new G4Box("world", world_size_xy/2, world_size_xy/2, world_size_z/2);

  G4LogicalVolume* logical_world = new G4LogicalVolume(world, air, "logical_world");
  G4VisAttributes * world_vis_att = new G4VisAttributes(G4Color::Grey());
  world_vis_att->SetForceWireframe(true);
  logical_world->SetVisAttributes(world_vis_att);

  G4VPhysicalVolume* physical_world = new G4PVPlacement(0, G4ThreeVector(),
    logical_world, "physical_world", 0, false, 0);

  G4PeriodicBoundaryBuilder* pbb = new G4PeriodicBoundaryBuilder();
  G4LogicalVolume* logical_periodic_world = pbb->Construct(logical_world);

  //continue defining the geometry with the logical Periodic world as mother volume

  // define a volume with dimensions smaller those of the Periodic world volume
  // checking that PBC only occure at periodic world boundaries.
  G4Box* daughter = new G4Box("daughter", world_size_xy/4, world_size_xy/4, world_size_z/2);
  G4LogicalVolume* logical_daughter = new G4LogicalVolume(daughter, air, "logical_daughter");

  new G4PVPlacement(0, G4ThreeVector(), logical_daughter, "physical_daughter",
    logical_periodic_world, false, 0);

  logical_daughter->SetVisAttributes(G4Color::Brown());

  return physical_world;
}

void DetectorConstruction::ConstructSDandField()
{
}
