// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <boost/core/demangle.hpp>

#include "../interface/ntuple_content.h"
#include "../interface/ntuple_SV.h"
#include "../interface/ntuple_V0Ks.h"
#include "../interface/ntuple_pairwise.h"
#include "../interface/ntuple_JetInfo.h"
#include "../interface/ntuple_pfCands.h"
#include "../interface/ntuple_bTagVars.h"
#include "../interface/ntuple_FatJetInfo.h"
#include "../interface/ntuple_LT.h"
//ROOT includes
#include "TTree.h"
#include <TFile.h>
#include <TROOT.h>
#include "TBranch.h"
#include <string>
#include <vector>
#include "TSystem.h"
#include <TRandom.h>

//CMSSW includes
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Common/interface/AssociationMap.h"

// for ivf
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "TLorentzVector.h"

#include "DataFormats/BTauReco/interface/PixelClusterTagInfo.h"

#include <algorithm>
#include <iterator>
#include <map>

//trash?

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "TLorentzVector.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/Measurement1D.h"

#if defined( __GXX_EXPERIMENTAL_CXX0X__)
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#endif

struct MagneticField;


class DeepNtuplizer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
public:
  explicit DeepNtuplizer(const edm::ParameterSet&);
  ~DeepNtuplizer();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  Measurement1D vertexDxy(const reco::VertexCompositePtrCandidate &svcand, const reco::Vertex &pv) const;
  Measurement1D vertexD3d(const reco::VertexCompositePtrCandidate &sv, const reco::Vertex &pv) const ;
  float vertexDdotP(const reco::VertexCompositePtrCandidate &sv, const reco::Vertex &pv) const ;


  // ----------member data ---------------------------
  edm::EDGetTokenT<reco::VertexCollection> vtxToken_;
  edm::EDGetTokenT<reco::VertexCompositePtrCandidateCollection> svToken_;
  edm::EDGetTokenT<reco::VertexCompositePtrCandidateCollection> v0KsToken_;
  edm::EDGetTokenT<edm::View<pat::Jet> >      jetToken_;
  typedef edm::AssociationMap<edm::OneToOne<reco::JetView, reco::JetView> > JetMatchMap;
  edm::EDGetTokenT<JetMatchMap> unsubjetMapToken_;
  edm::EDGetTokenT<std::vector<PileupSummaryInfo> > puToken_;
  edm::EDGetTokenT<double> rhoToken_;
  edm::EDGetTokenT< edm::View<reco::BaseTagInfo> > pixHitsToken_;
  edm::EDGetTokenT<pat::TauCollection> tau_token_;
  std::string t_qgtagger;

  edm::Service<TFileService> fs;
  TTree *tree_;

  size_t njetstotal_;
  size_t njetswithgenjet_;
  size_t njetsselected_;
  size_t njetsselected_nogen_;

  ntuple_content * addModule(ntuple_content *m, std::string name = ""){
    modules_.push_back(m);
    module_names_.push_back(name);
    return m;
  }
  std::vector<ntuple_content* > modules_;
  std::vector<std::string> module_names_;

  bool applySelection_;
};

