#include "Filtro_PrimaryGeneratorAction.hh"
#include "Filtro_Constantes.hh"

#include "Randomize.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "G4SystemOfUnits.hh"

// ============================================================================

Filtro_PrimaryGeneratorAction::Filtro_PrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction(), 
   particleGun(0)
{   
    // default
    flag_alpha=true;
    G4int n_particle = 1;
    particleGun = new G4ParticleGun(n_particle);
    // G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    // G4ParticleDefinition* particle = particleTable->FindParticle("alpha");   
}

// ============================================================================

Filtro_PrimaryGeneratorAction::~Filtro_PrimaryGeneratorAction() {
    
    delete particleGun;
    
}

// ============================================================================

void Filtro_PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) { 
    // Set particle energy
    particleGun->SetParticleEnergy(4.44*MeV);

    // Set particle type to gamma
    G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
    particleGun->SetParticleDefinition(particle);

    // --- Generate isotropic direction ---
    G4double theta = std::acos(1 - 2 * G4UniformRand());  // theta in [0, pi]
    G4double phi   = 2 * CLHEP::pi * G4UniformRand();     // phi in [0, 2pi]
    G4ThreeVector dir(std::sin(theta) * std::cos(phi),
                      std::sin(theta) * std::sin(phi),
                      std::cos(theta));
    particleGun->SetParticleMomentumDirection(dir);

    // --- Generate random point in a cylinder ---
    G4double radius = 3.9 * mm;
    G4double height = 10.0 * mm;

    // Uniform point in disk (use sqrt for proper radial distribution)
    G4double r = radius * std::sqrt(G4UniformRand());
    G4double angle = 2 * CLHEP::pi * G4UniformRand();
    G4double x = r * std::cos(angle);
    G4double y = r * std::sin(angle);

    // Uniform height along z-axis of cylinder
    G4double z = (G4UniformRand() - 0.5) * height;

    // Apply translation (you previously had y = 2 mm offset)
    particleGun->SetParticlePosition(G4ThreeVector(x, y + 2.0 * mm, z - 4*cm));

    // Generate the primary
    particleGun->GeneratePrimaryVertex(anEvent);
}

// ============================================================================

void Filtro_PrimaryGeneratorAction::SetOptPhotonPolar() {
    
    G4double angle = G4UniformRand() * 360.0*deg;
    SetOptPhotonPolar(angle);
    
}

// ============================================================================

void Filtro_PrimaryGeneratorAction::SetOptPhotonPolar(G4double angle) {
    
    if (particleGun->GetParticleDefinition()->GetParticleName() != "opticalphoton") {
        G4cout << "--> warning from PrimaryGeneratorAction::SetOptPhotonPolar() :"
                "the particleGun is not an opticalphoton" << G4endl;
        return;
    }

    G4ThreeVector normal (1., 0., 0.);
    G4ThreeVector kphoton = particleGun->GetParticleMomentumDirection();
    G4ThreeVector product = normal.cross(kphoton);
    G4double modul2       = product*product;

    G4ThreeVector e_perpend (0., 0., 1.);
    if (modul2 > 0.) e_perpend = (1./std::sqrt(modul2))*product;
    G4ThreeVector e_paralle    = e_perpend.cross(kphoton);

    G4ThreeVector polar = std::cos(angle)*e_paralle + std::sin(angle)*e_perpend;
    particleGun->SetParticlePolarization(polar);
    
}

// ============================================================================
