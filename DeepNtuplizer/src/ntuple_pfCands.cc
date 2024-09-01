/*
 * ntuple_pfCands.cc
 *
 *  Created on: 13 Feb 2017
 *      Author: jkiesele
 */


#include "../interface/ntuple_pfCands.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "../interface/sorting_modules.h"


#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"
#include "TVector3.h"


#include "DataFormats/GeometrySurface/interface/Line.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/GeomPropagators/interface/AnalyticalTrajectoryExtrapolatorToLine.h"
#include "TrackingTools/GeomPropagators/interface/AnalyticalImpactPointExtrapolator.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/Measurement1D.h"
#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"
#include "RecoVertex/VertexPrimitives/interface/ConvertToFromReco.h"

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
    // AS edm::ESHandle<TransientTrackBuilder> track_builder_;
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


void ntuple_pfCands::readSetup(const edm::EventSetup& iSetup){

    // AS iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", builder);
    
    builder = iSetup.getHandle(track_builder_token_);
    

}

void ntuple_pfCands::getInput(const edm::ParameterSet& iConfig){
	min_candidate_pt_ = (iConfig.getParameter<double>("minCandidatePt"));
    sort_cand_by_pt_ = iConfig.getParameter<bool>("sort_cand_by_pt");
    usePuppi_ = iConfig.getParameter<bool>("puppi");
}