DeepNtuplizer::DeepNtuplizer(const edm::ParameterSet& iConfig):
  vtxToken_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"))),
  svToken_(consumes<reco::VertexCompositePtrCandidateCollection>(iConfig.getParameter<edm::InputTag>("secVertices"))),
  v0KsToken_(consumes<reco::VertexCompositePtrCandidateCollection>(iConfig.getParameter<edm::InputTag>("V0_ks"))),
  jetToken_(consumes<edm::View<pat::Jet> >(iConfig.getParameter<edm::InputTag>("jets"))),
  unsubjetMapToken_(consumes<JetMatchMap>(iConfig.getParameter<edm::InputTag>("unsubjet_map"))),
  puToken_(consumes<std::vector<PileupSummaryInfo >>(iConfig.getParameter<edm::InputTag>("pupInfo"))),
  rhoToken_(consumes<double>(iConfig.getParameter<edm::InputTag>("rhoInfo"))),
  pixHitsToken_(consumes< edm::View<reco::BaseTagInfo> > (iConfig.getParameter<edm::InputTag>("pixelhit"))),
  tau_token_(consumes<pat::TauCollection>(iConfig.getParameter<edm::InputTag>("taus"))),
  t_qgtagger(iConfig.getParameter<std::string>("qgtagger"))
{

  /*
   *  Initialise the modules here
   *  Everything else does not need to be changed if
   *  modules don't interact.
   */
  // read configuration parameters
  const double jetR = iConfig.getParameter<double>("jetR");
  const bool  runFatJets_ = iConfig.getParameter<bool>("runFatJet");
  // AS const bool  runDeepVertex_ = iConfig.getParameter<bool>("runDeepVertex");

  //not implemented yet
  const bool useHerwigCompatibleMatching=iConfig.getParameter<bool>("useHerwigCompatible");
  const bool isHerwig=iConfig.getParameter<bool>("isHerwig");

  ntuple_content::useoffsets = iConfig.getParameter<bool>("useOffsets");

  applySelection_=iConfig.getParameter<bool>("applySelection");

  ntuple_SV* svmodule=new ntuple_SV("", jetR);
  svmodule->setTrackBuilderToken(
      esConsumes<TransientTrackBuilder, TransientTrackRecord>(
                          edm::ESInputTag("", "TransientTrackBuilder")));
  addModule(svmodule, "SVNtuple");

  ntuple_V0Ks* v0ksmodule=new ntuple_V0Ks("", jetR);
  addModule(v0ksmodule, "V0KsNtuple");

  ntuple_JetInfo* jetinfo=new ntuple_JetInfo();
  jetinfo->setQglToken(consumes<edm::ValueMap<float>>(edm::InputTag(t_qgtagger, "qgLikelihood")));
  jetinfo->setPtDToken(consumes<edm::ValueMap<float>>(edm::InputTag(t_qgtagger, "ptD")));
  jetinfo->setAxis2Token(consumes<edm::ValueMap<float>>(edm::InputTag(t_qgtagger, "axis2")));
  jetinfo->setMultToken(consumes<edm::ValueMap<int>>(edm::InputTag(t_qgtagger, "mult")));

  jetinfo->setUseHerwigCompatibleMatching(useHerwigCompatibleMatching);
  jetinfo->setIsHerwig(isHerwig);

  jetinfo->setGenJetMatchReclusterToken(consumes<edm::Association<reco::GenJetCollection>>(iConfig.getParameter<edm::InputTag>( "genJetMatchRecluster" )));
  jetinfo->setGenJetsToken(consumes<reco::GenJetCollection>(iConfig.getParameter<edm::InputTag>("genJets")));
  jetinfo->setGenJetsWnuToken(mayConsume<reco::GenJetCollection>(iConfig.getParameter<edm::InputTag>("genJetsWnu")));
  jetinfo->setGenJetMatchWithNuToken(consumes<edm::Association<reco::GenJetCollection>>(iConfig.getParameter<edm::InputTag>( "genJetMatchWithNu" )));
  jetinfo->setGenJetMatchAllowDuplicatesToken(consumes<edm::Association<reco::GenJetCollection>>(iConfig.getParameter<edm::InputTag>( "genJetMatchAllowDuplicates" ))); 
  jetinfo->setGenParticlesToken(consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("pruned")));
  jetinfo->setMuonsToken(consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons")));
  jetinfo->setElectronsToken(consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electrons")));
  jetinfo->setTausToken(consumes<pat::TauCollection>(iConfig.getParameter<edm::InputTag>("taus")));
  if (iConfig.existsAs<edm::InputTag>("cent"))
    jetinfo->setCentToken(consumes<int>(iConfig.getParameter<edm::InputTag>("cent")));

  addModule(jetinfo, "jetinfo");

  ntuple_pfCands * pfcands = new ntuple_pfCands();
  pfcands->setJetRadius(jetR);
  pfcands->setTrackBuilderToken(
      esConsumes<TransientTrackBuilder, TransientTrackRecord>(
                          edm::ESInputTag("", "TransientTrackBuilder")));

  addModule(pfcands, "pfcands");

  ntuple_LT * LT = new ntuple_LT();
  LT->setJetRadius(jetR);
  LT->setTrackBuilderToken(
      esConsumes<TransientTrackBuilder, TransientTrackRecord>(
                          edm::ESInputTag("", "TransientTrackBuilder")));
  LT->setLTToken(consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>("losttracks")));

  addModule(LT, "LT");

  ntuple_pairwise * pairwise = new ntuple_pairwise();
  pairwise->setJetRadius(jetR);
  pairwise->setTrackBuilderToken( esConsumes<TransientTrackBuilder, TransientTrackRecord>(edm::ESInputTag("", "TransientTrackBuilder")));

  addModule(pairwise, "pairwise");

  addModule(new ntuple_bTagVars(), "bTagVars");

  if(runFatJets_){
    auto *fatjetinfo = new ntuple_FatJetInfo(jetR);
    fatjetinfo->setGenParticleToken(consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("pruned")));
    fatjetinfo->setFatJetToken(consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("fatjets")));
    addModule(fatjetinfo, "fatJets");
  }

  for(auto& m: modules_)
    m->getInput(iConfig);

}


