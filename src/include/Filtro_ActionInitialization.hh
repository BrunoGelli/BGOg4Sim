#ifndef Filtro_ActionInitialization_h
#define Filtro_ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class Filtro_DetectorConstruction;

/// Action initialization class.
class Filtro_ActionInitialization : public G4VUserActionInitialization {
    
  public:
    Filtro_ActionInitialization(Filtro_DetectorConstruction*);
    virtual ~Filtro_ActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;

  private:
    Filtro_DetectorConstruction* fDetConstruction;
    
};

#endif

    