void ntuple_pfCands::initBranches(TTree* tree){

    addBranch(tree,"n_Cpfcand", &n_Cpfcand_,"n_Cpfcand_/I");
    addBranch(tree,"nCpfcand", &nCpfcand_,"nCpfcand_/F");

    addBranch(tree,"Cpfcan_pt", &Cpfcan_pt_,"Cpfcan_pt_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_px", &Cpfcan_px_,"Cpfcan_px_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_py", &Cpfcan_py_,"Cpfcan_py_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_pz", &Cpfcan_pz_,"Cpfcan_pz_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_eta", &Cpfcan_eta_,"Cpfcan_eta_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_phi", &Cpfcan_phi_,"Cpfcan_phi_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_ptrel", &Cpfcan_ptrel_,"Cpfcan_ptrel_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_e", &Cpfcan_e_,"Cpfcan_e_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_erel", &Cpfcan_erel_,"Cpfcan_erel_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_phirel",&Cpfcan_phirel_,"Cpfcan_phirel_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_etarel",&Cpfcan_etarel_,"Cpfcan_etarel_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_deltaR",&Cpfcan_deltaR_,"Cpfcan_deltaR_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_puppiw",&Cpfcan_puppiw_,"Cpfcan_puppiw_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_dxy",&Cpfcan_dxy_,"Cpfcan_dxy_[n_Cpfcand_]/F");

    addBranch(tree,"Cpfcan_dxyerrinv",&Cpfcan_dxyerrinv_,"Cpfcan_dxyerrinv_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_dxysig",&Cpfcan_dxysig_,"Cpfcan_dxysig_[n_Cpfcand_]/F");

    addBranch(tree,"Cpfcan_dz",&Cpfcan_dz_,"Cpfcan_dz_[n_Cpfcand_]/F");

    addBranch(tree,"Cpfcan_VTX_ass",&Cpfcan_VTX_ass_,"Cpfcan_VTX_ass_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_firsthit",&Cpfcan_firsthit_,"Cpfcan_firsthit_[n_Cpfcand_]/F");

    addBranch(tree,"Cpfcan_fromPV",&Cpfcan_fromPV_,"Cpfcan_fromPV_[n_Cpfcand_]/F");

    addBranch(tree,"Cpfcan_drminsv",&Cpfcan_drminsv_,"Cpfcan_drminsv_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_distminsv",&Cpfcan_distminsv_,"Cpfcan_distminsv_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_distminsv2",&Cpfcan_distminsv2_,"Cpfcan_distminsv2_[n_Cpfcand_]/F");

    //commented ones don't work
    addBranch(tree,"Cpfcan_vertex_rho",&Cpfcan_vertex_rho_,"Cpfcan_vertex_rho_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_vertex_phirel",&Cpfcan_vertex_phirel_,"Cpfcan_vertex_phirel_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_vertex_etarel",&Cpfcan_vertex_etarel_,"Cpfcan_vertex_etarel_[n_Cpfcand_]/F");

    addBranch(tree,"Cpfcan_BtagPf_trackMomentum",&Cpfcan_BtagPf_trackMomentum_,"Cpfcan_BtagPf_trackMomentum_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackEta",&Cpfcan_BtagPf_trackEta_,"Cpfcan_BtagPf_trackEta_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackEtaRel",&Cpfcan_BtagPf_trackEtaRel_,"Cpfcan_BtagPf_trackEtaRel_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackPtRel",&Cpfcan_BtagPf_trackPtRel_,"Cpfcan_BtagPf_trackPtRel_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackPPar",&Cpfcan_BtagPf_trackPPar_,"Cpfcan_BtagPf_trackPPar_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackDeltaR",&Cpfcan_BtagPf_trackDeltaR_,"Cpfcan_BtagPf_trackDeltaR_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackPtRatio",&Cpfcan_BtagPf_trackPtRatio_,"Cpfcan_BtagPf_trackPtRatio_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackPParRatio",&Cpfcan_BtagPf_trackPParRatio_,"Cpfcan_BtagPf_trackPParRatio[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackSip3dVal",&Cpfcan_BtagPf_trackSip3dVal_,"Cpfcan_BtagPf_trackSip3dVal_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackSip3dSig",&Cpfcan_BtagPf_trackSip3dSig_,"Cpfcan_BtagPf_trackSip3dSig_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackSip2dVal",&Cpfcan_BtagPf_trackSip2dVal_,"Cpfcan_BtagPf_trackSip2dVal_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackSip2dSig",&Cpfcan_BtagPf_trackSip2dSig_,"Cpfcan_BtagPf_trackSip2dSig_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackDecayLen",&Cpfcan_BtagPf_trackDecayLen_,"Cpfcan_BtagPf_trackDecayLen_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackJetDistVal",&Cpfcan_BtagPf_trackJetDistVal_,"Cpfcan_BtagPf_trackJetDistVal_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackJetDistSig",&Cpfcan_BtagPf_trackJetDistSig_,"Cpfcan_BtagPf_trackJetDistSig_[n_Cpfcand_]/F");

    addBranch(tree,"Cpfcan_isMu",&Cpfcan_isMu_,"Cpfcan_isMu_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_isEl",&Cpfcan_isEl_,"Cpfcan_isEl_[n_Cpfcand_]/F");
    // did not give integers !!
    addBranch(tree,"Cpfcan_charge",&Cpfcan_charge_,"Cpfcan_charge_[n_Cpfcand_]/F");

    //in16 conversion broken
    addBranch(tree,"Cpfcan_lostInnerHits",&Cpfcan_lostInnerHits_,"Cpfcan_lostInnerHits_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_numberOfPixelHits",&Cpfcan_numberOfPixelHits_,"Cpfcan_numberOfPixelHits_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_numberOfStripHits",&Cpfcan_numberOfStripHits_,"Cpfcan_numberOfStripHits_[n_Cpfcand_]/F");

    addBranch(tree,"Cpfcan_chi2",&Cpfcan_chi2_,"Cpfcan_chi2_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_quality",&Cpfcan_quality_,"Cpfcan_quality_[n_Cpfcand_]/F");

    //Neutral Pf candidates
    addBranch(tree,"n_Npfcand", &n_Npfcand_,"n_Npfcand_/I");
    addBranch(tree,"nNpfcand", &nNpfcand_,"nNpfcand/F");

    addBranch(tree,"Npfcan_pt", &Npfcan_pt_,"Npfcan_pt_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_px", &Npfcan_px_,"Npfcan_px_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_py", &Npfcan_py_,"Npfcan_py_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_pz", &Npfcan_pz_,"Npfcan_pz_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_eta", &Npfcan_eta_,"Npfcan_eta_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_phi", &Npfcan_phi_,"Npfcan_phi_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_ptrel", &Npfcan_ptrel_,"Npfcan_ptrel_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_e", &Npfcan_e_,"Npfcan_e_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_erel", &Npfcan_erel_,"Npfcan_erel_[n_Npfcand_]/F");

    addBranch(tree,"Npfcan_puppiw", &Npfcan_puppiw_,"Npfcan_puppiw_[n_Npfcand_]/F");

    addBranch(tree,"Npfcan_phirel",&Npfcan_phirel_,"Npfcan_phirel_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_etarel",&Npfcan_etarel_,"Npfcan_etarel_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_deltaR",&Npfcan_deltaR_,"Npfcan_deltaR_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_isGamma",&Npfcan_isGamma_,"Npfcan_isGamma_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_HadFrac",&Npfcan_HadFrac_,"Npfcan_HadFrac_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_CaloFrac",&Npfcan_CaloFrac_,"Npfcan_CaloFrac_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_drminsv",&Npfcan_drminsv_,"Npfcan_drminsv_[n_Npfcand_]/F");

    addBranch(tree,"Npfcan_pdgID",&Npfcan_pdgID_,"Npfcan_pdgID_[n_Npfcand_]/F");
    addBranch(tree,"Cpfcan_pdgID",&Cpfcan_pdgID_,"Cpfcan_pdgID_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_pdg",&Cpfcan_pdg_,"Cpfcan_pdg_[n_Cpfcand_]/F");

    addBranch(tree,"Cpfcan_HadFrac",&Cpfcan_HadFrac_,"Cpfcan_HadFrac_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_CaloFrac",&Cpfcan_CaloFrac_,"Cpfcan_CaloFrac_[n_Cpfcand_]/F");

    addBranch(tree,"Cpfcan_tau_signal",&Cpfcan_tau_signal_,"Cpfcan_tau_signal_[n_Cpfcand_]/F");
    addBranch(tree,"Npfcan_tau_signal",&Npfcan_tau_signal_,"Npfcan_tau_signal_[n_Npfcand_]/F");
}

