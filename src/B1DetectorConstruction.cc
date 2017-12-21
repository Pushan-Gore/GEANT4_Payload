//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B1DetectorConstruction.cc 94307 2015-11-11 13:42:46Z gcosmo $
//
/// \file B1DetectorConstruction.cc
/// \brief Implementation of the B1DetectorConstruction class

#include "B1DetectorConstruction.hh"
#include "B5HodoscopeSD.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4PhysicalConstants.hh" 
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "G4Sipm.hh"
#include "MaterialFactory.hh"                                                   
#include "model/G4SipmModelFactory.hh"                                          
#include "housing/G4SipmHousing.hh"                                             
#include "housing/impl/HamamatsuCeramicHousing.hh"                              
#include "housing/impl/HamamatsuSmdHousing.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::B1DetectorConstruction()
    : G4VUserDetectorConstruction(),
    fScoringVolume(0)
{
    // Create SiPM and housing 
    // NOTE: The model used is generic model, refer to other models to fit the situation
    G4SipmModel* model = G4SipmModelFactory::getInstance()->createGenericSipmModel();
    // NOTE: Default housing model is used, the other options are smd and ceramic
    housing = new G4SipmHousing(new G4Sipm(model));

    lambda_min = 200*nm ;
    lambda_max = 700*nm ;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::~B1DetectorConstruction() { 
    delete housing;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B1DetectorConstruction::Construct()
{  
    // Get nist material manager
    G4NistManager* nist = G4NistManager::Instance();
    
    // Option to switch on/off checking of volumes overlaps
    G4bool checkOverlaps = true;

    // World
    G4double world_sizeXY = 200*cm;
    G4double world_sizeZ  = 1000*cm;
    G4double a, z, density;
    G4double temperature, pressure;
    G4String name;

    density = universe_mean_density;
    pressure = 3.e-18*pascal;
    temperature = 2.73*kelvin;
    G4Material* world_mat = new G4Material(name="Galactic", z=1., a=1.01*g/mole,
            density, kStateGas, temperature, pressure);

    /* Added to test RINDEX of vacuum */
    /*
       const G4int N_RINDEX_VAC = 2 ;                                             
       G4double X_RINDEX_VAC[N_RINDEX_VAC] = {h_Planck*c_light/lambda_max, h_Planck*c_light/lambda_min} ; 
       G4double RINDEX_VAC[N_RINDEX_VAC] = {1, 1};                       

       G4MaterialPropertiesTable *MPT_PMT = new G4MaterialPropertiesTable();         
       MPT_PMT->AddProperty("RINDEX", X_RINDEX_VAC, RINDEX_VAC, N_RINDEX_VAC);
       MPT_PMT->DumpTable();

       world_mat->SetMaterialPropertiesTable(MPT_PMT);   
       */ 
    /* Remove block till here */

    G4Box* solidWorld =    
        new G4Box("World",                                                //its name
                0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size

    G4LogicalVolume* logicWorld =                         
        new G4LogicalVolume(solidWorld,                                   //its solid
                world_mat,                                                //its material
                "World");                                                 //its name

    G4VPhysicalVolume* physWorld = 
        new G4PVPlacement(0,                                              //no rotation
                G4ThreeVector(),                                          //at (0,0,0)
                logicWorld,                                               //its logical volume
                "World",                                                  //its name
                0,                                                        //its mother  volume
                false,                                                    //no boolean operation
                0,                                                        //copy number
                checkOverlaps);                                           //overlaps checking

    vacuum_volume = logicWorld;
 
    // Plastic Detector
    G4Material* pl_detector_mat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    G4ThreeVector pos_pl = G4ThreeVector(0, 0, 0);

    const G4int NUMENTRIES = 2;
    G4double Scnt_PP[NUMENTRIES] = { h_Planck*c_light/lambda_max, h_Planck*c_light/lambda_min};
    G4double Scnt_FAST[NUMENTRIES] = { 0.017, 0.043 };
    G4double Scnt_RINDEX[NUMENTRIES] = { 1.58, 1.58 };
    G4double Scnt_absorption[NUMENTRIES] = {1600.*mm,1600*mm};
    
    G4MaterialPropertiesTable* Scnt_MPT = new G4MaterialPropertiesTable();
    Scnt_MPT->AddProperty("FASTCOMPONENT", Scnt_PP, Scnt_FAST, NUMENTRIES);
    Scnt_MPT->AddProperty("ABSLENGTH", Scnt_PP, Scnt_absorption,NUMENTRIES);
    Scnt_MPT->AddProperty("RINDEX", Scnt_PP, Scnt_RINDEX, NUMENTRIES);
    Scnt_MPT->AddConstProperty("SCINTILLATIONYIELD", 13600./MeV);         
    Scnt_MPT->AddConstProperty("RESOLUTIONSCALE", 1.);           
    Scnt_MPT->AddConstProperty("FASTTIMECONSTANT", 1.8*ns);
    Scnt_MPT->AddConstProperty("FASTSCINTILLATIONRISETIME", 0.7*ns);
    //Scnt_MPT->AddConstProperty("YIELDRATIO", 1.);

    //Scnt_MPT->DumpTable();
    pl_detector_mat->SetMaterialPropertiesTable(Scnt_MPT);
    pl_detector_mat->GetIonisation()->SetBirksConstant(0.111*mm/MeV);

    // Box Shape shape       
    G4double pl_detector_dx = 5*cm;
    G4double pl_detector_dy = 5*cm;
    G4double pl_detector_dz = 1.2*mm;//4.74*cm;        //Detector Thickness      

    G4Box* solidpl_detector =
    new G4Box("pl_detector",
        pl_detector_dx*0.5,
        pl_detector_dy*0.5,
        pl_detector_dz*0.5);

    logicpl_detector =                         
    new G4LogicalVolume(solidpl_detector,   //its solid
        pl_detector_mat,                    //its material
        "pl_detector");                     //its name
    
    new G4PVPlacement(0,                    //no rotation
        pos_pl,                                //at position
        logicpl_detector,                   //its logical volume
        "pl_detector",                      //its name
        logicWorld,                         //its mother  volume
        false,                              //no boolean operation
        0,                                  //copy number
        checkOverlaps);                     //overlaps checking
    
    // Set pl_detector as scoring volume
    fScoringVolume = logicpl_detector;
    G4cout << "Scoring volume set to Plastic detector" << G4endl;

    //CsI Crystal
    // Box Shape shape       
    G4double csi_crystal_dx = 5*cm;
    G4double csi_crystal_dy = 5*cm;
    G4double csi_crystal_dz = 3.2*cm;       //Crystal Thickness      
    
    G4Material* csi_crystal_mat = nist->FindOrBuildMaterial("G4_CESIUM_IODIDE");
    G4ThreeVector pos_csi = G4ThreeVector(0, 0, (pl_detector_dz*0.5) + (csi_crystal_dz*0.5));

    const G4int CSI_NUMENTRIES = 8;
    G4double CSI_Crystal_PP[CSI_NUMENTRIES] = {3.37*eV, 3.62*eV, 3.71*eV, 3.77*eV,
        4.02*eV, 4.07*eV, 4.19*eV, 4.42*eV};
        //h_Planck*c_light/lambda_max, h_Planck*c_light/lambda_min};//6.6*eV, 7.4*eV };
    G4double CSI_Crystal_FAST[CSI_NUMENTRIES] = {0.23, 0.62, 0.77, 0.95, 0.97, 0.92, 0.63, 0.27};
    G4double CSI_Crystal_RINDEX[CSI_NUMENTRIES] = { 1.78, 1.78, 1.78, 1.78, 1.78, 1.78, 1.78, 1.78};
    G4double CSI_Crystal_absorption[CSI_NUMENTRIES] = {40*cm, 40*cm, 40*cm, 40*cm, 40*cm, 40*cm, 40*cm, 40*cm};
    
    G4MaterialPropertiesTable* CSI_Crystal_MPT = new G4MaterialPropertiesTable();
    CSI_Crystal_MPT->AddProperty("FASTCOMPONENT", CSI_Crystal_PP, CSI_Crystal_FAST, CSI_NUMENTRIES);
    CSI_Crystal_MPT->AddProperty("ABSLENGTH", CSI_Crystal_PP, CSI_Crystal_absorption,CSI_NUMENTRIES);
    CSI_Crystal_MPT->AddProperty("RINDEX", CSI_Crystal_PP, CSI_Crystal_RINDEX, CSI_NUMENTRIES);
    CSI_Crystal_MPT->AddConstProperty("SCINTILLATIONYIELD", 54000./MeV);        
    CSI_Crystal_MPT->AddConstProperty("RESOLUTIONSCALE", 1.);            
    CSI_Crystal_MPT->AddConstProperty("FASTTIMECONSTANT", 0.6*us);
    //CSI_Crystal_MPT->AddConstProperty("SLOWTIMECONSTANT", 3500.*ns);
    //CSI_Crystal_MPT->AddConstProperty("FASTSCINTILLATIONRISETIME", 0.7*ns);
    //CSI_Crystal_MPT->AddConstProperty("SLOWSCINTILLATIONRISETIME", 1000.*ns);
    //CSI_Crystal_MPT->AddConstProperty("YIELDRATIO", 1.);

    //CSI_Crystal_MPT->DumpTable();
    csi_crystal_mat->SetMaterialPropertiesTable(CSI_Crystal_MPT);
    csi_crystal_mat->GetIonisation()->SetBirksConstant(0.*mm/MeV);

    G4Box* solidcsi_crystal =
    new G4Box("csi_crystal",
        csi_crystal_dx*0.5,
        csi_crystal_dy*0.5,
        csi_crystal_dz*0.5);

    logiccsi_crystal =                         
    new G4LogicalVolume(solidcsi_crystal,  //its solid
        csi_crystal_mat,                   //its material
        "csi_crystal");                    //its name

    new G4PVPlacement(0,                    //no rotation
        pos_csi,                            //at position
        logiccsi_crystal,                   //its logical volume
        "csi_crystal",                      //its name
        logicWorld,                         //its mother  volume
        false,                              //no boolean operation
        0,                                  //copy number
        checkOverlaps);                     //overlaps checking

    /* End of Detector and crystal construction */


    //G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;  
    //G4cout << *(G4Material::GetMaterialTable()) << G4endl; 

    /* If photons are not visible remove this block*/
    auto visAttributes = new G4VisAttributes(G4Colour(0.8888,0.0,0.0));           
    logicpl_detector->SetVisAttributes(visAttributes);       
    
    visAttributes = new G4VisAttributes(G4Colour(0.0,0.0,0.8888));
    logiccsi_crystal->SetVisAttributes(visAttributes);


    // Need to remove this IMPORTANT
  // fScoringVolume = logiccsi_crystal;

    // Always return the physical World
    return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1DetectorConstruction::ConstructSDandField()
{
    // sensitive detectors -----------------------------------------------------
    auto sdManager = G4SDManager::GetSDMpointer();
    G4String SDname;

    G4VSensitiveDetector* hodoscope = new B5HodoscopeSD(SDname="/pl_detector");
    if(hodoscope) 
        G4cout << "Hodoscope(Sensitive detector) set to : " << hodoscope->GetName() << G4endl;
    sdManager->AddNewDetector(hodoscope);
    logicpl_detector->SetSensitiveDetector(hodoscope);
}

G4SipmModel* B1DetectorConstruction::getSipmModel() const {
    return housing->getSipm()->getModel();
}

G4SipmHousing* B1DetectorConstruction::getSipmHousing() const {
    return housing;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
