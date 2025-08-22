
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

## Step 0 : Enviroment Setup For Admin And User Both

This step is about setting up the **CMSSW environment** so that both admin and user can access all the necessary tools for this workflow. It also provides access to the relevant codes used in this work and connects to the **pre-calibrated database for HGCal geometry (version 17)**.

<details>
  <summary>Click here</summary>

### 1. Set Up the CMSSW Environment


First, ensure you are using the correct CMSSW version. Set up the environment as follows:

```
# Load the CMSSW version
cd /path/to/your/working/directory
cmsrel CMSSW_15_1_0_pre4
cd CMSSW_14_1_0_pre4/src
cmsenv
```

### 2. Unpack necessary Package for HGcal Geometery

```
git cms-addpkg Geometry/CMSCommonData
git cms-addpkg Configuration/Geometry
git cms-addpkg Geometry/HGCalCommonData
git cms-addpkg Geometry/HGCalGeometry
git cms-addpkg Geometry/Records
git cms-addpkg Fireworks

```

### 3. Git clone this repo.

```
git clone https:https://github.com/Bharat0014/HGCal-ValidDetId-SQLite.git
scram b -j8

```

### 4. Path to Pre-generated SimHit Dataset and SQLite Output

```
Raw detIds
https://cernbox.cern.ch/s/QYeBlRIib2XMP4G

Sqlite database for V17
https://cernbox.cern.ch/s/oaIpes2jVCXOYib

Validate detIds in csv
https://cernbox.cern.ch/s/EEJgchI8OqolSEH

step1.root
https://cernbox.cern.ch/s/YO8oZKZ3p6tD2sJ
```

</details>

---

## Admin Workflow (Once per Release)

This setup is required **once per each HGCal geometry release** to validate and store valid DetIds in Sqlite databse.


###  Step A : DetId Definition and Raw CSV Creation (Pre validation)

In this step, we create a complete list of all possible DetIds for the HGCal subdetectors—EE, HE Silicon, and HE Scintillator—using the bit patterns shown in the tables below. Each DetId is a unique code that describes a part of the detector, such as its layer number, wafer or tile type, and position (u and v for silicon; ring and iPhi for scintillator). We do this to make sure we include every possible detector location before checking if they are valid. These tables follow the official HGCal DetId format and will be used in the next step to check the IDs against the actual detector layout. This step generates **all possible raw DetIds** for HGCal geometry before applying any validation.

#### DetId Definations (Pre-validation) 

<details>
  <summary>Click here</summary>

##### EE, HESilicon 

| Parameter         | Bit Position | Bit Length | Value Range                                         |
|------------------|--------------|------------|-----------------------------------------------------|
| Detector Type     | 28–31        | 4 bits     | 8 (EE Silicon), 9 (HE Silicon)                      |
| Wafer Type        | 26–27        | 2 bits     | 0, 1, 2, 3                                          |
| Z Side            | 25           | 1 bit      | 0 (for +z), 1 (for –z)                              |
| Layer Number      | 20–24        | 5 bits     | 1–26 (for DetType 8), 1–21 (for DetType 9)          |
| Sign of v         | 19           | 1 bit      | 0 (+v), 1 (–v)                                      |
| Absolute v        | 15–18        | 4 bits     | 0 to 13                                             |
| Sign of u         | 14           | 1 bit      | 0 (+u), 1 (–u)                                      |
| Absolute u        | 10–13        | 4 bits     | 0 to 13                                             |
| v-coordinate      | 5–9          | 5 bits     | 0 to 15 (LD wafers 1,2), 0 to 23 (HD wafer 0)       |
| u-coordinate      | 0–4          | 5 bits     | 0 to 15 (LD wafers 1,2), 0 to 23 (HD wafer 0)       |

##### HE Scintillator

| Parameter           | Bit Position | Bit Length | Value Range                     |
|--------------------|--------------|------------|---------------------------------|
| Detector Type       | 28–31        | 4 bits     | 10 (HGCalHSc = 1010)            |
| Tile Type           | 26–27        | 2 bits     | 1 (type "c"), 2 (type "m")      |
| Z Side              | 25           | 1 bit      | 0 for +z, 1 for –z              |
| Granularity         | 24–24        | 1 bit      | 0 (normal), 1 (fine)            |
| SiPM Type           | 23–23        | 1 bit      | 0 (small), 1 (large)            |
| Trigger/Detector Cell | 22–22      | 1 bit      | 0 (Detector), 1 (Trigger)       |
| Layer Number        | 17–21        | 5 bits     | 8 to 21                         |
| Ring Index          | 9–16         | 8 bits     | 11 to 50                        |
| iPhi Index          | 0–8          | 9 bits     | 0 to 380                        |


</details>


The `DetIDRaw.py` script is responsible for generating the **raw Detector IDs (DetIDs)**. The code is listed below:

<details>
  <summary>Show DetIDRaw.py code</summary>

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

- **Configuration File**: `HGCalProducerDatabaseGen_cfi.py`  
  Found in the `python` directory, this file configures the producer for CMSSW execution. You must specify the path to your raw DetId input CSV here.



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

The User Workflow outlines how users interact with the DetId validation framework and make use of the generated resources in their own studies or applications. The main purpose of this workflow is to make the validated DetId information easily accessible and usable, especially for those developing detector-level simulations or analysis tools. Users begin by working with the precomputed SQLite database (detid_data_all_feature.db), which contains only the DetIds that have been confirmed to be valid according to the latest HGCal geometry (v17). By running simple SQL queries, users can extract specific sets of DetIds—such as those corresponding to a particular layer, detector type, or region and export the results into a CSV file for further use.

In the next stage, a custom SimHit producer takes over. This producer reads the quried csv file, and transforms them into a standardized format called pCaloHits. These hits carry energy, position, and timing information (for this case the energy and time is set to fixed value). The processed data is stored in an output file named step1.root, which acts as an intermediate checkpoint for quality checks, visualization, and future tasks. This two-step workflow—starting with DetId extraction and followed by hit processing in the Pcalohit.


### Step 1: Use the Provided SQLite DB and Run Queries 

This step allows users to interact with the **precomputed SQLite database (`detid_data_all_feature.db`)**, which contains all the relevant DetIds and their associated geometry details. By running the provided Python script, users can explore the database, check available tables and columns, and run custom SQL queries. This is important because it provides an easy way to access HGCal geometry information without regenerating DetIds every time, making the workflow faster, reproducible, and more efficient.


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

- **Configuration File**: `HGCalProducerSimHit_cfi.py`  
  Found in the `python` directory, this file configures the producer for CMSSW execution. You must specify the path to your quried DetId csv file for input here.


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

