
import FWCore.ParameterSet.Config as cms

import FWCore.ParameterSet.VarParsing as VarParsing
### parsing job options 
import sys

options = VarParsing.VarParsing()

options.register('inputScript','',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"input Script")
options.register('outputFile','output',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"output File (w/o .root)")
options.register('maxEvents', 100,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"maximum events")
options.register('skipEvents', 0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "skip N events")
options.register('job', 0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "job number")
options.register('nJobs', 1, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "total jobs")
options.register('reportEvery', 1000, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "report every")
options.register('gluonReduction', 0.0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.float, "gluon reduction")
options.register('selectJets', True, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "select jets with good gen match")
options.register('phase2', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "apply jet selection for phase 2. Currently sets JetEtaMax to 3.0 and picks slimmedJetsPuppi as jet collection.")
options.register('puppi', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "use puppi jets")
options.register('eta', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "use eta up to 5.0")


import os
release=os.environ['CMSSW_VERSION'][6:]
print("Using release "+release)


options.register(
	'inputFiles','',
	VarParsing.VarParsing.multiplicity.list,
	VarParsing.VarParsing.varType.string,
	"input files (default is the tt RelVal)"
	)

if hasattr(sys, "argv"):
    options.parseArguments()

from Configuration.Eras.Era_Run3_pp_on_PbPb_2023_cff import Run3_pp_on_PbPb_2023
process = cms.Process("DNNFiller", Run3_pp_on_PbPb_2023)

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.EventContent.EventContent_cff")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '132X_mcRun3_2023_realistic_HI_v10', '')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = options.reportEvery

process.options = cms.untracked.PSet(
   allowUnscheduled = cms.untracked.bool(True),  
   wantSummary=cms.untracked.bool(True)
)

options.inputFiles = 'root://eoscms.cern.ch//eos/cms//store/group/cmst3/group/hintt/Run3/MC/PbPb2023/Embedded/2024_04_19/POWHEG_5p36TeV_2023Run3/TT_hvq_POWHEG_Hydjet_5p36TeV_TuneCP5_2023Run3_MINIAOD_2024_04_19/240419_231333/0000/POWHEG_TT_hvq_MINIAOD_1.root'
process.source = cms.Source('PoolSource', fileNames=cms.untracked.vstring(options.inputFiles))
if options.inputFiles:
	process.source.fileNames = options.inputFiles

if options.inputScript != '' and options.inputScript != 'DeepNTuples.DeepNtuplizer.samples.TTJetsPhase1_cfg':
    process.load(options.inputScript)

numberOfFiles = len(process.source.fileNames)
numberOfJobs = options.nJobs
jobNumber = options.job


process.source.fileNames = process.source.fileNames[jobNumber:numberOfFiles:numberOfJobs]
if options.nJobs > 1:
    print ("running over these files:")
    print (process.source.fileNames)

process.source.skipEvents = cms.untracked.uint32(options.skipEvents)
process.maxEvents  = cms.untracked.PSet( 
    input = cms.untracked.int32 (options.maxEvents) 
)
releases = release.split("_")

# ------------------------------------------------------------------------- #
# Heavy-ion settings
# ------------------------------------------------------------------------- #
if options.eta :
    jetPtMin = 10
    jetAbsEtaMax = 5.0
else:
    jetPtMin = 15
    jetAbsEtaMax = 2.5

if options.phase2 :
    jetAbsEtaMax = 3.0

jetCorrectionsAK4 = ('AK4PFchs', ['L2Relative', 'L3Absolute'], 'None')

bTagInfos = ['pfDeepFlavourTagInfos',
             'pfImpactParameterTagInfos',
             'pfInclusiveSecondaryVertexFinderTagInfos',
             'pfParticleTransformerAK4TagInfos',
             'pfUnifiedParticleTransformerAK4TagInfos',] #'pfParticleNetAK4TagInfos',]

