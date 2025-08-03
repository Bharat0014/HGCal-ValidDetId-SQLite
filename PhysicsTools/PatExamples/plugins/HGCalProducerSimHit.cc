// -*- C++ -*-
//
// Package:    HGCalDetIDvalidation/HGCalRawProducernew
// Class:      
//
/*

 Description: Example module for raw detID validation store in Pcalo step1.root file 

 Implementation:
    
*/
//
// Original Author:  Bsirasva
//    



#ifndef HGCALPRODUCERSIMHIT_H
#define HGCALPRODUCERSIMHIT_H

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm> // For std::min_element and std::max_element
#include <iterator>
#include <random>
#include "TCanvas.h"
#include "TH2F.h"
#include "TStyle.h"
#include <TPolyMarker.h>
#include <TGraph.h>
#include <TColor.h>

#include "SimDataFormats/CaloHit/interface/PCaloHit.h"
#include "DataFormats/DetId/interface/DetId.h"

#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/CaloHit/interface/PCaloHitContainer.h"

#include "DataFormats/ForwardDetId/interface/ForwardSubdetector.h"
#include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCScintillatorDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCalTriggerDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetIdToModule.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetIdToROC.h"
#include "DataFormats/ForwardDetId/interface/HGCHEDetId.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/EventSetupRecord.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "FWCore/Framework/interface/ModuleFactory.h"

#include "Geometry/Records/interface/HGCalGeometryRecord.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloTopology/interface/HGCalTopology.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "Geometry/HGCalCommonData/interface/HGCalParameters.h"
#include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"
#include "Geometry/HGCalCommonData/interface/HGCalDDDConstants.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/ForwardGeometry/interface/CastorGeometry.h"
#include "Geometry/HGCalCommonData/interface/HGCalWaferType.h"
#include "HepMC/GenEvent.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"

class HGCalProducerSimHit : public edm::stream::EDProducer<> {
public:
  explicit HGCalProducerSimHit(const edm::ParameterSet&);
  ~HGCalProducerSimHit() override {}

  void produce(edm::Event&, const edm::EventSetup&) override;
  
private:
  std::string infoFileName_;

  edm::ESGetToken<HGCalGeometry, IdealGeometryRecord> geomTokenEE_;
  edm::ESGetToken<HGCalGeometry, IdealGeometryRecord> geomTokenHESil_;
  edm::ESGetToken<HGCalGeometry, IdealGeometryRecord> geomTokenHESci_;

    edm::PCaloHitContainer HGCHitsEE;
    edm::PCaloHitContainer HGCHitsHEback;
    edm::PCaloHitContainer HGCHitsHEfront;
    std::vector<PSimHit> hitsBarrel_;
    std::vector<PSimHit> hitsEndcap_;
    std::vector<SimTrack> simTracks_;
    std::vector<SimVertex> simVertices_;
    std::vector<PSimHit> muonCSCHits_;
    std::vector<PSimHit> muonDTHits_;
    std::vector<PSimHit> muonGEMHits_;
    std::vector<PSimHit> muonME0Hits_;
    std::vector<PSimHit> muonRPCHits_;
    std::vector<PSimHit> plTHits_;
    std::vector<PSimHit> trackerHitsPixelBarrelHighTof_;
    std::vector<PSimHit> trackerHitsPixelBarrelLowTof_;
    std::vector<PSimHit> trackerHitsPixelEndcapHighTof_;
    std::vector<PSimHit> trackerHitsPixelEndcapLowTof_;
    std::vector<PSimHit> trackerHitsTECHighTof_;
    std::vector<PSimHit> trackerHitsTECLowTof_;
    std::vector<PSimHit> trackerHitsTIBHighTof_;
    std::vector<PSimHit> trackerHitsTIBLowTof_;
    std::vector<PSimHit> trackerHitsTIDHighTof_;
    std::vector<PSimHit> trackerHitsTIDLowTof_;
    std::vector<PSimHit> trackerHitsTOBHighTof_;
    std::vector<PSimHit> trackerHitsTOBLowTof_;
    std::vector<PCaloHit> hcalHits_;
    std::vector<PCaloHit> zdcHits_;
    std::vector<PSimHit> bcm1fHits_;
    std::vector<PSimHit> bhmHits_;
    std::vector<PSimHit> ctppsPixelHits_;
    std::vector<PSimHit> ctppsTimingHits_;
    std::vector<PCaloHit> calibrationHGCHitsEE_;
    std::vector<PCaloHit> calibrationHGCHitsHEback_;
    std::vector<PCaloHit> calibrationHGCHitsHEfront_;
    std::vector<PCaloHit> caloHitsTk_;
    std::vector<PCaloHit> ecalHitsEB_;
    std::vector<PCaloHit> hfnoseHits_;
};

