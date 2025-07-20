#ifndef Filtro_RunAction_h
#define Filtro_RunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "g4root.hh"

#include "G4NeutronHPManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4Timer;
class G4Run;

class Filtro_RunAction : public G4UserRunAction
{
  public:
    Filtro_RunAction();
   ~Filtro_RunAction();
  static G4NeutronHPManager *   GetInstance ();


  public:
    void BeginOfRunAction(const G4Run* aRun);
    void EndOfRunAction(const G4Run* aRun);
    G4int nOfReflections_Total;
    G4int nOfDetections_Total;
// Soma do tempo de voo de todos os fotons detectados
    G4double TOF_Detections_Total;

  private:
    G4Timer* timer;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*Filtro_RunAction_h*/
