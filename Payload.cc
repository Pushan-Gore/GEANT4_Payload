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
// $Id: exampleB1.cc 86065 2014-11-07 08:51:15Z gcosmo $
//
/// \file exampleB1.cc
/// \brief Main program of the B1 example

#include "B1DetectorConstruction.hh"

#include "OpNovicePhysicsList.hh"
#include "LXePhysicsList.hh"
#include "PhysicsList.hh"  

#include "B1ActionInitialization.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "FTFP_BERT.hh"
#include "QBBC.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4SystemOfUnits.hh" 

#include "Randomize.hh"

// Goddess include files
#include <GODDeSS_Messenger.hh>
#include <ScintillatorTileConstructor.hh>
#include <FibreConstructor.hh>
#include <PhotonDetectorConstructor.hh>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  // Detect interactive mode (if no arguments) and define UI session
  G4UIExecutive* ui = 0;
  if ( argc == 1 ) {
    ui = new G4UIExecutive(argc, argv);
  }

  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  
  // Construct the default run manager
#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
#else
  G4RunManager* runManager = new G4RunManager;
#endif
  
  //Goddess messenger initialisation:
  //GODDeSS: energy range for the property distributions
  vector<G4double> energyRangeVector;
  G4double energyRangeVectorSize = 10;
  G4double energiesMin = 2.2*eV;
  G4double energiesMax = 7.2*eV;
  for(int i = 0; i < energyRangeVectorSize; i++) 
    energyRangeVector.push_back( energiesMin + i * (energiesMax - energiesMin) / (energyRangeVectorSize - 1) );

  //GODDeSS Messenger:
  GODDeSS_Messenger * goddessMessenger = new GODDeSS_Messenger(energyRangeVector);



  // Set mandatory initialization classes
  // 1. Detector construction
  B1DetectorConstruction* DetectorConstruction = new B1DetectorConstruction(goddessMessenger); // added goddess messenger
  runManager->SetUserInitialization(DetectorConstruction);

  // 2. Physics list
  
  LXePhysicsList* physicsList = new LXePhysicsList(); // If this doesn't work add G4VModularPhysicsList
  
  // Goddess Physiocs list modification
  //ScintillatorTileConstructor * scintillatorTileConstructor = new ScintillatorTileConstructor(physicsList, goddessMessenger->GetPropertyToolsManager(), goddessMessenger->GetDataStorage(), SearchOverlaps);
  //goddessMessenger->SetScintillatorTileConstructor(scintillatorTileConstructor);

  //FibreConstructor * fibreConstructor = new FibreConstructor(physicsList, goddessMessenger->GetPropertyToolsManager(), goddessMessenger->GetDataStorage(), SearchOverlaps);
  //goddessMessenger->SetFibreConstructor(fibreConstructor);

  //G4String hitFile = path/to/file/which/the/hitting/photons/should/be/saved/in;
  //goddessMessenger->GetDataStorage()->SetPhotonDetectorHitFile(hitFile);
  //PhotonDetectorConstructor * photonDetectorConstructor = new PhotonDetectorConstructor(physicsList, goddessMessenger->GetPropertyToolsManager(), goddessMessenger->GetDataStorage(), SearchOverlaps);
  //goddessMessenger->SetPhotonDetectorConstructor(photonDetectorConstructor);
  
  //physicsList->SetVerboseLevel(1);
  //runManager->SetUserInitialization(physicsList);
  //runManager->SetUserInitialization(new OpNovicePhysicsList());
  runManager->SetUserInitialization(physicsList);
  //runManager->SetUserInitialization(new PhysicsList());


  // 3. User action initialization
  runManager->SetUserInitialization(new B1ActionInitialization());
  
  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive("quite");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  if ( ! ui ) { 
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    printf("Filename %s\n", argv[1]);
    UImanager->ApplyCommand(command+fileName);
  }
  else { 
    // interactive mode
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  //GODDeSS
  //delete goddessMessenger;
  //delete scintillatorTileConstructor;
  //delete fibreConstructor;
  //delete photonDetectorConstructor;

  delete visManager;
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