HGCalProducerSimHit::HGCalProducerSimHit(const edm::ParameterSet& iConfig)
    : infoFileName_(iConfig.getParameter<std::string>("infoFileName")),
      geomTokenEE_(esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag{"", "HGCalEESensitive"})),
      geomTokenHESil_(esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag{"", "HGCalHESiliconSensitive"})),
      geomTokenHESci_(esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag{"", "HGCalHEScintillatorSensitive"})){	      
      produces<edm::PCaloHitContainer>("HGCHitsEE");
      produces<edm::PCaloHitContainer>("HGCHitsHEback");
      produces<edm::PCaloHitContainer>("HGCHitsHEfront");     
      produces<std::vector<PSimHit>>("FastTimerHitsBarrel");
      produces<std::vector<PSimHit>>("FastTimerHitsEndcap");
      produces<std::vector<SimTrack>>("");
      produces<std::vector<SimVertex>>("");     
      produces<std::vector<PSimHit>>("MuonCSCHits");
      produces<std::vector<PSimHit>>("MuonDTHits");
      produces<std::vector<PSimHit>>("MuonGEMHits");
      produces<std::vector<PSimHit>>("MuonME0Hits");
      produces<std::vector<PSimHit>>("MuonRPCHits");
      produces<std::vector<PSimHit>>("PLTHits");
      produces<std::vector<PSimHit>>("TrackerHitsPixelBarrelHighTof");
  produces<std::vector<PSimHit>>("TrackerHitsPixelBarrelLowTof");
  produces<std::vector<PSimHit>>("TrackerHitsPixelEndcapHighTof");
  produces<std::vector<PSimHit>>("TrackerHitsPixelEndcapLowTof");
  produces<std::vector<PSimHit>>("TrackerHitsTECHighTof");
  produces<std::vector<PSimHit>>("TrackerHitsTECLowTof");
  produces<std::vector<PSimHit>>("TrackerHitsTIBHighTof");
  produces<std::vector<PSimHit>>("TrackerHitsTIBLowTof");
  produces<std::vector<PSimHit>>("TrackerHitsTIDHighTof");
  produces<std::vector<PSimHit>>("TrackerHitsTIDLowTof");
  produces<std::vector<PSimHit>>("TrackerHitsTOBHighTof");
  produces<std::vector<PSimHit>>("TrackerHitsTOBLowTof");
  produces<std::vector<PCaloHit>>("HcalHits");
  produces<std::vector<PCaloHit>>("ZDCHITS");
  produces<std::vector<PSimHit>>("BCM1FHits");
  produces<std::vector<PSimHit>>("BHMHits");
  produces<std::vector<PSimHit>>("CTPPSPixelHits");
  produces<std::vector<PSimHit>>("CTPPSTimingHits");
      produces<std::vector<PCaloHit>>("CalibrationHGCHitsEE");
  produces<std::vector<PCaloHit>>("CalibrationHGCHitsHEback");
  produces<std::vector<PCaloHit>>("CalibrationHGCHitsHEfront");
  produces<std::vector<PCaloHit>>("CaloHitsTk");
  produces<std::vector<PCaloHit>>("EcalHitsEB");
  produces<std::vector<PCaloHit>>("HFNoseHits");
      }

