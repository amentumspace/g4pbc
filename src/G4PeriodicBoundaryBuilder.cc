#include "G4PeriodicBoundaryBuilder.hh"
#include "G4LogicalVolumePeriodic.hh"

#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"

G4PeriodicBoundaryBuilder::G4PeriodicBoundaryBuilder()
{
  logical_periodic = NULL;
}

G4PeriodicBoundaryBuilder::~G4PeriodicBoundaryBuilder(){
}

G4LogicalVolume* G4PeriodicBoundaryBuilder::Construct(G4LogicalVolume* logical_world){

  G4Box* world = (G4Box*)logical_world->GetSolid();

  double buffer = 1 * micrometer;

  double periodic_world_hx = world->GetXHalfLength();
  double periodic_world_hy = world->GetYHalfLength();
  double periodic_world_hz = world->GetZHalfLength();

  /*reset the size of the world volume to be slightly larger to accommodate the
  the cyclic world volume without sharing a surface*/
  world->SetXHalfLength(world->GetXHalfLength() + buffer);
  world->SetYHalfLength(world->GetYHalfLength() + buffer);
  world->SetZHalfLength(world->GetZHalfLength() + buffer);

  G4Box* periodic_world = new G4Box("cyclic", periodic_world_hx, periodic_world_hy,
    periodic_world_hz);

  logical_periodic = new G4LogicalVolumePeriodic(periodic_world,
    logical_world->GetMaterial(), "logical_periodic");

  logical_periodic->SetVisAttributes(G4Color::Magenta());

  new G4PVPlacement(0, G4ThreeVector(), logical_periodic, "physical_cyclic",
    logical_world, false, 0, true);//check for overlaps

  return logical_periodic;

}
