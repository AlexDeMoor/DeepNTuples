#include "../interface/helpers.h"
#include <vector>

namespace deep_ntuples {

std::vector<std::size_t> jet_muonsIds(const pat::Jet& jet, const std::vector<pat::Muon>& event_muons) {
    std::vector <std::size_t> muonsIds;
    for (std::size_t i = 0; i < event_muons.size(); i++) {
        const auto & muon = event_muons.at(i);
        if(reco::deltaR(muon.eta(),muon.phi(),jet.eta(),jet.phi()) < 0.4) muonsIds.emplace_back(i);
    }
    return muonsIds;
}

std::vector<std::size_t> jet_electronsIds(const pat::Jet& jet, const std::vector<pat::Electron>& event_electrons) {
    std::vector <std::size_t> electronsIds;
    for (std::size_t i = 0; i < event_electrons.size(); i++) {
        const auto & electron = event_electrons.at(i);
        if(reco::deltaR(electron.eta(),electron.phi(),jet.eta(),jet.phi()) < 0.4) electronsIds.emplace_back(i);
    }
    return electronsIds;
}

JetFlavor jet_flavour(const pat::Jet& jet,
              const pat::Jet& unsubjet,
		      const std::vector<reco::GenParticle>& gToBB,
		      const std::vector<reco::GenParticle>& gToCC,
		      const std::vector<reco::GenParticle>& neutrinosLepB,
		      const std::vector<reco::GenParticle>& neutrinosLepB_C,
		      const std::vector<reco::GenParticle>& alltaus,
		      int pos_matched_genmu,
		      int pos_matched_genele,
		      int pos_matched_tauh,
		      int gentau_decaymode,
		      const std::vector<int> tau_gen_charge,
		      bool usePhysForLightAndUndefined) { 
    int hflav = abs(unsubjet.hadronFlavour());
    int pflav = abs(unsubjet.partonFlavour());
    int physflav = 0;
    int jet_tauflav_b, jet_muflav_b, jet_elflav_b, jet_taudecaymode_b, jet_taucharge_b;
    if( !( jet.genJet() ) ){
      if(pflav == 0){
	return JetFlavor::PU;
      }
      else{
	return JetFlavor::UNDEFINED;
      }
    }
    if(jet.genParton()) physflav=abs(jet.genParton()->pdgId());
    std::size_t nbs = unsubjet.jetFlavourInfo().getbHadrons().size();
    std::size_t ncs = unsubjet.jetFlavourInfo().getcHadrons().size();

    unsigned int nbFromGSP(0);
    for (reco::GenParticle p : gToBB) {
        double dr(reco::deltaR(jet, p));
        if (dr < 0.4) ++nbFromGSP;
    }

    unsigned int ncFromGSP(0);
    for (reco::GenParticle p : gToCC) {
        double dr(reco::deltaR(jet, p));
        if (dr < 0.4) ++ncFromGSP;
    }

    if(pos_matched_genmu >= 0){
      jet_muflav_b  = 1;
    }
    else{
      jet_muflav_b  = 0;
    }
    if(pos_matched_genele >= 0){
      jet_elflav_b  = 1;
    }
    else{
      jet_elflav_b  = 0;
    }
    if(pos_matched_tauh >= 0){
      jet_tauflav_b  = 1;
      jet_taudecaymode_b = gentau_decaymode;
      jet_taucharge_b = tau_gen_charge.at(pos_matched_tauh);
    }
    else{
      jet_tauflav_b  = 0;
      jet_taudecaymode_b = -1;
      jet_taucharge_b = 0;
    }

    if((jet_muflav_b==1) & (jet_elflav_b==0) & (jet_tauflav_b==0)){return JetFlavor::MU;}
    if((jet_muflav_b==0) & (jet_elflav_b==1) & (jet_tauflav_b==0)){return JetFlavor::ELE;}

    if((jet_muflav_b==0) & (jet_elflav_b==0) & (jet_tauflav_b==1) & (jet_taudecaymode_b==0) & (jet_taucharge_b>0)){return JetFlavor::TAUP1H0P;}
    if((jet_muflav_b==0) & (jet_elflav_b==0) & (jet_tauflav_b==1) & (jet_taudecaymode_b==1) & (jet_taucharge_b>0)){return JetFlavor::TAUP1H1P;}
    if((jet_muflav_b==0) & (jet_elflav_b==0) & (jet_tauflav_b==1) & (jet_taudecaymode_b==2) & (jet_taucharge_b>0)){return JetFlavor::TAUP1H2P;}
    if((jet_muflav_b==0) & (jet_elflav_b==0) & (jet_tauflav_b==1) & (jet_taudecaymode_b==10) & (jet_taucharge_b>0)){return JetFlavor::TAUP3H0P;}
    if((jet_muflav_b==0) & (jet_elflav_b==0) & (jet_tauflav_b==1) & (jet_taudecaymode_b==11) & (jet_taucharge_b>0)){return JetFlavor::TAUP3H1P;}

    if((jet_muflav_b==0) & (jet_elflav_b==0) & (jet_tauflav_b==1) & (jet_taudecaymode_b==0) & (jet_taucharge_b<0)){return JetFlavor::TAUM1H0P;}
    if((jet_muflav_b==0) & (jet_elflav_b==0) & (jet_tauflav_b==1) & (jet_taudecaymode_b==1) & (jet_taucharge_b<0)){return JetFlavor::TAUM1H1P;}
    if((jet_muflav_b==0) & (jet_elflav_b==0) & (jet_tauflav_b==1) & (jet_taudecaymode_b==2) & (jet_taucharge_b<0)){return JetFlavor::TAUM1H2P;}
    if((jet_muflav_b==0) & (jet_elflav_b==0) & (jet_tauflav_b==1) & (jet_taudecaymode_b==10) & (jet_taucharge_b<0)){return JetFlavor::TAUM3H0P;}
    if((jet_muflav_b==0) & (jet_elflav_b==0) & (jet_tauflav_b==1) & (jet_taudecaymode_b==11) & (jet_taucharge_b<0)){return JetFlavor::TAUM3H1P;}

    if(hflav == 5) { //B jet
        if(nbs > 1) {
            if (nbFromGSP > 0) return JetFlavor::GBB;
            else return JetFlavor::BB;
        }
        else if(nbs == 1) {
            for (std::vector<reco::GenParticle>::const_iterator it = neutrinosLepB.begin(); it != neutrinosLepB.end(); ++it){
                if(reco::deltaR(it->eta(),it->phi(),jet.eta(),jet.phi()) < 0.4) {
                    return JetFlavor::LeptonicB;
                }
            }
            for (std::vector<reco::GenParticle>::const_iterator it = neutrinosLepB_C.begin(); it != neutrinosLepB_C.end(); ++it){
                if(reco::deltaR(it->eta(),it->phi(),jet.eta(),jet.phi()) < 0.4) {
                    return JetFlavor::LeptonicB_C;
                }
            }
            return JetFlavor::B;
        }
        else {
            if(usePhysForLightAndUndefined){
                if(physflav == 21) return JetFlavor::G;
                else if(physflav == 3) return JetFlavor::S;
                else if(physflav == 2) return JetFlavor::U;
		else if(physflav == 1) return JetFlavor::D;
                else return JetFlavor::UNDEFINED;
            }
            else return JetFlavor::UNDEFINED;
        }
    }
    else if(hflav == 4) { //C jet
        if (ncs > 1) {
            if (ncFromGSP > 0) return JetFlavor::GCC;
            else return JetFlavor::CC;
        }
        else return JetFlavor::C;
    }
    else { //not a heavy jet
        if(alltaus.size()>0){ //check for tau in a simplistic way
            bool ishadrtaucontained=true;
            for(const auto& p:alltaus){
                size_t ndau=p.numberOfDaughters();
                for(size_t i=0;i<ndau;i++){
                    const reco::Candidate* dau=p.daughter(i);
                    int daupid=std::abs(dau->pdgId());
                    if(daupid == 13 || daupid == 11){
                        ishadrtaucontained=false;
                        break;
                    }
                    if(daupid != 12 && daupid!=14 && daupid!=16 &&
                            reco::deltaR(*dau,jet) > 0.4){
                        ishadrtaucontained=false;
                        break;
                    }
                }
            }
            if(ishadrtaucontained) return JetFlavor::TAU;
        }
        if(std::abs(pflav) == 4 || std::abs(pflav) == 5 || nbs || ncs) {
            if(usePhysForLightAndUndefined){
                if(physflav == 21) return JetFlavor::G;
                else if(physflav == 3) return JetFlavor::S;
                else if(physflav == 2) return JetFlavor::U;
		else if(physflav == 1) return JetFlavor::D;
                else return JetFlavor::UNDEFINED;
            }
            else return JetFlavor::UNDEFINED;
        }
        else if(usePhysForLightAndUndefined){
           if(physflav == 21) return JetFlavor::G;
            else if(physflav == 3) return JetFlavor::S;
	    else if(physflav == 2) return JetFlavor::U;
	    else if(physflav == 1) return JetFlavor::D;
            else return JetFlavor::UNDEFINED;
        }
        else {
            if(pflav == 21) return JetFlavor::G;
            else if(pflav == 3) return JetFlavor::S;
            else if(pflav == 2) return JetFlavor::U;
	    else if(pflav == 1) return JetFlavor::D;
            else return JetFlavor::UNDEFINED;
        }
    }
    return JetFlavor::UNDEFINED;
}
}