void HGCalProducerSimHit::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
   
  auto  HGCHitsEE = std::make_unique<edm::PCaloHitContainer>();
    auto HGCHitsHEback = std::make_unique<edm::PCaloHitContainer>();
    auto HGCHitsHEfront = std::make_unique<edm::PCaloHitContainer>();
  edm::ESHandle<HGCalGeometry> geomHandleEE;
  edm::ESHandle<HGCalGeometry> geomHandleHESil;
  edm::ESHandle<HGCalGeometry> geomHandleHESci;

  // Attempt to retrieve HGCalGeometry using the provided token
  try {
    geomHandleEE = iSetup.getHandle(geomTokenEE_);
    geomHandleHESil = iSetup.getHandle(geomTokenHESil_);
    geomHandleHESci = iSetup.getHandle(geomTokenHESci_);
  } catch (cms::Exception& e) {
    edm::LogError("HGCalRawDet") << "Failed to retrieve HGCalGeometry: " << e.what();
    throw;
  }

  // Check if the handle is valid before proceeding
  if (!geomHandleEE.isValid() || !geomHandleHESil.isValid() || !geomHandleHESci.isValid()) {
    edm::LogError("HGCalRawDet") << "Failed to retrieve valid HGCalGeometry!";
    throw cms::Exception("DataError") << "Failed to retrieve valid HGCalGeometry!";
  }
  const HGCalGeometry& geomEE = *geomHandleEE;
  const HGCalGeometry& geomHESil = *geomHandleHESil;
  const HGCalGeometry& geomHESci = *geomHandleHESci;

  // Debug output to confirm HGCalGeometry is loaded
  edm::LogInfo("HGCalRawDet") << "Successfully retrieved HGCalGeometry.";


std::ifstream inputFile(infoFileName_);
  if (!inputFile.is_open()) {
    throw cms::Exception("FileOpenError") << "Failed to open file: " << infoFileName_;
  }
   
  std::string line;
  int lineNumber = 0;
  int itra =0;
  while (std::getline(inputFile, line)) {
    lineNumber++;
     std::cout<<"linenumber" <<lineNumber<<std::endl;
    std::istringstream iss(line);
     uint32_t detId;
    int det_type, wafer_type, z_side, layer_number;
    int abs_u, abs_v, u_coordinate, v_coordinate;
    int layertype, frontBack, index_;
    int partialType, orient, placeIndex, waferType_;

    char delimiter;

    if (!(iss >> detId >> delimiter
              >> det_type >> delimiter
              >> layer_number)) {
     edm::LogWarning("HGCalRawDet") << "Skipping invalid data at line " << lineNumber<< ": " << line;
      continue;
    }
 
    
    
 

   float  em=1;
   float time = 20;
   float energy = 0.0002;
   int itra = 0;
   PCaloHit newHit; 
 
  
   newHit = PCaloHit(detId,energy, time, itra, em, 0);


if ( det_type==8) {
        HGCHitsEE->push_back(newHit);
      } else if ((det_type==9 || det_type==10) &&(layer_number>=1 && layer_number<=11) ) {
        HGCHitsHEfront->push_back(newHit);
      } else if((det_type==9 || det_type==10) && layer_number>11){
        HGCHitsHEback->push_back(newHit);
      }
  }


inputFile.close();

