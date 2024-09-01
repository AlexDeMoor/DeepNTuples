/*
 * ntuple_pairwise.cc
 *
 *  Created on: 01 Sep 2022
 *      Authors: Matteo Malucchi & Alexandre De Moor
 */

#include "../interface/ntuple_pairwise.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "../interface/sorting_modules.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"
#include "TrackingTools/PatternTools/interface/TwoTrackMinimumDistance.h"
#include "TVector3.h"


class TrackInfoBuilder{
public:
  TrackInfoBuilder(edm::ESHandle<TransientTrackBuilder> & build):
    builder(build),
    trackMomentum_(0),
    trackEta_(0),
    trackEtaRel_(0),
    trackPtRel_(0),
    trackPPar_(0),
    trackDeltaR_(0),
    trackPtRatio_(0),
    trackPParRatio_(0),
    trackSip2dVal_(0),
    trackSip2dSig_(0),
    trackSip3dVal_(0),
    trackSip3dSig_(0),

    trackJetDecayLen_(0),
    trackJetDistVal_(0),
    trackJetDistSig_(0),
    ttrack_(0)
  {


  }

  void buildTrackInfo(const pat::PackedCandidate* PackedCandidate_ ,const math::XYZVector&  jetDir, GlobalVector refjetdirection, const reco::Vertex & pv){
    TVector3 jetDir3(jetDir.x(),jetDir.y(),jetDir.z());
    if(!PackedCandidate_->hasTrackDetails()) {
      TVector3 trackMom3(
			 PackedCandidate_->momentum().x(),
			 PackedCandidate_->momentum().y(),
			 PackedCandidate_->momentum().z()
			 );
      trackMomentum_=PackedCandidate_->p();
      trackEta_= PackedCandidate_->eta();
      trackEtaRel_=reco::btau::etaRel(jetDir, PackedCandidate_->momentum());
      trackPtRel_=trackMom3.Perp(jetDir3);
      trackPPar_=jetDir.Dot(PackedCandidate_->momentum());
      trackDeltaR_=reco::deltaR(PackedCandidate_->momentum(), jetDir);
      trackPtRatio_=trackMom3.Perp(jetDir3) / PackedCandidate_->p();
      trackPParRatio_=jetDir.Dot(PackedCandidate_->momentum()) / PackedCandidate_->p();
      trackSip2dVal_=0.;
      trackSip2dSig_=0.;
      trackSip3dVal_=0.;
      trackSip3dSig_=0.;
      trackJetDecayLen_=0.;
      trackJetDistVal_=0.;
      trackJetDistSig_=0.;
      return;
    }

    const reco::Track & PseudoTrack =  PackedCandidate_->pseudoTrack();

    reco::TransientTrack transientTrack;
    transientTrack=builder->build(PseudoTrack);
    Measurement1D meas_ip2d=IPTools::signedTransverseImpactParameter(transientTrack, refjetdirection, pv).second;
    Measurement1D meas_ip3d=IPTools::signedImpactParameter3D(transientTrack, refjetdirection, pv).second;
    Measurement1D jetdist=IPTools::jetTrackDistance(transientTrack, refjetdirection, pv).second;
    Measurement1D decayl = IPTools::signedDecayLength3D(transientTrack, refjetdirection, pv).second;
    math::XYZVector trackMom = PseudoTrack.momentum();
    double trackMag = std::sqrt(trackMom.Mag2());
    TVector3 trackMom3(trackMom.x(),trackMom.y(),trackMom.z());


    trackMomentum_=std::sqrt(trackMom.Mag2());
    trackEta_= trackMom.Eta();
    trackEtaRel_=reco::btau::etaRel(jetDir, trackMom);
    trackPtRel_=trackMom3.Perp(jetDir3);
    trackPPar_=jetDir.Dot(trackMom);
    trackDeltaR_=reco::deltaR(trackMom, jetDir);
    trackPtRatio_=trackMom3.Perp(jetDir3) / trackMag;
    trackPParRatio_=jetDir.Dot(trackMom) / trackMag;

    trackSip2dVal_=(meas_ip2d.value());
    trackSip2dSig_=(meas_ip2d.significance());
    trackSip3dVal_=(meas_ip3d.value());
    trackSip3dSig_=meas_ip3d.significance();

    trackJetDecayLen_= decayl.value();
    trackJetDistVal_= jetdist.value();
    trackJetDistSig_= jetdist.significance();

    ttrack_ = transientTrack;

  }

