/*
 * ntuple_JetInfo.h
 *
 *  Created on: 13 Feb 2017
 *      Author: jkiesele
 */

#ifndef DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_NTUPLE_JETINFO_H_
#define DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_NTUPLE_JETINFO_H_

#include <TLorentzVector.h>
#include "ntuple_content.h"
#include "TRandom3.h"
#include <map>
#include <string>

/*
 * For global jet info such as eta, pt, gen info
 */
class ntuple_JetInfo: public ntuple_content{
public:
    ntuple_JetInfo():ntuple_content(),
    gluonReduction_(0),
    useherwcompat_matching_(false),
    isherwig_(false)
{}

    void getInput(const edm::ParameterSet& iConfig);
    void initBranches(TTree* );
    void readEvent(const edm::Event& iEvent);

    //use either of these functions

    bool fillBranches(const pat::Jet &, const pat::Jet &, const size_t& jetidx, const  edm::View<pat::Jet> * coll=0);

    void setAxis2Token(edm::EDGetTokenT<edm::ValueMap<float> > axis2Token) {
        axis2Token_ = axis2Token;
    }

    void setMultToken(edm::EDGetTokenT<edm::ValueMap<int> > multToken) {
        multToken_ = multToken;
    }

    void setPtDToken(edm::EDGetTokenT<edm::ValueMap<float> > ptDToken) {
        ptDToken_ = ptDToken;
    }

    void setQglToken(edm::EDGetTokenT<edm::ValueMap<float> > qglToken) {
        qglToken_ = qglToken;
    }

    void setGenJetMatchReclusterToken(
            edm::EDGetTokenT<edm::Association<reco::GenJetCollection> > genJetMatchReclusterToken) {
        genJetMatchReclusterToken_ = genJetMatchReclusterToken;
    }

    void setGenJetsToken(
            edm::EDGetTokenT<reco::GenJetCollection > genJetsToken) {
        genJetsToken_ = genJetsToken;
    }

    void setGenJetsWnuToken(
            edm::EDGetTokenT<reco::GenJetCollection > genJetsWnuToken) {
        genJetsWnuToken_ = genJetsWnuToken;
    }

    void setGenJetMatchWithNuToken(
            edm::EDGetTokenT<edm::Association<reco::GenJetCollection> > genJetMatchWithNuToken) {
        genJetMatchWithNuToken_ = genJetMatchWithNuToken;
    }

    void setGenJetMatchAllowDuplicatesToken(
            edm::EDGetTokenT<edm::Association<reco::GenJetCollection> > genJetMatchAllowDuplicatesToken) {
        genJetMatchAllowDuplicatesToken_ = genJetMatchAllowDuplicatesToken;
    }


    void setGenParticlesToken(edm::EDGetTokenT<reco::GenParticleCollection> genParticlesToken) {
        genParticlesToken_ = genParticlesToken;
    }

    void setMuonsToken(edm::EDGetTokenT<pat::MuonCollection> muonsToken) {
        muonsToken_ = muonsToken;
    }

    void setElectronsToken(edm::EDGetTokenT<pat::ElectronCollection> electronsToken) {
        electronsToken_ = electronsToken;
    }

    void setTausToken(edm::EDGetTokenT<pat::TauCollection> tausToken) {
        tausToken_ = tausToken;
    }

    void setCentToken(const edm::EDGetTokenT<int>& centToken) {
        centToken_ = centToken;
    }

    void setUseHerwigCompatibleMatching(const bool use){
        useherwcompat_matching_=use;
    }
    void setIsHerwig(const bool use){
        isherwig_=use;
    }

    //private:

    double                    jetPtMin_;
    double                    jetPtMax_;
    double                    jetAbsEtaMin_;
    double                    jetAbsEtaMax_;

    //Quark gluon likelihood
    edm::EDGetTokenT<edm::ValueMap<float>>   qglToken_;
    edm::EDGetTokenT<edm::ValueMap<float>>   ptDToken_;
    edm::EDGetTokenT<edm::ValueMap<float>>   axis2Token_;
    edm::EDGetTokenT<edm::ValueMap<int>>     multToken_;

    edm::Handle<edm::ValueMap<float>> qglHandle;
    edm::Handle<edm::ValueMap<float>> ptDHandle;
    edm::Handle<edm::ValueMap<float>> axis2Handle;
    edm::Handle<edm::ValueMap<int>> multHandle;


