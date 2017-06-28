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
/// \file OpNovice/src/OpNovicePhysicsList.cc
/// \brief Implementation of the OpNovicePhysicsList class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "globals.hh"
#include "OpNovicePhysicsList.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4StepLimiter.hh"
#include "G4SystemOfUnits.hh"

#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"


//#include "G4LowEnergyCompton.hh"
//#include "G4LowEnergyRayleigh.hh"
//#include "G4LowEnergyPhoto-Electric.hh"

#include "G4OpticalPhysics.hh"
#include "G4OpWLS.hh" 
#include "G4PhotoElectricEffect.hh"
#include "G4RayleighScattering.hh"
#include "G4eMultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4ProcessManager.hh"

#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpMieHG.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4EmParameters.hh"   

#include "G4LossTableManager.hh"
#include "G4UAtomicDeexcitation.hh"
#include "G4EmSaturation.hh"

G4ThreadLocal G4int OpNovicePhysicsList::fVerboseLevel = 2;     // Set this to 2
G4ThreadLocal G4int OpNovicePhysicsList::fMaxNumPhotonStep = 20;
G4ThreadLocal G4PhotoElectricEffect* OpNovicePhysicsList::fPhotoElectricEffect = 0;
G4ThreadLocal G4RayleighScattering* OpNovicePhysicsList::fRayleighScattering = 0;
G4ThreadLocal G4eMultipleScattering* OpNovicePhysicsList::feMultipleScattering = 0;
G4ThreadLocal G4eIonisation* OpNovicePhysicsList::feIonization = 0;
G4ThreadLocal G4eBremsstrahlung* OpNovicePhysicsList::feBremsstrahlung = 0;

