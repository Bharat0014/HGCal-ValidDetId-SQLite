
#  HGCal DetId Validation, SQLite Database Creation, PCaloHit Workflow, and Visualization

This repository documents the validation and usage of **ValidDetIds** in the High Granularity Calorimeter (HGCal) geometry of CMS. The workflow supports **raw DetId generation**, **validation**, **database storage**, and **SimHit (PCaloHit) creation**, followed by **visualization** using Fireworks.

---

## 📌 Overview

The workflow is split into **Admin Setup (once per Geometery release)** and **User Workflow**. 

---

---
## Step 0 : For Admin And User Both

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
git clone https:
scram b -j8

```


## 🧰 Admin Workflow (Once per Release)

This setup is required **once per each HGCal geometry release** to validate and store valid DetIds in Sqlite databse.

---

### 🧱 Step A: DetId Definition and Raw CSV Creation (Pre-validation)

This step generates **all possible raw DetIds** for HGCal geometry before applying any validation.

---

####  DetId Bit-Level Definition

In this step, we create a complete list of all possible DetIds for the HGCal subdetectors—EE, HE Silicon, and HE Scintillator—using the bit patterns shown in the tables below. Each DetId is a unique code that describes a part of the detector, such as its layer number, wafer or tile type, and position (u and v for silicon; ring and iPhi for scintillator). We do this to make sure we include every possible detector location before checking if they are valid. These tables follow the official HGCal DetId format and will be used in the next step to check the IDs against the actual detector layout.



#### 📘 EE & HESilicon

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
| v-coordinate      | 5–9          | 5 bits     | Varies by wafer type and index                      |
| u-coordinate      | 0–4          | 5 bits     | Varies by wafer type and index                      |

#### 📘 HE Scintillator

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

---

**Usage**  
```
python3 DetIDEE_HElayers.py
```
The script creates every valid combination of these values for both silicon-based and scintillator-based detectors. Once all combinations are generated, it saves the entire list into a CSV file called `detid_list_all_combinations.csv`. This file acts as a raw inventory of possible detector elements, which can then be used for further validation or detector studies.

By running this script, we ensure that no detector configuration is missed in the initial setup phase.

#### 🔢 Total Number of Raw DetIds (Before Validation)

| Subdetector      | Count        |
|------------------|--------------|
| EE (Electromagnetic Endcap)        | 67,837,952   |
| HE Silicon (Hadronic Endcap - Silicon) | 54,792,192   |
| HE Scintillator (Hadronic Endcap - Scintillator) | 7,660,814    |

These DetIds are generated programmatically and **span all layers**, **module types**, and **wafer configurations** supported in the CMSSW geometry. The next step is to pass them through the validation producer to filter only those compatible with the current release geometry.


#### 🧰 Output (from Step A)
- `detid_list_all_combinations.csv`: Contains all generated DetIds before validation.
- Used as **input** for validation and database creation in Step B.


📌 **Note**: These raw DetIds are NOT guaranteed to be valid — they include every logically possible configuration. The validation logic filters out only those consistent with the current detector geometry.

---

###  Step B & C: 🔍📊 DetId Validation and Database Generation (Once Per Release)

In this step, we take the list of all possible DetIds created earlier and check which ones are actually valid in the current HGCal detector design. This is important because not every combination of numbers represents a real or physically allowed part of the detector. Using the latest detector geometry, we carefully go through each DetId and keep only those that match the layout and structure of the real detector. The valid DetIds are then saved in two formats: a CSV file for easy viewing and an SQLite database file for use in software tools or analysis. This ensures we are working only with meaningful, accurate detector IDs in the next steps.

---

#### DetId Validation Process

- **CMSSW Version**: `CMSSW_15_1_0_pre4`
- **Geometry**: `GeometryExtendedRun4D110`
- Validation is performed via a custom **EDProducer** that:
  - Checks each DetId against the geometry using HGCal-specific rules.
  - Ensures valid combinations of wafer, layer, cell, and positioning.
  - Filters out invalid DetIds.

---

**Usage**  
```
cd Admin/python
python3 raw_detid_generator.py
```
---
#### 📤 Output

After successful validation, this step produces:

- ✅ `valid_detids.csv` — List of all validated and accepted DetIds
- ✅ `valid_detids.db` — SQLite database (produced via CMSSW tools or custom DB writer module)

These outputs are automatically saved when running the producer, and are used as trusted inputs for downstream modules, simulations, or analysis.

---

#### 📊 Total Validated DetIds

- **EE + HESilicon + HEScintillator** combined:
  - `6,083,940` DetIds passed all geometry checks

---

#### DetId Parameter Ranges After Validation

##### 🟩 EE & HE Silicon Validated Ranges

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
| v-coordinate      | 5–9          | 5 bits     | 0 to 15 (LD wafers 1,2), 0 to 23 (HD wafer 0)
                             |
| u-coordinate      | 0–4          | 5 bits     | 0 to 15 (LD wafers 1,2), 0 to 23 (HD wafer 0)                             |

##### 🟩 HE Scintillator Validated Ranges

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

---

📌 **Note**:  
This `.csv` and `.db` file pair will be used in later steps (e.g., during simulation, digitization, or analysis). No manual intervention is required for DB generation — it is handled by the same producer that performs the DetId validation.



---

## 👤 User Workflow


The User Workflow outlines how users interact with the DetId validation framework and make use of the generated resources in their own studies or applications. The main purpose of this workflow is to make the validated DetId information easily accessible and usable, especially for those developing detector-level simulations, analysis tools, or reconstruction algorithms. Users begin by working with the precomputed SQLite database (valid_detids.db), which contains only the DetIds that have been confirmed to be valid according to the latest HGCal geometry. By running simple SQL queries, users can extract specific sets of DetIds—such as those corresponding to a particular layer, detector type, or region—and export the results into a CSV file for further use.

In the next stage, a custom SimHit producer takes over. This producer reads the raw SimHit data, uses the validated DetIds to map them correctly, and transforms them into a standardized format called pCaloHits. These hits carry energy, position, and timing information that can be used in downstream validation and reconstruction steps. The processed data is stored in an output file named step1.root, which acts as an intermediate checkpoint for quality checks, visualization, and future simulation tasks. This two-step workflow—starting with DetId extraction and followed by hit processing—ensures accuracy, reproducibility, and flexibility in working with HGCal simulation data.


### Step 1: Use the Provided SQLite DB and Run Queries 

- Users can query the precomputed `valid_detids.db` database for required DetIds.

**Usage**  
```
cd User/python
python3 sqliteuser.py
```

---
#### 📤 Output
**Terminal output**
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
>> 
```
**File output** :  valid_detids.csv
- Users extract specific DetIds and export to CSV.
- Output CSV format
  - `DetId`, `NLayer`, `DetType`



