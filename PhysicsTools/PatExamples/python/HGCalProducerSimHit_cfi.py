import FWCore.ParameterSet.Config as cms

# Import the era to configure the process
from Configuration.Eras.Era_Phase2C17I13M9_cff import Phase2C17I13M9

# Define the process with the appropriate era
process = cms.Process('SIM', Phase2C17I13M9)

process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.Geometry.GeometryExtendedRun4D110Reco_cff')
process.load('Configuration.Geometry.GeometryExtendedRun4D110_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.Generator_cff')
process.load('IOMC.EventVertexGenerators.VtxSmearedHLLHC14TeV_cfi')
process.load('GeneratorInterface.Core.genFilterSummary_cff')
process.load('Configuration.StandardSequences.SimIdeal_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.load("SimG4Core.Application.g4SimHits_cfi")
# Optional: Include specific sub-detector simulation configurations

# Input source
process.source = cms.Source("EmptySource")


process.options = cms.untracked.PSet(
    IgnoreCompletely = cms.untracked.vstring(),
    Rethrow = cms.untracked.vstring(),
    TryToContinue = cms.untracked.vstring(),
    accelerators = cms.untracked.vstring('*'),
    allowUnscheduled = cms.obsolete.untracked.bool,
    canDeleteEarly = cms.untracked.vstring(),
    deleteNonConsumedUnscheduledModules = cms.untracked.bool(True),
    dumpOptions = cms.untracked.bool(False),
    emptyRunLumiMode = cms.obsolete.untracked.string,
    eventSetup = cms.untracked.PSet(
        forceNumberOfConcurrentIOVs = cms.untracked.PSet(
            allowAnyLabel_=cms.required.untracked.uint32
        ),
        numberOfConcurrentIOVs = cms.untracked.uint32(0)
    ),
    fileMode = cms.untracked.string('FULLMERGE'),
    forceEventSetupCacheClearOnNewRun = cms.untracked.bool(False),
    holdsReferencesToDeleteEarly = cms.untracked.VPSet(),
    makeTriggerResults = cms.obsolete.untracked.bool,
    modulesToCallForTryToContinue = cms.untracked.vstring(),
    modulesToIgnoreForDeleteEarly = cms.untracked.vstring(),
    numberOfConcurrentLuminosityBlocks = cms.untracked.uint32(0),
    numberOfConcurrentRuns = cms.untracked.uint32(1),
    numberOfStreams = cms.untracked.uint32(0),
    numberOfThreads = cms.untracked.uint32(1),
    printDependencies = cms.untracked.bool(False),
    sizeOfStackForThreadsInKB = cms.optional.untracked.uint32,
    throwIfIllegalParameter = cms.untracked.bool(True),
    wantSummary = cms.untracked.bool(False)
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)  # Set to the number of events you want to simulate
)

# Event content configuration (output all data)
process.load('Configuration.EventContent.EventContent_cff')

