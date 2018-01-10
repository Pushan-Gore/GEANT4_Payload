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
// $Id: B1EventAction.cc 93886 2015-11-03 08:28:26Z gcosmo $
//
/// \file B1EventAction.cc
/// \brief Implementation of the B1EventAction class

#include "B1EventAction.hh"
#include "B1RunAction.hh"
#include "B5HodoscopeHit.hh"
#include "PayloadSteppingAction.hh"

//Goddess include file
//#include "EventAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4DCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
//#include <G4RunManager.hh>                                                      
#include "G4DigiManager.hh"

#include "persistency/PersistencyHandler.hh" 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1EventAction::B1EventAction(B1RunAction* runAction)
    : G4UserEventAction(),
    fRunAction(runAction),
    fEdep(0.),
    fHodHCID(-1)
{
    G4RunManager::GetRunManager()->SetPrintProgress(0);
} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1EventAction::~B1EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::BeginOfEventAction(const G4Event*)
{   
    fEdep = 0.;
    if (fHodHCID==-1) {                                              
        auto sdManager = G4SDManager::GetSDMpointer();                  
        fHodHCID = sdManager->GetCollectionID("pl_detector/hodoscopeColl");
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::EndOfEventAction(const G4Event* event)
{  
    PersistencyHandler* persistency =
        ((B1RunAction*) G4RunManager::GetRunManager()->GetUserRunAction())->getPersistencyHandler();
	// Run all digitizer modules.
    G4DigiManager* digiManager = G4DigiManager::GetDMpointer();
    G4DCtable* dcTable = digiManager->GetDCtable();
    for (int i = 0; i < dcTable->entries(); i++) {
        G4String dmName = dcTable->GetDMname(i);
        G4VDigitizerModule* dm = digiManager->FindDigitizerModule(dmName);
	    if (dm) {
	        dm->Digitize();
	    }
    }
    G4Timer timer;
    timer.Start();
    // Process hits collections.
    G4HCofThisEvent* hCof = event->GetHCofThisEvent();
    if (hCof != NULL) {
        for (int i = 0; i < hCof->GetCapacity(); ++i) {
            G4VHitsCollection* hc = hCof->GetHC(i);
            if (hc != NULL) {
                if (dynamic_cast<G4SipmHitsCollection*>(hc)) {
                    persistency->persist((G4SipmHitsCollection*) hc);
                }
            }
        }
    }
    // Process digi collections.
    G4DCofThisEvent* dCof = event->GetDCofThisEvent();
    if (dCof != NULL) {
        for (int i = 0; i < dCof->GetCapacity(); ++i) {
            G4VDigiCollection* dc = dCof->GetDC(i);
            if (dc != NULL) {
                if (dynamic_cast<G4SipmDigiCollection*>(dc)) {
                    persistency->persist((G4SipmDigiCollection*) dc);
                }
                if (dynamic_cast<G4SipmVoltageTraceDigiCollection*>(dc)) {
                    persistency->persist((G4SipmVoltageTraceDigiCollection*) dc);
                }
            }
        }
    }
    timer.Stop();
    //std::cout << "EventAction::EndOfEventAction(): persist time (" << timer << ")." << std::endl;
    
    // accumulate statistics in run action
    fRunAction->AddEdep(fEdep);
    auto hce = event->GetHCofThisEvent();
    if (!hce) {
        G4ExceptionDescription msg;
        msg << "No hits collection of this event found." << G4endl; 
        G4Exception("B5EventAction::EndOfEventAction()",
                "B5Code001", JustWarning, msg);
        return;
    }

    // Get hits collections 
    auto hHC = static_cast<B5HodoscopeHitsCollection*>(hce->GetHC(fHodHCID));   

    auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
    if ( printModulo==0 || event->GetEventID() % printModulo != 0) return;

    auto primary = event->GetPrimaryVertex(0)->GetPrimary(0);
    G4cout 
        << G4endl
        << ">>> Event " << event->GetEventID() << " >>> Simulation truth : "
        << primary->GetG4code()->GetParticleName()
        << " " << primary->GetMomentum() << G4endl;
    
    /*
    // Hodoscope Plastic_SC
    auto nhit  = hHC->entries();
    G4cout << "Hodoscope has " << nhit  << " hit(s)." << G4endl;
    for (auto i=0;i<nhit ;i++) {
      auto hit = (*hHC)[i];
      hit->Print();
    }
    */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
