/*
 * ntuple_LT.cc
 *
 *  Created on: 28 Mai 2023
 *      Author: Alexandre De Moor
 */


#include "../interface/ntuple_LT.h"
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

void ntuple_LT::readSetup(const edm::EventSetup& iSetup){
  builder = iSetup.getHandle(track_builder_token_);
}

void ntuple_LT::getInput(const edm::ParameterSet& iConfig){
  min_candidate_pt_ = (iConfig.getParameter<double>("minCandidatePt"));
  sort_cand_by_pt_ = iConfig.getParameter<bool>("sort_cand_by_pt");
}

void ntuple_LT::initBranches(TTree* tree){

  addBranch(tree,"n_LTcand", &n_LTcand_,"n_LTcand_/I");
  addBranch(tree,"n_LT", &n_LT_,"n_LT_/I");

  addBranch(tree,"LT_pt", &LT_pt_,"LT_pt_[n_LTcand_]/F");
  addBranch(tree,"LT_eta", &LT_eta_,"LT_eta_[n_LTcand_]/F");
  addBranch(tree,"LT_phi", &LT_phi_,"LT_phi_[n_LTcand_]/F");
  addBranch(tree,"LT_e", &LT_e_,"LT_e_[n_LTcand_]/F");

  addBranch(tree,"LT_puppiw",&LT_puppiw_,"LT_puppiw_[n_LTcand_]/F");
  addBranch(tree,"LT_dz",&LT_dz_,"LT_dz_[n_LTcand_]/F");

  addBranch(tree,"LT_drminsv",&LT_drminsv_,"LT_drminsv_[n_LTcand_]/F");
  addBranch(tree,"LT_distminsvold",&LT_distminsvold_,"LT_distminsvold_[n_LTcand_]/F");
  addBranch(tree,"LT_distminsv",&LT_distminsv_,"LT_distminsv_[n_LTcand_]/F");
  addBranch(tree,"LT_distminsv2",&LT_distminsv2_,"LT_distminsv2_[n_LTcand_]/F");
  
  addBranch(tree,"LT_BtagPf_trackEtaRel",&LT_BtagPf_trackEtaRel_,"LT_BtagPf_trackEtaRel_[n_LTcand_]/F");
  addBranch(tree,"LT_BtagPf_trackPtRel",&LT_BtagPf_trackPtRel_,"LT_BtagPf_trackPtRel_[n_LTcand_]/F");
  addBranch(tree,"LT_BtagPf_trackPPar",&LT_BtagPf_trackPPar_,"LT_BtagPf_trackPPar_[n_LTcand_]/F");
  addBranch(tree,"LT_BtagPf_trackDeltaR",&LT_BtagPf_trackDeltaR_,"LT_BtagPf_trackDeltaR_[n_LTcand_]/F");
  addBranch(tree,"LT_BtagPf_trackPParRatio",&LT_BtagPf_trackPParRatio_,"LT_BtagPf_trackPParRatio[n_LTcand_]/F");
  addBranch(tree,"LT_BtagPf_trackSip3dVal",&LT_BtagPf_trackSip3dVal_,"LT_BtagPf_trackSip3dVal_[n_LTcand_]/F");
  addBranch(tree,"LT_BtagPf_trackSip3dSig",&LT_BtagPf_trackSip3dSig_,"LT_BtagPf_trackSip3dSig_[n_LTcand_]/F");
  addBranch(tree,"LT_BtagPf_trackSip2dVal",&LT_BtagPf_trackSip2dVal_,"LT_BtagPf_trackSip2dVal_[n_LTcand_]/F");
  addBranch(tree,"LT_BtagPf_trackSip2dSig",&LT_BtagPf_trackSip2dSig_,"LT_BtagPf_trackSip2dSig_[n_LTcand_]/F");
  addBranch(tree,"LT_BtagPf_trackDecayLen",&LT_BtagPf_trackDecayLen_,"LT_BtagPf_trackDecayLen_[n_LTcand_]/F");
  addBranch(tree,"LT_BtagPf_trackJetDistVal",&LT_BtagPf_trackJetDistVal_,"LT_BtagPf_trackJetDistVal_[n_LTcand_]/F");
  
  addBranch(tree,"LT_charge",&LT_charge_,"LT_charge_[n_LTcand_]/F");
  addBranch(tree,"LT_chi2",&LT_chi2_,"LT_chi2_[n_LTcand_]/F");
  addBranch(tree,"LT_quality",&LT_quality_,"LT_quality_[n_LTcand_]/F");
  
  addBranch(tree,"LT_lostInnerHits",&LT_lostInnerHits_,"LT_lostInnerHits_[n_LTcand_]/F");
  addBranch(tree,"LT_numberOfPixelHits",&LT_numberOfPixelHits_,"LT_numberOfPixelHits_[n_LTcand_]/F");
  addBranch(tree,"LT_numberOfStripHits",&LT_numberOfStripHits_,"LT_numberOfStripHits_[n_LTcand_]/F");
  
  addBranch(tree,"LT_pdgID",&LT_pdgID_,"LT_pdgID_[n_LTcand_]/F");
  addBranch(tree,"LT_HadFrac",&LT_HadFrac_,"LT_HadFrac_[n_LTcand_]/F");
  addBranch(tree,"LT_CaloFrac",&LT_CaloFrac_,"LT_CaloFrac_[n_LTcand_]/F");
  
}

