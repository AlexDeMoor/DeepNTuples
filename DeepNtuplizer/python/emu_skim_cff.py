### Code from Raffaele Gerosa : https://gitlab.cern.ch/rgerosa/particlenetstudiesrun2 ###
import FWCore.ParameterSet.Config as cms

def emuSelection(process,scoreLabel="pfParticleNetAK4base"):
    
    ## trigger selection
    process.triggerResultFilter = cms.EDFilter('TriggerResultsFilter',
            hltResults = cms.InputTag('TriggerResults','',"HLT"),
            l1tResults = cms.InputTag(''),
            l1tIgnoreMaskAndPrescale = cms.bool(False),
            throw = cms.bool(False),
            triggerConditions = cms.vstring('HLT_IsoMu24_v*')
    );

    ## muon selection
    process.tagMuons = cms.EDFilter("PATMuonSelector",
        src = cms.InputTag("slimmedMuons"),
        cut = cms.string("pt > 26 && abs(eta) < 2.4 && passed('CutBasedIdTight') && passed('PFIsoTight')"),
        filter = cms.bool(False)
    );

    process.filterTagMuons = cms.EDFilter("PATCandViewCountFilter",
        src = cms.InputTag("tagMuons"),
        minNumber = cms.uint32(1),
        maxNumber = cms.uint32(1)
    );

    ## electron selection
    process.tagElectrons = cms.EDFilter("PATElectronSelector",
        src = cms.InputTag("slimmedElectrons"),
        cut = cms.string("pt > 20 && abs(eta) < 2.5 && electronID('mvaEleID-RunIIIWinter22-iso-V1-wp80')"),
        filter = cms.bool(False)
    );

    process.filterTagElectrons = cms.EDFilter("PATCandViewCountFilter",
        src = cms.InputTag("tagElectrons"),
        minNumber = cms.uint32(1),
        maxNumber = cms.uint32(1),
    );

    ## lepton veto
    process.vetoMuons = cms.EDFilter("PATMuonSelector",
        src = cms.InputTag("slimmedMuons"),
        cut = cms.string("pt > 15 && abs(eta) < 2.4 && passed('CutBasedIdLoose') && passed('PFIsoLoose')"),
        filter = cms.bool(False)
    );

    process.filterVetoMuons = cms.EDFilter("PATCandViewCountFilter",
        src = cms.InputTag("vetoMuons"),
        minNumber = cms.uint32(1),
        maxNumber = cms.uint32(1)
    );

    process.vetoElectrons = cms.EDFilter("PATElectronSelector",
        src = cms.InputTag("slimmedElectrons"),
        cut = cms.string("pt > 15 && abs(eta) < 2.5 && electronID('mvaEleID-RunIIIWinter22-iso-V1-wp90')"),
        filter = cms.bool(False)
    );

    process.filterVetoElectrons = cms.EDFilter("PATCandViewCountFilter",
        src = cms.InputTag("vetoElectrons"),
        minNumber = cms.uint32(1),
        maxNumber = cms.uint32(1)
    );
    
    ## emu pairs
    process.emuPairs = cms.EDProducer("CandViewShallowCloneCombiner",
        decay = cms.string("tagMuons@+ tagElectrons@-"),
        cut = cms.string("mass > 40 && charge == 0"),
        checkCharge = cms.bool(False)
    );

    process.filterEMuPairs = cms.EDFilter("PATCandViewCountFilter",
        src = cms.InputTag("emuPairs"),
        minNumber = cms.uint32(1),
        maxNumber = cms.uint32(1)
    );

    ## final sequence
    process.leptonSelection = cms.Sequence(
        process.triggerResultFilter+
        process.tagMuons+
        process.filterTagMuons+
        process.vetoMuons+
        process.filterVetoMuons+
        process.tagElectrons+
        process.filterTagElectrons+
        process.vetoElectrons+
        process.filterVetoElectrons+
        process.emuPairs+
        process.filterEMuPairs
    )

    ## require dR between jets and the muons
    process.cleanJets = cms.EDProducer("PATJetCleaner",
        src = cms.InputTag("selectedUpdatedPatJetsDeepFlavour"),
        preselection = cms.string(''),
        checkOverlaps = cms.PSet(
            muons = cms.PSet(
                src          = cms.InputTag("tagMuons"),
                algorithm    = cms.string("byDeltaR"),
                preselection = cms.string(""),
                deltaR       = cms.double(0.4),
                checkRecoComponents = cms.bool(False),
                pairCut             = cms.string(""),
                requireNoOverlaps   = cms.bool(True)
            ),
            electrons = cms.PSet(
                src          = cms.InputTag("tagElectrons"),
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
    process.selectedCleanJets = selectedPatJets.clone();
    process.selectedCleanJets.src = cms.InputTag("cleanJets");
    process.selectedCleanJets.cut = cms.string('correctedJet("Uncorrected").pt() > 25 && abs(eta) < 2.5 && bDiscriminator("'+scoreLabel+'JetTags:probb")/(bDiscriminator("'+scoreLabel+'JetTags:probb")+bDiscriminator("'+scoreLabel+'JetTags:probc")+bDiscriminator("'+scoreLabel+'JetTags:probuds")+bDiscriminator("'+scoreLabel+'JetTags:probg")) > 0.1');
    process.selectedCleanJets.filter = cms.bool(False)
        
    process.filterCleanJets = cms.EDFilter("PATCandViewCountFilter",
                        src = cms.InputTag("selectedCleanJets"),
                        minNumber = cms.uint32(2),
                        maxNumber = cms.uint32(99)
    );


    process.jetSelection = cms.Sequence(
        process.cleanJets +
        process.selectedCleanJets +
        process.filterCleanJets
    )

    return process;
