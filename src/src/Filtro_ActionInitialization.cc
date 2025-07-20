#include "Filtro_ActionInitialization.hh"
#include "Filtro_DetectorConstruction.hh"
#include "Filtro_EventAction.hh"

#include "Filtro_PrimaryGeneratorAction.hh"
#include "Filtro_RunAction.hh"
#include "Filtro_SteppingAction.hh"

// ============================================================================

Filtro_ActionInitialization::Filtro_ActionInitialization
                                (Filtro_DetectorConstruction* detConstruction)
: G4VUserActionInitialization(), fDetConstruction(detConstruction)
{;}

// ============================================================================

Filtro_ActionInitialization::~Filtro_ActionInitialization() {;}

// ============================================================================

void Filtro_ActionInitialization::BuildForMaster() const {
    
    SetUserAction(new Filtro_RunAction);
    
}

// ============================================================================

void Filtro_ActionInitialization::Build() const {
    
    Filtro_EventAction* eventAction = new Filtro_EventAction;
    Filtro_RunAction* runAction = new Filtro_RunAction;
    Filtro_PrimaryGeneratorAction* PrimaryGeneratorAction = new Filtro_PrimaryGeneratorAction;
    
    SetUserAction(new Filtro_PrimaryGeneratorAction);
    SetUserAction(runAction);
    SetUserAction(eventAction);
    SetUserAction(new Filtro_SteppingAction(eventAction, runAction, PrimaryGeneratorAction));
    
}  

// ============================================================================
