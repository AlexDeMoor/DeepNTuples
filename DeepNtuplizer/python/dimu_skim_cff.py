### Code from Raffaele Gerosa : https://gitlab.cern.ch/rgerosa/particlenetstudiesrun2 ###
import FWCore.ParameterSet.Config as cms

## trigger selection
triggerResultFilter = cms.EDFilter('TriggerResultsFilter',
        hltResults = cms.InputTag('TriggerResults','',"HLT"),
        l1tResults = cms.InputTag(''),
        l1tIgnoreMaskAndPrescale = cms.bool(False),
        throw = cms.bool(False),
        triggerConditions = cms.vstring('HLT_IsoMu24_v*')
);

## dimuon selection
tagMuons = cms.EDFilter("PATMuonSelector",
        src = cms.InputTag("slimmedMuons"),
        cut = cms.string("pt > 26 && abs(eta) < 2.4 && passed('CutBasedIdTight') && passed('PFIsoTight')"),
        filter = cms.bool(False)
);

filterTagMuons = cms.EDFilter("PATCandViewCountFilter",
        src = cms.InputTag("tagMuons"),
        minNumber = cms.uint32(1),
        maxNumber = cms.uint32(2)
);

probeMuons = cms.EDFilter("PATMuonSelector",
        src = cms.InputTag("slimmedMuons"),
        cut = cms.string("pt > 20 && abs(eta) < 2.4 && passed('CutBasedIdMedium') && passed('PFIsoLoose')"),
        filter = cms.bool(False)
);

filterProbeMuons = cms.EDFilter("PATCandViewCountFilter",
        src = cms.InputTag("probeMuons"),
        minNumber = cms.uint32(2),
        maxNumber = cms.uint32(2),
);

## dimuon pair selection
dimuonPairs = cms.EDProducer("CandViewShallowCloneCombiner",
        decay = cms.string("probeMuons@+ probeMuons@-"),
        cut = cms.string("mass > 70 && mass < 110 && charge == 0"),
        checkCharge = cms.bool(False)
);

filterDiMuonPairs = cms.EDFilter("PATCandViewCountFilter",
        src = cms.InputTag("dimuonPairs"),
        minNumber = cms.uint32(1),
        maxNumber = cms.uint32(1)
);

## sequence definition
leptonSelection = cms.Sequence(
      triggerResultFilter+
      tagMuons+
      filterTagMuons+
      probeMuons+
      filterProbeMuons+
      dimuonPairs+
      filterDiMuonPairs
) 
    
## require dR between jets and the muons
from PhysicsTools.PatAlgos.cleaningLayer1.jetCleaner_cfi import cleanPatJets
cleanJets = cms.EDProducer("PATJetCleaner",
    src = cms.InputTag("selectedUpdatedPatJetsDeepFlavour"),
    preselection = cms.string(''),
    checkOverlaps = cms.PSet(
        muons = cms.PSet(
            src          = cms.InputTag("probeMuons"),
            algorithm    = cms.string("byDeltaR"),
            preselection = cms.string(""),
            deltaR       = cms.double(0.4),
            checkRecoComponents = cms.bool(False),
            pairCut             = cms.string(""),
            requireNoOverlaps   = cms.bool(True)
        )
    ),
    finalCut = cms.string('')
)

from PhysicsTools.PatAlgos.selectionLayer1.jetSelector_cfi import selectedPatJets
selectedCleanJets = selectedPatJets.clone();
selectedCleanJets.src = cms.InputTag("cleanJets");
selectedCleanJets.cut = cms.string("correctedJet('Uncorrected').pt() > 25 && abs(eta) < 2.5");
selectedCleanJets.filter = cms.bool(False)

filterCleanJets = cms.EDFilter("PATCandViewCountFilter",
        src = cms.InputTag("selectedCleanJets"),
        minNumber = cms.uint32(1),
        maxNumber = cms.uint32(99),
);

jetSelection = cms.Sequence(
    cleanJets +
    selectedCleanJets +
    filterCleanJets
)
