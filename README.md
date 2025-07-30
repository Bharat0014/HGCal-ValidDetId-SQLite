
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

## 🧱 Step A: DetId Definition and Raw CSV Creation (Pre-validation)

This step generates **all possible raw DetIds** for HGCal geometry before applying any validation.

---

###  DetId Bit-Level Definition

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

### 🔢 Total Number of Raw DetIds (Before Validation)

| Subdetector      | Count        |
|------------------|--------------|
| EE (Electromagnetic Endcap)        | 67,837,952   |
| HE Silicon (Hadronic Endcap - Silicon) | 54,792,192   |
| HE Scintillator (Hadronic Endcap - Scintillator) | 7,660,814    |

These DetIds are generated programmatically and **span all layers**, **module types**, and **wafer configurations** supported in the CMSSW geometry. The next step is to pass them through the validation producer to filter only those compatible with the current release geometry.


### 🧰 Output (from Step A)
- `detid_list_all_combinations.csv`: Contains all generated DetIds before validation.
- Used as **input** for validation and database creation in Step B.

---

📌 **Note**: These raw DetIds are NOT guaranteed to be valid — they include every logically possible configuration. The validation logic filters out only those consistent with the current detector geometry.

## ✅ Step B: DetId Validation and Database Generation (Once Per Release)

This step validates DetIds generated in **Step A** using the latest **HGCal geometry**, and produces both a validated CSV file and a corresponding SQLite `.db` file.

---

### 🛠️ DetId Validation Process

- **CMSSW Version**: `CMSSW_15_1_0_pre4`
- **Geometry**: `GeometryExtendedRun4D110`
- Validation is performed via a custom **EDProducer** that:
  - Checks each DetId against the geometry using HGCal-specific rules.
  - Ensures valid combinations of wafer, layer, cell, and positioning.
  - Filters out invalid DetIds and only retains those that comply.

---

### 📤 Output Artifacts

After successful validation, this step produces:

- ✅ `valid_detids.csv` — List of all validated and accepted DetIds
- ✅ `valid_detids.db` — SQLite database (produced via CMSSW tools or custom DB writer module)

These outputs are automatically saved when running the producer, and are used as trusted inputs for downstream modules, simulations, or analysis.

---

### 📊 Total Validated DetIds

- **EE + HESilicon + HEScintillator** combined:
  - `6,083,940` DetIds passed all geometry checks

---

### 📏 DetId Parameter Ranges After Validation

#### 🟩 EE & HE Silicon Validated Ranges

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
| v-coordinate      | 5–9          | 5 bits     | 0 to 23 (HD wafer 0)                             |
| u-coordinate      | 0–4          | 5 bits     | 0 to 23 (HD wafer 0)                             |

#### 🟨 HE Scintillator Validated Ranges

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





## 👤 User Workflow

### Step 1: Use the Provided SQLite DB and Run Queries 

- Users can query the precomputed `valid_detids.db` database for required DetIds.
- Users extract specific DetIds and export to CSV.
- Output CSV format:
  - `DetId`, `NLayer`, `DetType`



## 2. Development of SimHit Producer
A specialized producer was implemented to:
- Process raw SimHit data.
- Convert RawHits(DetIds) into `pCaloHits`.
- Store intermediate results in `step1.root` for validation and subsequent processing.

#### Step1.root Generation
- The `HGCalRawProducer.cc` code, available in the `plugins` folder, is used for generating the `step1.root` file.
- The corresponding configuration file, `HGCalRawProducer_cfi.py`, is located in the `python` folder.
- You must specify the input raw DetIDs file in the configuration file to run the process.

#### Workflow
1. **Input CSV File**: The process begins with a CSV file containing the required Det_type and DetID and information. This file serves as the input for the producer.
2. **Populating g4SimHits**: The producer generates all the necessary `g4SimHits` branches. However, only the relevant branches—`HGCHitsEE`, `HGCHitsHEback`, and `HGCHitsHEfront`—are populated with data, while the rest remain empty.
3. **Additional Branches**: 
   - Additional branches such as `HEPMCProducer` and `genProduct` are created using a **Pythia8 generator**, defined in the configuration file.
   - The Pythia8 generator ensures that all required branches are present in the `step1.root` file without interfering with the data filled by the producer.


**Usage** 
```
cd HGCalDetIDvalidation/python
cmsRun HGCalRawProducer_cfi.py
```

Using this method, the `step1.root` file is generated successfully, with all necessary branches in place. When this file is used as input for the step2 process, it works without any errors or conflicts.


## 3. Multi-Step Processing Pipeline
The raw data processing involves the following steps:
1. **`step1.root`**: Generated by the SimHit producer, serving as the initial processed output.
2. **`step2.root`**: Produced using `step1.root` as input, incorporating additional validations and refined processing.

**Usage** 
```
cmsDriver.py step2  -s DIGI:pdigi_valid,L1TrackTrigger,L1,DIGI2RAW,HLT:@relval2026 --conditions auto:phase2_realistic_T33 --datatier GEN-SIM-DIGI-RAW -n 1 --eventcontent FEVTDEBUGHLT --geometry Extended2026D110 --era Phase2C17I13M9 --filein  file:updatedstep1.root  --fileout file:step2.root --nThreads 4 > step2.log  2>&1

```
4. **`step3.root`**: Final output generated using `step2.root` for extended validation and analysis.

**Usage** 
```
cmsDriver.py step3  -s RAW2DIGI,RECO,RECOSIM,PAT,VALIDATION:@phase2Validation+@miniAODValidation,DQM:@phase2+@miniAODDQM --conditions auto:phase2_realistic_T33 --datatier GEN-SIM-RECO,MINIAODSIM,DQMIO -n 1 --eventcontent FEVTDEBUGHLT,MINIAODSIM,DQM --geometry Extended2026D110 --era Phase2C17I13M9 --filein  file:step2.root  --fileout file:step3.root > step3.log  2>&1

```

## 4. Visualization with Fireworks

The Fireworks visualization tool was configured to:  

#### Steps to Visualize Raw DetIDs
1. **Generate the Geometry File**:
   - Use the following command to create a geometry file for the 2026 tag D110 configuration:  

     **Usage** 
     ```
     cd $CMSSW/src
     cmsRun Fireworks/Geometry/python/dumpSimGeometry_cfg.py tag=2026 version=D110
     ```
   - This command generates the geometry file: `cmsSimGeom-2026D110.root`.

2. **Run Fireworks for Visualization**:
   - Use the generated geometry file and the `step3.root` file to visualize the DetIDs:  

     **Usage** 
     ```
     cmsShow --sim-geom-file cmsSimGeom-2026D110.root step3.root
     ```

This process enables an interactive and detailed inspection of raw DetIDs, ensuring accuracy in the data processing workflow.


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

## 💡 Future Work

- Extend support for additional detector versions or eras.
- Automate DetId generation and validation via CI scripts.
- Add web interface for visualizing DetId distributions layer-wise.

---

## 📬 Contact

For questions or contributions, feel free to open an issue or pull request.

Maintainer: **Bharat Kumar Sirasva**, IISER Mohali

---