bTagDiscriminators = [
    'pfDeepFlavourJetTags:probb',
    'pfDeepFlavourJetTags:probbb',
    'pfDeepFlavourJetTags:probc',
    'pfDeepFlavourJetTags:probg',
    'pfDeepFlavourJetTags:problepb',
    'pfDeepFlavourJetTags:probuds',
    'pfParticleTransformerAK4JetTags:probb',
    'pfParticleTransformerAK4JetTags:probbb',
    'pfParticleTransformerAK4JetTags:probc',
    'pfParticleTransformerAK4JetTags:probg',
    'pfParticleTransformerAK4JetTags:problepb',
    'pfParticleTransformerAK4JetTags:probuds',
    'pfUnifiedParticleTransformerAK4JetTags:probb',
    'pfUnifiedParticleTransformerAK4JetTags:probbb',
    'pfUnifiedParticleTransformerAK4JetTags:probc',
    'pfUnifiedParticleTransformerAK4JetTags:probg',
    'pfUnifiedParticleTransformerAK4JetTags:problepb',
    'pfUnifiedParticleTransformerAK4JetTags:probu',
    'pfUnifiedParticleTransformerAK4JetTags:probd',
    'pfUnifiedParticleTransformerAK4JetTags:probs',
    'pfUnifiedParticleTransformerAK4JetTags:probtaup1h0p',
    'pfUnifiedParticleTransformerAK4JetTags:probtaup1h1p',
    'pfUnifiedParticleTransformerAK4JetTags:probtaup1h2p',
    'pfUnifiedParticleTransformerAK4JetTags:probtaup3h0p',
    'pfUnifiedParticleTransformerAK4JetTags:probtaup3h1p',
    'pfUnifiedParticleTransformerAK4JetTags:probtaum1h0p',
    'pfUnifiedParticleTransformerAK4JetTags:probtaum1h1p',
    'pfUnifiedParticleTransformerAK4JetTags:probtaum1h2p',
    'pfUnifiedParticleTransformerAK4JetTags:probtaum3h0p',
    'pfUnifiedParticleTransformerAK4JetTags:probtaum3h1p',
    'pfUnifiedParticleTransformerAK4JetTags:probele',
    'pfUnifiedParticleTransformerAK4JetTags:probmu',
    'pfUnifiedParticleTransformerAK4JetTags:ptcorr',
    'pfUnifiedParticleTransformerAK4JetTags:ptnu'
]

# Create gen-level information
from RecoHI.HiJetAlgos.hiSignalParticleProducer_cfi import hiSignalParticleProducer as hiSignalGenParticles
process.hiSignalGenParticles = hiSignalGenParticles.clone(
    src = "prunedGenParticles"
)
from PhysicsTools.PatAlgos.producersHeavyIons.heavyIonJets_cff import allPartons
process.allPartons = allPartons.clone(
    src = 'hiSignalGenParticles'
)
from RecoJets.JetProducers.ak4GenJets_cfi import ak4GenJets
process.ak4GenJetsWithNu = ak4GenJets.clone(
    src = 'packedGenParticlesSignal'
)
process.packedGenParticlesForJetsNoNu = cms.EDFilter("CandPtrSelector",
    src = cms.InputTag("packedGenParticlesSignal"),
    cut = cms.string("abs(pdgId) != 12 && abs(pdgId) != 14 && abs(pdgId) != 16")
)
process.ak4GenJetsRecluster = ak4GenJets.clone(
    src = 'packedGenParticlesForJetsNoNu'
)
process.genTask = cms.Task(process.hiSignalGenParticles, process.allPartons, process.ak4GenJetsWithNu, process.packedGenParticlesForJetsNoNu, process.ak4GenJetsRecluster)

# Remake secondary vertices
from RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff import inclusiveCandidateVertexFinder, candidateVertexMerger, candidateVertexArbitrator, inclusiveCandidateSecondaryVertices
process.inclusiveCandidateVertexFinder = inclusiveCandidateVertexFinder.clone(
    tracks = "packedPFCandidates",
    primaryVertices = "offlineSlimmedPrimaryVertices",
    minHits = 0,
    minPt = 0.8
)
process.candidateVertexMerger = candidateVertexMerger.clone()
process.candidateVertexArbitrator = candidateVertexArbitrator.clone(
    tracks = "packedPFCandidates",
    primaryVertices = "offlineSlimmedPrimaryVertices"
)
process.inclusiveCandidateSecondaryVertices = inclusiveCandidateSecondaryVertices.clone()
process.svTask = cms.Task(process.inclusiveCandidateVertexFinder, process.candidateVertexMerger, process.candidateVertexArbitrator, process.inclusiveCandidateSecondaryVertices)
svSource = cms.InputTag("inclusiveCandidateSecondaryVertices")

