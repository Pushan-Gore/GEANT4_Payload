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
//#include "PhysicalConstants.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::B1DetectorConstruction()
: G4VUserDetectorConstruction(),
  fScoringVolume(0)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::~B1DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B1DetectorConstruction::Construct()
{  
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
   
  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;

  // World
  G4double world_sizeXY = 20*cm;
  G4double world_sizeZ  = 100*cm;
  G4double a, z, density;
  G4double temperature, pressure;
  G4String name;

  density = 1e-25*g/cm3;
  pressure = 1.e-19*pascal;
  temperature = 273*kelvin;
  G4Material* world_mat = new G4Material(name="Galactic", z=1., a=1.01*g/mole,
          density, kStateGas, temperature, pressure);
  //G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
  
  G4Box* solidWorld =    
    new G4Box("World",                          //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size
      
  G4LogicalVolume* logicWorld =                         
    new G4LogicalVolume(solidWorld,             //its solid
                        world_mat,              //its material
                        "World");               //its name
                                   
  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                        //no rotation
                      G4ThreeVector(),          //at (0,0,0)
                      logicWorld,               //its logical volume
                      "World",                  //its name
                      0,                        //its mother  volume
                      false,                    //no boolean operation
                      0,                        //copy number
                      checkOverlaps);           //overlaps checking
   
  // Plastic Detector
  G4Material* pl_detector_mat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  //G4Material* pl_detector_mat = nist->FindOrBuildMaterial("G4_CESIUM_IODIDE");
  G4ThreeVector pos = G4ThreeVector(0, 0, 0);
 
  const G4int NUMENTRIES = 9;
  G4double Scnt_PP[NUMENTRIES] = { 6.6*eV, 6.7*eV, 6.8*eV, 6.9*eV,
      7.0*eV, 7.1*eV, 7.2*eV, 7.3*eV, 7.4*eV };
  G4double Scnt_FAST[NUMENTRIES] = { 0.000134, 0.004432,
       0.053991, 0.241971,
       0.398942, 0.000134,
        0.004432, 0.053991,
        0.241971 };
  G4double Scnt_SLOW[NUMENTRIES] = { 0.000010, 0.000020,
       0.000030, 0.004000,
       0.008000, 0.005000,
        0.020000, 0.001000,
        0.000010 };
  G4double Scnt_RINDEX[NUMENTRIES] = { 1.58, 1.58, 1.58, 1.58,
        1.58, 1.58, 1.58, 1.58, 1.58};
  G4MaterialPropertiesTable* Scnt_MPT = new G4MaterialPropertiesTable();
  Scnt_MPT->AddProperty("FASTCOMPONENT", Scnt_PP, Scnt_FAST, NUMENTRIES);
  Scnt_MPT->AddProperty("SLOWCOMPONENT", Scnt_PP, Scnt_SLOW, NUMENTRIES);
  Scnt_MPT->AddProperty("RINDEX", Scnt_PP, Scnt_RINDEX, NUMENTRIES)->SetSpline(true);
  Scnt_MPT->AddConstProperty("SCINTILLATIONYIELD", 11000./MeV);        // 5000./Mev
  Scnt_MPT->AddConstProperty("RESOLUTIONSCALE", 2.0);
  Scnt_MPT->AddConstProperty("FASTTIMECONSTANT", 1.*ns);
  Scnt_MPT->AddConstProperty("SLOWTIMECONSTANT", 10.*ns);
  Scnt_MPT->AddConstProperty("YIELDRATIO", 0.8);
  pl_detector_mat->SetMaterialPropertiesTable(Scnt_MPT);

 /* 
  const G4int NUMENTRIES = 32;
  G4double ppckov[NUMENTRIES] = {2.034*eV, ......, 4.136*eV};
  G4double rindex[NUMENTRIES] = {1.3435, ......, 1.3608};
  G4double absorption[NUMENTRIES] = {344.8*cm, ......, 1450.0*cm];
  G4MaterialPropertiesTable *MPT = new G4MaterialPropertiesTable();

  MPT -> AddConstProperty("SCINTILLATIONYIELD",100./MeV);
  MPT -> AddProperty("RINDEX"<Plug>PeepOpenpckov,rindex,NUMENTRIES}->SetSpline(true);
  MPT -> AddProperty("ABSLENGTH"<Plug>PeepOpenpckov,absorption,NUMENTRIES}->SetSpline(true);
  pl_detector_mat-> SetMaterialPropertiesTable(MPT);
  */

  // Box Shape shape       
  G4double pl_detector_dx = 5*cm;
  G4double pl_detector_dy = 5*cm;
  G4double pl_detector_dz = 4.74*cm;           //Detector Thickness      
  
  G4Box* solidpl_detector =
    new G4Box("pl_detector",
            pl_detector_dx,
            pl_detector_dy,
            pl_detector_dz);

  logicpl_detector =                         
    new G4LogicalVolume(solidpl_detector,       //its solid
                        pl_detector_mat,        //its material
                        "pl_detector");         //its name
               
  new G4PVPlacement(0,                          //no rotation
                    pos,                        //at position
                    logicpl_detector,           //its logical volume
                    "pl_detector",              //its name
                    logicWorld,                 //its mother  volume
                    false,                      //no boolean operation
                    0,                          //copy number
                    checkOverlaps);             //overlaps checking
                
  // Set pl_detector as scoring volume
  fScoringVolume = logicpl_detector;

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
    //if(hodoscope) 
    //    G4cout << "Hodoscope not null";
    sdManager->AddNewDetector(hodoscope);
    logicpl_detector->SetSensitiveDetector(hodoscope);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
