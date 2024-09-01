/*
 * ntuple_JetInfo.cc
 *
 *  Created on: 13 Feb 2017
 *      Author: jkiesele
 */

#include "../interface/ntuple_JetInfo.h"
#include "../interface/helpers.h"
#include <vector>
#include <algorithm>
#include "DataFormats/Math/interface/deltaR.h"

using namespace std;

template<typename T> 
class PatPtSorter {
public:
  bool operator()(const T& i, const T& j) const {
    return (i.pt() > j.pt());
  }
};


template<typename T>
class PatRefPtSorter {
public:
  bool operator()(const T& i, const T& j) const {
    return (i->pt() > j->pt());
  }
};

void ntuple_JetInfo::getInput(const edm::ParameterSet& iConfig){

    gluonReduction_=(iConfig.getParameter<double>("gluonReduction"));
    jetPtMin_=(iConfig.getParameter<double>("jetPtMin"));
    jetPtMax_=(iConfig.getParameter<double>("jetPtMax"));
    jetAbsEtaMin_=(iConfig.getParameter<double>("jetAbsEtaMin"));
    jetAbsEtaMax_=(iConfig.getParameter<double>("jetAbsEtaMax"));
    min_candidate_pt_ = (iConfig.getParameter<double>("minCandidatePt"));
    
    vector<string> disc_names = iConfig.getParameter<vector<string> >("bDiscriminators");
    for(auto& name : disc_names) {
        discriminators_[name] = 0.;
    }
}
void ntuple_JetInfo::initBranches(TTree* tree){

    //more general event info, here applied per jet
    addBranch(tree,"npv"    ,&npv_    ,"npv/F"    );
    addBranch(tree,"rho", &rho_, "rho/F");
    addBranch(tree,"ntrueInt",&ntrueInt_,"ntrueInt/F");
    addBranch(tree,"event_no"    ,&event_no_    ,"event_no/I"    );
    addBranch(tree,"jet_no"    ,&jet_no_    ,"jet_no/I"    );

    // truth labels
    addBranch(tree,"gen_pt"    ,&gen_pt_    ,"gen_pt_/F"    );
    addBranch(tree,"Delta_gen_pt"    ,&Delta_gen_pt_,"Delta_gen_pt_/F"    );

    addBranch(tree,"isMC",&isMC_, "isMC_/I");
    addBranch(tree,"isemu",&isemu_, "isemu_/I");
    addBranch(tree,"isdimu",&isdimu_, "isdimu_/I");
    addBranch(tree,"ismutau",&ismutau_, "ismutau_/I");
    
    addBranch(tree,"isB",&isB_, "isB_/I");
    addBranch(tree,"isGBB",&isGBB_, "isGBB_/I");
    addBranch(tree,"isBB",&isBB_, "isBB_/I");
    addBranch(tree,"isLeptonicB",&isLeptonicB_, "isLeptonicB_/I");
    addBranch(tree,"isLeptonicB_C",&isLeptonicB_C_, "isLeptonicB_C_/I");
    addBranch(tree,"isC",&isC_, "isC_/I");
    addBranch(tree,"isGCC",&isGCC_, "isGCC_/I");
    addBranch(tree,"isCC",&isCC_, "isCC_/I");
    addBranch(tree,"isD",&isD_, "isD_/I");
    addBranch(tree,"isU",&isU_, "isU_/I");
    addBranch(tree,"isS",&isS_, "isS_/I");
    addBranch(tree,"isG",&isG_, "isG_/I");
    addBranch(tree,"isMU",&isMU_, "isMU_/I");
    addBranch(tree,"isELE",&isELE_, "isELE_/I");
    addBranch(tree,"isTaup1h0p_",&isTaup1h0p_, "isTaup1h0p_/I");
    addBranch(tree,"isTaup1h1p_",&isTaup1h1p_, "isTaup1h1p_/I");
    addBranch(tree,"isTaup1h2p_",&isTaup1h2p_, "isTaup1h2p_/I");
    addBranch(tree,"isTaup3h0p_",&isTaup3h0p_, "isTaup3h0p_/I");
    addBranch(tree,"isTaup3h1p_",&isTaup3h1p_, "isTaup3h1p_/I");
    addBranch(tree,"isTaum1h0p_",&isTaum1h0p_, "isTaum1h0p_/I");
    addBranch(tree,"isTaum1h1p_",&isTaum1h1p_, "isTaum1h1p_/I");
    addBranch(tree,"isTaum1h2p_",&isTaum1h2p_, "isTaum1h2p_/I");
    addBranch(tree,"isTaum3h0p_",&isTaum3h0p_, "isTaum3h0p_/I");
    addBranch(tree,"isTaum3h1p_",&isTaum3h1p_, "isTaum3h1p_/I");
    addBranch(tree,"isPU",&isPU_, "isPU_/I");
    addBranch(tree,"isUndefined",&isUndefined_, "isUndefined_/I");
    addBranch(tree,"genDecay",&genDecay_, "genDecay_/F"); //dxy corresponds to the distance the Bhadron traveled

    // Heavy ion variables
    if (not centToken_.isUninitialized())
      addBranch(tree,"cent",&cent_, "cent_/F");
    
    addBranch(tree,"jet_hflav", &jet_hflav_);
    addBranch(tree,"jet_pflav", &jet_pflav_);
    addBranch(tree,"jet_phflav", &jet_phflav_);

    // jet regression
    addBranch(tree,"jet_genmatch_pt", &jet_genmatch_pt_);
    addBranch(tree,"jet_genmatch_wnu_pt", &jet_genmatch_wnu_pt_);
    addBranch(tree,"&jet_genmatch_lep_vis_pt", &jet_genmatch_lep_vis_pt_);
    addBranch(tree,"jet_mumatch_pt", &jet_mumatch_pt_);
    addBranch(tree,"jet_elematch_pt", &jet_elematch_pt_);
    addBranch(tree,"jet_taumatch_pt", &jet_taumatch_pt_);

    // jet variables
    addBranch(tree,"jet_pt", &jet_pt_);
    addBranch(tree,"jet_corr_pt", &jet_corr_pt_);
    addBranch(tree,"jet_eta", &jet_eta_);
    addBranch(tree,"jet_phi", &jet_phi_);
    addBranch(tree,"jet_mass", &jet_mass_);
    addBranch(tree,"jet_energy", &jet_energy_);

    //jet id
    addBranch(tree,"jet_looseId", &jet_looseId_);
    addBranch(tree,"jet_jetId", &jet_jetId_);
    addBranch(tree,"jet_puId", &jet_puId_); 

    // quark gluon
    addBranch(tree,"jet_qgl",   &jet_qgl_);  // qg tagger from jmar
    addBranch(tree,"QG_ptD",   &QG_ptD_);   // momentum fraction per jet constituent
    addBranch(tree,"QG_axis2", &QG_axis2_); // jet shape i.e. gluon are wider than quarks
    addBranch(tree,"QG_mult",  &QG_mult_);  // multiplicity i.e. total num of PFcands reconstructed

    addBranch(tree,"gen_pt_Recluster"    ,&gen_pt_Recluster_    ,"gen_pt_Recluster_/F"    );
    addBranch(tree,"gen_pt_WithNu"    ,&gen_pt_WithNu_    ,"gen_pt_WithNu_/F"    );
    addBranch(tree,"Delta_gen_pt_Recluster"    ,&Delta_gen_pt_Recluster_    ,"Delta_gen_pt_Recluster_/F"    );
    addBranch(tree,"Delta_gen_pt_WithNu"    ,&Delta_gen_pt_WithNu_    ,"Delta_gen_pt_WithNu_/F"    );

    // Ele/Mu/Tau gen
    addBranch(tree,"gen_number", &gen_number_, "gen_number_/I");
    addBranch(tree,"gend_number", &gend_number_, "gend_number_/I");
    addBranch(tree,"gen_particle_pt", &gen_particle_pt_, "gen_particle_pt_[gen_number_]/F");
    addBranch(tree,"gen_particle_eta", &gen_particle_eta_, "gen_particle_eta_[gen_number_]/F");
    addBranch(tree,"gen_particle_phi", &gen_particle_phi_, "gen_particle_phi_[gen_number_]/F");
    addBranch(tree,"gen_particle_mass", &gen_particle_mass_, "gen_particle_mass_[gen_number_]/F");
    addBranch(tree,"gen_particle_id", &gen_particle_id_, "gen_particle_id_[gen_number_]/F");
    addBranch(tree,"gen_particle_status", &gen_particle_status_, "gen_particle_status_[gen_number_]/F");
    addBranch(tree,"gen_particle_daughters_id", &gen_particle_daughters_id_, "gen_particle_daughters_id_[gend_number_]/F");
    addBranch(tree,"gen_particle_daughters_igen", &gen_particle_daughters_igen_, "gen_particle_daughters_igen_[gend_number_]/F");
    addBranch(tree,"gen_particle_daughters_pt", &gen_particle_daughters_pt_, "gen_particle_daughters_pt_[gend_number_]/F");
    addBranch(tree,"gen_particle_daughters_eta", &gen_particle_daughters_eta_, "gen_particle_daughters_eta_[gend_number_]/F");
    addBranch(tree,"gen_particle_daughters_phi", &gen_particle_daughters_phi_, "gen_particle_daughters_phi_[gend_number_]/F");
    addBranch(tree,"gen_particle_daughters_mass", &gen_particle_daughters_mass_, "gen_particle_daughters_mass_[gend_number_]/F");
    addBranch(tree,"gen_particle_daughters_status", &gen_particle_daughters_status_, "gen_particle_daughters_status_[gend_number_]/F");
    addBranch(tree,"gen_particle_daughters_charge", &gen_particle_daughters_charge_, "gen_particle_daughters_charge_[gend_number_]/F");

    if(1) // discriminators might need to be filled differently. FIXME
        for(auto& entry : discriminators_) {
            string better_name(entry.first);
            std::replace(better_name.begin(), better_name.end(), ':', '_');
            addBranch(tree,better_name.c_str(), &entry.second, (better_name+"/F").c_str());
        }
}

