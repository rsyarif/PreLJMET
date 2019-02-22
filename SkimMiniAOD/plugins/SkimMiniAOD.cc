// -*- C++ -*-
//
// Package:    PreLJMET/SkimMiniAOD
// Class:      SkimMiniAOD
// 
/**\class SkimMiniAOD SkimMiniAOD.cc PreLJMET/SkimMiniAOD/plugins/SkimMiniAOD.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Rizki Syarif
//         Created:  Thu, 21 Feb 2019 20:38:33 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "FWCore/Common/interface/TriggerNames.h"


//
// class declaration
//

class SkimMiniAOD : public edm::stream::EDFilter<> {
   public:
      explicit SkimMiniAOD(const edm::ParameterSet&);
      ~SkimMiniAOD();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginStream(edm::StreamID) override;
      virtual bool filter(edm::Event&, const edm::EventSetup&) override;
      virtual void endStream() override;

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
      
      edm::EDGetTokenT<pat::MuonCollection> 		muonsToken;
      edm::EDGetTokenT<pat::ElectronCollection> 	electronsToken;
      edm::EDGetTokenT<edm::TriggerResults> 		triggersToken;      
      std::vector<std::string>						vTargetTrigs;
      int											minLeptons; 
      bool											debug;
      bool											filter_by_minLeptons;
      bool											filter_by_HLT;
            
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
SkimMiniAOD::SkimMiniAOD(const edm::ParameterSet& iConfig)
{
   //now do what ever initialization is needed

   debug				= iConfig.getParameter<bool>("debug");
   
   muonsToken 			= consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muonCollection"));;
   electronsToken 		= consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electronCollection"));
   triggersToken		= consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("HLT"));
   vTargetTrigs			= iConfig.getParameter<std::vector<std::string>>("HLTpaths");
   minLeptons			= iConfig.getParameter<int>("minLeptons");
   filter_by_minLeptons	= iConfig.getParameter<bool>("filter_by_minLeptons");if(debug && filter_by_minLeptons )std::cout << "minLeptons = " << minLeptons <<std::endl;
   filter_by_HLT		= iConfig.getParameter<bool>("filter_by_HLT");

}


SkimMiniAOD::~SkimMiniAOD()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
SkimMiniAOD::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
      
   //===============================
   //Filter events based on nLeptons
   //===============================
   
   if(filter_by_minLeptons){
   
	   edm::Handle< pat::MuonCollection > muonsHandle;
	   iEvent.getByToken(muonsToken, muonsHandle);   
	   unsigned int nMu =  muonsHandle->size();
		 
	   edm::Handle< pat::ElectronCollection > electronsHandle;
	   iEvent.getByToken(electronsToken, electronsHandle);
	   unsigned int nEl =  electronsHandle->size();   

	   if(debug)std::cout << " nMu = "<< nMu << " nEl = "<< nEl << std::endl;   
	   if ( (nMu + nEl) < (unsigned int)minLeptons && minLeptons>0){ //if minLepton==-1 , skip this check.
			return false;   		
	   }
	   if(debug)std::cout << "Has 3 leptons ! "<< std::endl;
   
   }
   

   //===============================
   //Filter events based on HLT
   //===============================
   
   if(filter_by_HLT){
         
	   edm::Handle< edm::TriggerResults > triggersHandle;
	   iEvent.getByToken(triggersToken,triggersHandle);
	   const edm::TriggerNames trigNames = iEvent.triggerNames(*triggersHandle);
   
	   unsigned int nTrig = triggersHandle->size();

	   for(std::vector<std::string>::const_iterator targetTrig = vTargetTrigs.begin(); targetTrig!= vTargetTrigs.end(); targetTrig++){

		   for(unsigned int i=0; i<nTrig;i++){

				std::string trigName = trigNames.triggerName(i);			
				if(trigName.find((*targetTrig)) == std::string::npos) continue;

				if(triggersHandle->accept(trigNames.triggerIndex(trigName))){
					if(debug)std::cout << "FIRED "<< (*targetTrig) << std::endl;
					return true;			
				}

			}

	   }
	   return false;
   }
   
   return true; 
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
SkimMiniAOD::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
SkimMiniAOD::endStream() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
SkimMiniAOD::beginRun(edm::Run const&, edm::EventSetup const&)
{ 
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
SkimMiniAOD::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
SkimMiniAOD::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
SkimMiniAOD::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
SkimMiniAOD::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(SkimMiniAOD);
