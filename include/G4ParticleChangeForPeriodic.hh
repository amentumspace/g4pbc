#pragma once

#include "globals.hh"
#include "G4VParticleChange.hh"

class G4DynamicParticle;

class G4ParticleChangeForPeriodic : public G4VParticleChange {

public:

  G4ParticleChangeForPeriodic();
  virtual ~G4ParticleChangeForPeriodic();

  G4Step* UpdateStepForPostStep(G4Step* Step);

  void InitializeForPostStep(const G4Track&);

  void AddSecondary(G4DynamicParticle* aParticle);

  const G4ThreeVector& GetProposedMomentumDirection() const;
  void ProposeMomentumDirection(G4double Px, G4double Py, G4double Pz);
  void ProposeMomentumDirection(const G4ThreeVector& Pfinal);

  const G4ThreeVector& GetProposedPolarization() const;
  void ProposePolarization(const G4ThreeVector& dir);
  void ProposePolarization(G4double Px, G4double Py, G4double Pz);

  const G4ThreeVector& GetProposedPosition() const;
  void ProposePosition(const G4ThreeVector& pos);
  void ProposePosition(G4double x, G4double y, G4double z);

  const G4Track* GetCurrentTrack() const;

  virtual void DumpInfo() const;

  G4ParticleChangeForPeriodic(const G4ParticleChangeForPeriodic& right) = delete;
  G4ParticleChangeForPeriodic& operator=(const G4ParticleChangeForPeriodic& right) = delete;

private:

  const G4Track* currentTrack;
  G4ThreeVector proposedMomentumDirection;
  G4ThreeVector proposedPolarization;
  G4ThreeVector proposedPosition;

};

inline
 const G4ThreeVector& G4ParticleChangeForPeriodic::GetProposedMomentumDirection() const
{
  return proposedMomentumDirection;
}

inline
 void G4ParticleChangeForPeriodic::ProposeMomentumDirection(const G4ThreeVector& dir)
{
  proposedMomentumDirection = dir;
}

inline
 void G4ParticleChangeForPeriodic::ProposeMomentumDirection(G4double Px, G4double Py, G4double Pz)
{
  proposedMomentumDirection.setX(Px);
  proposedMomentumDirection.setY(Py);
  proposedMomentumDirection.setZ(Pz);
}


inline
 const G4ThreeVector& G4ParticleChangeForPeriodic::GetProposedPolarization() const
{
  return proposedPolarization;
}

inline
 void G4ParticleChangeForPeriodic::ProposePolarization(const G4ThreeVector& dir)
{
  proposedPolarization = dir;
}

inline
 void G4ParticleChangeForPeriodic::ProposePolarization(G4double Px, G4double Py, G4double Pz)
{
  proposedPolarization.setX(Px);
  proposedPolarization.setY(Py);
  proposedPolarization.setZ(Pz);
}


inline
 const G4ThreeVector& G4ParticleChangeForPeriodic::GetProposedPosition() const
{
  return proposedPosition;
}

inline
 void G4ParticleChangeForPeriodic::ProposePosition(const G4ThreeVector& dir)
{
  proposedPosition = dir;
}

inline
 void G4ParticleChangeForPeriodic::ProposePosition(G4double Px, G4double Py, G4double Pz)
{
  proposedPosition.setX(Px);
  proposedPosition.setY(Py);
  proposedPosition.setZ(Pz);
}


inline void G4ParticleChangeForPeriodic::InitializeForPostStep(const G4Track& track)
{
  theStatusChange = track.GetTrackStatus();
  theLocalEnergyDeposit = 0.0;
  theNonIonizingEnergyDeposit = 0.0;
  InitializeSecondaries();
  theParentWeight = track.GetWeight();
  isParentWeightProposed = false;
  proposedMomentumDirection = track.GetMomentumDirection();
  proposedPolarization = track.GetPolarization();
  proposedPosition = track.GetPosition();
  currentTrack = &track;
}


inline const G4Track* G4ParticleChangeForPeriodic::GetCurrentTrack() const
{
  return currentTrack;
}
