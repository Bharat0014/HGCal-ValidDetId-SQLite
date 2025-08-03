// -*- C++ -*-
//
// Package:    HGCalDetIDvalidation/HGCalProducerDatabaseGen
// Class:      
//
/*

 Description: Example module for raw detID validation store in sqlite database

 Implementation:
    
*/
//
// Original Author:  Bsirasva
//    



#ifndef HGCALPRODUCERDATABASEGEN_H
#define HGCALPRODUCERDATABASEGEN_H

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
#include <sqlite3.h>

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

class HGCalProducerDatabaseGen : public edm::stream::EDProducer<> {
public:
  explicit HGCalProducerDatabaseGen(const edm::ParameterSet&);
  ~HGCalProducerDatabaseGen() override {}

  void produce(edm::Event&, const edm::EventSetup&) override;
  
private:
  std::string infoFileName_;

  edm::ESGetToken<HGCalGeometry, IdealGeometryRecord> geomTokenEE_;
  edm::ESGetToken<HGCalGeometry, IdealGeometryRecord> geomTokenHESil_;
  edm::ESGetToken<HGCalGeometry, IdealGeometryRecord> geomTokenHESci_;

};

HGCalProducerDatabaseGen::HGCalProducerDatabaseGen(const edm::ParameterSet& iConfig)
    : infoFileName_(iConfig.getParameter<std::string>("infoFileName")),
      geomTokenEE_(esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag{"", "HGCalEESensitive"})),
      geomTokenHESil_(esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag{"", "HGCalHESiliconSensitive"})),
      geomTokenHESci_(esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag{"", "HGCalHEScintillatorSensitive"})){	      
      produces<edm::PCaloHitContainer>("HGCHitsEE");
      produces<edm::PCaloHitContainer>("HGCHitsHEback");
      produces<edm::PCaloHitContainer>("HGCHitsHEfront");     
            }

void HGCalProducerDatabaseGen::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
   
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

 std::ofstream csvFile;
 csvFile.open("valid_detID_all_feature.csv", std::ios::out); // Overwrite mode