### 🚀 Step 2: Development of SimHit Producer

In this step, we introduce a **custom CMSSW EDProducer** designed specifically to handle raw SimHit data using validated DetIds. The purpose of this module is to simulate calorimeter hits (`pCaloHits`) based on raw inputs (such as hit positions, energy, and time), and link them correctly to the detector geometry using validated DetIds. This is an essential step in preparing realistic data for detector studies and performance validation.

The producer processes the raw hit information, maps each hit to a corresponding **validated DetId**, and writes the output into a file called **`step1.root`**. This file contains all relevant information such as energy, time, and detector ID for each hit, and is formatted for easy use in the next step of the simulation chain.

---

### 🧩 Components Involved

- **Producer Code**: `HGCalRawProducer.cc`  
  Located in the `plugins` directory, this C++ source defines the logic for converting raw hits into `pCaloHits`.

- **Configuration File**: `HGCalRawProducer_cfi.py`  
  Found in the `python` directory, this file configures the producer for CMSSW execution. You must specify the path to your raw DetId input CSV here.

---

### ⚙️ How to Run

To run the producer and generate `step1.root`, follow these steps:

```
cd User/HGCalDetIDvalidation/python
cmsRun HGCalRawProducer_cfi.py
```

### 🔄 Step 3: Multi-Step Processing Pipeline