iEvent.put(std::move(HGCHitsEE), "HGCHitsEE");
iEvent.put(std::move(HGCHitsHEback), "HGCHitsHEback");
iEvent.put(std::move(HGCHitsHEfront), "HGCHitsHEfront");
iEvent.put(std::make_unique<std::vector<PSimHit>>(hitsBarrel_), "FastTimerHitsBarrel");
iEvent.put(std::make_unique<std::vector<PSimHit>>(hitsEndcap_), "FastTimerHitsEndcap");
iEvent.put(std::make_unique<std::vector<SimTrack>>(simTracks_), "");
iEvent.put(std::make_unique<std::vector<SimVertex>>(simVertices_), "");
iEvent.put(std::make_unique<std::vector<PSimHit>>(muonCSCHits_), "MuonCSCHits");
iEvent.put(std::make_unique<std::vector<PSimHit>>(muonDTHits_), "MuonDTHits");
iEvent.put(std::make_unique<std::vector<PSimHit>>(muonGEMHits_), "MuonGEMHits");
iEvent.put(std::make_unique<std::vector<PSimHit>>(muonME0Hits_), "MuonME0Hits");
iEvent.put(std::make_unique<std::vector<PSimHit>>(muonRPCHits_), "MuonRPCHits");
iEvent.put(std::make_unique<std::vector<PSimHit>>(plTHits_), "PLTHits");
iEvent.put(std::make_unique<std::vector<PSimHit>>(trackerHitsPixelBarrelHighTof_), "TrackerHitsPixelBarrelHighTof");
iEvent.put(std::make_unique<std::vector<PSimHit>>(trackerHitsPixelBarrelLowTof_), "TrackerHitsPixelBarrelLowTof");
iEvent.put(std::make_unique<std::vector<PSimHit>>(trackerHitsPixelEndcapHighTof_), "TrackerHitsPixelEndcapHighTof");
iEvent.put(std::make_unique<std::vector<PSimHit>>(trackerHitsPixelEndcapLowTof_), "TrackerHitsPixelEndcapLowTof");
iEvent.put(std::make_unique<std::vector<PSimHit>>(trackerHitsTECHighTof_), "TrackerHitsTECHighTof");
iEvent.put(std::make_unique<std::vector<PSimHit>>(trackerHitsTECLowTof_), "TrackerHitsTECLowTof");
iEvent.put(std::make_unique<std::vector<PSimHit>>(trackerHitsTIBHighTof_), "TrackerHitsTIBHighTof");
iEvent.put(std::make_unique<std::vector<PSimHit>>(trackerHitsTIBLowTof_), "TrackerHitsTIBLowTof");
iEvent.put(std::make_unique<std::vector<PSimHit>>(trackerHitsTIDHighTof_), "TrackerHitsTIDHighTof");
iEvent.put(std::make_unique<std::vector<PSimHit>>(trackerHitsTIDLowTof_), "TrackerHitsTIDLowTof");
iEvent.put(std::make_unique<std::vector<PSimHit>>(trackerHitsTOBHighTof_), "TrackerHitsTOBHighTof");
iEvent.put(std::make_unique<std::vector<PSimHit>>(trackerHitsTOBLowTof_), "TrackerHitsTOBLowTof");
iEvent.put(std::make_unique<std::vector<PCaloHit>>(hcalHits_), "HcalHits");
iEvent.put(std::make_unique<std::vector<PCaloHit>>(zdcHits_), "ZDCHITS");
iEvent.put(std::make_unique<std::vector<PSimHit>>(bcm1fHits_), "BCM1FHits");
iEvent.put(std::make_unique<std::vector<PSimHit>>(bhmHits_), "BHMHits");
iEvent.put(std::make_unique<std::vector<PSimHit>>(ctppsPixelHits_), "CTPPSPixelHits");
iEvent.put(std::make_unique<std::vector<PSimHit>>(ctppsTimingHits_), "CTPPSTimingHits");
iEvent.put(std::make_unique<std::vector<PCaloHit>>(calibrationHGCHitsEE_), "CalibrationHGCHitsEE");
iEvent.put(std::make_unique<std::vector<PCaloHit>>(calibrationHGCHitsHEback_), "CalibrationHGCHitsHEback");
iEvent.put(std::make_unique<std::vector<PCaloHit>>(calibrationHGCHitsHEfront_), "CalibrationHGCHitsHEfront");
iEvent.put(std::make_unique<std::vector<PCaloHit>>(caloHitsTk_), "CaloHitsTk");
iEvent.put(std::make_unique<std::vector<PCaloHit>>(ecalHitsEB_), "EcalHitsEB");
iEvent.put(std::make_unique<std::vector<PCaloHit>>(hfnoseHits_), "HFNoseHits");
}

DEFINE_FWK_MODULE(HGCalProducerSimHit);

#endif // HGCALPRODUCERSIMHIT_H

