
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

### Step B: Validate DetIds
- Use a dedicated EDProducer in:
  - `CMSSW_15_1_X_2025-07-13-2300`
  - `GeometryExtended2026D110`
- Validate DetIds based on HGCal geometry.
- Output: `valid_detids.csv`

### Step C: Store Valid DetIds into Database
- Read the valid DetIds from the CSV.
- Store them into a local database:
  - **Table**: `hgcal_detids_v5`
  - **27 columns** of features per DetId.
- Database options:
  
  - **Recommended**: SQLite (`.db` file per release)

 ## ✅ Step B & C: Validate and Store HGCal DetIds
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
| `valid_detids.sqlite` | SQLite DB containing DetId table     |

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

