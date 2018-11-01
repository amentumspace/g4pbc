#pragma once
#include "G4LogicalVolume.hh"

/*a derived class of logical volume that is used to indicate whether
periodic boundary conditions should be applied to this volume*/

class G4LogicalVolumePeriodic : public G4LogicalVolume {
    public:
        G4LogicalVolumePeriodic(G4VSolid* pSolid,
            G4Material* pMaterial,
            const G4String& name) : 
            G4LogicalVolume( pSolid, pMaterial, name){
                //
            };
        ~G4LogicalVolumePeriodic(){};    
        
        G4bool IsExtended() const { return true; }
        // Return true if it is not a base-class object.

        
};