    edm::EDGetTokenT<edm::Association<reco::GenJetCollection> > genJetMatchReclusterToken_;
    edm::EDGetTokenT<edm::Association<reco::GenJetCollection> > genJetMatchWithNuToken_;
    edm::EDGetTokenT<edm::Association<reco::GenJetCollection> > genJetMatchAllowDuplicatesToken_;
    edm::EDGetTokenT<reco::GenJetCollection > genJetsWnuToken_;
    edm::EDGetTokenT<reco::GenJetCollection > genJetsToken_;

    edm::EDGetTokenT<reco::GenParticleCollection> genParticlesToken_;

    edm::EDGetTokenT<pat::MuonCollection> muonsToken_;       
    edm::EDGetTokenT<pat::ElectronCollection> electronsToken_;
    edm::EDGetTokenT<pat::TauCollection> tausToken_;
    edm::EDGetTokenT<int> centToken_;

    edm::Handle<edm::Association<reco::GenJetCollection> > genJetMatchRecluster;
    edm::Handle<edm::Association<reco::GenJetCollection> > genJetMatchWithNu;
    edm::Handle<edm::Association<reco::GenJetCollection> > genJetMatchAllowDuplicates;

    edm::Handle<reco::GenJetCollection> genJetsWnuH;
    edm::Handle<reco::GenJetCollection> genJetsH;  

    edm::Handle<reco::GenParticleCollection> genParticlesHandle;

    edm::Handle<pat::MuonCollection> muonsHandle;
    edm::Handle<pat::ElectronCollection> electronsHandle;
    edm::Handle<pat::TauCollection> tausH;

    TRandom3 TRandom_;
    float gluonReduction_;

    // GenJet with and without nu
    std::vector<reco::GenJetRef> jetv_gen;
    std::vector<reco::GenJetRef> jetv_gen_wnu;   
  
    // Generator-level information (GEN particles)
    std::vector<float>  gen_particle_pt;
    std::vector<float>  gen_particle_eta;
    std::vector<float>  gen_particle_phi;
    std::vector<float>  gen_particle_mass;
    std::vector<int>    gen_particle_id;
    std::vector<unsigned int>  gen_particle_status;
    std::vector<int>    gen_particle_daughters_id;
    std::vector<unsigned int> gen_particle_daughters_igen;
    std::vector<unsigned int> gen_particle_daughters_status;
    std::vector<float>  gen_particle_daughters_pt;
    std::vector<float>  gen_particle_daughters_eta;
    std::vector<float>  gen_particle_daughters_phi;
    std::vector<float>  gen_particle_daughters_mass;
    std::vector<int>    gen_particle_daughters_charge;

    // Gen leptons from resonance decay 
    std::vector<TLorentzVector> genLepFromResonance4V_;
    std::vector<TLorentzVector> genMuonsFromResonance4V_;
    std::vector<TLorentzVector> genElectronsFromResonance4V_;
    std::vector<TLorentzVector> tau_gen_visible_;
    std::vector<TLorentzVector> tau_gen_;
    std::vector<int> tau_gen_charge_;
    std::vector<unsigned int> tau_gen_nch_;
    std::vector<unsigned int> tau_gen_np0_;
    std::vector<unsigned int> tau_gen_nnh_;

    std::vector <reco::GenParticle> neutrinosLepB;
    std::vector <reco::GenParticle> neutrinosLepB_C;

    std::vector<reco::GenParticle> gToBB;
    std::vector<reco::GenParticle> gToCC;
    std::vector<reco::GenParticle> alltaus_;

    std::vector<reco::GenParticle> Bhadron_;
    std::vector<reco::GenParticle> Bhadron_daughter_;



    bool useherwcompat_matching_;
    bool isherwig_;

    /////////branches

    // labels (MC truth)
    // regressions pt, Deta, Dphi
    float min_candidate_pt_;
    float gen_pt_;
   
    float Delta_gen_pt_;
    //classification
    int isMC_;
    int isemu_;
    int ismutau_;
    int isdimu_;