  const float& getTrackDeltaR() const {return trackDeltaR_;}
  const float& getTrackEta() const {return trackEta_;}
  const float& getTrackEtaRel() const {return trackEtaRel_;}
  const float& getTrackJetDecayLen() const {return trackJetDecayLen_;}
  const float& getTrackJetDistSig() const {return trackJetDistSig_;}
  const float& getTrackJetDistVal() const {return trackJetDistVal_;}
  const float& getTrackMomentum() const {return trackMomentum_;}
  const float& getTrackPPar() const {return trackPPar_;}
  const float& getTrackPParRatio() const {return trackPParRatio_;}
  const float& getTrackPtRatio() const {return trackPtRatio_;}
  const float& getTrackPtRel() const {return trackPtRel_;}
  const float& getTrackSip2dSig() const {return trackSip2dSig_;}
  const float& getTrackSip2dVal() const {return trackSip2dVal_;}
  const float& getTrackSip3dSig() const {return trackSip3dSig_;}
  const float& getTrackSip3dVal() const {return trackSip3dVal_;}
  const reco::TransientTrack getTTrack() const {return ttrack_;}

private:

  edm::ESHandle<TransientTrackBuilder>& builder;
  edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> track_builder_token_;
    
  float trackMomentum_;
  float trackEta_;
  float trackEtaRel_;
  float trackPtRel_;
  float trackPPar_;
  float trackDeltaR_;
  float trackPtRatio_;
  float trackPParRatio_;
  float trackSip2dVal_;
  float trackSip2dSig_;
  float trackSip3dVal_;
  float trackSip3dSig_;

  float trackJetDecayLen_;
  float trackJetDistVal_;
  float trackJetDistSig_;
  reco::TransientTrack ttrack_;

};

void ntuple_pairwise::readSetup(const edm::EventSetup& iSetup){

  builder = iSetup.getHandle(track_builder_token_);

}

void ntuple_pairwise::getInput(const edm::ParameterSet& iConfig){
  min_candidate_pt_ = (iConfig.getParameter<double>("minCandidatePt"));
}

void ntuple_pairwise::initBranches(TTree* tree){

  addBranch(tree,"n_Cpfpairs", &n_Cpfpairs_,"n_Cpfpairs_/I");
  addBranch(tree,"nCpfpairs", &nCpfpairs_,"nCpfpairs_/F");

  addBranch(tree,"pair_pca_distance", &pair_pca_distance_,"pair_pca_distance_[n_Cpfpairs_]/F");
  addBranch(tree,"pair_pca_significance", &pair_pca_significance_,"pair_pca_significance_[n_Cpfpairs_]/F");

  addBranch(tree,"pair_pcaSeed_x1", &pair_pcaSeed_x1_,"pair_pcaSeed_x1_[n_Cpfpairs_]/F");
  addBranch(tree,"pair_pcaSeed_y1", &pair_pcaSeed_y1_,"pair_pcaSeed_y1_[n_Cpfpairs_]/F");
  addBranch(tree,"pair_pcaSeed_z1", &pair_pcaSeed_z1_,"pair_pcaSeed_z1_[n_Cpfpairs_]/F");

  addBranch(tree,"pair_pcaSeed_x2", &pair_pcaSeed_x2_,"pair_pcaSeed_x2_[n_Cpfpairs_]/F");
  addBranch(tree,"pair_pcaSeed_y2", &pair_pcaSeed_y2_,"pair_pcaSeed_y2_[n_Cpfpairs_]/F");
  addBranch(tree,"pair_pcaSeed_z2", &pair_pcaSeed_z2_,"pair_pcaSeed_z2_[n_Cpfpairs_]/F");

  addBranch(tree,"pair_pcaSeed_xerr1", &pair_pcaSeed_xerr1_,"pair_pcaSeed_xerr1_[n_Cpfpairs_]/F");
  addBranch(tree,"pair_pcaSeed_yerr1", &pair_pcaSeed_yerr1_,"pair_pcaSeed_yerr1_[n_Cpfpairs_]/F");
  addBranch(tree,"pair_pcaSeed_zerr1", &pair_pcaSeed_zerr1_,"pair_pcaSeed_zerr1_[n_Cpfpairs_]/F");

  addBranch(tree,"pair_pcaSeed_xerr2", &pair_pcaSeed_xerr2_,"pair_pcaSeed_xerr2_[n_Cpfpairs_]/F");
  addBranch(tree,"pair_pcaSeed_yerr2", &pair_pcaSeed_yerr2_,"pair_pcaSeed_yerr2_[n_Cpfpairs_]/F");
  addBranch(tree,"pair_pcaSeed_zerr2", &pair_pcaSeed_zerr2_,"pair_pcaSeed_zerr2_[n_Cpfpairs_]/F");
  
  addBranch(tree,"pair_dotprod1", &pair_dotprod1_,"pair_dotprod1_[n_Cpfpairs_]/F");
  addBranch(tree,"pair_dotprod2", &pair_dotprod2_,"pair_dotprod2_[n_Cpfpairs_]/F");
  
  addBranch(tree,"pair_pca_dist1", &pair_pca_dist1_,"pair_pca_dist1_[n_Cpfpairs_]/F");
  addBranch(tree,"pair_pca_dist2", &pair_pca_dist2_,"pair_pca_dist2_[n_Cpfpairs_]/F");
    
  addBranch(tree,"pair_dotprod12_2D", &pair_dotprod12_2D_,"pair_dotprod12_2D_[n_Cpfpairs_]/F");
  addBranch(tree,"pair_dotprod12_2DV", &pair_dotprod12_2DV_,"pair_dotprod12_2DV_[n_Cpfpairs_]/F");
  addBranch(tree,"pair_dotprod12_3D", &pair_dotprod12_3D_,"pair_dotprod12_3D_[n_Cpfpairs_]/F");
  addBranch(tree,"pair_dotprod12_3DV", &pair_dotprod12_3DV_,"pair_dotprod12_3DV_[n_Cpfpairs_]/F");
    
  addBranch(tree,"pair_pca_jetAxis_dist", &pair_pca_jetAxis_dist_,"pair_pca_jetAxis_dist_[n_Cpfpairs_]/F");
  addBranch(tree,"pair_pca_jetAxis_dotprod", &pair_pca_jetAxis_dotprod_,"pair_pca_jetAxis_dotprod_[n_Cpfpairs_]/F");
  addBranch(tree,"pair_pca_jetAxis_dEta", &pair_pca_jetAxis_dEta_,"pair_pca_jetAxis_dEta_[n_Cpfpairs_]/F");
  addBranch(tree,"pair_pca_jetAxis_dPhi", &pair_pca_jetAxis_dPhi_,"pair_pca_jetAxis_dPhi_[n_Cpfpairs_]/F");
  
  addBranch(tree,"pfcand_dist_vtx_12", &pfcand_dist_vtx_12_,"pfcand_dist_vtx_12_[n_Cpfpairs_]/F");

}