void ntuple_JetInfo::readEvent(const edm::Event& iEvent){

    iEvent.getByToken(qglToken_, qglHandle);
    iEvent.getByToken(ptDToken_, ptDHandle);
    iEvent.getByToken(axis2Token_, axis2Handle);
    iEvent.getByToken(multToken_, multHandle);

    iEvent.getByToken(genJetMatchReclusterToken_, genJetMatchRecluster);
    iEvent.getByToken(genJetMatchWithNuToken_, genJetMatchWithNu);
    iEvent.getByToken(genJetMatchAllowDuplicatesToken_, genJetMatchAllowDuplicates);

    iEvent.getByToken(genParticlesToken_, genParticlesHandle);
    iEvent.getByToken(genJetsWnuToken_, genJetsWnuH);
    iEvent.getByToken(genJetsToken_, genJetsH);

    iEvent.getByToken(muonsToken_, muonsHandle);
    iEvent.getByToken(electronsToken_, electronsHandle);
    iEvent.getByToken(tausToken_,tausH);

    event_no_=iEvent.id().event();

    if (not centToken_.isUninitialized())
      cent_ = iEvent.get(centToken_) / 2.0;

    //presumably this whole part can be removed!

    neutrinosLepB.clear();
    neutrinosLepB_C.clear();
    gToBB.clear();
    gToCC.clear();
    alltaus_.clear();
    Bhadron_.clear();
    Bhadron_daughter_.clear();

    gen_particle_pt.clear();
    gen_particle_eta.clear();
    gen_particle_phi.clear();
    gen_particle_mass.clear();
    gen_particle_id.clear();
    gen_particle_status.clear();
    gen_particle_daughters_id.clear();
    gen_particle_daughters_igen.clear();
    gen_particle_daughters_status.clear();
    gen_particle_daughters_pt.clear();
    gen_particle_daughters_eta.clear();
    gen_particle_daughters_phi.clear();
    gen_particle_daughters_mass.clear();
    gen_particle_daughters_charge.clear();
    genLepFromResonance4V_.clear();
    genMuonsFromResonance4V_.clear();
    genElectronsFromResonance4V_.clear();
    tau_gen_visible_.clear();
    tau_gen_.clear();

    jetv_gen_wnu.clear();
    jetv_gen.clear();

    PatRefPtSorter<reco::GenJetRef> genJetRefSorter;

    // Standard gen-jets excluding the neutrinos
    if(genJetsH.isValid()){
      for (auto jets_iter = genJetsH->begin(); jets_iter != genJetsH->end(); ++jets_iter) {                                                                                                   
	reco::GenJetRef jref (genJetsH,jets_iter-genJetsH->begin());                                                                                                                      
	jetv_gen.push_back(jref);                                                                                                                                                              
      }
      sort(jetv_gen.begin(), jetv_gen.end(), genJetRefSorter);
    }
    
    // GEN jets with neutrinos
    if(genJetsWnuH.isValid()){
      for (auto jets_iter = genJetsWnuH->begin(); jets_iter != genJetsWnuH->end(); ++jets_iter) {                                                                                           
	reco::GenJetRef jref  (genJetsWnuH, jets_iter-genJetsWnuH->begin());                                                                                                                 
	jetv_gen_wnu.push_back(jref);                                                                                                                                                              
      }
      sort(jetv_gen_wnu.begin(), jetv_gen_wnu.end(), genJetRefSorter);
      }

 for (const reco::Candidate &genC : *genParticlesHandle)
   {
     const reco::GenParticle &gen = static_cast< const reco::GenParticle &>(genC);
     
     if((abs(gen.pdgId())>500&&abs(gen.pdgId())<600)||(abs(gen.pdgId())>5000&&abs(gen.pdgId())<6000)) {

       Bhadron_.push_back(gen);
       if(gen.numberOfDaughters()>0){
     
	 if( (abs(gen.daughter(0)->pdgId())>500&&abs(gen.daughter(0)->pdgId())<600)||(abs(gen.daughter(0)->pdgId())>5000&&abs(gen.daughter(0)->pdgId())<6000))
	   {
	     if(gen.daughter(0)->numberOfDaughters()>0)
	       {
		
		 const reco::GenParticle &daughter_ = static_cast< const reco::GenParticle &>(*(gen.daughter(0)->daughter(0)));
		 
		 if(daughter_.vx()!=gen.vx())
		   { 
		     Bhadron_daughter_.push_back(daughter_);
		   }
                 else Bhadron_daughter_.push_back(gen);
	       }
	     else  Bhadron_daughter_.push_back(gen);
	     
	   }
	 else{
	   const reco::GenParticle &daughter_ = static_cast< const reco::GenParticle &>(*gen.daughter(0));
	   Bhadron_daughter_.push_back(daughter_);
	 }

       }// if daughter is there
       else {
	 Bhadron_daughter_.push_back(gen);
       }
     }
   }

 for (const reco::Candidate &genC : *genParticlesHandle) {
        const reco::GenParticle &gen = static_cast< const reco::GenParticle &>(genC);
        if(abs(gen.pdgId())==12||abs(gen.pdgId())==14||abs(gen.pdgId())==16) {
            const reco::GenParticle* mother =  static_cast< const reco::GenParticle*> (gen.mother());
            if(mother!=NULL) {
                if((abs(mother->pdgId())>500&&abs(mother->pdgId())<600)||(abs(mother->pdgId())>5000&&abs(mother->pdgId())<6000)) {
                    neutrinosLepB.emplace_back(gen);
                }
                if((abs(mother->pdgId())>400&&abs(mother->pdgId())<500)||(abs(mother->pdgId())>4000&&abs(mother->pdgId())<5000)) {
                    neutrinosLepB_C.emplace_back(gen);
                }
            }
            else {
                std::cout << "No mother" << std::endl;
            }
        }

        int id(std::abs(gen.pdgId())); 
        int status(gen.status());

        if (id == 21 && status >= 21 && status <= 59) { //// Pythia8 hard scatter, ISR, or FSR
            if ( gen.numberOfDaughters() == 2 ) {
                const reco::Candidate* d0 = gen.daughter(0);
                const reco::Candidate* d1 = gen.daughter(1);
                if ( std::abs(d0->pdgId()) == 5 && std::abs(d1->pdgId()) == 5
                        && d0->pdgId()*d1->pdgId() < 0 && reco::deltaR(*d0, *d1) < 0.4) gToBB.push_back(gen) ;
                if ( std::abs(d0->pdgId()) == 4 && std::abs(d1->pdgId()) == 4
                        && d0->pdgId()*d1->pdgId() < 0 && reco::deltaR(*d0, *d1) < 0.4) gToCC.push_back(gen) ;
            }
        }

        if(id == 15 && false){
            alltaus_.push_back(gen);
        }
 }
 // GEN particle information
 if(genParticlesHandle.isValid()){
   unsigned int igen = 0;
   for (auto gens_iter = genParticlesHandle->begin(); gens_iter != genParticlesHandle->end(); ++gens_iter) {      
     if((abs(gens_iter->pdgId()) == 25 or abs(gens_iter->pdgId()) == 24 or abs(gens_iter->pdgId()) == 23) and
	gens_iter->isLastCopy() and gens_iter->statusFlags().fromHardProcess()){ 

       gen_particle_pt.push_back(gens_iter->pt());
       gen_particle_eta.push_back(gens_iter->eta());
       gen_particle_phi.push_back(gens_iter->phi());
       gen_particle_mass.push_back(gens_iter->mass());
       gen_particle_id.push_back(gens_iter->pdgId());
       gen_particle_status.push_back(gens_iter->status());

       for(size_t idau = 0; idau < gens_iter->numberOfDaughters(); idau++){
	 gen_particle_daughters_id.push_back(gens_iter->daughter(idau)->pdgId());
	 gen_particle_daughters_igen.push_back(igen);
	 gen_particle_daughters_pt.push_back(gens_iter->daughter(idau)->pt());
	 gen_particle_daughters_eta.push_back(gens_iter->daughter(idau)->eta());
	 gen_particle_daughters_phi.push_back(gens_iter->daughter(idau)->phi());
	 gen_particle_daughters_mass.push_back(gens_iter->daughter(idau)->mass());
	 gen_particle_daughters_status.push_back(gens_iter->daughter(idau)->status());
	 gen_particle_daughters_charge.push_back(gens_iter->daughter(idau)->charge());
       }
       igen++;
     }

     // Final states Leptons (e,mu) and Neutrinos --> exclude taus. They need to be prompt or from Tau decay      
     if (abs(gens_iter->pdgId()) > 10 and abs(gens_iter->pdgId()) < 17 and abs(gens_iter->pdgId()) != 15  and 
	 (gens_iter->isPromptFinalState() or gens_iter->isDirectPromptTauDecayProductFinalState())) { 

       gen_particle_pt.push_back(gens_iter->pt());
       gen_particle_eta.push_back(gens_iter->eta());
       gen_particle_phi.push_back(gens_iter->phi());
       gen_particle_mass.push_back(gens_iter->mass());
       gen_particle_id.push_back(gens_iter->pdgId());
       gen_particle_status.push_back(gens_iter->status());

       // No need to save daughters here
       igen++;
     }

     // Final state quarks or gluons from the hard process before the shower --> partons in which H/Z/W/top decay into
     if (((abs(gens_iter->pdgId()) >= 1 and abs(gens_iter->pdgId()) <= 5) or abs(gens_iter->pdgId()) == 21) and 
	 gens_iter->statusFlags().fromHardProcess() and gens_iter->statusFlags().isFirstCopy()){
       gen_particle_pt.push_back(gens_iter->pt());
       gen_particle_eta.push_back(gens_iter->eta());
       gen_particle_phi.push_back(gens_iter->phi());
       gen_particle_mass.push_back(gens_iter->mass());
       gen_particle_id.push_back(gens_iter->pdgId());
       gen_particle_status.push_back(gens_iter->status());
       igen++;
       // no need to save daughters
     }

     // Special case of taus: last-copy, from hard process and, prompt and decayed
     if(abs(gens_iter->pdgId()) == 15 and gens_iter->isLastCopy() and
	gens_iter->statusFlags().fromHardProcess() and gens_iter->isPromptDecayed()){ 
            
       // hadronic taus
       gen_particle_pt.push_back(gens_iter->pt());
       gen_particle_eta.push_back(gens_iter->eta());
       gen_particle_phi.push_back(gens_iter->phi());
       gen_particle_mass.push_back(gens_iter->mass());
       gen_particle_id.push_back(gens_iter->pdgId());
       gen_particle_status.push_back(gens_iter->status());

       // only store the final decay particles
       for(size_t idau = 0; idau < gens_iter->numberOfDaughters(); idau++){
	 if(not dynamic_cast<const reco::GenParticle*>(gens_iter->daughter(idau))->statusFlags().isPromptTauDecayProduct()) continue;
	 gen_particle_daughters_id.push_back(gens_iter->daughter(idau)->pdgId());
	 gen_particle_daughters_igen.push_back(igen);
	 gen_particle_daughters_pt.push_back(gens_iter->daughter(idau)->pt());
	 gen_particle_daughters_eta.push_back(gens_iter->daughter(idau)->eta());
	 gen_particle_daughters_phi.push_back(gens_iter->daughter(idau)->phi());
	 gen_particle_daughters_mass.push_back(gens_iter->daughter(idau)->mass());    
	 gen_particle_daughters_status.push_back(gens_iter->daughter(idau)->status());
	 gen_particle_daughters_charge.push_back(gens_iter->daughter(idau)->charge());
       }
       igen++;
     }  
   }
 }

}