# Create unsubtracted reco jets
from PhysicsTools.PatAlgos.tools.jetTools import addJetCollection
addJetCollection(
  process,
  postfix            = "UnsubJets",
  labelName          = "AK4PF",
  jetSource          = cms.InputTag("ak4PFUnsubJets"),
  algo               = "ak", #name of algo must be in this format
  rParam             = 0.4,
  pvSource           = cms.InputTag("offlineSlimmedPrimaryVertices"),
  pfCandidates       = cms.InputTag("packedPFCandidates"),
  svSource           = svSource,
  muSource           = cms.InputTag("slimmedMuons"),
  elSource           = cms.InputTag("slimmedElectrons"),
  genJetCollection   = cms.InputTag("ak4GenJetsWithNu"),
  genParticles       = cms.InputTag("hiSignalGenParticles"),
  jetCorrections     = ('AK4PF',) + jetCorrectionsAK4[1:],
)
process.patJetsAK4PFUnsubJets.useLegacyJetMCFlavour = False

from PhysicsTools.PatAlgos.producersLayer1.jetProducer_cff import ak4PFJets
process.ak4PFUnsubJets = ak4PFJets.clone(
    src = 'packedPFCandidates',
    jetPtMin = jetPtMin
)
process.patAlgosToolsTask.add(process.ak4PFUnsubJets)

# Create HIN subtracted reco jets
from PhysicsTools.PatAlgos.tools.jetTools import addJetCollection
addJetCollection(
  process,
  postfix            = "",
  labelName          = "AKCs4PF",
  jetSource          = cms.InputTag("akCs4PFJets"),
  algo               = "ak", #name of algo must be in this format
  rParam             = 0.4,
  pvSource           = cms.InputTag("offlineSlimmedPrimaryVertices"),
  pfCandidates       = cms.InputTag("packedPFCandidates"),
  svSource           = svSource,
  muSource           = cms.InputTag("slimmedMuons"),
  elSource           = cms.InputTag("slimmedElectrons"),
  genJetCollection   = cms.InputTag("ak4GenJetsWithNu"),
  genParticles       = cms.InputTag("hiSignalGenParticles"),
  jetCorrections     = jetCorrectionsAK4,
)
process.patJetsAKCs4PF.embedPFCandidates = True

from PhysicsTools.PatAlgos.producersHeavyIons.heavyIonJets_cff import PackedPFTowers, hiPuRho
process.PackedPFTowers = PackedPFTowers.clone()
process.hiPuRho = hiPuRho.clone(
    src = 'PackedPFTowers'
)
from PhysicsTools.PatAlgos.producersLayer1.jetProducer_cff import akCs4PFJets
process.akCs4PFJets = akCs4PFJets.clone(
    src = 'packedPFCandidates',
    jetPtMin = jetPtMin
)
for mod in ["PackedPFTowers", "hiPuRho", "akCs4PFJets"]:
    process.patAlgosToolsTask.add(getattr(process, mod))

# Create b-tagging sequence ----------------
from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection
updateJetCollection(
        process,
        labelName = "DeepFlavour",
        jetSource = cms.InputTag('patJetsAKCs4PF'), # 'ak4Jets'
        jetCorrections = jetCorrectionsAK4,
        pfCandidates = cms.InputTag('packedPFCandidates'),
        pvSource = cms.InputTag("offlineSlimmedPrimaryVertices"),
        svSource = svSource,
        muSource = cms.InputTag('slimmedMuons'),
        elSource = cms.InputTag('slimmedElectrons'),
        btagInfos = bTagInfos,
        btagDiscriminators = bTagDiscriminators,
        explicitJTA = False
)

process.unsubUpdatedPatJetsDeepFlavour = cms.EDProducer("JetMatcherDR",
    source = cms.InputTag("updatedPatJetsDeepFlavour"),
    matched = cms.InputTag("patJetsAK4PFUnsubJets")
)
process.patAlgosToolsTask.add(process.unsubUpdatedPatJetsDeepFlavour)

process.pfUnifiedParticleTransformerAK4JetTagsDeepFlavour.model_path = 'DeepNTuples/DeepNtuplizer/data/UParTAK4.onnx'
process.pfUnifiedParticleTransformerAK4TagInfosDeepFlavour.sort_cand_by_pt = True

if hasattr(process,'updatedPatJetsTransientCorrectedDeepFlavour'):
    process.updatedPatJetsTransientCorrectedDeepFlavour.addTagInfos = True
    process.updatedPatJetsTransientCorrectedDeepFlavour.addBTagInfo = True
else:
  raise ValueError('I could not find updatedPatJetsTransientCorrectedDeepFlavour to embed the tagInfos, please check the cfg')

