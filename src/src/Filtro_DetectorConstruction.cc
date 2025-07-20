    #include "Filtro_DetectorConstruction.hh"
    #include "Filtro_Constantes.hh"

    #include "G4Material.hh"
    #include "G4Element.hh"
    #include "G4NistManager.hh"

    #include "G4Box.hh"
    #include "G4Orb.hh"
    #include "G4Tubs.hh"
    #include "G4Sphere.hh"
    #include "G4Trd.hh"

    #include "G4LogicalVolume.hh"
    #include "G4ThreeVector.hh"
    #include "G4PVPlacement.hh"
    #include "G4AutoDelete.hh"
    #include "G4LogicalBorderSurface.hh"
    #include "G4LogicalSkinSurface.hh"
    #include "G4OpticalSurface.hh"

    #include "G4SubtractionSolid.hh"
    #include "G4VSolid.hh"
    #include "G4UnionSolid.hh"

    #include "G4VisAttributes.hh"
    #include "G4Colour.hh"

    #include "G4PhysicalConstants.hh"
    #include "G4SystemOfUnits.hh"
    #include "globals.hh"

    #include "G4Navigator.hh"
    #include "G4VPhysicalVolume.hh"
    #include "G4TransportationManager.hh"

    #include <fstream>
    using namespace std;

    #include "G4NeutronHPManager.hh"
    #include <G4HadronicProcessStore.hh>
    
    //================================================================================

    Filtro_DetectorConstruction::Filtro_DetectorConstruction (G4double RIndex)
    : G4VUserDetectorConstruction(), fCheckOverlaps(true), Refr_Index(RIndex) {

        G4NeutronHPManager::GetInstance()->SetVerboseLevel(0);
        G4HadronicProcessStore::Instance()->SetVerbose(0);
        
        // World
        world_x = world_y = world_z = 50 * cm * 0.5;
        
        // crystal 
        BGO_radius = 5 * cm * 0.5;
        BGO_height = 5 * cm * 0.5;

        // tape
        Tape_thickness  = 0.01 *cm;
        Tape_gap        = 0.01 *cm;
        Tape_pmt        = 2.45 * cm * 0.5;

        Tape_IR = BGO_radius + Tape_gap;
        Tape_OR = BGO_radius + Tape_gap + Tape_thickness/2;

        // pmt
        Pmt_radius = 2.45 * cm * 0.5;
        Pmt_height = 10.0 * cm * 0.5;

        //distances
        Pmt_to_BGO_z = Pmt_height + BGO_height +  RIndex * mm;
        Tape_z_translation = BGO_height + Tape_gap;

    }

    //================================================================================

    Filtro_DetectorConstruction::~Filtro_DetectorConstruction (){;}

    //================================================================================

    G4VPhysicalVolume* Filtro_DetectorConstruction::Construct () {

        // Define materials 
        DefineMaterials();
        
        // Define volumes
        return DefineVolumes();
        
    }

    //================================================================================

    void Filtro_DetectorConstruction::DefineMaterials() {
        
        G4NistManager* nistManager = G4NistManager::Instance();

        nistManager->FindOrBuildMaterial("G4_BGO");
        nistManager->FindOrBuildMaterial("G4_Galactic");
        nistManager->FindOrBuildMaterial("G4_TEFLON");
        
        G4cout << *(G4Material::GetMaterialTable()) << G4endl;
        
    }

    //================================================================================

    G4VPhysicalVolume* Filtro_DetectorConstruction::DefineVolumes() {
        // Get materials
       
        G4Material* Vacmat       = G4Material::GetMaterial("G4_Galactic");
        G4Material* BGOmat      = G4Material::GetMaterial("G4_BGO");
        G4Material* PTFEmat     = G4Material::GetMaterial("G4_TEFLON");

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Construction %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    //====================== World ===========-===========

        G4Box* WorldS = new G4Box (MUNDO_NOME, world_x, world_y, world_z);
    	
        G4LogicalVolume* WorldLV =
        	new G4LogicalVolume (WorldS, Vacmat, MUNDO_NOME);
    	
        G4VPhysicalVolume* WorldPV =
        	new G4PVPlacement (0, G4ThreeVector (), WorldLV, MUNDO_NOME, 0, true, 0, fCheckOverlaps);

    //======================= BGO Crystal =======================

  
        G4Tubs* DetecS = new G4Tubs ("BGOcrystal", 0.1 *cm, BGO_radius, BGO_height, 0.0, 2*M_PI*rad);
        
        G4LogicalVolume* DetecLV =
            new G4LogicalVolume (DetecS, BGOmat, "BGOcrystal");
        
        G4VPhysicalVolume* DetecPV =
            new G4PVPlacement (0, G4ThreeVector (), DetecLV, "BGOcrystal", WorldLV, true, 0, fCheckOverlaps);

    //======================= Teflon Tape shell =======================


        // 1. Thin cylindrical shell
        G4Tubs* shell       = new G4Tubs("TapeCyl",     Tape_IR,    Tape_OR,    Tape_z_translation,        0., 360. * deg);
        G4Tubs* topDisk     = new G4Tubs("TapeTop",     Tape_pmt,   Tape_OR,    Tape_thickness,    0., 360. * deg);
        G4Tubs* bottomDisk  = new G4Tubs("TapeBottom",  0,          Tape_OR,    Tape_thickness,    0., 360. * deg);

        auto shellWithBottom = new G4UnionSolid("ShellWithBottom", shell, bottomDisk, nullptr,
                G4ThreeVector(0, 0, -Tape_z_translation - Tape_thickness));

        auto TapeShellS = new G4UnionSolid("FullGeometry", shellWithBottom, topDisk, nullptr,
                G4ThreeVector(0, 0, Tape_z_translation + Tape_thickness));


        G4LogicalVolume* Tape_CylLV =
            new G4LogicalVolume (TapeShellS, PTFEmat, "TapeLV");
        
        G4VPhysicalVolume* Tape_CylPV =
            new G4PVPlacement (0, G4ThreeVector (), Tape_CylLV, "TapeLV", WorldLV, true, 0, fCheckOverlaps);
    

    //======================= PMT =======================

        // 1. Thin cylindrical shell
        G4Tubs* pmtS = new G4Tubs("pmt", 0.0, Pmt_radius, Pmt_height, 0., 360. * deg);

        G4LogicalVolume* pmtLV = new G4LogicalVolume (pmtS, PTFEmat, "PMTLV");
        
        G4VPhysicalVolume* pmtPV =
            new G4PVPlacement (0, G4ThreeVector (0, 0, Pmt_to_BGO_z), pmtLV, "PMTLV", WorldLV, true, 0, fCheckOverlaps);
    
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Optical properties %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      
        const G4int NUMENTRIES = 106;
        G4double Tef_RI[NUMENTRIES];
        G4double Vac_RI[NUMENTRIES];
        G4double Vac_ABS[NUMENTRIES];
        G4double Tef_ABS[NUMENTRIES];
        G4double Tef_R[NUMENTRIES];

        G4double WvLngArray[NUMENTRIES] = {
        1.771*eV, 1.784*eV, 1.797*eV, 1.810*eV, 1.823*eV, 1.837*eV, 1.850*eV, 1.864*eV, 1.878*eV, 1.893*eV,
        1.907*eV, 1.922*eV, 1.937*eV, 1.952*eV, 1.968*eV, 1.984*eV, 2.000*eV, 2.016*eV, 2.032*eV, 2.049*eV,
        2.066*eV, 2.084*eV, 2.101*eV, 2.119*eV, 2.138*eV, 2.156*eV, 2.175*eV, 2.194*eV, 2.214*eV, 2.234*eV,
        2.254*eV, 2.275*eV, 2.296*eV, 2.317*eV, 2.339*eV, 2.362*eV, 2.384*eV, 2.407*eV, 2.431*eV, 2.455*eV,
        2.480*eV, 2.505*eV, 2.530*eV, 2.556*eV, 2.583*eV, 2.610*eV, 2.638*eV, 2.666*eV, 2.695*eV, 2.725*eV,
        2.755*eV, 2.786*eV, 2.818*eV, 2.850*eV, 2.883*eV, 2.917*eV, 2.952*eV, 2.987*eV, 3.024*eV, 3.061*eV,
        3.100*eV, 3.139*eV, 3.179*eV, 3.220*eV, 3.263*eV, 3.306*eV, 3.351*eV, 3.397*eV, 3.444*eV, 3.492*eV,
        3.542*eV, 3.594*eV, 3.646*eV, 3.701*eV, 3.757*eV, 3.815*eV, 3.874*eV, 3.936*eV, 3.999*eV, 4.065*eV,
        4.133*eV, 4.203*eV, 4.275*eV, 4.350*eV, 4.428*eV, 4.508*eV, 4.592*eV, 4.678*eV, 4.768*eV, 4.862*eV,
        4.959*eV, 5.060*eV, 5.166*eV, 5.276*eV, 5.390*eV, 5.510*eV, 5.635*eV, 5.767*eV, 5.904*eV, 6.048*eV,
        6.199*eV, 6.358*eV, 6.525*eV, 6.702*eV, 6.888*eV, 7.085*eV};

        G4double Scint_FAST[NUMENTRIES] = {
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0826, 0.0943, 0.1107, 0.1316, 0.1504, 0.1712, 0.1901, 0.2109, 0.2307,
        0.2510, 0.2785, 0.3060, 0.3399, 0.3800, 0.4184, 0.4487, 0.4763, 0.5089, 0.5449,
        0.5810, 0.6244, 0.6682, 0.7102, 0.7504, 0.7905, 0.8301, 0.8709, 0.8979, 0.9094,
        0.9220, 0.9492, 0.9769, 0.9913, 0.9913, 0.9911, 0.9762, 0.9602, 0.9362, 0.9036,
        0.8704, 0.8142, 0.7525, 0.6948, 0.6422, 0.5885, 0.5065, 0.4223, 0.3542, 0.3019,
        0.2496, 0.1899, 0.1304, 0.0796, 0.0400, 0.0037, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000};

        G4double Scint_SLOW[NUMENTRIES] = {
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0826, 0.0943, 0.1107, 0.1316, 0.1504, 0.1712, 0.1901, 0.2109, 0.2307,
        0.2510, 0.2785, 0.3060, 0.3399, 0.3800, 0.4184, 0.4487, 0.4763, 0.5089, 0.5449,
        0.5810, 0.6244, 0.6682, 0.7102, 0.7504, 0.7905, 0.8301, 0.8709, 0.8979, 0.9094,
        0.9220, 0.9492, 0.9769, 0.9913, 0.9913, 0.9911, 0.9762, 0.9602, 0.9362, 0.9036,
        0.8704, 0.8142, 0.7525, 0.6948, 0.6422, 0.5885, 0.5065, 0.4223, 0.3542, 0.3019,
        0.2496, 0.1899, 0.1304, 0.0796, 0.0400, 0.0037, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
        0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000};

        G4double BGO_RI[NUMENTRIES] = {
        2.0843, 2.0849, 2.0858, 2.0871, 2.0877, 2.0885, 2.0900, 2.0906, 2.0919, 2.0928,
        2.0938, 2.0949, 2.0956, 2.0969, 2.0984, 2.0990, 2.1003, 2.1018, 2.1032, 2.1045,
        2.1060, 2.1072, 2.1089, 2.1102, 2.1117, 2.1130, 2.1149, 2.1166, 2.1183, 2.1201,
        2.1218, 2.1235, 2.1257, 2.1276, 2.1296, 2.1320, 2.1339, 2.1362, 2.1385, 2.1410,
        2.1435, 2.1460, 2.1488, 2.1518, 2.1543, 2.1576, 2.1608, 2.1640, 2.1672, 2.1709,
        2.1744, 2.1783, 2.1823, 2.1865, 2.1912, 2.1957, 2.2001, 2.2051, 2.2103, 2.2156,
        2.2213, 2.2272, 2.2335, 2.2407, 2.2480, 2.2550, 2.2626, 2.2708, 2.2795, 2.2889,
        2.2984, 2.3089, 2.3196, 2.3311, 2.3437, 2.3570, 2.3715, 2.3867, 2.4032, 2.4159,
        2.4303, 2.4447, 2.4590, 2.4734, 2.4878, 2.5022, 2.5166, 2.5310, 2.5454, 2.5598,
        2.5742, 2.5885, 2.6029, 2.6173, 2.6317, 2.6461, 2.6605, 2.6749, 2.6893, 2.7037,
        2.7180, 2.7324, 2.7468, 2.7612, 2.7756, 2.7900};

        G4double BGO_ABS[NUMENTRIES] = {
        10.05*cm, 10.03*cm, 10.01*cm, 9.99*cm, 10.04*cm, 10.04*cm, 9.93*cm, 9.93*cm, 9.92*cm, 9.85*cm,
         9.95*cm,  9.92*cm,  9.79*cm, 9.85*cm,  9.87*cm,  9.86*cm, 9.78*cm, 9.83*cm, 9.80*cm, 9.75*cm,
         9.70*cm,  9.72*cm,  9.65*cm, 9.76*cm,  9.73*cm,  9.70*cm, 9.68*cm, 9.65*cm, 9.70*cm, 9.70*cm,
         9.65*cm,  9.58*cm,  9.65*cm, 9.64*cm,  9.60*cm,  9.51*cm, 9.47*cm, 9.47*cm, 9.42*cm, 9.43*cm,
         9.40*cm,  9.39*cm,  9.33*cm, 9.39*cm,  9.33*cm,  9.26*cm, 9.25*cm, 9.17*cm, 9.13*cm, 9.08*cm,
         9.08*cm,  9.06*cm,  8.95*cm, 8.89*cm,  8.86*cm,  8.90*cm, 8.82*cm, 8.71*cm, 8.72*cm, 8.60*cm,
         8.53*cm,  8.40*cm,  8.45*cm, 8.25*cm,  8.22*cm,  7.97*cm, 7.70*cm, 7.76*cm, 7.54*cm, 7.30*cm,
         7.06*cm,  6.71*cm,  6.27*cm, 5.98*cm,  5.52*cm,  4.94*cm, 4.15*cm, 2.60*cm, 1.06*cm, 0.54*cm,
         1e-6*cm,  1e-6*cm,  1e-6*cm, 1e-6*cm,  1e-6*cm,  1e-6*cm, 1e-6*cm, 1e-6*cm, 1e-6*cm, 1e-6*cm,
         1e-6*cm,  1e-6*cm,  1e-6*cm, 1e-6*cm,  1e-6*cm,  1e-6*cm, 1e-6*cm, 1e-6*cm, 1e-6*cm, 1e-6*cm,
         1e-6*cm,  1e-6*cm,  1e-6*cm, 1e-6*cm,  1e-6*cm};

         // now for some constant properties
        for (int i = 0; i < NUMENTRIES; ++i) {
            Vac_RI[i] = 1.00;       // Vacuum refractive index
            Tef_RI[i] = 1.35;       // PTFE   refractive index
            Vac_ABS[i] = 1.0*m;     // Vacuum absorption length (just to speed the sim)
            Tef_ABS[i] = 1e-6*m;    // PTFE   absorptison length
            Tef_R[i] = 0.9;         // PTFE   reflectance
        }

    //==================== BGO =====================



        G4MaterialPropertiesTable* BGO_MPT = new G4MaterialPropertiesTable();

        BGO_MPT->AddProperty("FASTCOMPONENT", WvLngArray, Scint_FAST, NUMENTRIES);
        BGO_MPT->AddProperty("SLOWCOMPONENT", WvLngArray, Scint_SLOW, NUMENTRIES);

        BGO_MPT->AddConstProperty("SCINTILLATIONYIELD", 10000/MeV); // <-----------------------------------------
        BGO_MPT->AddConstProperty("RESOLUTIONSCALE",    2.0);

        BGO_MPT->AddConstProperty("FASTTIMECONSTANT",   1.*ns);
        BGO_MPT->AddConstProperty("SLOWTIMECONSTANT",   300.*ns);
        BGO_MPT->AddConstProperty("YIELDRATIO",         0.0);

        BGO_MPT->AddProperty("RINDEX",      WvLngArray,   BGO_RI,     NUMENTRIES);
        BGO_MPT->AddProperty("ABSLENGTH",   WvLngArray,   BGO_ABS,    NUMENTRIES);

        BGOmat->SetMaterialPropertiesTable(BGO_MPT);



    //==================== Al =====================

        G4MaterialPropertiesTable* PTFE_MPT = new G4MaterialPropertiesTable();

        PTFE_MPT->AddProperty("RINDEX",         WvLngArray, Tef_RI,     NUMENTRIES);
        PTFE_MPT->AddProperty("REFLECTIVITY",   WvLngArray, Tef_R,      NUMENTRIES);
        PTFE_MPT->AddProperty("ABSLENGTH",      WvLngArray, Tef_ABS,    NUMENTRIES);

        PTFEmat->SetMaterialPropertiesTable(PTFE_MPT);

        G4OpticalSurface *PTFE_OptSurf = new G4OpticalSurface("PTFE_OptSurf");
        PTFE_OptSurf->SetModel(glisur);
        PTFE_OptSurf->SetType(dielectric_metal);
        PTFE_OptSurf->SetFinish(ground);
        PTFE_OptSurf->SetPolish(0.9);

        PTFE_OptSurf->SetMaterialPropertiesTable(PTFE_MPT);

        new G4LogicalBorderSurface("Tape_BorderSurface", WorldPV, Tape_CylPV,  PTFE_OptSurf);
    
    //==================== Vacuum ====================
        
        G4MaterialPropertiesTable* vac_MPT = new G4MaterialPropertiesTable();

        vac_MPT -> AddProperty("RINDEX",    WvLngArray,   Vac_RI,     NUMENTRIES);
        vac_MPT -> AddProperty("ABSLENGTH", WvLngArray,   Vac_ABS,    NUMENTRIES);

        Vacmat->SetMaterialPropertiesTable(vac_MPT);



    // //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Visualization Attributes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        // defining some colors in (Red, Green, Blue, Gamma) space
        G4VisAttributes* Gray   = new G4VisAttributes (G4Colour (0.5, 0.5, 0.5, 1.0));
        G4VisAttributes* Blue   = new G4VisAttributes (G4Colour (0.5, 0.5, 1.0, 0.5));
        G4VisAttributes* Red    = new G4VisAttributes (G4Colour (1.0, 0.5, 0.5, 0.5));

        Gray->SetForceSolid (true);
        Blue->SetForceSolid (true);
        Red->SetForceSolid (true);
        
        DetecLV->SetVisAttributes(Gray);
        Tape_CylLV->SetVisAttributes(Blue);
        pmtLV->SetVisAttributes(Red);
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% End %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        return WorldPV;
        
    }