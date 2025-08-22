
#  HGCal DetId Validation, SQLite Database Creation, PCaloHit Workflow, and Visualization

This repository explains how **ValidDetIds** are used in the High Granularity Calorimeter (HGCal) geometry of CMS. The process includes creating raw DetIds, checking and validating them, storing them in the SQLite database, and generating SimHits (PCaloHits). Finally, the results can be visualized using Fireworks.

The workflow is split into **Admin Setup (once per Geometery release)** and **User Workflow**.

---

 ## Table of Contents
- [Step 0 : Enviroment Setup For Admin And User Both](#step-0--Enviroment-Setup-for-admin-and-user-both)
- [Admin Workflow (Once per Release)](#admin-workflow-once-per-release)
  - [Step A : DetId Definition and Raw CSV Creation](#step-a--detid-definition-and-raw-csv-creation-pre-validation)
  - [Step B : DetId Validation and Database Generation](#step-b--detid-validation-and-database-generation-once-per-release)
- [User Workflow](#user-workflow)
  - [Step 1: Use the Provided SQLite DB and Run Queries](#step-1-use-the-provided-sqlite-db-and-run-queries)
  - [Step 2: Development of SimHit Producer](#step-2-development-of-simhit-producer)
  - [Step 3: Multi-Step Processing Pipeline](#step-3-multi-step-processing-pipeline)
  - [Step 4: Visualization with Fireworks](#step-4-visualization-with-fireworks)
- [Folder Structure](#Folder-Structure)


---

## Enviroment Setup For Admin And User Both

This step is about setting up the **CMSSW environment** so that both admin and user can access all the necessary tools for this workflow. It also provides access to the relevant codes used in this work and connects to the **pre-calibrated database for HGCal geometry (version 17)**.

<details>
  <summary>Click here</summary>

### 1. Set Up the CMSSW Environment.


First login to your lxplus account then Set up the environment as follows:

```
# Load the CMSSW version
>> cd /path/to/your/working/directory
>> cmsrel CMSSW_15_1_0_pre4
>> cd CMSSW_15_1_0_pre4/src
>> cmsenv
```

### 2. Unpack necessary Package for HGcal Geometery.

```
>> git cms-addpkg Geometry/CMSCommonData
>> git cms-addpkg Configuration/Geometry
>> git cms-addpkg Geometry/HGCalCommonData
>> git cms-addpkg Geometry/HGCalGeometry
>> git cms-addpkg Geometry/Records
>> git cms-addpkg Fireworks

```

### 3. Git Clone This Repository.

```
>> git clone https://github.com/Bharat0014/HGCal-ValidDetId-SQLite.git
>> scram b -j8
```
**scarm b -j8** - This command is for compaliation of the cmssw code and the **-j8** is used to achive the good compilation speed through 8 cores.  

### 4. Download the Pre-generated SimHit and SQLite Dataset (For User Only).

```
Raw detIds (detid_list_all_combinations.csv)
>> cd src/PhysicsTools/PatExamples/Raw_detids/
>> wget https://cernbox.cern.ch/s/QYeBlRIib2XMP4G

Sqlite database for V17 (detid_data_all_feature.db)
>> cd src/PhysicsTools/PatExamples/python/
>> wget https://cernbox.cern.ch/s/oaIpes2jVCXOYib

Validate detIds in csv (valid_detID_all_feature.csv)
>> cd src/PhysicsTools/PatExamples/python/
>> wget https://cernbox.cern.ch/s/EEJgchI8OqolSEH

Producer output (step1.root)
>> cd src/PhysicsTools/PatExamples/Root_Files/
>> wget https://cernbox.cern.ch/s/YO8oZKZ3p6tD2sJ
```

</details>

---

## Admin Workflow (Once per Release)

This setup has to be used by the Admin and it is required to be executed **ONLY once per each HGCal geometry release** to first validate and then store valid DetIds in Sqlite databse for a new release of the geometry.


###  Step A : DetId Definition And Store Possible Combination Of DetIds In CSV Formate (Pre Validation)

In this step, we create a complete list of all possible Combination of DetIds for the given DetIds definations in the table below: [DetId Definations (Pre validation)](#DetId-Definations-(Pre-validation)). Then we store each created detIds in the CSV file for each of the HGCal subdetectors—EE, HE Silicon, and HE Scintillator—using the bit patterns shown in the tables. Each DetId is a unique code that describes a part of the detector, such as its layer number, wafer or tile type, and position (u and v for silicon; ring and iPhi for scintillator). We do this to make sure we include every possible detector location. These tables follow the official HGCal DetId format and the stroed detIds will be used in the next step to check the detIds against the actual detector layout. 

#### DetId Definations (Pre validation) 

<details>
  <summary>Click here</summary>

##### EE, HESilicon 

| Parameter         | Bit Position | Bit Length | Value Range                                         | Acceptable Combination |
|------------------|--------------|------------|-----------------------------------------------------|-------------------------|
| Detector Type     | 28–31        | 4 bits     | 8 (EE Silicon), 9 (HE Silicon)                      |2                       |
| Wafer Type        | 26–27        | 2 bits     | 0, 1, 2, 3                                          |4					   |
| Z Side            | 25           | 1 bit      | 0 (for +z), 1 (for –z)                              |2					   |
| Layer Number      | 20–24        | 5 bits     | 1–26 (for DetType 8), 1–21 (for DetType 9)          |26 (DetType 8), 21 (DetType 9) |
| Sign of v         | 19           | 1 bit      | 0 (+v), 1 (–v)                                      |2					   |
| Absolute v        | 15–18        | 4 bits     | 0 to 13                                             |14					   |
| Sign of u         | 14           | 1 bit      | 0 (+u), 1 (–u)                                      |2					   |
| Absolute u        | 10–13        | 4 bits     | 0 to 13                                             |14					   |
| v-coordinate      | 5–9          | 5 bits     | 0 to 15 (LD wafers 1,2), 0 to 23 (HD wafer 0)       |16(LD wafers 1,2), 24(HD wafer 0) |
| u-coordinate      | 0–4          | 5 bits     | 0 to 15 (LD wafers 1,2), 0 to 23 (HD wafer 0)       |16(LD wafers 1,2), 24(HD wafer 0) |

##### HE Scintillator

| Parameter           | Bit Position | Bit Length | Value Range                     | Acceptable Combination |
|--------------------|--------------|------------|---------------------------------|-------------------------|
| Detector Type       | 28–31        | 4 bits     | 10 (HGCalHSc = 1010)            |1                       |
| Tile Type           | 26–27        | 2 bits     | 1 (type "c"), 2 (type "m")      |2						 |
| Z Side              | 25           | 1 bit      | 0 for +z, 1 for –z              |2                       |
| Granularity         | 24–24        | 1 bit      | 0 (normal), 1 (fine)            |2                       | 
| SiPM Type           | 23–23        | 1 bit      | 0 (small), 1 (large)            |2                       |
| Trigger/Detector Cell | 22–22      | 1 bit      | 0 (Detector), 1 (Trigger)       |2                       |
| Layer Number        | 17–21        | 5 bits     | 8 to 21                         |14                      |
| Ring Index          | 9–16         | 8 bits     | 1 to 45                        |45                      |
| iPhi Index          | 0–8          | 9 bits     | 0 to 320                        |321                     |


</details>


The `DetIDRaw.py` script is generating the **raw Detector IDs (DetIDs)**. The code is listed below:

<details>
  <summary>Show DetIDRaw.py</summary>

```python
#DetIDRaw.py

import csv

def generate_all_hgcal_silicon_detids():
    """Generates all possible detector IDs for EE or HE Silicon"""
    combinations = []
    counter = 0  # Initialize a counter for combinations
    
    for detector_type in [8, 9]:  # [28:31] (1000 or 1001)
        # Set the layer range based on the detector type
        if detector_type == 8:
            layer_range = range(1, 27)  # Layer number from 1 to 26 for dettype 8
        elif detector_type == 9:
            layer_range = range(1, 22)  # Layer number from 1 to 21 for dettype 9
        
        for wafer_type in range(0, 4):  # [26:27] (00, 01, 10, 11)
            for z_side in range(0, 2):  # [25:25] (0 or 1)
                for layer_number in layer_range:  # Use the defined range for each dettype
                    for sign_v in range(0, 2):  # [19:19] (0 or 1)
                        for abs_v in range(0, 14):  # [15:18] (0 to 13)
                            for sign_u in range(0, 2):  # [14:14] (0 or 1)
                                for abs_u in range(0, 14):  # [10:13] (0 to 13)
                                    # Determine ranges based on wafer_type
                                    if wafer_type == 0:
                                        # Iterate over all possible combinations of (v_coordinate, u_coordinate) for wafer_type 0
                                        for v_coordinate in range(0, 24):   # [5:9] (5 bits for wafer_type 0)
                                            for u_coordinate in range(0, 24):   # [0:4] (5 bits for wafer_type 0)
                                                detid_bin = (
                                                    (detector_type << 28) |
                                                    (wafer_type << 26) |
                                                    (z_side << 25) |
                                                    (layer_number << 20) |
                                                    (sign_v << 19) |
                                                    (abs_v << 15) |
                                                    (sign_u << 14) |
                                                    (abs_u << 10) |
                                                    (v_coordinate << 5) |
                                                    u_coordinate
                                                )
                                                combinations.append((detector_type, detid_bin))
                                                counter += 1
                                                print(f"Combination #{counter}: Generated - DetectorType: {detector_type}, DetID: {detid_bin}")
                                    else:
                                        # Iterate over all possible combinations of (v_coordinate, u_coordinate) for wafer_type 1 and 2
                                        for v_coordinate in range(0, 16):   # [5:9] (4 bits for wafer_type 1 and 2)
                                            for u_coordinate in range(0, 16):   # [0:4] (4 bits for wafer_type 1 and 2)
                                                detid_bin = (
                                                    (detector_type << 28) |
                                                    (wafer_type << 26) |
                                                    (z_side << 25) |
                                                    (layer_number << 20) |
                                                    (sign_v << 19) |
                                                    (abs_v << 15) |
                                                    (sign_u << 14) |
                                                    (abs_u << 10) |
                                                    (v_coordinate << 5) |
                                                    u_coordinate
                                                )
                                                combinations.append((detector_type, detid_bin))
                                                counter += 1
                                                print(f"Combination #{counter}: Generated - DetectorType: {detector_type}, DetID: {detid_bin}")
    return combinations

def generate_all_hgcal_scintillator_detids():
    """Generates all possible detector IDs for HE Scintillator"""
    combinations = []
    counter = 0  # Initialize a counter for combinations
    
    for detector_type in [10]:  # [28:31] (1010) for HEScintillator
        for tile_type in range(1, 3):  # [26:27] (1 for type "c", 2 for type "m")
            for z_side in range(0, 2):  # [25:25] (0 for +z, 1 for -z)
                for granularity in range(0, 2):  # [24:24] (0 for normal, 1 for fine)
                    for sipm_type in range(0, 2):  # [23:23] (0 for Small, 1 for Large)
                    # Alternate trigger_cell between 0 and 1
                        for trigger_cell in range(2):  # [22:22] (0 or 1)
                            for layer_number in range(8, 22):  # [17:21] (8 to 21)
                                for ring_index in range(1, 46):  # [9:16] (1 to 45)
                                    for iphi_index in range(0, 321):  # [0:8] (0 to 320)
                                        # Combine all segments into a 32-bit binary number
                                        detid_bin = (
                                            (detector_type << 28) |
                                            (tile_type << 26) |
                                            (z_side << 25) |
                                            (granularity << 24) |
                                            (sipm_type << 23) |
                                            (trigger_cell << 22) |
                                            (layer_number << 17) |
                                            (ring_index << 9) |
                                            iphi_index
                                        )
                                        combinations.append((detector_type, detid_bin))
                                        counter += 1  # Increment the counter
                                        print(f"Combination #{counter}: Generated - DetectorType: {detector_type}, DetID: {detid_bin}")  # Print while generating
    return combinations

# Generate all combinations
silicon_detid_list = generate_all_hgcal_silicon_detids()
scintillator_detid_list = generate_all_hgcal_scintillator_detids()

# Combine both lists
combined_detid_list = silicon_detid_list + scintillator_detid_list

# Write the detector IDs to a CSV file
with open('detid_list_all_combinations.csv', mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["DetectorType", "DetID"])  # Write header with DetectorType and DetID
    for det_type, detid in combined_detid_list:
        writer.writerow([det_type, detid])  # Write DetectorType and DetID in the file

print(f"\nGenerated all possible HGCal DetIDs (Silicon and Scintillator) and stored them in 'detid_list_all_combinations.csv'")
```

</details>

**How to Run**  
```
cd src/PhysicsTools/PatExamples/Raw_detids/
python3 DetIDRaw.py
```
The script creates every valid combination of these values for both silicon-based and scintillator-based detectors. Once all combinations are generated, it saves the entire list into a CSV file called `detid_list_all_combinations.csv`. This file acts as a raw inventory of possible detector elements, which can then be used for further validation or detector studies.

By running this script, we ensure that no detector configuration is missed in the initial setup phase.

##### Total Number of Raw DetIds (Before Validation)

| Subdetector      | Count        |
|------------------|--------------|
| EE (Electromagnetic Endcap)        | 54,792,192   |
| HE Silicon (Hadronic Endcap - Silicon) | 44,255,232   |
| HE Scintillator (Hadronic Endcap - Scintillator) | 6,471,360    |


#### Output (from Step A)
- `detid_list_all_combinations.csv`: Contains all DetIds before validation.


The next step is to pass them through the validation producer to filter only those compatible with the current release geometry (v17).


**Note**: These raw DetIds are NOT guaranteed to be valid — they include every logically possible configuration. The validation logic filters out only those consistent with the current detector geometry.

---

###  Step B : DetId Validation and Database Generation (Once Per Release)

In this step, we take the list of all possible DetIds created earlier (from Step A) and check which ones are actually valid in the current HGCal detector geometery release. This is important because not every combination of numbers represents a real or physically allowed part of the detector. Using the latest detector geometry, we carefully go through each DetId and keep only those that match the layout and structure of the detector. The valid DetIds are then saved in two formats: a CSV file for easy viewing and an SQLite database file. This ensures we are working only with meaningful, accurate detector IDs in the next steps.


#### DetId Validation Process

- **CMSSW Version**: `CMSSW_15_1_0_pre4`
- **Geometry**: `GeometryExtendedRun4D110`
- Validation is performed via a custom **EDProducer** that:
  - Checks each DetId against the geometry using HGCal-specific rules.
  - Ensures valid combinations of wafer, layer, cell, and positioning.
  - Filters out invalid DetIds.
  

#### Components Involved

- **Producer Code**: `HGCalProducerDatabaseGen.cc`  
  Located in the `plugins` directory, this C++ source defines the logic for detId validation and Database.

<details>
  <summary>Show HGCalProducerDatabaseGen.cc </summary>

```cpp
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

```
</details>

- **Configuration File**: `HGCalProducerDatabaseGen_cfi.py`  
  Found in the `python` directory, this file configures the producer for CMSSW execution. You must specify the path to your raw DetId input CSV here.
  
<details>
  <summary>Show HGCalProducerDatabaseGen_cfi.py </summary>

```python

import FWCore.ParameterSet.Config as cms

# Import the era to configure the process
from Configuration.Eras.Era_Phase2C17I13M9_cff import Phase2C17I13M9

# Define the process with the appropriate era
process = cms.Process('SIM', Phase2C17I13M9)


# Load necessary configurations
process.load('Configuration.Geometry.GeometryExtendedRun4D110Reco_cff')
process.load('Configuration.Geometry.GeometryExtendedRun4D110_cff')

process.load('Geometry.HGCalGeometry.HGCalGeometryESProducer_cfi')
# Define the source
process.source = cms.Source("EmptySource")

# Set the maximum number of events to process
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)  # Process only 1 event
)

# Define the DetIdNewProducer module
process.g4SimHits = cms.EDProducer('HGCalProducerDatabaseGen',
    infoFileName = cms.string("../Raw_detids/detid_list_all_combinations.csv"),
)

#process.out = cms.OutputModule("PoolOutputModule",
#    fileName = cms.untracked.string('step1.root')
   # outputCommands = cms.untracked.vstring('keep *')
#)

# Define the processing path
process.p = cms.Path(process.g4SimHits)

# Define the end path for the output module
#process.e = cms.EndPath(process.out)

# Schedule definition
process.schedule = cms.Schedule(process.p)
```

</details>




**How to Run**  
```
cd src/PhysicsTools/PatExamples/python
cmsRun HGCalProducerDatabaseGen_cfi.py
```

#### Output

After successful validation, this step produces:

- `valid_detID_all_feature.csv` — List of all validated and accepted DetIds
- `detid_data_all_feature.db` — SQLite database (produced via CMSSW tools or custom DB writer module)

These outputs are automatically saved when running the producer, and are used as trusted inputs for downstream modules, or analysis.


#### Total Validated DetIds

| Subdetector      | Count        |
|------------------|--------------|
| EE (Electromagnetic Endcap)        | 37,13,400   |
| HE Silicon (Hadronic Endcap - Silicon) | 21,33,612   |
| HE Scintillator (Hadronic Endcap - Scintillator) | 2,36,928    |
| Total valid detIds | 60,83,940    |


#### DetId Parameter Ranges After Validation

<details>
  <summary>Click here</summary>


##### EE & HE Silicon Validated Ranges

| Parameter        | Bit Position | Bit Length | Value Range                                       |
|------------------|--------------|------------|--------------------------------------------------|
| Detector Type     | 28–31        | 4 bits     | 8 (EE Silicon), 9 (HE Silicon)                   |
| Wafer Type        | 26–27        | 2 bits     | 0, 1, 2                                          |
| Z Side            | 25           | 1 bit      | 0 (+z), 1 (–z)                                   |
| Layer Number      | 20–24        | 5 bits     | 1–26 (DetType 8), 1–21 (DetType 9)               |
| Sign of v         | 19           | 1 bit      | 0 (+v), 1 (–v)                                   |
| Absolute v        | 15–18        | 4 bits     | 0 to 13                                          |
| Sign of u         | 14           | 1 bit      | 0 (+u), 1 (–u)                                   |
| Absolute u        | 10–13        | 4 bits     | 0 to 13                                          |
| v-coordinate      | 5–9          | 5 bits     | 0 to 15 (LD wafers 1,2), 0 to 23 (HD wafer 0)    |
| u-coordinate      | 0–4          | 5 bits     | 0 to 15 (LD wafers 1,2), 0 to 23 (HD wafer 0)    |

##### HE Scintillator Validated Ranges

| Parameter           | Bit Position | Bit Length | Value Range                     |
|--------------------|--------------|------------|---------------------------------|
| Detector Type       | 28–31        | 4 bits     | 10 (HGCalHSc = 1010)            |
| Tile Type           | 26–27        | 2 bits     | 1 (type "c"), 2 (type "m")      |
| Z Side              | 25           | 1 bit      | 0 for +z, 1 for –z              |
| Granularity         | 24–24        | 1 bit      | 0 (normal)                      |
| SiPM Type           | 23–23        | 1 bit      | 0 (small), 1 (large)            |
| Trigger/Detector    | 22–22        | 1 bit      | 0 (Detector)                    |
| Layer Number        | 17–21        | 5 bits     | 8 to 21                         |
| Ring Index          | 9–16         | 8 bits     | 1 to 42                         |
| iPhi Index          | 0–8          | 9 bits     | 1 to 288                        |


</details>


**Note**:  
This `.csv` and `.db` file pair will be used in later steps (e.g., during simulation, digitization, or analysis). No manual intervention is required for DB generation — it is handled by the same producer that performs the DetId validation.


---


## User Workflow

The User Workflow outlines how users interact with the DetId validation framework and make use of the generated resources in their own studies or applications. The main purpose of this workflow is to make the validated DetId information easily accessible and usable. Users begin by working with the precomputed SQLite database (detid_data_all_feature.db), which contains only the DetIds that have been confirmed to be valid according to the latest HGCal geometry (v17). By running simple SQL queries, users can extract specific sets of DetIds—such as those corresponding to a particular layer, detector type, or region and export the results into a CSV file for further use.

In the next stage, a custom SimHit producer takes over. This producer reads the quried csv file, and transforms them into a standardized format called pCaloHits. These hits carry energy, position, and timing information (for this case the energy and time is set to fixed value). The processed data is stored in an output file named step1.root, which acts as an intermediate checkpoint for quality checks, visualization, and future tasks. This two-step workflow—starting with DetId extraction and followed by hit processing in the Pcalohit.


### Step 1: Use the Provided SQLite DB and Run Queries 

This step allows users to interact with the **precomputed SQLite database (`detid_data_all_feature.db`)**, which contains all the relevant DetIds and their associated geometry details. By running the provided Python script, users can explore the database, check available tables and columns, and run custom SQL queries. This is important because it provides an easy way to access HGCal geometry information without regenerating DetIds every time, making the workflow faster, reproducible, and more efficient.

<details>
  <summary>Show sqliteuser.py </summary>

```python

import sqlite3
import re
import csv
import time

# === DB FILE (unencrypted) ===
db_file = "detid_data_all_feature.db"  # replace with your actual SQLite DB file

# === Connect to SQLite DB ===
conn = sqlite3.connect(db_file)
cursor = conn.cursor()

# === List all tables ===
cursor.execute("SELECT name FROM sqlite_master WHERE type='table';")
tables = cursor.fetchall()

print("\n📦 Available tables:")
for idx, (tbl,) in enumerate(tables):
    print(f"{idx + 1}: {tbl}")

# === Choose a table ===
choice = int(input("\nEnter the number of the table to use: "))
table_name = tables[choice - 1][0]

# === Get column names ===
cursor.execute(f"PRAGMA table_info({table_name});")
columns_info = cursor.fetchall()

# Build column name mapping
column_map = {}
print("\n🧾 Available columns:")
for col in columns_info:
    col_name = col[1]  # second field is column name
    column_map[col_name] = col_name
    print(f"- {col_name}")

# === Ask for WHERE condition ===
print("\nEnter your SQL WHERE condition using AND / OR / BETWEEN, etc.")
print("Example: (WaferType = 2 AND Zside = -1) OR Nlayer BETWEEN 5 AND 15")
user_input = input(">> ")

# Replace plain column names with quoted ones
for clean, original in column_map.items():
    user_input = re.sub(rf'\b{clean}\b', f"`{original}`", user_input)

# === Define columns to SELECT ===
selected_columns = [
    'DetId',
    'DetType',
    'Nlayer'
]
selected_column_str = ', '.join(f"`{column_map[col]}`" for col in selected_columns)

# === Build and run the query ===
query = f"SELECT {selected_column_str} FROM `{table_name}` WHERE {user_input}"

try:
    start_time = time.time()
    cursor.execute(query)
    results = cursor.fetchall()
    end_time = time.time()

    print(f"\n🕒 Query execution time: {end_time - start_time:.4f} seconds")
    print(f"🔍 Found {len(results)} matching entries:")

    for row in results[:10]:
        print(row)

    # Count DetType values
    det8 = sum(1 for r in results if r[1] == 8)
    det9 = sum(1 for r in results if r[1] == 9)
    det10 = sum(1 for r in results if r[1] == 10)

    print(f"\n📊 Counts by DetType:")
    print(f"  DetType = 8 : {det8}")
    print(f"  DetType = 9 : {det9}")
    print(f"  DetType = 10: {det10}")

    # === Save to CSV ===
    output_file = "quried_detid_output.csv"
    with open(output_file, "w", newline="") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(selected_columns)
        writer.writerows(results)

    print(f"\n✅ Results saved to {output_file}")

except Exception as e:
    print(f"\n❌ Query failed: {e}")

# === Close connection ===
conn.close()
```
</details>


**How to Run**
```
cd src/PhysicsTools/PatExamples/python
python3 sqliteuser.py
```

#### Terminal Output

<details>
  <summary>Terminal output</summary>


```
📦 Available tables:
1: hgcal_detids_v5

Enter the number of the table to use: 1

🧾 Available columns:
- DetId
- Zside
- DetType
- Nlayer
- LayerType
- FrontBack
- WaferType
- WaferIndex
- PartType
- Cassette
- CassetteType
- Orient
- AbsU
- AbsV
- CellU
- CellV
- x
- y
- z
- TileType
- SipmType
- TriggerCell
- RingIndex
- IphiIndex
- Granularity
- eta
- phi

Enter your SQL WHERE condition using AND / OR / BETWEEN, etc.
Example: (WaferType = 2 AND Zside = -1) OR Nlayer BETWEEN 5 AND 15
>> (here you can write your query)
```
</details>

**File output** :  quried_detid_output.csv

- Users extract specific DetIds and export to CSV.
- Output CSV format
  - `DetId`, `NLayer`, `DetType`

---

### Step 2: Development of SimHit Producer

In this step, we introduce a **custom CMSSW EDProducer** designed specifically to handle SimHit data using validated DetIds. The purpose of this module is to simulate calorimeter hits (`pCaloHits`) based on raw inputs (such as hit positions, energy, and time), and link them correctly to the detector geometry using validated DetIds. This is an essential step in preparing realistic data for detector studies and performance validation.

The producer processes the raw hit information, maps each hit to a corresponding **validated DetId**, and writes the output into a file called **`step1.root`**. This file contains all relevant information such as energy, time, and detector ID for each hit, and is formatted for easy use in the next step of the simulation chain.


#### Components Involved

- **Producer Code**: `HGCalProducerSimHit.cc`  
  Located in the `plugins` directory, this C++ source defines the logic for converting raw hits into `pCaloHits`.

<details>
  <summary>Show HGCalProducerSimHit.cc </summary>

```cpp


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
```
</details>

- **Configuration File**: `HGCalProducerSimHit_cfi.py`  
  Found in the `python` directory, this file configures the producer for CMSSW execution. You must specify the path to your quried DetId csv file for input here.

<details>
  <summary>Show HGCalProducerSimHit_cfi.py </summary>

```python
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
```
</details>

**How to Run**

To run the producer and generate `step1.root`, follow these steps:

```
cd src/PhysicsTools/PatExamples/python
cmsRun HGCalProducerSimHit_cfi.py
```

**Output** :  Step1.root
- Generated by the custom SimHit producer.
- Initial processed hits (`pCaloHits`) with validated DetIds.
- **Used as input** for the next stage of CMSSW processing.

---

### Step 3: Multi-Step Processing Pipeline

The raw SimHit data undergoes a multi-step processing pipeline. Each step builds upon the previous one, refining the data through official CMS workflows.


#### Step a : `step2.root`
- **Purpose**: Simulates digitization, trigger, and HLT chain from the `step1.root` file.

- **Command**:
```bash
cmsDriver.py step2  -s DIGI:pdigi_valid,L1TrackTrigger,L1,L1P2GT,DIGI2RAW,HLT:@relvalRun4 --conditions auto:phase2_realistic_T33 --datatier GEN-SIM-DIGI-RAW -n 1 --eventcontent FEVTDEBUGHLT --geometry ExtendedRun4D110 --era Phase2C17I13M9 --filein  file:step1.root  --fileout file:step2.root  > step2.log  2>&1
```

#### Step b : `step3.root`

**Purpose**:  
Performs full reconstruction (`RECO`), Physics Analysis Toolkit (`PAT`) processing, and complete validation including **Data Quality Monitoring (DQM)**. This is the final step that transforms simulated detector output into high-level physics objects ready for analysis.

**Command**:
```bash
cmsDriver.py step3  -s RAW2DIGI,RECO,RECOSIM,PAT,VALIDATION:@phase2Validation+@miniAODValidation,DQM:@phase2+@miniAODDQM --conditions auto:phase2_realistic_T33 --datatier GEN-SIM-RECO,MINIAODSIM,DQMIO -n 1 --eventcontent FEVTDEBUGHLT,MINIAODSIM,DQM --geometry ExtendedRun4D110 --era Phase2C17I13M9 --filein  file:step2.root  --fileout file:step3.root  > step3.log  2>&1

```
---

### Step 4: Visualization with Fireworks

**Purpose**:  
This step allows users to visually inspect the simulated detector hits and validated DetIds using the **Fireworks** event display tool in CMSSW. It helps confirm that hits are correctly mapped to the detector geometry and provides a powerful way to debug or showcase events.


#### Steps to Visualize Raw DetIDs

1. **Generate the Geometry File**  
   Use the following command to create a detector geometry file compatible with Fireworks, based on the 2026 D110 configuration:

```bash
cd $CMSSW/src
cmsRun Fireworks/Geometry/python/dumpSimGeometry_cfg.py tag=Run4 version=D110
```

- This command generates the geometry file: cmsSimGeom-2026D110.root.

2. **Run Fireworks for Visualization**

Use the generated geometry file along with your simulation output (`step3.root`) to launch the Fireworks GUI and visualize the raw `DetIds`.

#### Command:

```bash
cmsShow --sim-geom-file cmsSimGeom-Run4D110.root PhysicsTools/PatExamples/Root_Files/step3.root
```

---

## Folder Structure

<details>
  <summary>Click here</summary>
 
```
/src/PhysicsTools/PatExamples/
│
├── Raw_detids /
│   ├── DetIDRaw.py
│   └── detid_list_all_combinations.csv
|
├── plugins/
│   ├── BuildFile.xml
│   ├── HGCalProducerDatabaseGen.cc
│   └── HGCalProducerSimHit.cc
│
├── python/
│   ├── HGCalProducerDatabaseGen_cfi.py
│   ├── HGCalProducerSimHit_cfi.py
│   ├── detid_data_all_feature.db
│   ├── valid_detID_all_feature.csv
│   ├── sqliteread.py
│   └── quried_detid_output.csv
│
├── Root_Files/
│   ├── step1.root
│   ├── step2.root
│   └── step3.root
│
└── README.md
```

</details>

---

## 📬 Contact

For questions or contributions, feel free to open an issue or pull request.

Maintainer: **Bharat Kumar Sirasva**, IISER Mohali

---

