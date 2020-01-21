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

G4VPhysicalVolume *DetectorConstruction::Construct()
{

  G4Material *water = G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");

  // set optical properties of air for optical photon transport taken from OpNovice
  // example of the Geant4 toolkit

  //
  // Air
  //

  //
  // ------------ Generate & Add Material Properties Table ------------
  //
  G4double photonEnergy[] =
      {2.034 * eV, 2.068 * eV, 2.103 * eV, 2.139 * eV,
       2.177 * eV, 2.216 * eV, 2.256 * eV, 2.298 * eV,
       2.341 * eV, 2.386 * eV, 2.433 * eV, 2.481 * eV,
       2.532 * eV, 2.585 * eV, 2.640 * eV, 2.697 * eV,
       2.757 * eV, 2.820 * eV, 2.885 * eV, 2.954 * eV,
       3.026 * eV, 3.102 * eV, 3.181 * eV, 3.265 * eV,
       3.353 * eV, 3.446 * eV, 3.545 * eV, 3.649 * eV,
       3.760 * eV, 3.877 * eV, 4.002 * eV, 4.136 * eV};

  const G4int nEntries = sizeof(photonEnergy) / sizeof(G4double);

  //
  // Water
  //
  G4double refractiveIndex1[] =
      {1.3435, 1.344, 1.3445, 1.345, 1.3455,
       1.346, 1.3465, 1.347, 1.3475, 1.348,
       1.3485, 1.3492, 1.35, 1.3505, 1.351,
       1.3518, 1.3522, 1.3530, 1.3535, 1.354,
       1.3545, 1.355, 1.3555, 1.356, 1.3568,
       1.3572, 1.358, 1.3585, 1.359, 1.3595,
       1.36, 1.3608};

  assert(sizeof(refractiveIndex1) == sizeof(photonEnergy));

  G4double absorption[] =
      {3.448 * m, 4.082 * m, 6.329 * m, 9.174 * m, 12.346 * m, 13.889 * m,
       15.152 * m, 17.241 * m, 18.868 * m, 20.000 * m, 26.316 * m, 35.714 * m,
       45.455 * m, 47.619 * m, 52.632 * m, 52.632 * m, 55.556 * m, 52.632 * m,
       52.632 * m, 47.619 * m, 45.455 * m, 41.667 * m, 37.037 * m, 33.333 * m,
       30.000 * m, 28.500 * m, 27.000 * m, 24.500 * m, 22.000 * m, 19.500 * m,
       17.500 * m, 14.500 * m};

  assert(sizeof(absorption) == sizeof(photonEnergy));


  G4MaterialPropertiesTable* myMPT1 = new G4MaterialPropertiesTable();

  myMPT1->AddProperty("RINDEX",       photonEnergy, refractiveIndex1,nEntries)
        ->SetSpline(true);
  myMPT1->AddProperty("ABSLENGTH",    photonEnergy, absorption,     nEntries)
        ->SetSpline(true);

  G4cout << "Water G4MaterialPropertiesTable" << G4endl;
  myMPT1->DumpTable();

  water->SetMaterialPropertiesTable(myMPT1);

  G4Material *air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  G4double refractiveIndex2[] =
      {1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
       1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
       1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
       1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
       1.00, 1.00, 1.00, 1.00};

  G4MaterialPropertiesTable *myMPT2 = new G4MaterialPropertiesTable();
  myMPT2->AddProperty("RINDEX", photonEnergy, refractiveIndex2, nEntries);

  G4cout << "Air G4MaterialPropertiesTable" << G4endl;
  myMPT2->DumpTable();

  air->SetMaterialPropertiesTable(myMPT2);

  double world_size_xy = 1.0 * cm;
  double world_size_z = 10.0 * cm;

  G4Box *world = new G4Box("world", world_size_xy / 2, world_size_xy / 2, world_size_z / 2);

  G4LogicalVolume *logical_world = new G4LogicalVolume(world, air, "logical_world");
  G4VisAttributes *world_vis_att = new G4VisAttributes(G4Color::Grey());
  world_vis_att->SetForceWireframe(true);
  logical_world->SetVisAttributes(world_vis_att);

  G4VPhysicalVolume *physical_world = new G4PVPlacement(0, G4ThreeVector(),
                                                        logical_world, "physical_world", 0, false, 0);

  G4PeriodicBoundaryBuilder *pbb = new G4PeriodicBoundaryBuilder();
  G4LogicalVolume *logical_periodic_world = pbb->Construct(logical_world);

  //continue defining the geometry with the logical Periodic world as mother volume

  // define a volume with dimensions smaller those of the Periodic world volume
  // checking that PBC only occure at periodic world boundaries.
  G4Box *daughter = new G4Box("daughter", world_size_xy / 4, world_size_xy / 4, world_size_z / 2);
  G4LogicalVolume *logical_daughter = new G4LogicalVolume(daughter, water, "logical_daughter");

  new G4PVPlacement(0, G4ThreeVector(), logical_daughter, "physical_daughter",
                    logical_periodic_world, false, 0);

  logical_daughter->SetVisAttributes(G4Color::Brown());

  return physical_world;
}

void DetectorConstruction::ConstructSDandField()
{
}