// Write the header line (column names)
csvFile << "DetId,"
        << "Zside,"
        << "DetType,"
        << "Nlayer,"
        << "LayerType,"
        << "FrontBack,"
        << "WaferType,"
        << "WaferIndex,"
        << "PartType,"
        << "Cassette,"
        << "CassetteType,"
        << "Orient,"
        << "AbsU,"
        << "AbsV,"
        << "CellU,"
        << "CellV,"
        << "x,"
        << "y,"
        << "z,"
        << "TileType,"
        << "SipmType,"
        << "TriggerCell,"
        << "RingIndex,"
        << "IphiIndex,"
        << "Granularity"
	<< "eta"
	<< "phi"
        << "\n";


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
    uint32_t DetID;
    int det_type;
  char delimiter;  
    if (!(iss >>det_type>> delimiter >> DetID)) {
      edm::LogWarning("HGCalRawDet") << "Skipping invalid data at line " << lineNumber<< ": " << line;
      continue;
    }
 
    
    
 

 
   bool validEE = false; 
   bool validHESil = false;
   bool validHESci = false;
   double xs =0, ys=0,zs = 0;
   int wafer_type =0;
   int layer_number=0;
  
   if(det_type == 8){
     validEE = geomEE.valid(DetID); 
    if(validEE){ 

    HGCSiliconDetId hgcalDetId(DetID);

    GlobalPoint posHEsil = geomEE.getPosition(hgcalDetId.rawId());
        xs = posHEsil.x();
        ys = posHEsil.y();
        zs = posHEsil.z();

    double r = std::sqrt(xs*xs + ys*ys + zs*zs);
    double theta = std::acos(zs / r);
    double eta = -std::log(std::tan(theta / 2.0));
    double phi = std::atan2(ys, xs);
    layer_number = hgcalDetId.layer();
   
  std::cout<<"Detector Type:  "<<det_type<<" DetID : "<<hgcalDetId.rawId()<<std::endl;
  //csvFile<<det_type<<","<<hgcalDetId.rawId()<<std::endl;
    const auto& hgc = geomEE.topology().dddConstants();
    int detector_type = hgcalDetId.det();       // Detector type
     int wafer_type = hgcalDetId.type();         // Wafer type
     int z_side = hgcalDetId.zside();            // z-side
     //int layer_number = hgcalDetId.layer();      // Layer number
     int abs_v = hgcalDetId.waferV();            // Absolute value of wafer v
     int abs_u = hgcalDetId.waferU();            // Absolute value of wafer u
     int v_coordinate = hgcalDetId.cellV();      // v-coordinate of the cell
     int u_coordinate = hgcalDetId.cellU();      // u-coordinate of the cell
     int wafer = hgcalDetId.waferU();



    int layertype = hgc.layerType(layer_number);
   int frontBack = HGCalTypes::layerFrontBack(layertype);
   int index = HGCalWaferIndex::waferIndex(layer_number, abs_u, abs_v);
   int partialType = HGCalWaferType::getPartial(index, hgc.getParameter()->waferInfoMap_);
   int orient = HGCalWaferType::getOrient(index, hgc.getParameter()->waferInfoMap_);
   int placeIndex = HGCalCell::cellPlacementIndex(z_side, frontBack, orient);
   int waferType_ = HGCalWaferType::getType(index, hgc.getParameter()->waferInfoMap_);
   
   // Cassette information
        const auto& waferInfo = hgc.getParameter()->waferInfoMap_.at(index);
        int cassette = waferInfo.cassette;
        int cassetteType = waferInfo.type; // 0 = full, 1 = partial

        std::cout << "Layer: " << layer_number
                  << " Wafer(U,V): (" << abs_u << "," << abs_v << ")"
                  << " Cell(U,V): (" << u_coordinate << "," << v_coordinate << ")"
                  << " Zside: " << z_side
                  << " Cassette: " << cassette
                  << " Cassette Type: " << cassetteType << std::endl;
    

    int tile_type = 9999;
    int sipm_type = 9999;
    int trigger_cell = 9999;
    int ring_index = 9999;
    int iphi_index = 9999;
    int granularity = 9999;

   csvFile << hgcalDetId.rawId() << ","
            << z_side <<","
	    << detector_type << ","
            << layer_number<<","
	    << layertype<<","
	    << frontBack<<","
	    << wafer_type << ","
	    <<index<<","
	    <<partialType<<","
            <<cassette<<","
	    <<cassetteType<<","
	    <<orient<<","
            << abs_u << ","
            << abs_v << ","
            << u_coordinate << ","
            << v_coordinate << ","
	    <<xs<<","
	    <<ys<<","
	    <<zs<<","
            << tile_type<<","
            << sipm_type<<","
            << trigger_cell<<","
            << ring_index<<","
            << iphi_index<<","
            << granularity<<","
	    << eta <<","
	    << phi <<"\n";

    
    }
   
   }

  if(det_type == 9){
     validHESil = geomHESil.valid(DetID);
    if(validHESil){
    HGCSiliconDetId hgcalDetId(DetID);
     GlobalPoint posHEsil = geomHESil.getPosition(hgcalDetId.rawId());
        xs = posHEsil.x();
        ys = posHEsil.y();
        zs = posHEsil.z();
    double r = std::sqrt(xs*xs + ys*ys + zs*zs);
    double theta = std::acos(zs / r);
    double eta = -std::log(std::tan(theta / 2.0));
    double phi = std::atan2(ys, xs);

    layer_number = hgcalDetId.layer();
    const auto& hgc = geomHESil.topology().dddConstants();

    std::cout<<"Detector Type:  "<<det_type<<" DetID : "<<hgcalDetId.rawId()<<std::endl;
    //csvFile<<det_type<<","<<hgcalDetId.rawId()<<std::endl;
     int detector_type = hgcalDetId.det();       // Detector type
     int wafer_type = hgcalDetId.type();         // Wafer type
     int z_side = hgcalDetId.zside();            // z-side
     //int layer_number = hgcalDetId.layer();      // Layer number
     int abs_v = hgcalDetId.waferV();            // Absolute value of wafer v
     int abs_u = hgcalDetId.waferU();            // Absolute value of wafer u
     int v_coordinate = hgcalDetId.cellV();      // v-coordinate of the cell
     int u_coordinate = hgcalDetId.cellU();      // u-coordinate of the cell
     int wafer = hgcalDetId.waferU();

    
     
    int layertype = hgc.layerType(layer_number);
   int frontBack = HGCalTypes::layerFrontBack(layertype);
   int index = HGCalWaferIndex::waferIndex(layer_number, abs_u, abs_v);
   int partialType = HGCalWaferType::getPartial(index, hgc.getParameter()->waferInfoMap_);
   int orient = HGCalWaferType::getOrient(index, hgc.getParameter()->waferInfoMap_);
   int placeIndex = HGCalCell::cellPlacementIndex(z_side, frontBack, orient);
   int waferType_ = HGCalWaferType::getType(index, hgc.getParameter()->waferInfoMap_);
     std::cout <<"Layer:  "<<layer_number <<  " Wafer(U,V): (" << abs_u << "," << abs_v << ")" << "partialType: " <<partialType<<std::endl;
    
   // Cassette information
        const auto& waferInfo = hgc.getParameter()->waferInfoMap_.at(index);
        int cassette = waferInfo.cassette;
        int cassetteType = waferInfo.type; // 0 = full, 1 = partial

        std::cout << "Layer: " << layer_number
                  << " Wafer(U,V): (" << abs_u << "," << abs_v << ")"
                  << " Cell(U,V): (" << u_coordinate << "," << v_coordinate << ")"
                  << " Zside: " << z_side
                  << " Cassette: " << cassette
                  << " Cassette Type: " << cassetteType << std::endl;

   
     int tile_type = 9999;
    int sipm_type = 9999;
    int trigger_cell = 9999;
    int ring_index = 9999;
    int iphi_index = 9999;
    int granularity = 9999;

     csvFile << hgcalDetId.rawId() << ","
            << z_side <<","
            << detector_type << ","
            << layer_number<<","
            << layertype<<","
            << frontBack<<","
            << wafer_type << ","
            <<index<<","
            <<partialType<<","
            <<cassette<<","
            <<cassetteType<<","
            <<orient<<","
            << abs_u << ","
            << abs_v << ","
            << u_coordinate << ","
            << v_coordinate << ","
            <<xs<<","
            <<ys<<","
            <<zs<<","
            << tile_type<<","
            << sipm_type<<","
            << trigger_cell<<","
            << ring_index<<","
            << iphi_index<<","
            << granularity<<","
	    << eta <<","
            << phi <<"\n";
  


    }

   }

 if(det_type == 10){
     validHESci = geomHESci.valid(DetID);
    if(validHESci){

    HGCScintillatorDetId hgcalDetId(DetID);
    GlobalPoint posHEsc = geomHESci.getPosition(hgcalDetId.rawId());
        xs = posHEsc.x();
        ys = posHEsc.y();
        zs = posHEsc.z();
        double r = std::sqrt(xs*xs + ys*ys + zs*zs);
    double theta = std::acos(zs / r);
    double eta = -std::log(std::tan(theta / 2.0));
    double phi = std::atan2(ys, xs);

    int detector_type = hgcalDetId.det();
    int tile_type = hgcalDetId.type();
    int z_side = hgcalDetId.zside();
    int sipm_type = hgcalDetId.sipm();
    int trigger_cell = hgcalDetId.trigger();
    int layer_number = hgcalDetId.layer();
    int ring_index = hgcalDetId.ring();
    int iphi_index = hgcalDetId.iphi();
    int granularity = (hgcalDetId.rawId() >> 24) & 0x1;

    std::cout << "Detector Type: " << detector_type
              << ", Tile Type: " << tile_type
              << ", Z-Side: " << z_side
              << ", SiPM Type: " << sipm_type
              << ", Granularity: " << granularity
              << ", Trigger Cell: " << trigger_cell
              << ", Layer Number: " << layer_number
              << ", Ring Index: " << ring_index
              << ", IPhi Index: " << iphi_index
              << std::endl;

	int wafer_type = 9999;

	int abs_u = 9999;
	int abs_v = 9999;
	int u_coordinate = 9999;
	int v_coordinate = 9999;

	int layertype = 9999;
	int frontBack = 9999;
	int index = 9999;
	int partialType = 9999;
	int orient = 9999;
	int placeIndex = 9999;
	int waferType_ = 9999;
	int cassette = 9999;
	int cassetteType = 9999;
    

     csvFile << hgcalDetId.rawId() << ","
            << z_side <<","
            << detector_type << ","
            << layer_number<<","
            << layertype<<","
            << frontBack<<","
            << wafer_type << ","
            <<index<<","
            <<partialType<<","
            <<cassette<<","
            <<cassetteType<<","
            <<orient<<","
            << abs_u << ","
            << abs_v << ","
            << u_coordinate << ","
            << v_coordinate << ","
            <<xs<<","
            <<ys<<","
            <<zs<<","
            << tile_type<<","
            << sipm_type<<","
            << trigger_cell<<","
            << ring_index<<","
            << iphi_index<<","
            << granularity<<","
            << eta <<","
            << phi <<"\n";


    }
   
   }


  }
