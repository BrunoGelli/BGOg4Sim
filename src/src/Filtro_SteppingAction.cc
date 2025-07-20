#include "Filtro_SteppingAction.hh"
#include "Filtro_Constantes.hh"
#include "Filtro_PrimaryGeneratorAction.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"

#include "G4Run.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"

#include "G4SystemOfUnits.hh"

/* Alteracao 5 5 17 */
#include "G4ios.hh"
#include "G4GeometryTolerance.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4PhysicalConstants.hh"
// */

// Arquivo txt de out put 
#include <fstream>
using namespace std;
//ofstream OutPut_Step(TXT_FILE);

// Conversao de radianos para graus
#define RAD_TO_DEG 180/pi
//================================================================================

Filtro_SteppingAction::Filtro_SteppingAction( Filtro_EventAction *eventAction, Filtro_RunAction* runAction, Filtro_PrimaryGeneratorAction* PrimaryGeneratorAction)
: G4UserSteppingAction (), fEventAction (eventAction), fRunAction (runAction), fPrimaryGeneratorAction (PrimaryGeneratorAction) {

    fEventNumber = -1;

}

Filtro_SteppingAction::~Filtro_SteppingAction() { ; }

//================================================================================

void Filtro_SteppingAction::UserSteppingAction (const G4Step* aStep) {
    
    // G4Track* track = aStep->GetTrack();
    // track->SetTrackStatus(fStopAndKill);

    
    // ======================================make======================================
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    

    //PreStep Info
    G4StepPoint * aPrePoint = aStep->GetPreStepPoint();
    G4VPhysicalVolume * aPrePV = aPrePoint->GetPhysicalVolume();
    G4String PreVolName = "";
    if (aPrePV) PreVolName = aPrePV->GetName();
    
    //PostStep Info
    G4StepPoint * aPostPoint = aStep->GetPostStepPoint();
    G4VPhysicalVolume * aPostPV = aPostPoint->GetPhysicalVolume();
    G4String PostVolName = "";
    if (aPostPV) PostVolName= aPostPV->GetName();

    // if (parti != "opticalphoton" && parti != "mu-")//PreVolName == MUNDO_NOME && PostVolName == "v2")//&& track -> GetTrackID()!=1 && creator) 
    // {
    //     track->SetTrackStatus(fStopAndKill);
    // }
  
    if (PostVolName == "PMTLV")
    {
        // Step
        G4Track* track = aStep->GetTrack();
        G4String parti = track->GetDefinition()->GetParticleName();

        if (parti == "opticalphoton")
        {

            fEventNumber = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

            // position information
            G4double step_x = aStep->GetTrack()->GetPosition().x();
            G4double step_y = aStep->GetTrack()->GetPosition().y();
            G4double step_z = aStep->GetTrack()->GetPosition().z();
            G4double eventT = track->GetGlobalTime();

            G4String procName = "unknown";

            if (track->GetCreatorProcess()) {
                procName = track->GetCreatorProcess()->GetProcessName();
            }

            int processID = -1; // Default value for undefined

            if (procName == "Scintillation") {
                processID = 0;
            }
            else if (procName == "Cerenkov") {
                processID = 1;
            }
            else {
                processID = 2;
            }

            const G4DynamicParticle* dynParticle = track -> GetDynamicParticle();
            G4double kinEnergy = dynParticle -> GetKineticEnergy(); 

            analysisManager->FillNtupleIColumn(1,0,fEventNumber);
            analysisManager->FillNtupleDColumn(1,1,step_x/cm);
            analysisManager->FillNtupleDColumn(1,2,step_y/cm);
            analysisManager->FillNtupleDColumn(1,3,step_z/cm);
            analysisManager->FillNtupleDColumn(1,4,eventT/ns);
            analysisManager->FillNtupleDColumn(1,5,kinEnergy/eV);
            analysisManager->FillNtupleIColumn(1,6,processID);
            analysisManager->AddNtupleRow(1);


            track->SetTrackStatus(fStopAndKill);
        }
    }

       
}

//================================================================================