void ntuple_LT::readEvent(const edm::Event& iEvent){

    iEvent.getByToken(ltToken_, LTs);
    n_LTcand_=0;

}

//use either of these functions
bool ntuple_LT::fillBranches(const pat::Jet & jet, const size_t& jetidx, const  edm::View<pat::Jet> * coll){

    math::XYZVector jetDir = jet.momentum().Unit();
    GlobalVector jetRefTrackDir(jet.px(),jet.py(),jet.pz());
    const reco::Vertex & pv = vertices()->at(0);

    std::vector<sorting::sortingClass<size_t> > sortedcharged;

    const float jet_uncorr_pt=jet.correctedJet("Uncorrected").pt();

    TrackInfoBuilder trackinfo(builder);
    int n_lts = 0;

    std::vector<reco::CandidatePtr> cpfPtrs;

    for (size_t i = 0; i < LTs->size(); ++i) {
      auto cand = LTs->ptrAt(i);
      if ((reco::deltaR(*cand, jet) < 0.2)) {
	const auto *PackedCandidate = dynamic_cast<const pat::PackedCandidate*>(&(*cand));
	if(PackedCandidate){
	  if(PackedCandidate->pt() < 1.0) continue; 
	  trackinfo.buildTrackInfo(PackedCandidate,jetDir,jetRefTrackDir,pv);
      if (sort_cand_by_pt_)
        sortedcharged.push_back(sorting::sortingClass<size_t>
                    (i, PackedCandidate->pt()/jet_uncorr_pt,
                     trackinfo.getTrackSip2dSig(), -mindrsvltcand(PackedCandidate)));
      else
	    sortedcharged.push_back(sorting::sortingClass<size_t>
				    (i, trackinfo.getTrackSip2dSig(),
				     -mindrsvltcand(PackedCandidate), PackedCandidate->pt()/jet_uncorr_pt));
	  cpfPtrs.push_back(cand);
	  n_lts++;
	}
      }
    }

    std::sort(sortedcharged.begin(),sortedcharged.end(),sorting::sortingClass<size_t>::compareByABCInv);
    n_LTcand_ = std::min(sortedcharged.size(),max_ltcand_);

    std::vector<size_t> sortedchargedindices;
    sortedchargedindices=sorting::invertSortingVector(sortedcharged);

    for (unsigned int i = 0; i <  (unsigned int)n_LTcand_; i++){
      const auto *PackedCandidate_ = dynamic_cast<const pat::PackedCandidate*>(&(*cpfPtrs.at(i)));
      //const auto& PackedCandidate_=s.get();
      if(!PackedCandidate_) continue;
      if(PackedCandidate_->pt() < min_candidate_pt_) continue; 

        // get the dr with the closest sv
      float drminltcandsv_ = mindrsvltcand(PackedCandidate_);

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

      /// This might include more than PF candidates, e.g. Reco muons and could
      /// be double counting. Needs to be checked.!!!!
      ///
      /// Split to charged and neutral candidates
      if(PackedCandidate_->charge()!=0 ){

    size_t fillntupleentry= sortedchargedindices.at(sortedcharged[i].get());
	if(fillntupleentry>=(int)max_ltcand_) continue;

	LT_pt_[fillntupleentry] = PackedCandidate_->pt();
	LT_eta_[fillntupleentry] = PackedCandidate_->eta();
	LT_phi_[fillntupleentry] = PackedCandidate_->phi();
	LT_e_[fillntupleentry] = PackedCandidate_->energy();

	LT_puppiw_[fillntupleentry] = PackedCandidate_->puppiWeight();
	LT_dz_[fillntupleentry] = PackedCandidate_->dz();
	LT_VTX_ass_[fillntupleentry] = PackedCandidate_->pvAssociationQuality();

	float tempdontopt=PackedCandidate_->vx();
	tempdontopt++;

	LT_pdgID_[fillntupleentry] = pdgid_;
	LT_CaloFrac_[fillntupleentry] = PackedCandidate_->caloFraction();
	LT_HadFrac_[fillntupleentry] = PackedCandidate_->hcalFraction();


	trackinfo.buildTrackInfo(PackedCandidate_,jetDir,jetRefTrackDir,pv);

	const reco::TransientTrack ttrack = trackinfo.getTTrack();
	float mindistsvold = mindistsvltcandold(ttrack);

	LT_distminsvold_[fillntupleentry] = mindistsvold;

	float mindistsv = mindistsvltcand(ttrack);
	float eng_mindistsv = std::log(std::fabs(mindistsv)+1.0);

	LT_distminsv_[fillntupleentry] = mindistsv;
	LT_distminsv2_[fillntupleentry] = eng_mindistsv;

	LT_BtagPf_trackEtaRel_[fillntupleentry]     =catchInfsAndBound(trackinfo.getTrackEtaRel(),  0,-5,15);
	LT_BtagPf_trackPtRel_[fillntupleentry]      =catchInfsAndBound(trackinfo.getTrackPtRel(),   0,-1,4);
	LT_BtagPf_trackPPar_[fillntupleentry]       =catchInfsAndBound(trackinfo.getTrackPPar(),    0,-1e5,1e5 );
	LT_BtagPf_trackDeltaR_[fillntupleentry]     =catchInfsAndBound(trackinfo.getTrackDeltaR(),  0,-5,5 );
	LT_BtagPf_trackPParRatio_[fillntupleentry]  =catchInfsAndBound(trackinfo.getTrackPParRatio(),0,-10,100);
	LT_BtagPf_trackSip3dVal_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackSip3dVal(), 0, -1,1e5 );
	LT_BtagPf_trackSip3dSig_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackSip3dSig(), 0, -1,4e4 );
	LT_BtagPf_trackSip2dVal_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackSip2dVal(), 0, -1,70 );
	LT_BtagPf_trackSip2dSig_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackSip2dSig(), 0, -1,4e4 );
	LT_BtagPf_trackDecayLen_[fillntupleentry]   =trackinfo.getTrackJetDecayLen();
	LT_BtagPf_trackJetDistVal_[fillntupleentry] =catchInfsAndBound(trackinfo.getTrackJetDistVal(),0,-20,1 );

	float cand_charge_ = PackedCandidate_->charge();
	LT_charge_[fillntupleentry] = cand_charge_;
    LT_chi2_[fillntupleentry] = PackedCandidate_->hasTrackDetails() ? catchInfsAndBound(PackedCandidate_->pseudoTrack().normalizedChi2(),300,-1,300) : -1;
    LT_quality_[fillntupleentry] = PackedCandidate_->hasTrackDetails() ? PackedCandidate_->pseudoTrack().qualityMask() : (1 << reco::TrackBase::loose);

	LT_drminsv_[fillntupleentry] = catchInfsAndBound(drminltcandsv_,0,-0.4,0,-0.4);

	LT_lostInnerHits_[fillntupleentry] = catchInfs(PackedCandidate_->lostInnerHits(),2);
	LT_numberOfPixelHits_[fillntupleentry] = catchInfs(PackedCandidate_->numberOfPixelHits(),-1);
	LT_numberOfStripHits_[fillntupleentry] = catchInfs(PackedCandidate_->stripLayersWithMeasurement(),-1);
      }
    }

    n_LT_ = n_lts;

    return true;
}