DeepNtuplizer::~DeepNtuplizer()
{
  return;
  for(auto& m:modules_)
    delete m;
}


// ------------ method called for each event  ------------
void
DeepNtuplizer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  //global info

  edm::Handle<reco::VertexCollection> vertices;
  iEvent.getByToken(vtxToken_, vertices);
  if (vertices->empty()) return; // skip the event if no PV found

  edm::Handle<std::vector<reco::VertexCompositePtrCandidate> > secvertices;
  iEvent.getByToken(svToken_, secvertices);

  edm::Handle<std::vector<reco::VertexCompositePtrCandidate> > v0_ks;
  iEvent.getByToken(v0KsToken_, v0_ks);
  
  edm::Handle<std::vector<PileupSummaryInfo> > pupInfo;
  iEvent.getByToken(puToken_, pupInfo);

  edm::Handle<double> rhoInfo;
  iEvent.getByToken(rhoToken_,rhoInfo);

  edm::Handle<edm::View<pat::Jet> > jets;
  iEvent.getByToken(jetToken_, jets);
  const auto& unsubjet_map = iEvent.getHandle(unsubjetMapToken_);

  edm::Handle< edm::View<reco::BaseTagInfo> > pixHits;
  iEvent.getByToken(pixHitsToken_, pixHits);

  edm::Handle<std::vector<pat::Tau>> taus = iEvent.getHandle(tau_token_);
  
  for(auto& m:modules_){
    m->setPrimaryVertices(vertices.product());
    m->setSecVertices(secvertices.product());
    m->setV0ks(v0_ks.product());
    m->setTaus(taus.product());
    m->setPuInfo(pupInfo.product());
    m->setRhoInfo(rhoInfo.product());
    m->readSetup(iSetup);
    m->readEvent(iEvent);
  }

  std::vector<size_t> indices(jets->size());
  for(size_t i=0;i<jets->size();i++)
    indices.at(i)=i;

  if(applySelection_)
    std::random_shuffle (indices.begin(),indices.end());

  edm::View<pat::Jet>::const_iterator jetIter;
  // loop over the jets
  for(size_t j=0;j<indices.size();j++){
    njetstotal_++;
    size_t jetidx=indices.at(j);
    jetIter = jets->begin()+jetidx;
    const pat::Jet& jet = *jetIter;
    const auto& unsubjet_ref = unsubjet_map.isValid() ? (*unsubjet_map)[jets->refAt(jetidx)] : edm::RefToBase<reco::Jet>();
    const auto& unsubjet = unsubjet_ref.isNonnull() ? *dynamic_cast<const pat::Jet*>(unsubjet_ref.get()) : jet;

    if(jet.genJet())
      njetswithgenjet_++;

    bool writejet=true;
    size_t idx = 0;
    for(auto& m:modules_){
      if(! m->fillBranches(unsubjet, jet, jetidx, jets.product())){
	writejet=false;
	if(applySelection_) break;
      }
      idx++;
    }
    if( (writejet&&applySelection_) || !applySelection_ ){
      tree_->Fill();
      njetsselected_++;
      if(!jet.genJet())
	njetsselected_nogen_++;

    }
  } // end of looping over the jets
}

// ------------ method called once each job just before starting event loop  ------------
void
DeepNtuplizer::beginJob()
{
  if( !fs ){
    throw edm::Exception( edm::errors::Configuration,
			  "TFile Service is not registered in cfg file" );
  }
  tree_=(fs->make<TTree>("tree" ,"tree" ));
  for(auto& m:modules_)
    m->initBranches(tree_);
  njetstotal_=0;
  njetswithgenjet_=0;
  njetsselected_=0;
  njetsselected_nogen_=0;
}

// ------------ method called once each job just after ending the event loop  ------------
void
DeepNtuplizer::endJob()
{

  std::cout << "total number of processed jets: " << njetstotal_<<std::endl;
  std::cout << "total number of jets with gen:  " << njetswithgenjet_<<std::endl;
  std::cout << "total number of selected jets:  "<< njetsselected_<<std::endl;
  std::cout << "fraction of selected jets:      "<< (float)njetsselected_/(float)njetstotal_<<std::endl;
  std::cout << "fraction of selected jets with gen: "<< (float)njetsselected_/(float)njetswithgenjet_<<std::endl;
  std::cout << "fraction of selected jetsout with gen: "<< (float)njetsselected_nogen_/(float)njetsselected_<<std::endl;

}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
DeepNtuplizer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(DeepNtuplizer);