void ntuple_pfCands::readEvent(const edm::Event& iEvent){


    n_Npfcand_=0;
    n_Cpfcand_=0;

}

//use either of these functions

bool ntuple_pfCands::fillBranches(const pat::Jet & unsubjet, const pat::Jet & jet, const size_t& jetidx, const  edm::View<pat::Jet> * coll){

    float etasign = 1.;
    if (jet.eta()<0) etasign =-1.;
    math::XYZVector jetDir = jet.momentum().Unit();
    GlobalVector jetRefTrackDir(jet.px(),jet.py(),jet.pz());
    const reco::Vertex & pv = vertices()->at(0);

    std::vector<sorting::sortingClass<size_t> > sortedcharged, sortedneutrals;

    const float jet_uncorr_pt=jet.correctedJet("Uncorrected").pt();
    const float jet_uncorr_e=jet.correctedJet("Uncorrected").energy();

    // tau signal candidates
    float min_pt_for_taus_ = 5.0;
    float max_eta_for_taus_ = 2.5;
      
    std::vector<math::XYZTLorentzVector> tau_pfcandidates;
    const auto taus = Taus();
    for (size_t itau = 0; itau < taus->size(); itau++) {
      if (taus->at(itau).pt() < min_pt_for_taus_)
	continue;
      if (fabs(taus->at(itau).eta()) > max_eta_for_taus_)
	continue;
      for (unsigned ipart = 0; ipart < taus->at(itau).signalCands().size(); ipart++) {
	const pat::PackedCandidate *pfcand =
          dynamic_cast<const pat::PackedCandidate *>(taus->at(itau).signalCands()[ipart].get());
	tau_pfcandidates.push_back(pfcand->p4());
      }
    }

    TrackInfoBuilder trackinfo(builder);
    //create collection first, to be able to do some sorting
    for (unsigned int i = 0; i <  unsubjet.numberOfDaughters(); i++){
        const pat::PackedCandidate* PackedCandidate = dynamic_cast<const pat::PackedCandidate*>(unsubjet.daughter(i));
        if(PackedCandidate){
            if(PackedCandidate->pt() < min_candidate_pt_) continue; 
            if(PackedCandidate->charge()!=0){
                trackinfo.buildTrackInfo(PackedCandidate,jetDir,jetRefTrackDir,pv);
                if (sort_cand_by_pt_)
                  sortedcharged.push_back(sorting::sortingClass<size_t>
                  (i, PackedCandidate->pt()/jet_uncorr_pt,
                          trackinfo.getTrackSip2dSig(), -mindrsvpfcand(PackedCandidate)));
                else
                  sortedcharged.push_back(sorting::sortingClass<size_t>
                  (i, trackinfo.getTrackSip2dSig(),
                          -mindrsvpfcand(PackedCandidate), PackedCandidate->pt()/jet_uncorr_pt));
            }
            else{
                if (sort_cand_by_pt_)
                  sortedneutrals.push_back(sorting::sortingClass<size_t>
                  (i, PackedCandidate->pt()/jet_uncorr_pt, -mindrsvpfcand(PackedCandidate), -1));
                else
                  sortedneutrals.push_back(sorting::sortingClass<size_t>
                  (i, -1, -mindrsvpfcand(PackedCandidate), PackedCandidate->pt()/jet_uncorr_pt));
            }
        }
    }
		std::sort(sortedcharged.begin(),sortedcharged.end(),sorting::sortingClass<size_t>::compareByABCInv);
    n_Cpfcand_ = std::min(sortedcharged.size(),max_pfcand_);

    std::sort(sortedneutrals.begin(),sortedneutrals.end(),sorting::sortingClass<size_t>::compareByABCInv);
    std::vector<size_t> sortedchargedindices,sortedneutralsindices;
    n_Npfcand_ = std::min(sortedneutrals.size(),max_pfcand_);
		sortedchargedindices=sorting::invertSortingVector(sortedcharged);
		sortedneutralsindices=sorting::invertSortingVector(sortedneutrals);

    for (unsigned int i = 0; i <  unsubjet.numberOfDaughters(); i++){
        const pat::PackedCandidate* PackedCandidate_ = dynamic_cast<const pat::PackedCandidate*>(unsubjet.daughter(i));
        //const auto& PackedCandidate_=s.get();
        if(!PackedCandidate_) continue;
        if(PackedCandidate_->pt() < min_candidate_pt_) continue; 

        // get the dr with the closest sv
        float drminpfcandsv_ = mindrsvpfcand(PackedCandidate_);

	float pdgid_;
	if (abs(PackedCandidate_->pdgId()) == 11 and PackedCandidate_->charge() != 0){
	  pdgid_ = 0.0;
	}
	else if (abs(PackedCandidate_->pdgId()) == 13 and PackedCandidate_->charge() != 0){
	  pdgid_ = 1.0;
	}
	else if (abs(PackedCandidate_->pdgId()) == 22 and PackedCandidate_->charge() == 0){
	  pdgid_ = 2.0;
	}
	else if (abs(PackedCandidate_->pdgId()) != 22 and PackedCandidate_->charge() == 0 and abs(PackedCandidate_->pdgId()) != 1 and abs(PackedCandidate_->pdgId()) != 2){
	  pdgid_ = 3.0;
	}
	else if (abs(PackedCandidate_->pdgId()) != 11 and abs(PackedCandidate_->pdgId()) != 13 and PackedCandidate_->charge() != 0){
	  pdgid_ = 4.0;
	}
	else if (PackedCandidate_->charge() == 0 and abs(PackedCandidate_->pdgId()) == 1){
	  pdgid_ = 5.0;
	}
	else if (PackedCandidate_->charge() == 0 and abs(PackedCandidate_->pdgId()) == 2){
	  pdgid_ = 6.0;
	}
	else{
	  pdgid_ = 7.0;
	}

        if(PackedCandidate_->charge()!=0 ){

            size_t fillntupleentry= sortedchargedindices.at(i);
            if(fillntupleentry>=max_pfcand_) continue;


            Cpfcan_pdgID_[fillntupleentry] = pdgid_;
	    Cpfcan_pdg_[fillntupleentry] = abs(PackedCandidate_->pdgId());
            Cpfcan_pt_[fillntupleentry] = PackedCandidate_->pt();
            Cpfcan_px_[fillntupleentry] = PackedCandidate_->px();
            Cpfcan_py_[fillntupleentry] = PackedCandidate_->py();
            Cpfcan_pz_[fillntupleentry] = PackedCandidate_->pz();
            Cpfcan_eta_[fillntupleentry] = PackedCandidate_->eta();
            Cpfcan_phi_[fillntupleentry] = PackedCandidate_->phi();
            Cpfcan_ptrel_[fillntupleentry] = catchInfsAndBound(PackedCandidate_->pt()/jet_uncorr_pt,0,-1,0,-1);
            Cpfcan_erel_[fillntupleentry] = catchInfsAndBound(PackedCandidate_->energy()/jet_uncorr_e,0,-1,0,-1);
            Cpfcan_e_[fillntupleentry] = PackedCandidate_->energy();
            Cpfcan_phirel_[fillntupleentry] = catchInfsAndBound(fabs(reco::deltaPhi(PackedCandidate_->phi(),jet.phi())),0,-2,0,-0.5);
            Cpfcan_etarel_[fillntupleentry] = catchInfsAndBound(fabs(PackedCandidate_->eta()-jet.eta()),0,-2,0,-0.5);
            Cpfcan_deltaR_[fillntupleentry] =catchInfsAndBound(reco::deltaR(*PackedCandidate_,jet),0,-0.6,0,-0.6);
            Cpfcan_dxy_[fillntupleentry] = catchInfsAndBound(fabs(PackedCandidate_->dxy()),0,-50,50);
	    Cpfcan_firsthit_[fillntupleentry] = PackedCandidate_->firstHit();

            Cpfcan_dxyerrinv_[fillntupleentry]= PackedCandidate_->hasTrackDetails() ? catchInfsAndBound(1/PackedCandidate_->dxyError(),0,-1, 10000.) : -1;

            Cpfcan_dxysig_[fillntupleentry]= PackedCandidate_->hasTrackDetails() ? catchInfsAndBound(fabs(PackedCandidate_->dxy()/PackedCandidate_->dxyError()),0.,-2000,2000) : 0.;


            Cpfcan_dz_[fillntupleentry] = PackedCandidate_->dz();
            Cpfcan_VTX_ass_[fillntupleentry] = PackedCandidate_->pvAssociationQuality();

            Cpfcan_fromPV_[fillntupleentry] = PackedCandidate_->fromPV();

            float tempdontopt=PackedCandidate_->vx();
            tempdontopt++;

            Cpfcan_vertexChi2_[fillntupleentry]=PackedCandidate_->vertexChi2();
            Cpfcan_vertexNdof_[fillntupleentry]=PackedCandidate_->vertexNdof();

            Cpfcan_CaloFrac_[fillntupleentry] = PackedCandidate_->caloFraction();
            Cpfcan_HadFrac_[fillntupleentry] = PackedCandidate_->hcalFraction();

            //divided
            Cpfcan_vertexNormalizedChi2_[fillntupleentry]=PackedCandidate_->vertexNormalizedChi2();
            Cpfcan_vertex_rho_[fillntupleentry]=catchInfsAndBound(PackedCandidate_->vertex().rho(),0,-1,50);
            Cpfcan_vertex_phirel_[fillntupleentry]=reco::deltaPhi(PackedCandidate_->vertex().phi(),jet.phi());
            Cpfcan_vertex_etarel_[fillntupleentry]=etasign*(PackedCandidate_->vertex().eta()-jet.eta());
            Cpfcan_vertexRef_mass_[fillntupleentry]=PackedCandidate_->vertexRef()->p4().M();


            Cpfcan_puppiw_[fillntupleentry] = usePuppi_ ? PackedCandidate_->puppiWeight() : 1.0;

            trackinfo.buildTrackInfo(PackedCandidate_,jetDir,jetRefTrackDir,pv);

	    const reco::TransientTrack ttrack = trackinfo.getTTrack();
	    float mindistsv = mindistsvpfcand(ttrack);
	    float eng_mindistsv = std::log(std::fabs(mindistsv)+1.0);

	    Cpfcan_distminsv_[fillntupleentry] = mindistsv;
	    Cpfcan_distminsv2_[fillntupleentry] = eng_mindistsv;


            Cpfcan_BtagPf_trackMomentum_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackMomentum(),0,0 ,1000);
            Cpfcan_BtagPf_trackEta_[fillntupleentry]        =catchInfsAndBound(trackinfo.getTrackEta()   ,  0,-5,5);
            Cpfcan_BtagPf_trackEtaRel_[fillntupleentry]     =catchInfsAndBound(trackinfo.getTrackEtaRel(),  0,-5,15);
            Cpfcan_BtagPf_trackPtRel_[fillntupleentry]      =catchInfsAndBound(trackinfo.getTrackPtRel(),   0,-1,4);
            Cpfcan_BtagPf_trackPPar_[fillntupleentry]       =catchInfsAndBound(trackinfo.getTrackPPar(),    0,-1e5,1e5 );
            Cpfcan_BtagPf_trackDeltaR_[fillntupleentry]     =catchInfsAndBound(trackinfo.getTrackDeltaR(),  0,-5,5 );
            Cpfcan_BtagPf_trackPtRatio_[fillntupleentry]    =catchInfsAndBound(trackinfo.getTrackPtRatio(), 0,-1,10 );
            Cpfcan_BtagPf_trackPParRatio_[fillntupleentry]  =catchInfsAndBound(trackinfo.getTrackPParRatio(),0,-10,100);
            Cpfcan_BtagPf_trackSip3dVal_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackSip3dVal(), 0, -1,1e5 );
            Cpfcan_BtagPf_trackSip3dSig_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackSip3dSig(), 0, -1,4e4 );
            Cpfcan_BtagPf_trackSip2dVal_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackSip2dVal(), 0, -1,70 );
            Cpfcan_BtagPf_trackSip2dSig_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackSip2dSig(), 0, -1,4e4 );
            Cpfcan_BtagPf_trackDecayLen_[fillntupleentry]   =trackinfo.getTrackJetDecayLen();
            Cpfcan_BtagPf_trackJetDistVal_[fillntupleentry] =catchInfsAndBound(trackinfo.getTrackJetDistVal(),0,-20,1 );
            Cpfcan_BtagPf_trackJetDistSig_[fillntupleentry] =catchInfsAndBound(trackinfo.getTrackJetDistSig(),0,-1,1e5 );

            // TO DO: we can do better than that by including reco::muon informations
            Cpfcan_isMu_[fillntupleentry] = 0;
            if(abs(PackedCandidate_->pdgId())==13) {
                Cpfcan_isMu_[fillntupleentry] = 1;
            }
            // TO DO: we can do better than that by including reco::electron informations
            Cpfcan_isEl_[fillntupleentry] = 0;
            if(abs(PackedCandidate_->pdgId())==11) {
                Cpfcan_isEl_[fillntupleentry] = 1;
            }

	    // tau specific prior to any puppi weight application
	    if (std::find(tau_pfcandidates.begin(), tau_pfcandidates.end(), PackedCandidate_->p4()) != tau_pfcandidates.end())
	      Cpfcan_tau_signal_[fillntupleentry] = 1.0;
	    else
	      Cpfcan_tau_signal_[fillntupleentry] = 0.0;
	    
	    float cand_charge_ = PackedCandidate_->charge();
            Cpfcan_charge_[fillntupleentry] = cand_charge_;
            Cpfcan_lostInnerHits_[fillntupleentry] = catchInfs(PackedCandidate_->lostInnerHits(),2);
	    Cpfcan_numberOfPixelHits_[fillntupleentry] = catchInfs(PackedCandidate_->numberOfPixelHits(),-1);
	    Cpfcan_numberOfStripHits_[fillntupleentry] = catchInfs(PackedCandidate_->stripLayersWithMeasurement(),-1);

	    Cpfcan_chi2_[fillntupleentry] = PackedCandidate_->hasTrackDetails() ? \
	      catchInfsAndBound(PackedCandidate_->pseudoTrack().normalizedChi2(),300,-1,300) : -1;
			//for some reason this returns the quality enum not a mask.
	    Cpfcan_quality_[fillntupleentry] = PackedCandidate_->hasTrackDetails() ? 
	      PackedCandidate_->pseudoTrack().qualityMask() : (1 << reco::TrackBase::loose);
            Cpfcan_drminsv_[fillntupleentry] = catchInfsAndBound(drminpfcandsv_,0,-0.4,0,-0.4);
        }
        else{// neutral candidates

            size_t fillntupleentry= sortedneutralsindices.at(i);
            if(fillntupleentry>=max_pfcand_) continue;

            Npfcan_pt_[fillntupleentry] = PackedCandidate_->pt();
            Npfcan_px_[fillntupleentry] = PackedCandidate_->px();
            Npfcan_py_[fillntupleentry] = PackedCandidate_->py();
            Npfcan_pz_[fillntupleentry] = PackedCandidate_->pz();
            Npfcan_eta_[fillntupleentry] = PackedCandidate_->eta();
            Npfcan_phi_[fillntupleentry] = PackedCandidate_->phi();
            Npfcan_ptrel_[fillntupleentry] = catchInfsAndBound(PackedCandidate_->pt()/jet_uncorr_pt,0,-1,0,-1);
            Npfcan_erel_[fillntupleentry] = catchInfsAndBound(PackedCandidate_->energy()/jet_uncorr_e,0,-1,0,-1);
            Npfcan_e_[fillntupleentry] = PackedCandidate_->energy();
            Npfcan_puppiw_[fillntupleentry] = usePuppi_ ? PackedCandidate_->puppiWeight() : 1.0;
            Npfcan_phirel_[fillntupleentry] = catchInfsAndBound(fabs(reco::deltaPhi(PackedCandidate_->phi(),jet.phi())),0,-2,0,-0.5);
            Npfcan_etarel_[fillntupleentry] = catchInfsAndBound(fabs(PackedCandidate_->eta()-jet.eta()),0,-2,0,-0.5);
            Npfcan_deltaR_[fillntupleentry] = catchInfsAndBound(reco::deltaR(*PackedCandidate_,jet),0,-0.6,0,-0.6);
            Npfcan_isGamma_[fillntupleentry] = 0;
            if(fabs(PackedCandidate_->pdgId())==22)  Npfcan_isGamma_[fillntupleentry] = 1;
            Npfcan_CaloFrac_[fillntupleentry] = PackedCandidate_->caloFraction();
            Npfcan_HadFrac_[fillntupleentry] = PackedCandidate_->hcalFraction();
            Npfcan_pdgID_[fillntupleentry] = pdgid_;

            Npfcan_drminsv_[fillntupleentry] = catchInfsAndBound(drminpfcandsv_,0,-0.4,0,-0.4);

        }
    }

    nCpfcand_=n_Cpfcand_;
    nNpfcand_=n_Npfcand_;

    return true; //for making cuts
}


