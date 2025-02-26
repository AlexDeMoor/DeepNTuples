### Code from Raffaele Gerosa : https://gitlab.cern.ch/rgerosa/particlenetstudiesrun2 ###
import FWCore.ParameterSet.Config as cms

def mutauSelection (process,scoreLabel='pfParticleNetAK4base'):

    ## trigger selection
    process.triggerResultFilter = cms.EDFilter('TriggerResultsFilter',
        hltResults = cms.InputTag('TriggerResults','',"HLT"),
        l1tResults = cms.InputTag(''),
        l1tIgnoreMaskAndPrescale = cms.bool(False),
        throw = cms.bool(False),
        triggerConditions = cms.vstring('HLT_IsoMu24_v*')
    );

    ## muons selection
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
        minNumber = cms.uint32(0),
        maxNumber = cms.uint32(0)
    );

    ## tau selection
    tauvsjet = "(bDiscriminator('"+scoreLabel+":probtaup1h0p')+bDiscriminator('"+scoreLabel+":probtaup1h1p')+bDiscriminator('"+scoreLabel+":probtaup1h2p')+bDiscriminator('"+scoreLabel+":probtaup3h0p')+bDiscriminator('"+scoreLabel+":probtaup3h1p')+bDiscriminator('"+scoreLabel+":probtaum1h0p')+bDiscriminator('"+scoreLabel+":probtaum1h1p')+bDiscriminator('"+scoreLabel+":probtaum1h2p')+bDiscriminator('"+scoreLabel+":probtaum3h0p')+bDiscriminator('"+scoreLabel+":probtaum3h1p'))/(1-bDiscriminator('"+scoreLabel+":probele')-bDiscriminator('"+scoreLabel+":probmu')) > 0.90";
    tauvsele = "(bDiscriminator('"+scoreLabel+":probtaup1h0p')+bDiscriminator('"+scoreLabel+":probtaup1h1p')+bDiscriminator('"+scoreLabel+":probtaup1h2p')+bDiscriminator('"+scoreLabel+":probtaup3h0p')+bDiscriminator('"+scoreLabel+":probtaup3h1p')+bDiscriminator('"+scoreLabel+":probtaum1h0p')+bDiscriminator('"+scoreLabel+":probtaum1h1p')+bDiscriminator('"+scoreLabel+":probtaum1h2p')+bDiscriminator('"+scoreLabel+":probtaum3h0p')+bDiscriminator('"+scoreLabel+":probtaum3h1p'))/(1-(bDiscriminator('"+scoreLabel+":probb')+bDiscriminator('"+scoreLabel+":probc')+bDiscriminator('"+scoreLabel+":probuds')+bDiscriminator('"+scoreLabel+":probg')+bDiscriminator('"+scoreLabel+":probmu'))) > 0.50";
    tauvsmu = "(bDiscriminator('"+scoreLabel+":probtaup1h0p')+bDiscriminator('"+scoreLabel+":probtaup1h1p')+bDiscriminator('"+scoreLabel+":probtaup1h2p')+bDiscriminator('"+scoreLabel+":probtaup3h0p')+bDiscriminator('"+scoreLabel+":probtaup3h1p')+bDiscriminator('"+scoreLabel+":probtaum1h0p')+bDiscriminator('"+scoreLabel+":probtaum1h1p')+bDiscriminator('"+scoreLabel+":probtaum1h2p')+bDiscriminator('"+scoreLabel+":probtaum3h0p')+bDiscriminator('"+scoreLabel+":probtaum3h1p'))/(1-(bDiscriminator('"+scoreLabel+":probb')+bDiscriminator('"+scoreLabel+":probc')+bDiscriminator('"+scoreLabel+":probuds')+bDiscriminator('"+scoreLabel+":probg')+bDiscriminator('"+scoreLabel+":probele'))) > 0.95";
    
    ## tau preselection to remove background
    process.tagTaus = cms.EDFilter("PATJetSelector",
        src = cms.InputTag("selectedUpdatedPatJetsDeepFlavour"),
        cut = cms.string("correctedJet('Uncorrected').pt() > 25 && abs(eta) < 2.5 && "+tauvsmu+" && "+tauvsele+" && "+tauvsjet)
    );

    process.filterTagTaus = cms.EDFilter("PATCandViewCountFilter",
        src = cms.InputTag("tagTaus"),
        minNumber = cms.uint32(1),
        maxNumber = cms.uint32(1),
    );

    ## mu tau pair selection
    process.mutauPairs = cms.EDProducer("CandViewShallowCloneCombiner",
        decay = cms.string("tagMuons@+ tagTaus@-"),
        cut = cms.string("mass > 50 && mass < 90 && deltaR(daughter(0).eta,daughter(0).phi,daughter(1).eta,daughter(1).phi) > 0.4"),
        checkCharge = cms.bool(False)
    );

    process.filterMuTauPairs = cms.EDFilter("PATCandViewCountFilter",
        src = cms.InputTag("mutauPairs"),
        minNumber = cms.uint32(1),
        maxNumber = cms.uint32(1)                                
    );
    
    ## final path
    process.leptonSelection = cms.Sequence(
        process.triggerResultFilter+
        process.tagMuons+
        process.filterTagMuons+
        process.vetoMuons+
        process.filterVetoMuons+
        process.vetoElectrons+
        process.filterVetoElectrons+        
        process.tagTaus+
        process.filterTagTaus+
        process.mutauPairs+
        process.filterMuTauPairs
    )


    from PhysicsTools.PatAlgos.cleaningLayer1.jetCleaner_cfi import cleanPatJets
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
                )
            ),
            finalCut = cms.string('')
    )

    from PhysicsTools.PatAlgos.selectionLayer1.jetSelector_cfi import selectedPatJets
    process.selectedCleanJets = selectedPatJets.clone();
    process.selectedCleanJets.src = cms.InputTag("cleanJets");
    process.selectedCleanJets.cut = cms.string("correctedJet('Uncorrected').pt() > 25 && abs(eta) < 2.5");
    process.selectedCleanJets.filter = cms.bool(False)

    process.filterCleanJets = cms.EDFilter("PATCandViewCountFilter",
            src = cms.InputTag("selectedCleanJets"),
            minNumber = cms.uint32(1),
            maxNumber = cms.uint32(99)
    );

    ## final sequence                                                                                                                                                                                  
    process.jetSelection = cms.Sequence(
        process.cleanJets +
        process.selectedCleanJets +
        process.filterCleanJets 
    )

    return process;
