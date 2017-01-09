#include "SensitiveDetector.hh"

#include "G4RunManager.hh"
#include "G4VProcess.hh"

#include "G4EventManager.hh"
#include "G4Event.hh"

SensitiveDetector::SensitiveDetector( std::string name)
                 : G4VSensitiveDetector(name)
{
    filename = name + ".hdf5";
    SetupPacketTable();
}

SensitiveDetector::~SensitiveDetector()
{
    delete table_;
    H5Fclose(file_);
}

bool SensitiveDetector::ProcessHits( G4Step* step
                                   , G4TouchableHistory*
                                   )
{


    G4Track* track = step->GetTrack();
    G4StepPoint* poststep_point = step->GetPostStepPoint();

    // Position and direction.
    G4ThreeVector world_position = poststep_point->GetPosition();
    G4ThreeVector direction = track->GetMomentumDirection();

    // Particle type
    int particle_type = track->GetDefinition()->GetPDGEncoding();

    double kinetic_energy = track->GetKineticEnergy();

    unsigned int evtid = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();

    Packet packet = { evtid
                    , track->GetTrackID()
                    , track->GetParentID()
                    , particle_type
                    , kinetic_energy
                    , world_position.x()
                    , world_position.y()
                    , world_position.z()
                    , direction.x()
                    , direction.y()
                    , direction.z()
                    };

    table_->AppendPacket(&packet);

    // kill the track once it crosses into the SD to avoid multiple hits
    step->GetTrack()->SetTrackStatus(fStopAndKill);

    return true;
}

void SensitiveDetector::SetupPacketTable()
{
    hid_t table_data_type = H5Tcreate(H5T_COMPOUND, sizeof(Packet));

    H5Tinsert(table_data_type, "event"
    , HOFFSET(Packet, event)
    , H5T_NATIVE_UINT);

    H5Tinsert(table_data_type, "id"
    , HOFFSET(Packet, id)
    , H5T_NATIVE_INT);

    H5Tinsert(table_data_type, "parent_id"
    , HOFFSET(Packet, parent_id)
    , H5T_NATIVE_INT);

    H5Tinsert(table_data_type, "particle_type"
    , HOFFSET(Packet, particle_type)
    , H5T_NATIVE_INT);

    H5Tinsert(table_data_type, "kinetic_energy"
    , HOFFSET(Packet, kinetic_energy)
    , H5T_NATIVE_DOUBLE);

    H5Tinsert(table_data_type, "position_x"
    , HOFFSET(Packet, position_x)
    , H5T_NATIVE_DOUBLE);

    H5Tinsert(table_data_type, "position_y"
    , HOFFSET(Packet, position_y)
    , H5T_NATIVE_DOUBLE);

    H5Tinsert(table_data_type, "position_z"
    , HOFFSET(Packet, position_z)
    , H5T_NATIVE_DOUBLE);

    H5Tinsert(table_data_type, "direction_x"
    , HOFFSET(Packet, direction_x)
    , H5T_NATIVE_DOUBLE);

    H5Tinsert(table_data_type, "direction_y"
    , HOFFSET(Packet, direction_y)
    , H5T_NATIVE_DOUBLE);

    H5Tinsert(table_data_type, "direction_z"
    , HOFFSET(Packet, direction_z)
    , H5T_NATIVE_DOUBLE);

    file_ = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    table_ = new FL_PacketTable(file_, (char*) "data", table_data_type, 1024, 16);

}