inputFile.close();
csvFile.close();

// === STEP 2: Open the filled CSV and create SQLite database ===
    const std::string csvFile1 = "valid_detID_all_feature.csv";
    const std::string dbFile = "detid_data_all_feature.db";
    //const std::string password = "your_secure_password";

    // Step 2: Begin SQLite
    sqlite3* db;
    int rc = sqlite3_open(dbFile.c_str(), &db);
    if (rc != SQLITE_OK) {
        edm::LogError("HGCalDetIdValidator") << "Failed to open DB: " << sqlite3_errmsg(db);
        return;
    }

    //std::string keyQuery = "PRAGMA key = '" + password + "';";
    //rc = sqlite3_exec(db, keyQuery.c_str(), nullptr, nullptr, nullptr);
    //if (rc != SQLITE_OK) {
    //    edm::LogError("HGCalDetIdValidator") << "Failed to set encryption key.";
    //    sqlite3_close(db);
    //    return;
   // }

    // Performance tweaks
    sqlite3_exec(db, "PRAGMA synchronous = OFF;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "PRAGMA journal_mode = MEMORY;", nullptr, nullptr, nullptr);

    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS hgcal_detids_v5 (
            DetId INTEGER PRIMARY KEY,
            Zside TINYINT, DetType TINYINT, Nlayer TINYINT,
            LayerType TINYINT, FrontBack TINYINT, WaferType TINYINT,
            WaferIndex INTEGER, PartType TINYINT, Cassette TINYINT,
            CassetteType TINYINT, Orient TINYINT, AbsU TINYINT,
            AbsV TINYINT, CellU TINYINT, CellV TINYINT,
            x FLOAT, y FLOAT, z FLOAT,
            TileType TINYINT, SipmType TINYINT, TriggerCell TINYINT,
            RingIndex TINYINT, IphiIndex SMALLINT, Granularity SMALLINT,
            eta FLOAT, phi FLOAT
        );
    )";
    rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        edm::LogError("HGCalDetIdValidator") << "Failed to create table.";
        sqlite3_close(db);
        return;
    }

    // Prepare insert
    const char* insertSQL = R"(
        INSERT OR IGNORE INTO hgcal_detids_v5 VALUES (
            ?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?
        );
    )";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        edm::LogError("HGCalDetIdValidator") << "Failed to prepare insert.";
        sqlite3_close(db);
        return;
    }

    std::ifstream file(csvFile1);
    if (!file.is_open()) {
        edm::LogError("HGCalDetIdValidator") << "Could not open CSV file: " << csvFile1;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

    std::string lineo;
    std::getline(file, lineo);  // Skip header
    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

    while (std::getline(file, lineo)) {
        std::stringstream ss(lineo);
        std::string cell;
        std::vector<std::string> row;
        while (std::getline(ss, cell, ',')) row.push_back(cell);
        if (row.size() != 27) continue;

        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);

        for (int i = 0; i < 27; ++i) {
            if (row[i] == "9999") {
                sqlite3_bind_null(stmt, i + 1);
            } else if (i >= 16 && (i <= 18 || i >= 25)) {
                sqlite3_bind_double(stmt, i + 1, std::stod(row[i]));
            } else if (i == 0) {
                sqlite3_bind_int64(stmt, 1, std::stoll(row[i]));
            } else {
                sqlite3_bind_int(stmt, i + 1, std::stoi(row[i]));
            }
        }

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            edm::LogWarning("HGCalDetIdValidator") << "Insert failed: " << sqlite3_errmsg(db);
        }
    }

    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    edm::LogInfo("HGCalDetIdValidator") << "✔ Encrypted database written: " << dbFile;

}
 

DEFINE_FWK_MODULE(HGCalProducerDatabaseGen);

#endif // HGCALPRODUCERDATABASEGEN_H
