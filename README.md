
#  HGCal DetId Validation, SQLite Database Creation, PCaloHit Workflow, and Visualization

This repository documents the validation and usage of **ValidDetIds** in the High Granularity Calorimeter (HGCal) geometry of CMS. The workflow supports **raw DetId generation**, **validation**, **database storage**, and **SimHit (PCaloHit) creation**, followed by **visualization** using Fireworks.

---

## 📌 Overview

The workflow is split into **Admin Setup (once per Geometery release)** and **User Workflow**.

---

## 🧰 Admin Workflow (Once per Release)

This setup is required **once per each HGCal geometry release** to generate and store valid DetIds in Sqlite databse.

### Step A: Generate Raw DetIds
- Produce all possible HGCal `DetIds` (in CSV format).
- Includes EE, HESilicon, and HEScintillator regions.
- Output: `raw_detids.csv`


 ## Step B & C: Validate and Store HGCal DetIds
 - Use a dedicated EDProducer in:
  - `CMSSW_15_1_X_2025-07-13-2300`
  - `GeometryExtended2026D110`
- Validate DetIds based on HGCal geometry.

- **Stores DetIds** and their features in:
  - A `.csv` file for easy inspection
  - A `.sqlite` database for efficient querying

### 📂 Output Files

| File                | Description                          |
|---------------------|--------------------------------------|
| `valid_detids.csv`  | List of valid DetIds (27 columns)    |
| `valid_detids.db` | SQLite DB containing DetId table     |

### 🧾 Database Info

- **Table**: `hgcal_detids_v5`
- **Columns**: 27 columns per DetId, including:
  - Detector layer
  - Module type
  - Cell info
  - Wafer type
  - Global x/y/z position
  - Geometry region identifiers, etc.


> ⚠️ Steps A, B, and C must be followed **once per HGCal geometry version** to generate the corresponding database.

---

## 🧱 Step A: DetId Definition and Raw CSV Creation (Pre-validation)

This step generates **all possible raw DetIds** for HGCal geometry before applying any validation.

### 📥 Input
- Raw DetIds in **CSV format**
- DetIds cover all valid configurations in HGCal geometry

### 🔢 Total Number of Raw DetIds (Before Validation)

| Subdetector      | Count        |
|------------------|--------------|
| EE (Electromagnetic Endcap)        | 67,837,952   |
| HE Silicon (Hadronic Endcap - Silicon) | 54,792,192   |
| HE Scintillator (Hadronic Endcap - Scintillator) | 7,660,814    |

These DetIds are generated programmatically and **span all layers**, **module types**, and **wafer configurations** supported in the CMSSW geometry. The next step is to pass them through the validation producer to filter only those compatible with the current release geometry.

---

### 🧬 DetId Bit-Level Definition

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

### 🧰 Output (from Step A)
- `raw_detids.csv`: Contains all generated DetIds before validation.
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

### Step 1: Use the Provided SQLite DB

- Users can query the precomputed `valid_detids.sqlite` database for required DetIds.

### Step 2: Run Queries

- Users extract specific DetIds and export to CSV.
- Output CSV format:
  - `DetId`, `NLayer`, `DetType`

### Step 3: Feed to Producer for SimHit Generation

- This CSV is fed into a separate **SimHit producer** to fill `PCaloHit` for:
  - `EE`
  - `HE Silicon`
  - `HE Scintillator`

### Step 4: Output ROOT Files

- Step-wise ROOT output:
  - `step1.root`: PCaloHit (SimHits) generated
  - `step2.root`: Digi (digitization) output
  - `step3.root`: Reco (reconstruction) output

---

## 🔍 Visualization with Fireworks

- After `step3.root` is produced, hits can be visualized using the **Fireworks** event display.
- Allows inspection of:
  - Hit distribution across layers
  - Valid DetId mapping

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

