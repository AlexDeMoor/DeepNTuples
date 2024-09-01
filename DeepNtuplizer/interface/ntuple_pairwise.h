/*
 * ntuple_pairwise.h
 *
 *  Created on: 01 sep 2022
 *      Authors: Matteo Malucchi & Alexandre De Moor
 */

#ifndef DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_NTUPLE_PAIRWISE_H_
#define DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_NTUPLE_PAIRWISE_H_

#include "ntuple_content.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackPairInfoBuilder.h"

class ntuple_pairwise: public ntuple_content{
 public:

 ntuple_pairwise():ntuple_content(),jetradius_(0.4){}

  void setJetRadius(const float& radius){jetradius_=radius;}
  void getInput(const edm::ParameterSet& iConfig);
  void initBranches(TTree* );
  void readEvent(const edm::Event& iEvent);
  void readSetup(const edm::EventSetup& iSetup);

  bool fillBranches(const pat::Jet &, const pat::Jet &, const size_t& jetidx, const  edm::View<pat::Jet> * coll=0);

  void setTrackBuilderToken(const edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord>& track_builder_token) {
    track_builder_token_ = track_builder_token;
  }

 private:

  float jetradius_;
  float min_candidate_pt_ = -1;

  edm::ESHandle<TransientTrackBuilder> builder;
  edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> track_builder_token_;

  int n_Npfcand2_;
  int n_Cpfcand2_;
  int n_Cpfpairs_;
  float nCpfpairs_;

  static constexpr size_t max_pfcand_=800;

  float pair_pca_distance_[max_pfcand_];
  float pair_pca_significance_[max_pfcand_];

  float pair_pcaSeed_x1_[max_pfcand_];
  float pair_pcaSeed_y1_[max_pfcand_];
  float pair_pcaSeed_z1_[max_pfcand_];

  float pair_pcaSeed_x2_[max_pfcand_];
  float pair_pcaSeed_y2_[max_pfcand_];
  float pair_pcaSeed_z2_[max_pfcand_];

  float pair_pcaSeed_xerr1_[max_pfcand_];
  float pair_pcaSeed_yerr1_[max_pfcand_];
  float pair_pcaSeed_zerr1_[max_pfcand_];

  float pair_pcaSeed_xerr2_[max_pfcand_];
  float pair_pcaSeed_yerr2_[max_pfcand_];
  float pair_pcaSeed_zerr2_[max_pfcand_];

  float pair_dotprod1_[max_pfcand_];
  float pair_dotprod2_[max_pfcand_];

  float pair_pca_dist1_[max_pfcand_];
  float pair_pca_dist2_[max_pfcand_];

  float pair_dotprod12_2D_[max_pfcand_];
  float pair_dotprod12_2DV_[max_pfcand_];
  float pair_dotprod12_3D_[max_pfcand_];
  float pair_dotprod12_3DV_[max_pfcand_];

  float pair_pca_jetAxis_dist_[max_pfcand_];
  float pair_pca_jetAxis_dotprod_[max_pfcand_];
  float pair_pca_jetAxis_dEta_[max_pfcand_];
  float pair_pca_jetAxis_dPhi_[max_pfcand_];

  float pfcand_dist_vtx_12_[max_pfcand_];

  float mindrsvpfcand(const pat::PackedCandidate* pfcand);

};

#endif /* DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_NTUPLE_PAIRWISE_H_ */