# Remove PUPPI
process.patAlgosToolsTask.remove(process.packedpuppi)
process.patAlgosToolsTask.remove(process.packedpuppiNoLep)
process.pfInclusiveSecondaryVertexFinderTagInfosDeepFlavour.weights = ""
for taginfo in ["pfDeepFlavourTagInfosDeepFlavour", "pfParticleTransformerAK4TagInfosDeepFlavour", "pfUnifiedParticleTransformerAK4TagInfosDeepFlavour"]:
    getattr(process, taginfo).fallback_puppi_weight = True
    getattr(process, taginfo).fallback_vertex_association = True
    getattr(process, taginfo).unsubjet_map = "unsubUpdatedPatJetsDeepFlavour"
    getattr(process, taginfo).puppi_value_map = ""

# End of b-tagging sequence ----------------

# QGLikelihood
process.load("DeepNTuples.DeepNtuplizer.QGLikelihood_cfi")
process.es_prefer_jec = cms.ESPrefer("PoolDBESSource", "QGPoolDBESSource")
process.load('RecoJets.JetProducers.QGTagger_cfi')
process.QGTagger.srcJets   = "selectedUpdatedPatJetsDeepFlavour"
process.QGTagger.jetsLabel = 'QGL_AK4PFchs'
process.QGPoolDBESSource.connect = 'sqlite_file:QGL_cmssw8020_v2.db'

# Match with gen jets
from PhysicsTools.PatAlgos.producersLayer1.jetProducer_cff import patJetGenJetMatch
process.patGenJetMatchWithNu = patJetGenJetMatch.clone(
    src         = "selectedUpdatedPatJetsDeepFlavour", # RECO jets (any View<Jet> is ok)
    matched     = "ak4GenJetsWithNu",  # GEN jets  (must be GenJetCollection)
    resolveAmbiguities = True,         # Forbid two RECO objects to match to the same GEN object
)
process.patGenJetMatchAllowDuplicates = process.patGenJetMatchWithNu.clone(
    resolveAmbiguities = False,        # Forbid two RECO objects to match to the same GEN object
)
process.patGenJetMatchRecluster = process.patGenJetMatchWithNu.clone(
    matched = "ak4GenJetsRecluster",   # GEN jets  (must be GenJetCollection)              
)

# Match with unsubtracted jets
process.unsubJetMap = process.unsubUpdatedPatJetsDeepFlavour.clone(
    source = "selectedUpdatedPatJetsDeepFlavour"
)

for mod in ["QGTagger", "patGenJetMatchWithNu", "patGenJetMatchAllowDuplicates", "patGenJetMatchRecluster", "unsubJetMap"]:
    process.patAlgosToolsTask.add(getattr(process, mod))

# ------------------------------------------------------------------------- #

outFileName = options.outputFile + '_' + str(options.job) +  '.root'
print ('Using output file ' + outFileName)

process.TFileService = cms.Service("TFileService", 
                                   fileName = cms.string(outFileName))

# Heavy-ion specific
process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
process.centralityBin.Centrality = cms.InputTag("hiCentrality")
process.centralityBin.centralityVariable = cms.string("HFtowers")
process.patAlgosToolsTask.add(process.centralityBin)

# DeepNtuplizer
process.load("DeepNTuples.DeepNtuplizer.DeepNtuplizer_cfi")
process.deepntuplizer.jets = "selectedUpdatedPatJetsDeepFlavour"
process.deepntuplizer.bDiscriminators = bTagDiscriminators 
process.deepntuplizer.puppi = False
process.deepntuplizer.sort_cand_by_pt = True
process.deepntuplizer.unsubjet_map = cms.InputTag("unsubJetMap")
process.deepntuplizer.SVs = svSource
process.deepntuplizer.secVertices = svSource
process.deepntuplizer.cent = cms.InputTag("centralityBin","HFtowers")

process.deepntuplizer.applySelection = options.selectJets
process.deepntuplizer.tagInfoName = "pfDeepCSV"
process.deepntuplizer.jetPtMin = jetPtMin
process.deepntuplizer.jetAbsEtaMax = jetAbsEtaMax
process.deepntuplizer.gluonReduction  = options.gluonReduction

process.deepntuplizer.packed = "packedGenParticlesSignal"
process.deepntuplizer.genJets = "ak4GenJetsWithNu"
process.deepntuplizer.pruned = "hiSignalGenParticles"

#1631
process.ProfilerService = cms.Service (
      "ProfilerService",
       firstEvent = cms.untracked.int32(1631),
       lastEvent = cms.untracked.int32(1641),
       paths = cms.untracked.vstring('p') 
)

#==============================================================================================================================#
process.p = cms.Path(process.deepntuplizer)
process.p.associate(process.genTask)
process.p.associate(process.svTask)
process.p.associate(process.patAlgosToolsTask)