//use either of these functions

bool ntuple_JetInfo::fillBranches(const pat::Jet & unsubjet, const pat::Jet & jet, const size_t& jetidx, const edm::View<pat::Jet> * coll){
    if(!coll)
        throw std::runtime_error("ntuple_JetInfo::fillBranches: no jet collection");

    isMC_    = -1;
    isemu_   =  0;
    isdimu_  =  0;
    ismutau_ =  0;
    /// thresholds for matching
    static float dRCone        = 0.2;
    static float dRMatchingPF  = 0.1;
    static float ptGenLeptonMin = 8;
    static float ptGenTauVisibleMin = 15;

    // Gen leptons from resonance decay 
    std::vector<TLorentzVector> genLepFromResonance4V;
    std::vector<TLorentzVector> genMuonsFromResonance4V;
    std::vector<TLorentzVector> genElectronsFromResonance4V;
    std::vector<int> genMuonsFromResonanceCharge;
    std::vector<int> genElectronsFromResonanceCharge;
    std::vector<TLorentzVector> tau_gen_visible;
    std::vector<TLorentzVector> tau_gen;
    std::vector<int> tau_gen_charge;
    std::vector<unsigned int> tau_gen_nch;
    std::vector<unsigned int> tau_gen_np0;
    std::vector<unsigned int> tau_gen_nnh;
    
    for(size_t igen = 0; igen < gen_particle_pt.size(); igen++){
      // select resonances like Higgs, W, Z, taus
      if(abs(gen_particle_id.at(igen)) == 25 or
	 abs(gen_particle_id.at(igen)) == 23 or
	 abs(gen_particle_id.at(igen)) == 24 or
	 abs(gen_particle_id.at(igen)) == 15){
	for(size_t idau = 0; idau < gen_particle_daughters_id.size(); idau++){
	  // select electrons or muons from the resonance / tau decay
	  if(gen_particle_daughters_igen.at(idau) == igen and
	     (abs(gen_particle_daughters_id.at(idau)) == 11 or
	      abs(gen_particle_daughters_id.at(idau)) == 13)){
	    TLorentzVector gen4V;
	    gen4V.SetPtEtaPhiM(gen_particle_daughters_pt.at(idau),gen_particle_daughters_eta.at(idau),gen_particle_daughters_phi.at(idau),gen_particle_daughters_mass.at(idau));
	    if(std::find(genLepFromResonance4V.begin(),genLepFromResonance4V.end(),gen4V) == genLepFromResonance4V.end())
	      genLepFromResonance4V.push_back(gen4V);
	    if(abs(gen_particle_daughters_id.at(idau)) == 13 and 
	       std::find(genMuonsFromResonance4V.begin(),genMuonsFromResonance4V.end(),gen4V) == genMuonsFromResonance4V.end()){
	      genMuonsFromResonance4V.push_back(gen4V);
	      genMuonsFromResonanceCharge.push_back(gen_particle_daughters_charge.at(idau));
	    }
	    if(abs(gen_particle_daughters_id.at(idau)) == 11 and 
	       std::find(genElectronsFromResonance4V.begin(),genElectronsFromResonance4V.end(),gen4V) == genElectronsFromResonance4V.end()){
	      genElectronsFromResonance4V.push_back(gen4V);
	      genElectronsFromResonanceCharge.push_back(gen_particle_daughters_charge.at(idau));
	    }
	  }
	}
      }
    }

    // Gen hadronic taus
    for(size_t igen = 0; igen < gen_particle_pt.size(); igen++){
      if(abs(gen_particle_id.at(igen)) == 15){ // hadronic or leptonic tau
	TLorentzVector tau_gen_tmp;
	unsigned int tau_gen_nch_tmp = 0;
	unsigned int tau_gen_np0_tmp = 0;
	unsigned int tau_gen_nnh_tmp = 0;
	for(size_t idau = 0; idau < gen_particle_daughters_pt.size(); idau++){
	  if(gen_particle_daughters_igen.at(idau) == igen and
	     abs(gen_particle_daughters_id.at(idau)) != 11 and // no mu
	     abs(gen_particle_daughters_id.at(idau)) != 13 and // no el
	     abs(gen_particle_daughters_id.at(idau)) != 12 and // no neutrinos
	     abs(gen_particle_daughters_id.at(idau)) != 14 and
	     abs(gen_particle_daughters_id.at(idau)) != 16){
	    TLorentzVector tmp4V; 
	    tmp4V.SetPtEtaPhiM(gen_particle_daughters_pt.at(idau),gen_particle_daughters_eta.at(idau),gen_particle_daughters_phi.at(idau),gen_particle_daughters_mass.at(idau));
	    tau_gen_tmp += tmp4V;
	    if (gen_particle_daughters_charge.at(idau) != 0 and gen_particle_daughters_status.at(idau) == 1) tau_gen_nch_tmp ++; // charged particles
	    else if(gen_particle_daughters_charge.at(idau) == 0 and gen_particle_daughters_id.at(idau) == 111) tau_gen_np0_tmp++;
	    else if(gen_particle_daughters_charge.at(idau) == 0 and gen_particle_daughters_id.at(idau) != 111) tau_gen_nnh_tmp++;
	  }
	}
	if(tau_gen_tmp.Pt() > 0){ // good hadronic tau
	  tau_gen_visible.push_back(tau_gen_tmp);
	  tau_gen_tmp.SetPtEtaPhiM(gen_particle_pt.at(igen),gen_particle_eta.at(igen),gen_particle_phi.at(igen),gen_particle_mass.at(igen));
	  tau_gen_charge.push_back((gen_particle_id.at(igen) > 0) ? -1 : 1);
	  tau_gen.push_back(tau_gen_tmp);
	  tau_gen_nch.push_back(tau_gen_nch_tmp);
	  tau_gen_np0.push_back(tau_gen_np0_tmp);
	  tau_gen_nnh.push_back(tau_gen_nnh_tmp);
	}
      }
    }

    // matching with gen-leptons (muons/electrons/hadronic taus)
    float minDR = 1000;
    int nlep_in_cone  = 0;
    int pos_matched_genmu = -1;
    int pos_matched_genele = -1;
    int pos_matched_tauh = -1;
    int gentau_decaymode = -1;   
    TLorentzVector genLepton4V;
    TLorentzVector genLeptonVis4V;

    TLorentzVector jet4V;
    jet4V.SetPtEtaPhiM(jet.pt(),jet.eta(),jet.phi(),jet.mass());

    for(size_t igen = 0; igen < genMuonsFromResonance4V.size(); igen++){
      float dR = jet4V.DeltaR(genMuonsFromResonance4V.at(igen));      
      if(dR < dRCone) nlep_in_cone++;
      if(dR < dRCone and dR < minDR and genMuonsFromResonance4V.at(igen).Pt() >= ptGenLeptonMin){
	pos_matched_genmu = igen;
	minDR = dR;
	genLepton4V = genMuonsFromResonance4V.at(igen);
	genLeptonVis4V = genMuonsFromResonance4V.at(igen);
      }
    }
    
    for(size_t igen = 0; igen < genElectronsFromResonance4V.size(); igen++){
      float dR = jet4V.DeltaR(genElectronsFromResonance4V.at(igen));      
      if(dR < dRCone) nlep_in_cone++;
      if(dR < dRCone and dR < minDR and genElectronsFromResonance4V.at(igen).Pt() >= ptGenLeptonMin){
	pos_matched_genmu  = -1;
	pos_matched_genele = igen;
	minDR = dR;
	genLepton4V = genElectronsFromResonance4V.at(igen);
	genLeptonVis4V = genElectronsFromResonance4V.at(igen);
      }
    }
    
    for(size_t itau = 0; itau < tau_gen_visible.size(); itau++){      
      float dR = tau_gen_visible.at(itau).DeltaR(jet4V); 
      if(dR < dRCone) nlep_in_cone++;
      if(dR < dRCone and dR < minDR and tau_gen_visible.at(itau).Pt() >= ptGenTauVisibleMin){
	pos_matched_genmu  = -1;
	pos_matched_genele = -1;
	pos_matched_tauh = itau;
	minDR = dR;
	gentau_decaymode = 5*(tau_gen_nch.at(itau)-1)+tau_gen_np0.at(itau);
	genLepton4V = tau_gen.at(itau);
	genLeptonVis4V = tau_gen_visible.at(itau);
      }
    }
    
    jet_genmatch_lep_vis_pt_ = genLeptonVis4V.Pt();

    PatPtSorter<pat::Tau>      tauSorter;  
    pat::TauCollection tausColl = *tausH;
    
    // Sorting taus based on pT
    sort(tausColl.begin(),tausColl.end(),tauSorter);
    std::vector<math::XYZTLorentzVector> tau_pfcandidates;  // in order to match PF and HPS candidates

    for (size_t i = 0; i < tausColl.size(); i++) {
      for(unsigned ipart = 0; ipart < tausColl[i].signalCands().size(); ipart++){
	const pat::PackedCandidate* pfcand = dynamic_cast<const pat::PackedCandidate*> (tausColl[i].signalCands()[ipart].get());      
	tau_pfcandidates.push_back(pfcand->p4());
      }
    }
    
    TLorentzVector tau4V_fromPF;
    std::vector<TLorentzVector> muon4V_fromPF;
    std::vector<TLorentzVector> electron4V_fromPF;

    for (unsigned int i = 0; i <  unsubjet.numberOfDaughters(); i++){
      const pat::PackedCandidate* PackedCandidate_ = dynamic_cast<const pat::PackedCandidate*>(unsubjet.daughter(i));
      if(!PackedCandidate_) continue;
      if(PackedCandidate_->pt() < min_candidate_pt_) continue; 
      
      if (abs(PackedCandidate_->pdgId()) == 11){
	TLorentzVector tmp4V;
	tmp4V.SetPtEtaPhiM(PackedCandidate_->pt(),PackedCandidate_->eta(),PackedCandidate_->phi(),PackedCandidate_->mass());
	electron4V_fromPF.push_back(tmp4V);
      }
      if (abs(PackedCandidate_->pdgId()) == 13){
	TLorentzVector tmp4V;
	tmp4V.SetPtEtaPhiM(PackedCandidate_->pt(),PackedCandidate_->eta(),PackedCandidate_->phi(),PackedCandidate_->mass());
	muon4V_fromPF.push_back(tmp4V);
      }
      if(std::find(tau_pfcandidates.begin(),tau_pfcandidates.end(),PackedCandidate_->p4()) != tau_pfcandidates.end()){
	TLorentzVector tmp4V;
	tmp4V.SetPtEtaPhiM(PackedCandidate_->pt(),PackedCandidate_->eta(),PackedCandidate_->phi(),PackedCandidate_->mass());
	tau4V_fromPF += tmp4V;
      }
    }
    
    // Matching with muon by considering only the highest pt one
    std::sort(muon4V_fromPF.begin(),muon4V_fromPF.end(),
	      [&](const TLorentzVector & a, const TLorentzVector & b){
		return a.Pt() > b.Pt();
	      });
    
    minDR = 1000;
    int pos_matched_reco_mu = -1;
    if(not muon4V_fromPF.empty()){
      for(size_t imu = 0; imu < muon4V_fromPF.size(); imu++){
	TLorentzVector tmp4V;
	tmp4V = muon4V_fromPF.at(imu);
	float dR = tmp4V.DeltaR(jet4V);
	float dRm = muon4V_fromPF.front().DeltaR(tmp4V);
	if(dR < dRCone and dRm < dRMatchingPF and dRm < minDR){
	  pos_matched_reco_mu = imu;
	  minDR = dRm;	      
	}
      }
    }
    if(pos_matched_reco_mu >= 0){
      jet_mumatch_pt_ = muon4V_fromPF.at(pos_matched_reco_mu).Pt();
    }
    else if(not muon4V_fromPF.empty()){
      jet_mumatch_pt_ = muon4V_fromPF.front().Pt();
    }
    else{
      jet_mumatch_pt_ = -1;
    }

    std::sort(electron4V_fromPF.begin(),electron4V_fromPF.end(),
	      [&](const TLorentzVector & a, const TLorentzVector & b){
		return a.Pt() > b.Pt();
	      });
    
    minDR = 1000;
    int pos_matched_reco_ele = -1;
    if(not electron4V_fromPF.empty()){
      for(size_t imu = 0; imu < electron4V_fromPF.size(); imu++){
	TLorentzVector tmp4V;
	tmp4V = electron4V_fromPF.at(imu);
	float dR = tmp4V.DeltaR(jet4V);
	float dRm = electron4V_fromPF.front().DeltaR(tmp4V);
	if(dR < dRCone and dRm < dRMatchingPF and dRm < minDR){
	  pos_matched_reco_ele = imu;
	  minDR = dRm;	      
	}
      }
    }
    if(pos_matched_reco_ele >= 0){
      jet_elematch_pt_ = electron4V_fromPF.at(pos_matched_reco_ele).Pt();
    }
    else if(not electron4V_fromPF.empty()){
      jet_elematch_pt_ = electron4V_fromPF.front().Pt();
    }
    else{
      jet_elematch_pt_ = -1;
    }

    // Matching with reco tau
    minDR = 1000;
    int pos_matched_reco_tauh = -1;
    for (size_t itau = 0; itau < tausColl.size(); itau++) {
      TLorentzVector tmp4V;
      tmp4V.SetPtEtaPhiM(tausColl[itau].pt(),tausColl[itau].eta(),tausColl[itau].phi(),tausColl[itau].mass());
      float dR = tmp4V.DeltaR(jet4V);
      float dRt = tmp4V.DeltaR(tau4V_fromPF);	      
      if (dR < dRCone and dRt < dRMatchingPF and dRt < minDR){ // small numerical roundoff due to float to double approximation
	pos_matched_reco_tauh = itau;
	minDR = dR;
      }
    }
    if(pos_matched_reco_tauh >= 0){
      jet_taumatch_pt_ = tausColl[pos_matched_reco_tauh].pt();
    }
    else{
      jet_taumatch_pt_ = -1;
    }

    /// cuts ///
    bool returnval=true;
    
    // some cuts to contrin training region
    if ( jet.pt() < jetPtMin_ ||  jet.pt() > jetPtMax_ ) returnval=false;                  // apply jet pT cut
    if ( fabs(jet.eta()) < jetAbsEtaMin_ || fabs(jet.eta()) > jetAbsEtaMax_ ) returnval=false; // apply jet eta cut


    // often we have way to many gluons that we do not need. This randomply reduces the gluons
    if (gluonReduction_>0 && unsubjet.partonFlavour()==21)
        if(TRandom_.Uniform()>gluonReduction_) returnval=false;

    //branch fills
    for(auto& entry : discriminators_) {
        entry.second = catchInfs(jet.bDiscriminator(entry.first),-0.1);
    }

    npv_ = vertices()->size();

    for (auto const& v : *pupInfo()) {
        int bx = v.getBunchCrossing();
        if (bx == 0) {
            ntrueInt_ = v.getTrueNumInteractions();
        }
    }
    rho_ = rhoInfo()[0];

    jet_no_=jetidx;

    const auto jetRef = reco::CandidatePtr(coll->ptrs().at( jetidx));

    jet_qgl_ = (*qglHandle)[jetRef];
    QG_ptD_ = (*ptDHandle)[jetRef];
    QG_axis2_ = (*axis2Handle)[jetRef];
    QG_mult_ = (*multHandle)[jetRef];

    isB_=0; isGBB_=0; isBB_=0; isC_=0; isGCC_=0; isCC_=0; isU_=0; isD_=0; isMU_=0; isELE_=0;
    isS_=0; isG_=0, isPU_=0, isLeptonicB_=0, isLeptonicB_C_=0, isUndefined_=0;
    isTaup1h0p_=0, isTaup1h1p_=0, isTaup1h2p_=0, isTaup3h0p_=0, isTaup3h1p_=0; 
    isTaum1h0p_=0, isTaum1h1p_=0, isTaum1h2p_=0, isTaum3h0p_=0, isTaum3h1p_=0; 
    auto muIds = deep_ntuples::jet_muonsIds(jet,*muonsHandle);
    auto elecIds = deep_ntuples::jet_electronsIds(jet,*electronsHandle);

    muons_number_ = muIds.size();
    electrons_number_ = elecIds.size();

    float etasign = 1.;
    if (jet.eta()<0) etasign = -1.;

    for(std::size_t i=0; i<max_num_lept; i++) {
        if (i < muIds.size()) {
            const auto & muon = (*muonsHandle).at(muIds.at(i));
            muons_isLooseMuon_[i] = muon.isLooseMuon();
            muons_isTightMuon_[i] = muon.isTightMuon(vertices()->at(0));
            muons_isSoftMuon_[i] = muon.isSoftMuon(vertices()->at(0));
            muons_isHighPtMuon_[i] = muon.isHighPtMuon(vertices()->at(0));
            muons_pt_[i] = muon.pt();
            muons_relEta_[i] = etasign*(muon.eta()-jet.eta());
            muons_relPhi_[i] = reco::deltaPhi(muon.phi(),jet.phi());
            muons_energy_[i] = muon.energy()/jet.energy();
        }
        if (i < elecIds.size()) {
            const auto & electron = (*electronsHandle).at(elecIds.at(i));
            electrons_pt_[i] = electron.pt();
            electrons_relEta_[i] = etasign*(electron.eta()-jet.eta());
            electrons_relPhi_[i] = reco::deltaPhi(electron.phi(),jet.phi());
            electrons_energy_[i] = electron.energy()/jet.energy();
        }
    }
    gen_number_ = std::min(gen_particle_pt.size(),max_num_gen_);
    gend_number_ = std::min(gen_particle_daughters_pt.size(),max_num_gen_);
    for(size_t i=0; i< (size_t)gen_number_; i++) {
      gen_particle_pt_[i] = gen_particle_pt.at(i);
      gen_particle_eta_[i] = gen_particle_eta.at(i);
      gen_particle_phi_[i] = gen_particle_phi.at(i);
      gen_particle_mass_[i] = gen_particle_mass.at(i);
      gen_particle_id_[i] = gen_particle_id.at(i);
      gen_particle_status_[i] = gen_particle_status.at(i);
    }
    for(size_t i=0; i< (size_t)gend_number_; i++) {
      gen_particle_daughters_id_[i] = gen_particle_daughters_id.at(i);
      gen_particle_daughters_igen_[i] = gen_particle_daughters_igen.at(i);
      gen_particle_daughters_pt_[i] = gen_particle_daughters_pt.at(i);
      gen_particle_daughters_eta_[i] = gen_particle_daughters_eta.at(i);
      gen_particle_daughters_phi_[i] = gen_particle_daughters_phi.at(i);
      gen_particle_daughters_mass_[i] = gen_particle_daughters_mass.at(i);
      gen_particle_daughters_status_[i] = gen_particle_daughters_status.at(i);
      gen_particle_daughters_charge_[i] = gen_particle_daughters_charge.at(i);
    }

    //// Note that jets with gluon->bb (cc) and x->bb (cc) are in the same categories
    if(true){
      switch(deep_ntuples::jet_flavour(jet, unsubjet, gToBB, gToCC, neutrinosLepB, neutrinosLepB_C, alltaus_, pos_matched_genmu, pos_matched_genele, pos_matched_tauh, gentau_decaymode, tau_gen_charge)) {
        case deep_ntuples::JetFlavor::MU:  isMU_=1; break;
        case deep_ntuples::JetFlavor::ELE:  isELE_=1; break;
        case deep_ntuples::JetFlavor::TAUP1H0P:  isTaup1h0p_=1; break;
        case deep_ntuples::JetFlavor::TAUP1H1P:  isTaup1h1p_=1; break;
        case deep_ntuples::JetFlavor::TAUP1H2P:  isTaup1h2p_=1; break;
        case deep_ntuples::JetFlavor::TAUP3H0P:  isTaup3h0p_=1; break;
        case deep_ntuples::JetFlavor::TAUP3H1P:  isTaup3h1p_=1; break;
        case deep_ntuples::JetFlavor::TAUM1H0P:  isTaum1h0p_=1; break;
        case deep_ntuples::JetFlavor::TAUM1H1P:  isTaum1h1p_=1; break;
        case deep_ntuples::JetFlavor::TAUM1H2P:  isTaum1h2p_=1; break;
        case deep_ntuples::JetFlavor::TAUM3H0P:  isTaum3h0p_=1; break;
        case deep_ntuples::JetFlavor::TAUM3H1P:  isTaum3h1p_=1; break;
        case deep_ntuples::JetFlavor::B:  isB_=1; break;
        case deep_ntuples::JetFlavor::LeptonicB: isLeptonicB_=1; break;
        case deep_ntuples::JetFlavor::LeptonicB_C: isLeptonicB_C_=1; break;
        case deep_ntuples::JetFlavor::GBB: isGBB_=1; break;
        case deep_ntuples::JetFlavor::BB: isBB_=1; break;
        case deep_ntuples::JetFlavor::C:  isC_=1; break;
        case deep_ntuples::JetFlavor::GCC: isGCC_=1; break;
        case deep_ntuples::JetFlavor::CC: isCC_=1; break;
        case deep_ntuples::JetFlavor::G:  isG_=1; break;
        case deep_ntuples::JetFlavor::PU:  isPU_=1; break;
        case deep_ntuples::JetFlavor::S:  isS_=1; break;
        case deep_ntuples::JetFlavor::U: isU_=1; break;
        case deep_ntuples::JetFlavor::D: isD_=1; break;
        default : isUndefined_=1; break;
        }
    }

    //truth labeling with fallback to physics definition for light/gluon/undefined of standard flavor definition
    //// Note that jets with gluon->bb (cc) and x->bb (cc) are in the same categories
    isPhysB_=0; isPhysBB_=0; isPhysGBB_=0; isPhysC_=0; isPhysCC_=0;
    isPhysGCC_=0; isPhysD_=0; isPhysU_=0; isPhysS_=0; isPhysG_=0, isPhysLeptonicB_=0, isPhysLeptonicB_C_=0, isPhysUndefined_=0;
    isPhysTau_=0, isPhysPU_=0;
    if(true){
      switch(deep_ntuples::jet_flavour(jet, unsubjet, gToBB, gToCC, neutrinosLepB, neutrinosLepB_C, alltaus_, pos_matched_genmu, pos_matched_genele, pos_matched_tauh, gentau_decaymode, tau_gen_charge, true)) {
        case deep_ntuples::JetFlavor::S:  isPhysS_=1; break;
        case deep_ntuples::JetFlavor::U: isPhysU_=1; break;
        case deep_ntuples::JetFlavor::D: isPhysD_=1; break;
        case deep_ntuples::JetFlavor::B:  isPhysB_=1; break;
        case deep_ntuples::JetFlavor::BB: isPhysBB_=1; break;
        case deep_ntuples::JetFlavor::GBB: isPhysGBB_=1; break;
        case deep_ntuples::JetFlavor::C:  isPhysC_=1; break;
        case deep_ntuples::JetFlavor::CC: isPhysCC_=1; break;
        case deep_ntuples::JetFlavor::GCC: isPhysGCC_=1; break;
        case deep_ntuples::JetFlavor::TAU: isPhysTau_=1;break;
        case deep_ntuples::JetFlavor::G:  isPhysG_=1; break;
        case deep_ntuples::JetFlavor::LeptonicB: isPhysLeptonicB_=1; break;
        case deep_ntuples::JetFlavor::LeptonicB_C: isPhysLeptonicB_C_=1; break;
	case deep_ntuples::JetFlavor::PU: isPhysPU_=1; break;
        default : isPhysUndefined_=1; break;
        }
    }

    if(isUndefined_) returnval=false;
    pat::JetCollection h;

    jet_pt_ = jet.correctedJet("Uncorrected").pt();
    jet_eta_ = jet.eta();
    jet_phi_ = jet.phi();
    jet_corr_pt_ = jet.pt();
    jet_mass_ = jet.mass();
    jet_energy_ = jet.energy();

    // Matching with gen-jets                                                                                                                                                                               
    int genjet_pos_matched = -1;
    float gen_minDR = 0.4;
    for(size_t igen = 0; igen < jetv_gen.size(); igen++){
      if(reco::deltaR(jetv_gen[igen]->p4(),jet.p4()) < gen_minDR){
        genjet_pos_matched = igen;
        gen_minDR = reco::deltaR(jetv_gen[igen]->p4(),jet.p4());
      }
    }

    int genjet_wnu_pos_matched = -1;
    float gen_minDR_wnu = 0.4;
    for(size_t igen = 0; igen < jetv_gen_wnu.size(); igen++){
      if(reco::deltaR(jetv_gen_wnu[igen]->p4(),jet.p4()) < gen_minDR_wnu){
        genjet_wnu_pos_matched = igen;
        gen_minDR_wnu = reco::deltaR(jetv_gen_wnu[igen]->p4(),jet.p4());
      }
    }

    jet_genmatch_pt_ = -1.0;
    jet_genmatch_wnu_pt_ = -1.0;
    
    if(genjet_pos_matched >= 0){
      jet_genmatch_pt_ = jetv_gen[genjet_pos_matched]->pt();
    }

    if(genjet_wnu_pos_matched >= 0){
      jet_genmatch_wnu_pt_ = jetv_gen_wnu[genjet_wnu_pos_matched]->pt();
    }

    genDecay_ = -1.;
    
    try {
        reco::GenParticleRefVector Bhadrons_in_jet = unsubjet.jetFlavourInfo().getbHadrons();

        if (Bhadrons_in_jet.size() > 0){ 

            for (unsigned int idx=0; idx<Bhadron_.size(); ++idx){

                reco::GenParticle bhad = Bhadron_[idx];

                bool bhad_is_in_jet = false;

                for (reco::GenParticleRefVector::const_iterator bhad_in_jet = Bhadrons_in_jet.begin(); bhad_in_jet!=Bhadrons_in_jet.end(); ++bhad_in_jet) {

                    //check if bhad is identical to bhad_in_jet
                    if ( (*bhad_in_jet)->pt() == bhad.pt() && (*bhad_in_jet)->eta() == bhad.eta()
                            && (*bhad_in_jet)->phi() == bhad.phi() && (*bhad_in_jet)->pdgId() == bhad.pdgId())              
                        bhad_is_in_jet = true;
                }
                if (bhad_is_in_jet){

                    if (Bhadron_daughter_[idx].vx()!=bhad.vx()){

                        float vx = Bhadron_daughter_[idx].vx() - bhad.vx();
                        float vy = Bhadron_daughter_[idx].vy() - bhad.vy();

                        float dxy = sqrt(vx*vx+vy*vy);
                        if (dxy > genDecay_)
                            genDecay_= dxy;
                    }
                    else if (genDecay_ < 0) 
                        genDecay_ = -0.1;
                }
            }
        }
    }
    catch (const cms::Exception &e){
        genDecay_ = -1.;
    }

    try{
        float NHF  = jet.neutralHadronEnergyFraction();
        float NEMF = jet.neutralEmEnergyFraction();
        float CHF  = jet.chargedHadronEnergyFraction();
        float MUF  = jet.muonEnergyFraction();
        float CEMF = jet.chargedEmEnergyFraction();
        float NumConst = jet.chargedMultiplicity()+jet.neutralMultiplicity();
        float NumNeutralParticles =jet.neutralMultiplicity();
        float CHM      = jet.chargedMultiplicity();

        jet_looseId_ = ((NHF<0.99 && NEMF<0.99 && NumConst>1) && ((abs(jet_eta_)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || abs(jet_eta_)>2.4) && abs(jet_eta_)<=2.7) ||
                (NHF<0.98 && NEMF>0.01 && NumNeutralParticles>2 && abs(jet_eta_)>2.7 && abs(jet_eta_)<=3.0 ) ||
                (NEMF<0.90 && NumNeutralParticles>10 && abs(jet_eta_)>3.0 );
        //I do it only for eta that I use
        bool tightJetID = (NHF<0.90 && NEMF<0.90 && NumConst>1) && ((abs(jet_eta_)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || abs(jet_eta_)>2.4) && abs(jet_eta_)<=2.7;
        bool tightLepVetoJetID = (NHF<0.90 && NEMF<0.90 && NumConst>1 && MUF<0.8) && ((abs(jet_eta_)<=2.4 && CHF>0 && CHM>0 && CEMF<0.90) || abs(jet_eta_)>2.4) && abs(jet_eta_)<=2.7;
        jet_jetId_=tightJetID*2+4*tightLepVetoJetID;
        
    }catch(const cms::Exception &e){
        jet_looseId_ = -1;
        jet_jetId_=-1;
    }
    
    jet_puId_= 0;
    jet_hflav_=abs(unsubjet.hadronFlavour());
    jet_pflav_=abs(unsubjet.partonFlavour());
    jet_phflav_=0;
    if(jet.genParton()) jet_phflav_=abs(jet.genParton()->pdgId());

    gen_pt_ =  0;
    Delta_gen_pt_ =  0;
    gen_pt_Recluster_=0;
    gen_pt_WithNu_=0;
    Delta_gen_pt_Recluster_=0;
    Delta_gen_pt_WithNu_=0;
    if(!jet.genJet()){
        const edm::RefToBase<pat::Jet> patJetRef = coll->refAt(jetidx);
	reco::GenJetRef genjetDuplication = (*genJetMatchAllowDuplicates)[patJetRef];
        if (genjetDuplication.isNonnull() && genjetDuplication.isAvailable()) {
	  returnval=false;
        }

    }

    if(jet.genJet()){
        gen_pt_ =  jet.genJet()->pt();
        Delta_gen_pt_ =  jet.genJet()->pt()- jet_pt_;

	float deltaR_match = reco::deltaR(jet, *jet.genJet());
	if( deltaR_match > 0.2){
	  returnval = false;
	}

        const edm::RefToBase<pat::Jet> patJetRef = coll->refAt(jetidx);
        reco::GenJetRef genjetRecluster = (*genJetMatchRecluster)[patJetRef];

        gen_pt_Recluster_ = 0.;
        if (genjetRecluster.isNonnull() && genjetRecluster.isAvailable()) {
            gen_pt_Recluster_ = genjetRecluster->pt();
        }
        reco::GenJetRef genjetWithNu = (*genJetMatchWithNu)[patJetRef];

        gen_pt_WithNu_ = 0.;
        if (genjetWithNu.isNonnull() && genjetWithNu.isAvailable()) {
            gen_pt_WithNu_ = genjetWithNu->pt();
        }

        Delta_gen_pt_Recluster_=gen_pt_Recluster_-jet.pt();
        Delta_gen_pt_WithNu_=gen_pt_WithNu_-jet.pt();
    }

    auto qgtuple=yuta::calcVariables(&unsubjet);
    //(multiplicity, charged_multiplicity, neutral_multiplicity, ptD, axis1, axis2, pt_dr_log);

    y_multiplicity_=std::get<0>(qgtuple);
    y_charged_multiplicity_=std::get<1>(qgtuple);
    y_neutral_multiplicity_=std::get<2>(qgtuple);
    y_ptD_    =  std::get<3>(qgtuple);
    y_axis1_  =  std::get<4>(qgtuple);
    y_axis2_  =  std::get<5>(qgtuple);
    y_pt_dr_log_=std::get<6>(qgtuple);

    return returnval;
}
