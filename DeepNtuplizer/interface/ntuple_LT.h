/*
 * ntuple_LT.h
 *
 *  Created on: 28 Mai 2023
 *      Author: Alexandre De Moor
 */

#ifndef DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_NTUPLE_LT_H_
#define DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_NTUPLE_LT_H_

#include "ntuple_content.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

class ntuple_LT: public ntuple_content{
 public:

 ntuple_LT():ntuple_content(),jetradius_(0.4),
    n_LTcand_(0){}

  void setJetRadius(const float& radius){jetradius_=radius;}
  void getInput(const edm::ParameterSet& iConfig);
  void initBranches(TTree* );
  void readEvent(const edm::Event& iEvent);
  void readSetup(const edm::EventSetup& iSetup);


  //use either of these functions

  bool fillBranches(const pat::Jet &, const size_t& jetidx, const  edm::View<pat::Jet> * coll=0);

  void setTrackBuilderToken(const edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord>& track_builder_token) {
    track_builder_token_ = track_builder_token;
  }
  void setLTToken(const edm::EDGetTokenT<edm::View<reco::Candidate>> ltToken) {
    ltToken_ = ltToken;
  }

 private:

  float jetradius_;
  float min_candidate_pt_ = -1;

  edm::ESHandle<TransientTrackBuilder> builder;
  edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> track_builder_token_;

  edm::EDGetTokenT<edm::View<reco::Candidate>> ltToken_;
  edm::Handle<edm::View<reco::Candidate>> LTs;

  int n_LT_;
  int n_LTcand_;

  static constexpr size_t max_ltcand_=50;

  float  LT_pt_[max_ltcand_];
  float  LT_eta_[max_ltcand_];
  float  LT_phi_[max_ltcand_];
  float  LT_e_[max_ltcand_];

  float  LT_puppiw_[max_ltcand_];
  float  LT_VTX_ass_[max_ltcand_];
  float  LT_dz_[max_ltcand_];

  float LT_BtagPf_trackEtaRel_[max_ltcand_];
  float LT_BtagPf_trackPtRel_[max_ltcand_];
  float LT_BtagPf_trackPPar_[max_ltcand_];
  float LT_BtagPf_trackDeltaR_[max_ltcand_];
  float LT_BtagPf_trackPParRatio_[max_ltcand_];
  float LT_BtagPf_trackSip3dVal_[max_ltcand_];
  float LT_BtagPf_trackSip3dSig_[max_ltcand_];
  float LT_BtagPf_trackSip2dVal_[max_ltcand_];
  float LT_BtagPf_trackSip2dSig_[max_ltcand_];
  float LT_BtagPf_trackDecayLen_[max_ltcand_];
  float LT_BtagPf_trackJetDistVal_[max_ltcand_];
                                                                                                                                        
  float LT_charge_[max_ltcand_];
  float LT_chi2_[max_ltcand_];
  float LT_quality_[max_ltcand_];
  float LT_drminsv_[max_ltcand_];
  float LT_distminsvold_[max_ltcand_];
  float LT_distminsv_[max_ltcand_];
  float LT_distminsv2_[max_ltcand_];

  float LT_lostInnerHits_[max_ltcand_];
  float LT_numberOfPixelHits_[max_ltcand_];
  float LT_numberOfStripHits_[max_ltcand_];

  float LT_pdgID_[max_ltcand_];
  float LT_HadFrac_[max_ltcand_];
  float LT_CaloFrac_[max_ltcand_];

  float mindrsvltcand(const pat::PackedCandidate* pfcand);
  float mindistsvltcandold(const reco::TransientTrack track);
  float mindistsvltcand(const reco::TransientTrack track);
  GlobalPoint mingpsvltcand(const reco::TransientTrack track);
  GlobalPoint gppvltcand(const reco::TransientTrack track, GlobalVector direction, const reco::Vertex vertex);

};


#endif /* DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_NTUPLE_LT_H_ */