The raw SimHit data undergoes a multi-step processing pipeline to progressively add more realism and validation at each stage. Each step builds upon the previous one, refining the data through official CMS workflows.

---

#### 🧪 Step 1: `step1.root`
- **Purpose**: Generated by the custom SimHit producer.
- **Contains**: Initial processed hits (`pCaloHits`) with validated DetIds.
- **Used as input** for the next stage of CMSSW processing.

---

#### ⚙️ Step 2: `step2.root`
- **Purpose**: Simulates digitization, trigger, and HLT chain from the `step1.root` file.
- **Command**:
  ```bash
  cmsDriver.py step2  -s DIGI:pdigi_valid,L1TrackTrigger,L1,L1P2GT,DIGI2RAW,HLT:@relvalRun4 --conditions auto:phase2_realistic_T33 --datatier GEN-SIM-DIGI-RAW -n 1 --eventcontent FEVTDEBUGHLT --geometry ExtendedRun4D110 --era Phase2C17I13M9 --filein  file:step1_custom1.root  --fileout file:step2.root  > step2.log  2>&1
#### ✅ Step 3: `step3.root`

**Purpose**:  
Performs full reconstruction (`RECO`), Physics Analysis Toolkit (`PAT`) processing, and complete validation including **Data Quality Monitoring (DQM)**. This is the final step that transforms simulated detector output into high-level physics objects ready for analysis.

---

**Command to Run**:

```bash
cmsDriver.py step3  -s RAW2DIGI,RECO,RECOSIM,PAT,VALIDATION:@phase2Validation+@miniAODValidation,DQM:@phase2+@miniAODDQM --conditions auto:phase2_realistic_T33 --datatier GEN-SIM-RECO,MINIAODSIM,DQMIO -n 1 --eventcontent FEVTDEBUGHLT,MINIAODSIM,DQM --geometry ExtendedRun4D110 --era Phase2C17I13M9 --filein  file:step2.root  --fileout file:step3.root  > step3.log  2>&1
Root_Files$ vim step3.log

```

### 🎆 Step 4: Visualization with Fireworks

**Purpose**:  
This step allows users to visually inspect the simulated detector hits and validated DetIds using the **Fireworks** event display tool in CMSSW. It helps confirm that hits are correctly mapped to the detector geometry and provides a powerful way to debug or showcase events.

---

#### 🧭 Steps to Visualize Raw DetIDs

1. **Generate the Geometry File**  
   Use the following command to create a detector geometry file compatible with Fireworks, based on the 2026 D110 configuration:

   ```bash
   cd $CMSSW/src
   cmsRun Fireworks/Geometry/python/dumpSimGeometry_cfg.py tag=Run4 version=D110

- This command generates the geometry file: cmsSimGeom-2026D110.root.

2. **Run Fireworks for Visualization**

Use the generated geometry file along with your simulation output (`step3.root`) to launch the Fireworks GUI and visualize the raw `DetId`s.

#### 📌 Command:

```bash
cmsShow --sim-geom-file cmsSimGeom-Run4D110.root PhysicsTools/PatExamples/Root_Files/step3.root
```

---
## 📁 Folder Structure
```
HGCalValidDetId/
│
├── admin/
│   ├── raw_detid_generator.py
│   ├── detid_validator_producer.cc
│   ├── valid_detids.csv
│   └── valid_detids.sqlite
│
├── user/
│   ├── detid_query_tool.py
│   ├── filtered_detids.csv
│   └── pcalohit_producer.cc
│
├── config/
│   ├── step1_cfg.py
│   ├── step2_cfg.py
│   └── step3_cfg.py
│
└── README.md
```


---

## 📬 Contact

For questions or contributions, feel free to open an issue or pull request.

Maintainer: **Bharat Kumar Sirasva**, IISER Mohali

---

