#ifndef Filtro_SteppingAction_h
#define Filtro_SteppingAction_h 1

#include "Filtro_RunAction.hh"
#include "Filtro_EventAction.hh"
#include "Filtro_PrimaryGeneratorAction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4SteppingManager.hh"
#include "G4UserSteppingAction.hh"
#include "G4SystemOfUnits.hh"

class Filtro_EventAction;
class Filtro_RunAction;
class Filtro_PrimaryGeneratorAction;

//Stepping action class
class Filtro_SteppingAction : public G4UserSteppingAction {

   public:
       Filtro_SteppingAction(Filtro_EventAction *eventAction, Filtro_RunAction* runAction, Filtro_PrimaryGeneratorAction* PrimaryGeneratorAction);
	virtual ~Filtro_SteppingAction ();

	virtual void UserSteppingAction (const G4Step*);

   private:
	G4int fEventNumber;
    Filtro_EventAction* fEventAction;
    Filtro_RunAction* fRunAction;
    Filtro_PrimaryGeneratorAction* fPrimaryGeneratorAction;
};
#endif