void ntuple_pairwise::readEvent(const edm::Event& iEvent){

    n_Npfcand2_=0;
    n_Cpfcand2_=0;

}

//use either of these functions

bool ntuple_pairwise::fillBranches(const pat::Jet & unsubjet, const pat::Jet & jet, const size_t& jetidx, const  edm::View<pat::Jet> * coll){

    math::XYZVector jetDir = jet.momentum().Unit();
    GlobalVector jetRefTrackDir(jet.px(),jet.py(),jet.pz());
    const reco::Vertex & pv = vertices()->at(0);

    std::vector<sorting::sortingClass<size_t> > sortedcharged, sortedneutrals;

    const float jet_uncorr_pt=jet.correctedJet("Uncorrected").pt();

    TrackInfoBuilder trackinfo(builder);
    //create collection first, to be able to do some sorting
    for (unsigned int i = 0; i <  unsubjet.numberOfDaughters(); i++){
        const pat::PackedCandidate* PackedCandidate = dynamic_cast<const pat::PackedCandidate*>(unsubjet.daughter(i));
        if(PackedCandidate){
            if(PackedCandidate->pt() < min_candidate_pt_) continue; 
            if(PackedCandidate->charge()!=0){
                trackinfo.buildTrackInfo(PackedCandidate,jetDir,jetRefTrackDir,pv);
                sortedcharged.push_back(sorting::sortingClass<size_t>
					(i, trackinfo.getTrackSip2dSig(),
					 -mindrsvpfcand(PackedCandidate), PackedCandidate->pt()/jet_uncorr_pt));
            }
      }
    }
    std::sort(sortedcharged.begin(),sortedcharged.end(),sorting::sortingClass<size_t>::compareByABCInv);
    n_Cpfcand2_ = std::min(sortedcharged.size(),max_pfcand_);

    std::vector<size_t> sortedchargedindices; //,sortedneutralsindices;
    sortedchargedindices=sorting::invertSortingVector(sortedcharged);
    
    size_t counter = 0;
    int n_cpf_ = std::min((int)25, n_Cpfcand2_);

    for (int i = 0; i <  n_cpf_; i++){
      for (int j = 0; j < i; j++){
	
	deepntuples::TrackPairInfoBuilder trkpairinfo;
	int ind_i = sortedcharged.at(i).get();
	int ind_j = sortedcharged.at(j).get();
	const pat::PackedCandidate* Part_i_  = dynamic_cast<const pat::PackedCandidate*>(unsubjet.daughter(ind_i));
	
	if(!Part_i_){
	  std::cout << i << " Bug PackedCandidate " << j << std::endl;
	}
	const pat::PackedCandidate* Part_j_  = dynamic_cast<const pat::PackedCandidate*>(unsubjet.daughter(ind_j));
	if(!Part_j_){
	  std::cout << i << " Bug PackedCandidate " << j << std::endl;
	}
	  
	trackinfo.buildTrackInfo(Part_i_,jetDir,jetRefTrackDir,pv);
	const reco::TransientTrack it = trackinfo.getTTrack();
	trackinfo.buildTrackInfo(Part_j_,jetDir,jetRefTrackDir,pv);
	const reco::TransientTrack tt = trackinfo.getTTrack();
	
	trkpairinfo.buildTrackPairInfo(it,tt,vertices()->at(0),jet);

	//const reco::Candidate * pruned_part_match1 = Part_i_.lastPrunedRef().get();
        //const reco::Candidate * pruned_part_match2 = Part_j_.lastPrunedRef().get();
	float dist_vtx_12 = -1.0; //sqrt((pruned_part_match1->vertex()- pruned_part_match2->vertex()).mag2());
	      
	pair_pca_distance_[counter] = trkpairinfo.pca_distance();
	pair_pca_significance_[counter] = trkpairinfo.pca_significance();

	pair_pcaSeed_x1_[counter] = trkpairinfo.pcaSeed_x();
	pair_pcaSeed_y1_[counter] = trkpairinfo.pcaSeed_y();
	pair_pcaSeed_z1_[counter] = trkpairinfo.pcaSeed_z();

	pair_pcaSeed_x2_[counter] = trkpairinfo.pcaTrack_x();
	pair_pcaSeed_y2_[counter] = trkpairinfo.pcaTrack_y();
	pair_pcaSeed_z2_[counter] = trkpairinfo.pcaTrack_z();

	pair_pcaSeed_xerr1_[counter] = trkpairinfo.pcaSeed_xerr();
	pair_pcaSeed_yerr1_[counter] = trkpairinfo.pcaSeed_yerr();
	pair_pcaSeed_zerr1_[counter] = trkpairinfo.pcaSeed_zerr();

	pair_pcaSeed_xerr2_[counter] = trkpairinfo.pcaTrack_xerr();
	pair_pcaSeed_yerr2_[counter] = trkpairinfo.pcaTrack_yerr();
	pair_pcaSeed_zerr2_[counter] = trkpairinfo.pcaTrack_zerr();

	pair_dotprod1_[counter] = trkpairinfo.dotprodTrack();
	pair_dotprod2_[counter] = trkpairinfo.dotprodSeed();

	pair_pca_dist1_[counter] = trkpairinfo.pcaSeed_dist();
	pair_pca_dist2_[counter] = trkpairinfo.pcaTrack_dist();

	pair_dotprod12_2D_[counter] = trkpairinfo.dotprodTrackSeed2D();
	pair_dotprod12_2DV_[counter] = trkpairinfo.dotprodTrackSeed2DV();
	pair_dotprod12_3D_[counter] = trkpairinfo.dotprodTrackSeed3D();
	pair_dotprod12_3DV_[counter] = trkpairinfo.dotprodTrackSeed3DV();

	pair_pca_jetAxis_dist_[counter] = trkpairinfo.pca_jetAxis_dist();
	pair_pca_jetAxis_dotprod_[counter] = trkpairinfo.pca_jetAxis_dotprod();
	pair_pca_jetAxis_dEta_[counter] = trkpairinfo.pca_jetAxis_dEta();
	pair_pca_jetAxis_dPhi_[counter] = trkpairinfo.pca_jetAxis_dPhi();

	pfcand_dist_vtx_12_[counter] = dist_vtx_12;
	
	counter++;
      }
    }
    nCpfpairs_ = counter;
    n_Cpfpairs_ = counter;
    
    return true; //for making cuts
}

float ntuple_pairwise::mindrsvpfcand(const pat::PackedCandidate* pfcand) {

  float mindr_ = jetradius_;
  for (unsigned int i=0; i<secVertices()->size(); ++i) {
    if(!pfcand) continue;
    //if(!svs.at(i)) continue;                                                                                                                                                                             
    float tempdr_ = reco::deltaR(secVertices()->at(i),*pfcand);
    if (tempdr_<mindr_) { mindr_ = tempdr_; }

  }
  return mindr_;
}
