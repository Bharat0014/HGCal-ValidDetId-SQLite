
# HGCal ValidDetId Validation and PCaloHit Workflow

This repository documents the validation and usage of **ValidDetIds** in the High Granularity Calorimeter (HGCal) geometry of CMS. The workflow supports **raw DetId generation**, **validation**, **database storage**, and **SimHit (PCaloHit) creation**, followed by **visualization** using Fireworks.

---

## 📌 Overview

The workflow is split into **Admin Setup (once per CMSSW release)** and **User Workflow**.

---

## 🧰 Admin Workflow (Once per Release)

### Step 1: Generate Raw DetIds

- A script or producer generates **raw DetIds** based on the HGCal detector definition.
- Output: `raw_detids.csv`

### Step 2: Run EDProducer for Validation

- A custom **EDProducer** is used to validate DetIds for:
  - `EE` (Electromagnetic Endcap)
  - `HE Silicon`
  - `HE Scintillator`

### Step 3: Store Valid DetIds

- Validated DetIds are stored into:
  - `valid_detids.csv`
  - `valid_detids.sqlite` (SQLite format for fast query access)

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