G4ThreadLocal G4Cerenkov* OpNovicePhysicsList::fCerenkovProcess = 0;
G4ThreadLocal G4Scintillation* OpNovicePhysicsList::fScintillationProcess = 0;
G4ThreadLocal G4OpAbsorption* OpNovicePhysicsList::fAbsorptionProcess = 0;
G4ThreadLocal G4OpRayleigh* OpNovicePhysicsList::fRayleighScatteringProcess = 0;
G4ThreadLocal G4OpMieHG* OpNovicePhysicsList::fMieHGScatteringProcess = 0;
G4ThreadLocal G4OpBoundaryProcess* OpNovicePhysicsList::fBoundaryProcess = 0;
 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNovicePhysicsList::OpNovicePhysicsList() 
 : G4VUserPhysicsList()
{
    G4LossTableManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpNovicePhysicsList::~OpNovicePhysicsList() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNovicePhysicsList::ConstructParticle()
{
  // In this method, static member functions should be called
  // for all particles which you want to use.
  // This ensures that objects of these particle types will be
  // created in the program.

  G4BosonConstructor bConstructor;
  bConstructor.ConstructParticle();

  G4LeptonConstructor lConstructor;
  lConstructor.ConstructParticle();

  G4MesonConstructor mConstructor;
  mConstructor.ConstructParticle();

  G4BaryonConstructor rConstructor;
  rConstructor.ConstructParticle();

  G4IonConstructor iConstructor;
  iConstructor.ConstructParticle();

  G4OpticalPhoton::OpticalPhotonDefinition(); 
  G4Gamma::GammaDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNovicePhysicsList::ConstructProcess()
{
  AddTransportation();
  //ConstructDecay();
  ConstructEM();
  ConstructOp();

  G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();                        
  de->SetFluo(true);                                                            
  de->SetAuger(true);                                                          
  de->SetPIXE(false);                                                           
  G4LossTableManager::Instance()->SetAtomDeexcitation(de); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4Decay.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/*
void OpNovicePhysicsList::ConstructDecay() {
// Add Decay Process
G4Decay* theDecayProcess = new G4Decay();
auto particleIterator=GetParticleIterator();
particleIterator->reset();
while( (*particleIterator)() ){
G4ParticleDefinition* particle = particleIterator->value();
G4ProcessManager* pmanager = particle->GetProcessManager();
if (theDecayProcess->IsApplicable(*particle)) {
pmanager ->AddProcess(theDecayProcess);
// set ordering for PostStepDoIt and AtRestDoIt
pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
}
}
}
*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ComptonScattering.hh"
#include "G4PenelopeComptonModel.hh"

#include "G4GammaConversion.hh"

#include "G4PhotoElectricEffect.hh"
#include "G4PenelopePhotoElectricModel.hh"

#include "G4eMultipleScattering.hh"
#include "G4MuMultipleScattering.hh"
#include "G4hMultipleScattering.hh"

#include "G4eIonisation.hh"
#include "G4PenelopeIonisationModel.hh"

#include "G4eBremsstrahlung.hh"
#include "G4PenelopeBremsstrahlungModel.hh"

#include "G4PenelopeRayleighModel.hh"


#include "G4eplusAnnihilation.hh"

#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

#include "G4hIonisation.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNovicePhysicsList::ConstructEM()
{
    auto particleIterator=GetParticleIterator();
    particleIterator->reset();
    while( (*particleIterator)() ){
        G4ParticleDefinition* particle = particleIterator->value();
        G4ProcessManager* pmanager = particle->GetProcessManager();
        G4String particleName = particle->GetParticleName();

        if (particleName == "gamma") { //|| (particleName == "opticalphoton")) {
            // Construct processes for gamma
            //pmanager->AddDiscreteProcess(new G4GammaConversion());
            
            /*
            G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
            //thePhotoElectricEffect->SetEmModel(new G4PenelopePhotoElectricModel());
            pmanager->AddDiscreteProcess(thePhotoElectricEffect);                  
            
            G4ComptonScattering* theComptonScattering = new G4ComptonScattering(); 
            //theComptonScattering->SetEmModel(new G4PenelopeComptonModel());       
            pmanager->AddDiscreteProcess(theComptonScattering);

            G4RayleighScattering* theRayleighScattering = new G4RayleighScattering();
            //theRayleighScattering->SetEmModel(new G4PenelopeRayleighModel());
            pmanager->AddDiscreteProcess(theRayleighScattering);
            */

        } else if (particleName == "e-") { 
            // Construct processes for electron
            
            /*
            pmanager->AddProcess(new G4eMultipleScattering(),-1, -1, 1);// -1, 1, 1);

            G4eIonisation* eIonisation =  new G4eIonisation();
            //eIonisation->SetEmModel(new G4PenelopeIonisationModel());
            eIonisation->SetStepFunction(0.1, 100*um); // Improved precision in tracking
            pmanager->AddProcess(eIonisation,        -1, -1, 2);

            G4eBremsstrahlung* eBremsstrahlung = new G4eBremsstrahlung();
            //eBremsstrahlung->SetEmModel(new G4PenelopeBremsstrahlungModel());
            pmanager->AddProcess(eBremsstrahlung, -1, -1, 3);  // -1, 3, 3);
            pmanager->AddProcess(new G4StepLimiter,               -1,-1, 5);
            */

        } else if (particleName == "proton") {}

        }
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#include "G4Threading.hh"

void OpNovicePhysicsList::ConstructOp() {
        fCerenkovProcess = new G4Cerenkov("Cerenkov");
        fCerenkovProcess->SetMaxNumPhotonsPerStep(fMaxNumPhotonStep);
        fCerenkovProcess->SetMaxBetaChangePerStep(10.0);
        fCerenkovProcess->SetTrackSecondariesFirst(true);
        
        fScintillationProcess      =    new G4Scintillation("Scintillation");
        fScintillationProcess->SetScintillationYieldFactor(0.8);
        fScintillationProcess->SetTrackSecondariesFirst(true);
        fScintillationProcess->SetFiniteRiseTime(1.5*us);
        //fScintillationProcess->SetScintillationByParticleType(true);
        //fScintillationProcess->SetScintillationExcitationRatio(1.0);

        fAbsorptionProcess         =    new G4OpAbsorption();
        fRayleighScatteringProcess =    new G4OpRayleigh();
        fMieHGScatteringProcess    =    new G4OpMieHG();
        fBoundaryProcess           =    new G4OpBoundaryProcess();

        fScintillationProcess       ->SetVerboseLevel(fVerboseLevel);
        fAbsorptionProcess          ->SetVerboseLevel(fVerboseLevel);
        fRayleighScatteringProcess  ->SetVerboseLevel(fVerboseLevel);
        fMieHGScatteringProcess     ->SetVerboseLevel(fVerboseLevel);
        fBoundaryProcess            ->SetVerboseLevel(fVerboseLevel);

        // Use Birks Correction in the Scintillation process
        if(G4Threading::IsMasterThread()) {
            G4cout << "Birks was applied";
            G4EmSaturation* emSaturation =
            G4LossTableManager::Instance()->EmSaturation();
            fScintillationProcess->AddSaturation(emSaturation);
        }

        auto particleIterator=GetParticleIterator();
        particleIterator->reset();
        while( (*particleIterator)() ){
            G4ParticleDefinition* particle = particleIterator->value();
            G4ProcessManager* pmanager = particle->GetProcessManager();
            G4String particleName = particle->GetParticleName();

            if (fCerenkovProcess->IsApplicable(*particle)) {
                G4cout << "Cerenkov process applicable and added to " << particle->GetParticleName()<< G4endl;
            
                //pmanager->AddProcess(fCerenkovProcess);
                //pmanager->SetProcessOrdering(fCerenkovProcess,idxPostStep);
            }
            if (fScintillationProcess->IsApplicable(*particle)) {
                G4cout << "Scintillation process applicable and added to " << particle->GetParticleName()<< G4endl;

                pmanager->AddProcess(fScintillationProcess);
                pmanager->SetProcessOrderingToLast(fScintillationProcess, idxAtRest);
                pmanager->SetProcessOrderingToLast(fScintillationProcess, idxPostStep);

                //pmanager->AddDiscreteProcess(fBoundaryProcess);
            }
            if (particleName == "opticalphoton") {
                G4cout << " AddDiscreteProcess to OpticalPhoton " << G4endl;
                //pmanager->AddDiscreteProcess(fAbsorptionProcess);
                //pmanager->AddDiscreteProcess(fRayleighScatteringProcess);
                //pmanager->AddDiscreteProcess(fMieHGScatteringProcess);
                //pmanager->AddDiscreteProcess(fBoundaryProcess);
            }
        }
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNovicePhysicsList::SetVerbose(G4int verbose) {
        fVerboseLevel = 2;//verbose;

        fScintillationProcess->SetVerboseLevel(fVerboseLevel);
        fAbsorptionProcess->SetVerboseLevel(fVerboseLevel);
        fRayleighScatteringProcess->SetVerboseLevel(fVerboseLevel);
        fMieHGScatteringProcess->SetVerboseLevel(fVerboseLevel);
        fBoundaryProcess->SetVerboseLevel(fVerboseLevel);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpNovicePhysicsList::SetCuts() {
        //  " G4VUserPhysicsList::SetCutsWithDefault" method sets
        //   the default cut value for all particle types
        
        //SetCutsWithDefault();

        if (verboseLevel>0) {}; //DumpCutValuesTable();
}

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