# Configure the output
process.FEVTDEBUGoutput = cms.OutputModule("PoolOutputModule",
        SelectEvents = cms.untracked.PSet(
            SelectEvents = cms.vstring('generation_step')
    ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('file:../Root_Files/step1.root'),
    outputCommands = cms.untracked.vstring(
        'keep *',
    
    ),
    splitLevel = cms.untracked.int32(0)
)


# Other statements
process.genstepfilter.triggerConditions=cms.vstring("generation_step")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic_T33', '')

process.generator = cms.EDFilter("Pythia8ConcurrentGeneratorFilter",
    PythiaParameters = cms.PSet(
        parameterSets = cms.vstring(
            'pythia8CommonSettings',
            'pythia8CP5Settings',
            'processParameters'
        ),
        processParameters = cms.vstring(
            'Top:gg2ttbar = on ',
            'Top:qqbar2ttbar = on ',
            '6:m0 = 175 '
        ),
        pythia8CP5Settings = cms.vstring(
            'Tune:pp 14',
            'Tune:ee 7',
            'MultipartonInteractions:ecmPow=0.03344',
            'MultipartonInteractions:bProfile=2',
            'MultipartonInteractions:pT0Ref=1.41',
            'MultipartonInteractions:coreRadius=0.7634',
            'MultipartonInteractions:coreFraction=0.63',
            'ColourReconnection:range=5.176',
            'SigmaTotal:zeroAXB=off',
            'SpaceShower:alphaSorder=2',
            'SpaceShower:alphaSvalue=0.118',
            'SigmaProcess:alphaSvalue=0.118',
            'SigmaProcess:alphaSorder=2',
            'MultipartonInteractions:alphaSvalue=0.118',
            'MultipartonInteractions:alphaSorder=2',
            'TimeShower:alphaSorder=2',
            'TimeShower:alphaSvalue=0.118',
            'SigmaTotal:mode = 0',
            'SigmaTotal:sigmaEl = 21.89',
            'SigmaTotal:sigmaTot = 100.309',
            'PDF:pSet=LHAPDF6:NNPDF31_nnlo_as_0118'
        ),
        pythia8CommonSettings = cms.vstring(
            'Tune:preferLHAPDF = 2',
            'Main:timesAllowErrors = 10000',
            'Check:epTolErr = 0.01',
            'Beams:setProductionScalesFromLHEF = off',
            'SLHA:minMassSM = 1000.',
            'ParticleDecays:limitTau0 = on',
            'ParticleDecays:tau0Max = 10',
            'ParticleDecays:allowPhotonRadiation = on'
        )
    ),
    comEnergy = cms.double(14000.0),
    filterEfficiency = cms.untracked.double(1.0),
    maxEventsToPrint = cms.untracked.int32(0),
    pythiaHepMCVerbosity = cms.untracked.bool(False),
    pythiaPylistVerbosity = cms.untracked.int32(0)
)


process.ProductionFilterSequence = cms.Sequence(process.generator)
# Define paths
process.generation_step = cms.Path(process.pgen)
process.psim = cms.Path(process.psim)

# Define the DetIdNewProducer module (HGCalProducerSimHit) with the input CSV file
process.g4SimHits = cms.EDProducer('HGCalProducerSimHit',
    infoFileName = cms.string("quried_detid_output.csv"),
    CalibrationHGCHitsEE = cms.InputTag("g4SimHits", "CalibrationHGCHitsEE"),
    CalibrationHGCHitsHEback = cms.InputTag("g4SimHits", "CalibrationHGCHitsHEback"),
    CalibrationHGCHitsHEfront = cms.InputTag("g4SimHits", "CalibrationHGCHitsHEfront"),
    CaloHitsTk = cms.InputTag("g4SimHits", "CaloHitsTk"),
    EcalHitsEB = cms.InputTag("g4SimHits", "EcalHitsEB"),
    HFNoseHits = cms.InputTag("g4SimHits", "HFNoseHits"),
    HGCHitsEE = cms.InputTag("g4SimHits", "HGCHitsEE"),
    HGCHitsHEback = cms.InputTag("g4SimHits", "HGCHitsHEback"),
    HGCHitsHEfront = cms.InputTag("g4SimHits", "HGCHitsHEfront"),
    HcalHits = cms.InputTag("g4SimHits", "HcalHits"),
    ZDCHITS = cms.InputTag("g4SimHits", "ZDCHITS"),

    # Input tags for PSimHit collections
    BCM1FHits = cms.InputTag("g4SimHits", "BCM1FHits"),
    BHMHits = cms.InputTag("g4SimHits", "BHMHits"),
    CTPPSPixelHits = cms.InputTag("g4SimHits", "CTPPSPixelHits"),
    CTPPSTimingHits = cms.InputTag("g4SimHits", "CTPPSTimingHits"),
    FastTimerHitsBarrel = cms.InputTag("g4SimHits", "FastTimerHitsBarrel"),
    FastTimerHitsEndcap = cms.InputTag("g4SimHits", "FastTimerHitsEndcap"),
    MuonCSCHits = cms.InputTag("g4SimHits", "MuonCSCHits"),
    MuonDTHits = cms.InputTag("g4SimHits", "MuonDTHits"),
    MuonGEMHits = cms.InputTag("g4SimHits", "MuonGEMHits"),
    MuonME0Hits = cms.InputTag("g4SimHits", "MuonME0Hits"),
    MuonRPCHits = cms.InputTag("g4SimHits", "MuonRPCHits"),
    PLTHits = cms.InputTag("g4SimHits", "PLTHits"),
    TrackerHitsPixelBarrelHighTof = cms.InputTag("g4SimHits", "TrackerHitsPixelBarrelHighTof"),
    TrackerHitsPixelBarrelLowTof = cms.InputTag("g4SimHits", "TrackerHitsPixelBarrelLowTof"),
    TrackerHitsPixelEndcapHighTof = cms.InputTag("g4SimHits", "TrackerHitsPixelEndcapHighTof"),
    TrackerHitsPixelEndcapLowTof = cms.InputTag("g4SimHits", "TrackerHitsPixelEndcapLowTof"),
    TrackerHitsTECHighTof = cms.InputTag("g4SimHits", "TrackerHitsTECHighTof"),
    TrackerHitsTECLowTof = cms.InputTag("g4SimHits", "TrackerHitsTECLowTof"),
    TrackerHitsTIBHighTof = cms.InputTag("g4SimHits", "TrackerHitsTIBHighTof"),
    TrackerHitsTIBLowTof = cms.InputTag("g4SimHits", "TrackerHitsTIBLowTof"),
    TrackerHitsTIDHighTof = cms.InputTag("g4SimHits", "TrackerHitsTIDHighTof"),
    TrackerHitsTIDLowTof = cms.InputTag("g4SimHits", "TrackerHitsTIDLowTof"),
    TrackerHitsTOBHighTof = cms.InputTag("g4SimHits", "TrackerHitsTOBHighTof"),
    TrackerHitsTOBLowTof = cms.InputTag("g4SimHits", "TrackerHitsTOBLowTof"),

    # Input tags for SimTrack and SimVertex collections
    SimTracks = cms.InputTag("g4SimHits", "SimTrack"),
    SimVertices = cms.InputTag("g4SimHits", "SimVertex") 
)

process.g4SimHits_step = cms.Path(process.g4SimHits)

process.genfiltersummary_step = cms.EndPath(process.genFilterSummary)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.FEVTDEBUGoutput_step = cms.EndPath(process.FEVTDEBUGoutput)

process.schedule = cms.Schedule(process.generation_step,process.genfiltersummary_step,process.g4SimHits_step,process.psim,process.endjob_step,process.FEVTDEBUGoutput_step)

from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)
# filter all path with the production filter sequence
for path in process.paths:
    getattr(process,path).insert(0, process.ProductionFilterSequence)


from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)

# Options to continue on error
process.options = cms.untracked.PSet(
    TryToContinue = cms.untracked.vstring('ProductNotFound')
)
