/*
 *      Author: mverzett
 */

#ifndef DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_HELPERS_H_
#define DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_HELPERS_H_
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

namespace deep_ntuples {
  enum JetFlavor {UNDEFINED, G, U, D, S, C, GCC, CC, B, GBB, BB, LeptonicB, LeptonicB_C, MU, ELE,TAU,
    TAUP1H0P,TAUP1H1P,TAUP1H2P,TAUP3H0P,TAUP3H1P,TAUM1H0P,TAUM1H1P,TAUM1H2P,TAUM3H0P,TAUM3H1P, PU};
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
			  bool usePhysForLightAndUndefined=false);
    std::vector<std::size_t> jet_muonsIds(const pat::Jet& jet, const std::vector<pat::Muon>& event_muons); 
    std::vector<std::size_t> jet_electronsIds(const pat::Jet& jet, const std::vector<pat::Electron>& event_electrons); 
}

#endif //DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_HELPERS_H_

#include <tuple>
#include "DataFormats/JetReco/interface/Jet.h"
namespace yuta{

std::tuple<int, int, int, float, float, float, float>
calcVariables(const reco::Jet *jet);


}