float ntuple_LT::mindrsvltcand(const pat::PackedCandidate* pfcand) {

  float mindr_ = jetradius_;
  for (unsigned int i=0; i<secVertices()->size(); ++i) {
    if(!pfcand) continue;
    //if(!svs.at(i)) continue;
    float tempdr_ = reco::deltaR(secVertices()->at(i),*pfcand);
    if (tempdr_<mindr_) { mindr_ = tempdr_; }

  }
  return mindr_;
}

float ntuple_LT::mindistsvltcandold(const reco::TransientTrack track) {

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
      out_dist = sign * result.second.value();
      mindist_ = result.second.value();
    } 
  }
  return out_dist;
}

GlobalPoint ntuple_LT::mingpsvltcand(const reco::TransientTrack track) {

  float mindist_ = 999.999;
  GlobalPoint out_dist(0.0,0.0,0.0);
  GlobalPoint pca;
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

    pca = tsos.globalPosition();

    if(result.second.value() < mindist_){ 
      out_dist = pca;
      mindist_ = result.second.value();
    }
  }
  return out_dist;
}

GlobalPoint ntuple_LT::gppvltcand(const reco::TransientTrack track, const GlobalVector direction, const reco::Vertex vertex) {

  GlobalPoint out_dist(0.0,0.0,0.0);
  GlobalPoint pca;
  if ((track.isValid()) && (vertex.isValid())){
      
    AnalyticalImpactPointExtrapolator extrapolator(track.field());
    TrajectoryStateOnSurface tsos =  extrapolator.extrapolate(track.impactPointState(), RecoVertex::convertPos(vertex.position()));
      
    VertexDistance3D dist;

    if (tsos.isValid()) {
      GlobalPoint refPoint = tsos.globalPosition();
      GlobalError refPointErr = tsos.cartesianError().position();
      GlobalPoint vertexPosition = RecoVertex::convertPos(vertex.position());
      GlobalError vertexPositionErr = RecoVertex::convertError(vertex.error());

      std::pair<bool, Measurement1D> result(true, dist.distance(VertexState(vertexPosition, vertexPositionErr), VertexState(refPoint, refPointErr)));
      if (result.first) {
	pca = tsos.globalPosition();
      }
    }
  }
  out_dist = pca;
  return out_dist;
}

float ntuple_LT::mindistsvltcand(const reco::TransientTrack track) {

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
