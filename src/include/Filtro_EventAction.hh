/// \file Filtro_EventAction.hh
/// \brief Definition of the Filtro_EventAction class

#ifndef Filtro_EventAction_h
#define Filtro_EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

/// Event action class
///
/// It defines data members to hold the energy deposit and track lengths
/// of charged particles in Absober and Gap layers:
/// - fEnergyAbs, fEnergyGap, fTrackLAbs, fTrackLGap
/// which are collected step by step via the functions
/// - AddAbs(), AddGap()

class Filtro_EventAction : public G4UserEventAction
{
public:
    Filtro_EventAction();
    virtual ~Filtro_EventAction();
    
    virtual void  BeginOfEventAction(const G4Event* event);
    virtual void    EndOfEventAction(const G4Event* event);
    
    
public:
    G4int nOfReflections;
    G4int nOfDetections;
};


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