float ntuple_pfCands::mindrsvpfcand(const pat::PackedCandidate* pfcand) {

    float mindr_ = jetradius_;
    for (unsigned int i=0; i<secVertices()->size(); ++i) {
        if(!pfcand) continue;
        //if(!svs.at(i)) continue;
        float tempdr_ = reco::deltaR(secVertices()->at(i),*pfcand);
        if (tempdr_<mindr_) { mindr_ = tempdr_; }

    }
    return mindr_;
}

float ntuple_pfCands::mindistsvpfcand(const reco::TransientTrack track) {

  float mindist_ = 999.999;
  float out_dist = 0.0;
  for (unsigned int i=0; i<secVertices()->size(); ++i) {
    if (!track.isValid()) {continue;}
    reco::Vertex::CovarianceMatrix csv; secVertices()->at(i).fillVertexCovariance(csv);
    reco::Vertex vertex(secVertices()->at(i).vertex(), csv);
    if (!vertex.isValid()) {continue;}

    GlobalVector direction(secVertices()->at(i).px(),secVertices()->at(i).py(),secVertices()->at(i).pz());


    AnalyticalImpactPointExtrapolator extrapolator(track.field());
    TrajectoryStateOnSurface tsos =  extrapolator.extrapolate(track.impactPointState(), RecoVertex::convertPos(vertex.position()));
    
    VertexDistance3D dist;

    if (!tsos.isValid()) {continue;}
    GlobalPoint refPoint = tsos.globalPosition();
    GlobalError refPointErr = tsos.cartesianError().position();
    GlobalPoint vertexPosition = RecoVertex::convertPos(vertex.position());
    GlobalError vertexPositionErr = RecoVertex::convertError(vertex.error());

    std::pair<bool, Measurement1D> result(true, dist.distance(VertexState(vertexPosition, vertexPositionErr), VertexState(refPoint, refPointErr)));
    if (!result.first) {continue;}

    GlobalPoint impactPoint = tsos.globalPosition();
    GlobalVector IPVec(impactPoint.x() - vertex.x(), impactPoint.y() - vertex.y(), impactPoint.z() - vertex.z());
    double prod = IPVec.dot(direction);
    double sign = (prod >= 0) ? 1. : -1.;

    if(result.second.value() < mindist_){
      float inv_dist = 1.0 / (sign * result.second.value() + 0.001);
      out_dist = inv_dist;
      mindist_ = result.second.value();
    } 
  }
  return out_dist;
}
