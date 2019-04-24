#include "G4PeriodicBoundaryProcess.hh"
#include "G4EventManager.hh"
#include "G4GeometryTolerance.hh"
#include "G4ios.hh"
#include "G4LogicalVolumePeriodic.hh"
#include "G4Navigator.hh"
#include "G4ParticleChangeForPeriodic.hh"
#include "G4TrackingManager.hh"
#include "G4VTrajectory.hh"
#include "G4ParallelWorldProcess.hh"

G4PeriodicBoundaryProcess::G4PeriodicBoundaryProcess(const G4String& processName,
  G4ProcessType type, bool per_x, bool per_y, bool per_z, bool ref_walls) :
  G4VDiscreteProcess(processName, type)
{

  reflecting_walls = ref_walls;

  theStatus = Undefined;

  periodic_x = per_x;
  periodic_y = per_y;
  periodic_z = per_z;

  //the tolerance prevents trapped particles at boundaries
  kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();

  pParticleChange = &fParticleChange;

}

G4PeriodicBoundaryProcess::~G4PeriodicBoundaryProcess(){}

G4VParticleChange*
G4PeriodicBoundaryProcess::PostStepDoIt(const G4Track& aTrack, const G4Step& aStep)
{

  if ( verboseLevel > 0 )
    G4cout << "G4PeriodicBoundaryPhysics::verboseLevel " << verboseLevel << G4endl;

  theStatus = Undefined;

  fParticleChange.InitializeForPostStep(aTrack);

  const G4Step* pStep = &aStep;

  G4bool isOnBoundary = (pStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary);

  if (!isOnBoundary) {
    theStatus = NotAtBoundary;
    if (verboseLevel > 0) BoundaryProcessVerbose();
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }

  G4VPhysicalVolume* thePrePV = pStep->GetPreStepPoint()->GetPhysicalVolume();
  G4VPhysicalVolume* thePostPV = pStep->GetPostStepPoint()->GetPhysicalVolume();

  if ( verboseLevel > 0 ) {
    G4cout << " Particle at Boundary! " << G4endl;
    if (thePrePV)  G4cout << " thePrePV:  " << thePrePV->GetName()  << G4endl;
    if (thePostPV) G4cout << " thePostPV: " << thePostPV->GetName() << G4endl;
    G4cout << "step length " << aTrack.GetStepLength() << G4endl;
  }

  //avoid trapped particles at boundaries by testing for minimum step length
	if (aTrack.GetStepLength() <= kCarTolerance/2){
    theStatus = StepTooSmall;
    if ( verboseLevel > 0) BoundaryProcessVerbose();
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
	}

  const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();

  // store the current values
	OldMomentum = aParticle->GetMomentumDirection();
	OldPolarization = aParticle->GetPolarization();
  OldPosition = pStep->GetPostStepPoint()->GetPosition();
  NewPosition = OldPosition;

  if ( verboseLevel > 0 ) {
     G4cout << " Old Momentum Direction: " << OldMomentum << G4endl;
     G4cout << " Old Position: " << NewPosition << G4endl;
  }

  G4ThreeVector theGlobalPoint = pStep->GetPostStepPoint()->GetPosition();

  // calculation of the global normal. code adapted from G4OpBoundaryProcess

  G4bool valid;
  //  Use the new method for Exit Normal in global coordinates,
  //    which provides the normal more reliably.
  theGlobalNormal = G4TransportationManager::GetTransportationManager()\
    ->GetNavigatorForTracking()->GetGlobalExitNormal(theGlobalPoint,&valid);

  if (valid) {
    theGlobalNormal = -theGlobalNormal;
  }
  else {
    G4cout << "global normal " << theGlobalNormal << G4endl;
    G4ExceptionDescription ed;
    ed << " G4PeriodicBoundaryProcess/PostStepDoIt(): "
      << " The Navigator reports that it returned an invalid normal" << G4endl;
    G4Exception("G4PeriodicBoundaryProcess::PostStepDoIt", "PerBoun01",
      EventMustBeAborted,ed,
      "Invalid Surface Normal - Geometry must return valid surface normal");
  }

  if (OldMomentum * theGlobalNormal > 0.0) {

    if ( verboseLevel > 0 ) {

      G4cout << "theGlobalNormal points in a wrong direction." << G4endl;
      G4cout << "Invalid Surface Normal - Geometry must return valid surface \
        normal pointing in the right direction" << G4endl;

    }

    theGlobalNormal = -theGlobalNormal;
  }

  /*when the post step point is in the world volume, the eldest daughter will
  be the periodic world volume, which has a skin associated. so we cycle or reflect*/

  G4LogicalVolume* lvol = thePostPV->GetLogicalVolume();

  if ( verboseLevel > 0 ) 
    G4cout << "Post step logical " << lvol->GetName() << G4endl;

  G4LogicalVolume* dlvol = NULL;

  if (lvol->GetNoDaughters() > 0) {

    if ( verboseLevel > 0 )
      G4cout << "eldest daughter " << lvol->GetDaughter(0)->GetName()<< G4endl;

    dlvol = lvol->GetDaughter(0)->GetLogicalVolume();

  }

  if (dlvol && dlvol->IsExtended()){

    if (verboseLevel > 0) G4cout << " Logical surface, periodic " << G4endl;

    bool on_x = theGlobalNormal.isParallel(G4ThreeVector(1,0,0));
    bool on_y = theGlobalNormal.isParallel(G4ThreeVector(0,1,0));
    bool on_z = theGlobalNormal.isParallel(G4ThreeVector(0,0,1));

    //make sure that we are at a plane
    bool on_plane = (on_x || on_y || on_z);

    if(!on_plane){
      G4ExceptionDescription ed;
      ed << " G4PeriodicBoundaryProcess/PostStepDoIt(): "
        << " The particle is not on a surface of the cyclic world" << G4endl;
      G4Exception("G4PeriodicBoundaryProcess::PostStepDoIt", "Periodic01",
        EventMustBeAborted,ed,
        "Periodic boundary process must only occur for particle on periodic world surface");
    } else {

      bool on_x_and_periodic = (on_x && periodic_x);
      bool on_y_and_periodic = (on_y && periodic_y);
      bool on_z_and_periodic = (on_z && periodic_z);

      bool on_a_periodic_plane = 
        (on_x_and_periodic || on_y_and_periodic || on_z_and_periodic);

      if (on_a_periodic_plane) {

        if (verboseLevel > 0) G4cout << " on periodic plane " << G4endl;

        if(reflecting_walls){ // we are periodic through specular reflection

          if (verboseLevel > 0) G4cout << " reflecting " << G4endl;

          G4double PdotN = OldMomentum * theGlobalNormal;
          NewMomentum = OldMomentum - (2.0 * PdotN) * theGlobalNormal;
          G4double EdotN = OldPolarization * theGlobalNormal;
          NewPolarization = -OldPolarization + (2.*EdotN)*theGlobalNormal;
          theStatus = Reflection;

          NewMomentum = NewMomentum.unit();//unit vector
          NewPolarization = NewPolarization.unit();

          if (verboseLevel > 0) {
            G4cout << " New Momentum Direction: " << NewMomentum << G4endl;
            G4cout << " New Polarization:       " << NewPolarization << G4endl;
            BoundaryProcessVerbose();
          }

          fParticleChange.ProposeMomentumDirection(NewMomentum);
          fParticleChange.ProposePolarization(NewPolarization);

        } else { // we are periodic through cyclic

          theStatus = Cycling;

          if ( verboseLevel > 0) G4cout << " periodic " << G4endl;

          if ( verboseLevel > 0) G4cout << "Global normal " << theGlobalNormal << G4endl;

          //translate a component of the position vector according to which plane we are on
          if (on_x_and_periodic) 
            NewPosition.setX(-NewPosition.x());
          else if (on_y_and_periodic) 
            NewPosition.setY(-NewPosition.y());
          else if (on_z_and_periodic) 
            NewPosition.setZ(-NewPosition.z());
          else
            G4cout << "global normal does not belong to periodic plane!!" << G4endl;

          NewMomentum = OldMomentum.unit();
          NewPolarization = OldPolarization.unit();

          if (verboseLevel > 0) {
            G4cout << " New Position: " << NewPosition << G4endl;
            G4cout << " New Momentum Direction: " << NewMomentum << G4endl;
            G4cout << " New Polarization:       " << NewPolarization << G4endl;
            BoundaryProcessVerbose();
          }

          fParticleChange.ProposeMomentumDirection(NewMomentum);
          fParticleChange.ProposePolarization(NewPolarization);
          fParticleChange.ProposePosition(NewPosition);

          //we must notify the navigator that we have moved the particle artificially
          G4Navigator* gNavigator =
            G4TransportationManager::GetTransportationManager()
            ->GetNavigatorForTracking();
          //Locates the volume containing the specified global point.

          gNavigator->SetGeometricallyLimitedStep() ;
          //gNavigator->LocateGlobalPointWithinVolume(NewPosition);
          gNavigator->LocateGlobalPointAndSetup( NewPosition,
                                                &NewMomentum,
                                               true,
                                               false) ;//do not ignore direction
          gNavigator->ComputeSafety(NewPosition);


          //force drawing of the step prior to periodic the particle
          G4EventManager* evtm = G4EventManager::GetEventManager();
          G4TrackingManager* tckm = evtm->GetTrackingManager();
          G4VTrajectory* fpTrajectory = NULL;
          fpTrajectory = tckm->GimmeTrajectory();
          if (fpTrajectory) fpTrajectory->AppendStep(pStep);

        }
      }
    }
  }

  return &fParticleChange;

}

//mean free path is infinite, will be final process before transporation
G4double G4PeriodicBoundaryProcess::GetMeanFreePath(const G4Track& ,
                                              G4double ,
                                              G4ForceCondition* condition)
{
        *condition = Forced;
        return DBL_MAX;
}

void G4PeriodicBoundaryProcess::BoundaryProcessVerbose() const
{
        if ( theStatus == Undefined )
                G4cout << " *** Undefined *** " << G4endl;
        if ( theStatus == NotAtBoundary )
                G4cout << " *** NotAtBoundary *** " << G4endl;
        if ( theStatus == Reflection )
                G4cout << " *** Reflection *** " << G4endl;
        if ( theStatus == Cycling )
                G4cout << " *** periodic *** " << G4endl;
        if ( theStatus == StepTooSmall )
                G4cout << " *** StepTooSmall *** " << G4endl;
}
