#include "G4PeriodicBoundaryPhysics.hh"
#include "G4PeriodicBoundaryProcess.hh"

#include "G4PhysicsConstructorFactory.hh"
#include "G4ProcessManager.hh"

#include "globals.hh"

G4_DECLARE_PHYSCONSTR_FACTORY(G4PeriodicBoundaryPhysics);

G4PeriodicBoundaryPhysics::G4PeriodicBoundaryPhysics(const G4String& name,
  bool per_x, bool per_y, bool per_z, bool ref_walls)
  : G4VPhysicsConstructor(name)
{

  verboseLevel = 0;
  periodic_x = per_x; periodic_y = per_y; periodic_z = per_z;
  reflecting_walls = ref_walls;

}

G4PeriodicBoundaryPhysics::~G4PeriodicBoundaryPhysics(){
}

void G4PeriodicBoundaryPhysics::ConstructParticle()
{
  /*we construct the optical photon as the boundary process does not
  currently apply to it, and this requires a test in the physics process */
  G4OpticalPhoton::OpticalPhoton();

}

void G4PeriodicBoundaryPhysics::ConstructProcess(){

  if(verboseLevel > 0)
    G4cout << "Constructing cyclic boundary physics process" << G4endl;

  G4PeriodicBoundaryProcess* pbc = new G4PeriodicBoundaryProcess("Cyclic",
    fNotDefined, periodic_x, periodic_y, periodic_z, reflecting_walls);

  if(verboseLevel > 0) pbc->SetVerboseLevel(verboseLevel);

  auto aParticleIterator=GetParticleIterator();

  aParticleIterator->reset();

  G4ProcessManager * pManager = 0;

  while( (*aParticleIterator)() ){

    G4ParticleDefinition* particle = aParticleIterator->value();

    G4String particleName = particle->GetParticleName();

    pManager = particle->GetProcessManager();

    if (!pManager) {
       std::ostringstream o;
       o << "Particle " << particleName << "without a Process Manager";
       G4Exception("G4PeriodicBoundaryPhysics::ConstructProcess()", "",
        FatalException,o.str().c_str());
       return;
    }

    if(pbc->IsApplicable(*particle)){
      if(verboseLevel > 0)
        G4cout << "Adding pbc to " << particleName << G4endl;
      pManager->AddDiscreteProcess(pbc);          
      pManager->SetProcessOrderingToLast(pbc,idxPostStep);

    }
  }
}