    int isB_;
    int isGBB_;
    int isBB_;
    int isC_;
    int isGCC_;
    int isCC_;
    int isD_;
    int isU_;
    int isS_;
    int isG_;
    int isMU_;
    int isELE_;
    int isPU_;
    int isUndefined_;
    float genDecay_;
    int isLeptonicB_;
    int isLeptonicB_C_;
    int isTaup1h0p_;
    int isTaup1h1p_;
    int isTaup1h2p_;
    int isTaup3h0p_;
    int isTaup3h1p_;
    int isTaum1h0p_;
    int isTaum1h1p_;
    int isTaum1h2p_;
    int isTaum3h0p_;
    int isTaum3h1p_;

    // Heavy ion variables
    float cent_;

    //truth labeling with fallback to physics definition for light/gluon/undefined of standard flavor definition
    int isPhysB_;
    int isPhysGBB_;
    int isPhysBB_;
    int isPhysC_;
    int isPhysGCC_;
    int isPhysCC_;
    int isPhysU_;
    int isPhysD_;
    int isPhysS_;
    int isPhysG_;
    int isPhysUndefined_;
    int isPhysLeptonicB_;
    int isPhysLeptonicB_C_;
    int isPhysTau_;
    int isPhysPU_;
 
    // global variables
    float npv_;
    float ntrueInt_;
    float rho_;
    int event_no_;
    int jet_no_;

    // jet regression targets
    float jet_genmatch_pt_;
    float jet_genmatch_wnu_pt_;
    float jet_genmatch_lep_vis_pt_;
    float jet_mumatch_pt_;
    float jet_elematch_pt_;
    float jet_taumatch_pt_;
  
    // jet variables
    float jet_pt_;
    float jet_corr_pt_;
    float  jet_eta_;
    float  jet_phi_;
    float  jet_mass_;
    float  jet_energy_;

    float jet_looseId_;
    int jet_jetId_;
    int jet_puId_;
    
    int jet_hflav_;
    int jet_pflav_;
    int jet_phflav_;

    // quark/gluon
    float jet_qgl_;
    float QG_ptD_;
    float QG_axis2_;
    float QG_mult_;


    float y_multiplicity_;
    float y_charged_multiplicity_;
    float y_neutral_multiplicity_;
    float y_ptD_;
    float y_axis1_;
    float y_axis2_;
    float y_pt_dr_log_;

    static constexpr std::size_t max_num_lept = 5;
    int muons_isLooseMuon_[max_num_lept];
    int muons_isTightMuon_[max_num_lept];
    int muons_isSoftMuon_[max_num_lept];
    int muons_isHighPtMuon_[max_num_lept]; 
    float muons_pt_[max_num_lept]; 
    float muons_relEta_[max_num_lept]; 
    float muons_relPhi_[max_num_lept]; 
    float muons_energy_[max_num_lept]; 
    float electrons_pt_[max_num_lept]; 
    float electrons_relEta_[max_num_lept]; 
    float electrons_relPhi_[max_num_lept]; 
    float electrons_energy_[max_num_lept];

    int muons_number_ = 0;
    int electrons_number_ = 0;

    float gen_pt_Recluster_;
    float gen_pt_WithNu_;
    float Delta_gen_pt_Recluster_;
    float Delta_gen_pt_WithNu_;
    std::map<std::string, float> discriminators_;

    //Gen info for ele/mu/tau
    static constexpr std::size_t max_num_gen_ = 50;
    int gen_number_;
    int gend_number_;

    float gen_particle_pt_[max_num_gen_];
  float gen_particle_eta_[max_num_gen_];
  float gen_particle_phi_[max_num_gen_];
  float gen_particle_mass_[max_num_gen_];
  float gen_particle_id_[max_num_gen_];
  float gen_particle_status_[max_num_gen_];
  float gen_particle_daughters_id_[max_num_gen_];
  float gen_particle_daughters_igen_[max_num_gen_];
  float gen_particle_daughters_pt_[max_num_gen_];
  float gen_particle_daughters_eta_[max_num_gen_];
  float gen_particle_daughters_phi_[max_num_gen_];
  float gen_particle_daughters_mass_[max_num_gen_];
  float gen_particle_daughters_status_[max_num_gen_];
  float gen_particle_daughters_charge_[max_num_gen_];

};


#endif /* DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_NTUPLE_JETINFO_H_ */
