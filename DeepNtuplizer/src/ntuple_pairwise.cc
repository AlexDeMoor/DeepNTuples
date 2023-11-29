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
	trackSip2dSig_(0),
	ttrack_(0)

{

}

    void buildTrackInfo(const pat::PackedCandidate* PackedCandidate_ ,const math::XYZVector&  jetDir, GlobalVector refjetdirection, const reco::Vertex & pv){
      TVector3 jetDir3(jetDir.x(),jetDir.y(),jetDir.z());
      if(!PackedCandidate_->hasTrackDetails()) {
	trackSip2dSig_=0.;
	return;
      }
        const reco::Track & PseudoTrack =  PackedCandidate_->pseudoTrack();
        reco::TransientTrack transientTrack;
        transientTrack=builder->build(PseudoTrack);
	ttrack_ = transientTrack;

	Measurement1D meas_ip2d=IPTools::signedTransverseImpactParameter(transientTrack, refjetdirection, pv).second;
	trackSip2dSig_=(meas_ip2d.significance());
    }

    const reco::TransientTrack getTTrack() const {return ttrack_;}
    const float& getTrackSip2dSig() const {return trackSip2dSig_;}

private:

    edm::ESHandle<TransientTrackBuilder>& builder;
    float trackSip2dSig_;
    reco::TransientTrack ttrack_;

};

void ntuple_pairwise::readConfig(const edm::ParameterSet& iConfig, edm::ConsumesCollector&& cc) {
  packedToken_ = cc.consumes<std::vector<pat::PackedGenParticle>>(iConfig.getParameter<edm::InputTag>("packed"));
}


void ntuple_pairwise::readSetup(const edm::EventSetup& iSetup){

    iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", builder);

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

    iEvent.getByToken(packedToken_, packed);
    n_Npfcand2_=0;
    n_Cpfcand2_=0;

}

//use either of these functions

bool ntuple_pairwise::fillBranches(const pat::Jet & jet, const size_t& jetidx, const  edm::View<pat::Jet> * coll){

    math::XYZVector jetDir = jet.momentum().Unit();
    GlobalVector jetRefTrackDir(jet.px(),jet.py(),jet.pz());
    const reco::Vertex & pv = vertices()->at(0);

    std::vector<sorting::sortingClass<size_t> > sortedcharged, sortedneutrals;

    const float jet_uncorr_pt=jet.correctedJet("Uncorrected").pt();
    const float jet_uncorr_e=jet.correctedJet("Uncorrected").energy();

    TrackInfoBuilder trackinfo(builder);
    //create collection first, to be able to do some sorting
    for (unsigned int i = 0; i <  jet.numberOfDaughters(); i++){
        const pat::PackedCandidate* PackedCandidate = dynamic_cast<const pat::PackedCandidate*>(jet.daughter(i));
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

    std::vector<int> pf_packed_match;

    size_t counter = 0;
    int n_cpf_ = std::min((int)25, n_Cpfcand2_);

    for (int k = 0; k <  n_cpf_; k++){
      int ind = sortedcharged.at(k).get();
      const pat::PackedCandidate* Part_  = dynamic_cast<const pat::PackedCandidate*>(jet.daughter(ind));

      double dR_min=pow10(6);
      int index=-1;
      int i=0;

      for (const auto &packed_part : *packed){
	double dR = reco::deltaR(*Part_, packed_part);
	double dpt = std::abs((Part_->pt()- packed_part.pt())/Part_->pt());
	if(dR<0.01 && dpt<0.1 && Part_->charge()==packed_part.charge() && dR<dR_min){
	  index=i;
	}
	i++;
      }
      pf_packed_match.push_back(index);
    }

    for (int i = 0; i <  n_cpf_; i++){
      for (int j = 0; j < i; j++){
	
	deepntuples::TrackPairInfoBuilder trkpairinfo;
	int ind_i = sortedcharged.at(i).get();
	int ind_j = sortedcharged.at(j).get();
	const pat::PackedCandidate* Part_i_  = dynamic_cast<const pat::PackedCandidate*>(jet.daughter(ind_i));
	
	if(!Part_i_){
	  std::cout << i << " Bug PackedCandidate " << j << std::endl;
	}
	const pat::PackedCandidate* Part_j_  = dynamic_cast<const pat::PackedCandidate*>(jet.daughter(ind_j));
	if(!Part_j_){
	  std::cout << i << " Bug PackedCandidate " << j << std::endl;
	}
	  
	trackinfo.buildTrackInfo(Part_i_,jetDir,jetRefTrackDir,pv);
	const reco::TransientTrack it = trackinfo.getTTrack();
	trackinfo.buildTrackInfo(Part_j_,jetDir,jetRefTrackDir,pv);
	const reco::TransientTrack tt = trackinfo.getTTrack();
	
	trkpairinfo.buildTrackPairInfo(it,tt,vertices()->at(0),jet);

	float dist_vtx_12 = -1;

	int packed_index1 = pf_packed_match[i];
	int packed_index2 = pf_packed_match[j];
	if (packed_index1 != -1 && packed_index2 != -1){
	  const reco::Candidate * pruned_part_match1 = (*packed)[packed_index1].lastPrunedRef().get();
	  const reco::Candidate * pruned_part_match2 = (*packed)[packed_index2].lastPrunedRef().get();
	  dist_vtx_12 = sqrt((pruned_part_match1->vertex()- pruned_part_match2->vertex()).mag2());
	}

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
    float tempdr_ = reco::deltaR(secVertices()->at(i),*pfcand);
    if (tempdr_<mindr_) { mindr_ = tempdr_; }

  }
  return mindr_;
}
