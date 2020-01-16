#include "G4DynamicParticle.hh"
#include "G4ExceptionSeverity.hh"
#include "G4ParticleChangeForPeriodic.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"


G4ParticleChangeForPeriodic::G4ParticleChangeForPeriodic() : G4VParticleChange() {

}

G4ParticleChangeForPeriodic::~G4ParticleChangeForPeriodic(){}

G4ParticleChangeForPeriodic::G4ParticleChangeForPeriodic(
  const G4ParticleChangeForPeriodic &right): G4VParticleChange(right)
{
  if (verboseLevel>1)
    G4cout << "G4ParticleChangeForPeriodic::  copy constructor is called " << G4endl;

  currentTrack = right.currentTrack;
  proposedMomentumDirection = right.proposedMomentumDirection;
  proposedPolarization = right.proposedPolarization;
  proposedPosition = right.proposedPosition;
}


// assignment operator
G4ParticleChangeForPeriodic & G4ParticleChangeForPeriodic::operator=(
                           const G4ParticleChangeForPeriodic &right)
{
  if (verboseLevel>1)
    G4cout << "G4ParticleChangeForPeriodic:: assignment operator is called " << G4endl;

  if (this != &right) {
    if (theNumberOfSecondaries>0) {
      if (verboseLevel>0) {
        G4cout << "G4ParticleChangeForPeriodic: assignment operator Warning  ";
        G4cout << "theListOfSecondaries is not empty ";
      }
      for (G4int index= 0; index<theNumberOfSecondaries; index++){
        if ( (*theListOfSecondaries)[index] ) delete (*theListOfSecondaries)[index] ;
      }
    }
    delete theListOfSecondaries;
    theListOfSecondaries =  new G4TrackFastVector();
    theNumberOfSecondaries = right.theNumberOfSecondaries;
    for (G4int index = 0; index<theNumberOfSecondaries; index++){
    G4Track* newTrack =  new G4Track(*((*right.theListOfSecondaries)[index] ));
    theListOfSecondaries->SetElement(index, newTrack);                            }

    theStatusChange = right.theStatusChange;
    theLocalEnergyDeposit = right.theLocalEnergyDeposit;
    theSteppingControlFlag = right.theSteppingControlFlag;
    theParentWeight = right.theParentWeight;

    currentTrack = right.currentTrack;
    proposedMomentumDirection = right.proposedMomentumDirection;
    proposedPolarization = right.proposedPolarization;
    proposedPosition = right.proposedPosition;
  }
  return *this;
}


G4Step* G4ParticleChangeForPeriodic::UpdateStepForPostStep(G4Step* pStep)
{
  G4StepPoint* pPostStepPoint = pStep->GetPostStepPoint();

  pPostStepPoint->SetMomentumDirection( proposedMomentumDirection );
  pPostStepPoint->SetPolarization( proposedPolarization );
  pPostStepPoint->SetPosition( proposedPosition );

  if (isParentWeightProposed ){
    pPostStepPoint->SetWeight( theParentWeight );
  }

  pStep->AddTotalEnergyDeposit( theLocalEnergyDeposit );
  pStep->AddNonIonizingEnergyDeposit( theNonIonizingEnergyDeposit );

  // need to set the boundary status to ensure compatibility to G4OpBoundary process
  // as it comes last and will try to implement at periodic surface 
  pPostStepPoint->SetStepStatus(fUndefined);

  return pStep;
}

void G4ParticleChangeForPeriodic::AddSecondary(G4DynamicParticle* aParticle)
{
  G4Track* aTrack = new G4Track(aParticle, currentTrack->GetGlobalTime(),
    currentTrack->GetPosition());

  aTrack->SetTouchableHandle(currentTrack->GetTouchableHandle());

  G4VParticleChange::AddSecondary(aTrack);
}

void G4ParticleChangeForPeriodic::DumpInfo() const
{
  G4VParticleChange::DumpInfo();
  G4int oldprc = G4cout.precision(3);

  G4cout << "        Momentum Direction: "
       << std::setw(20) << proposedMomentumDirection
       << G4endl;
  G4cout << "        Polarization: "
       << std::setw(20) << proposedPolarization
       << G4endl;
  G4cout << "        Position: "
       << std::setw(20) << proposedPosition
       << G4endl;
  G4cout.precision(oldprc);
}
