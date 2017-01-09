#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction() : G4VUserPrimaryGeneratorAction()
{

  particle_gun = new G4GeneralParticleSource();

}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{

  delete particle_gun;

}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{

  particle_gun->GeneratePrimaryVertex(event);

}
