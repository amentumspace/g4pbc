#include "G4DynamicParticle.hh"
#include "G4ExceptionSeverity.hh"
#include "G4ParticleChangeForPeriodic.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"


G4ParticleChangeForPeriodic::G4ParticleChangeForPeriodic() : G4VParticleChange() {

}

G4ParticleChangeForPeriodic::~G4ParticleChangeForPeriodic(){}

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