namespace yuta{

std::tuple<int, int, int, float, float, float, float> calcVariables(const reco::Jet *jet){
    float sum_weight = 0., sum_deta = 0., sum_dphi = 0., sum_deta2 = 0., sum_dphi2 = 0., sum_detadphi = 0., sum_pt = 0.;
    int multiplicity = 0;
    int charged_multiplicity = 0, neutral_multiplicity = 0;
    float pt_dr_log = 0;

    bool useQC=false;

    //Loop over the jet constituents
    for (unsigned int i = 0; i <  jet->numberOfDaughters(); i++){
        const pat::PackedCandidate* daughter = dynamic_cast<const pat::PackedCandidate*>(jet->daughter(i));
        if(daughter){                                        //packed candidate situation
            auto part = static_cast<const pat::PackedCandidate*>(daughter);

            if(part->charge()){

                if(!(part->fromPV() > 1 && part->trackHighPurity())) continue;
                if(useQC){
                    if((part->dz()*part->dz())/(part->dzError()*part->dzError()) > 25.) continue;
                    if((part->dxy()*part->dxy())/(part->dxyError()*part->dxyError()) < 25.){
                        ++multiplicity;
                        ++charged_multiplicity;
                    }
                } else{
                    ++multiplicity;
                    ++charged_multiplicity;
                };

            } else {
                if(part->pt() < 1.0) continue;
                ++multiplicity;
                ++neutral_multiplicity;
            }

            float dr = reco::deltaR(*jet, *part);

	    pt_dr_log += std::log(part->pt()/dr);
        }
        else continue;

        float deta   = daughter->eta() - jet->eta();
        float dphi   = reco::deltaPhi(daughter->phi(), jet->phi());
        float partPt = daughter->pt();
        float weight = partPt*partPt;

        sum_weight   += weight;
        sum_pt       += partPt;
        sum_deta     += deta*weight;
        sum_dphi     += dphi*weight;
        sum_deta2    += deta*deta*weight;
        sum_detadphi += deta*dphi*weight;
        sum_dphi2    += dphi*dphi*weight;
    }

    //Calculate axis2 and ptD
    float a = 0., b = 0., c = 0.;
    float ave_deta = 0., ave_dphi = 0., ave_deta2 = 0., ave_dphi2 = 0.;
    if(sum_weight > 0){
        ave_deta  = sum_deta/sum_weight;
        ave_dphi  = sum_dphi/sum_weight;
        ave_deta2 = sum_deta2/sum_weight;
        ave_dphi2 = sum_dphi2/sum_weight;
        a         = ave_deta2 - ave_deta*ave_deta;
        b         = ave_dphi2 - ave_dphi*ave_dphi;
        c         = -(sum_detadphi/sum_weight - ave_deta*ave_dphi);
    }
    float delta = sqrt(fabs((a-b)*(a-b)+4*c*c));
    float axis1 = (a+b+delta > 0 ?  sqrt(0.5*(a+b+delta)) : 0);
    float axis2 = (a+b-delta > 0 ?  sqrt(0.5*(a+b-delta)) : 0);
    float ptD   = (sum_weight > 0 ? sqrt(sum_weight)/sum_pt : 0);

    return std::make_tuple(multiplicity, charged_multiplicity, neutral_multiplicity, ptD, axis1, axis2, pt_dr_log);
}


}

