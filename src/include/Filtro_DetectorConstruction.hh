#ifndef Filtro_DetectorConstruction_h
#define Filtro_DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;

class Filtro_DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    Filtro_DetectorConstruction(G4double RIndex);
    virtual ~Filtro_DetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();

  private:
    // methods
    //
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();

    // World
    G4double world_x, world_y, world_z;
   
    // BGO Crystal
    G4double BGO_radius, BGO_height;

    // Teflon tape shell
    G4double Tape_thickness, Tape_gap;
    G4double Tape_IR, Tape_OR, Tape_z_translation;
    G4double Tape_pmt;

    // Teflon tape shell
    G4double Pmt_radius, Pmt_height, Pmt_to_BGO_z;


    // espessura
    G4double pinhole, OD, ID, length, baff_esp; 

    G4double clearance;

    G4bool  fCheckOverlaps; // option to activate checking of volumes overlaps

    G4double Refr_Index;
};

#endif /*Filtro_DetectorConstruction_h*/
