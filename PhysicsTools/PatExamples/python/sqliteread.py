import sqlite3
import re
import csv
import time
import time
import argparse
import sys

db_file = "detid_data_all_feature.db"  # replace with your actual SQLite DB file


# === Column descriptions ===
column_descriptions = {
    "DetId": "Unique detector identifier",
    "Zside": "Detector side (1 = +z side, -1 = -z side)",
    "DetType": "Detector type (8 = CE-E, 9 = CE-H, 10 = CE-H scintillator)",
    "Nlayer": "Layer number within the detector stack",
    "LayerType": "Type of layer",
    "FrontBack": "Position in module",
    "WaferType": "Wafer type",
    "WaferIndex": "Unique index for wafers within a layer",
    "PartType": "Partition type",
    "Cassette": "Cassette identifier",
    "CassetteType": "Cassette type",
    "Orient": "Wafer orientation flag",
    "AbsU": "Absolute U-coordinate of wafer",
    "AbsV": "Absolute V-coordinate of wafer",
    "CellU": "Cell coordinate U",
    "CellV": "Cell coordinate V",
    "x": "x-position (mm) of cell center in global coordinates",
    "y": "y-position (mm) of cell center in global coordinates",
    "z": "z-position (mm) of cell center in global coordinates",
    "TileType": "Scintillator tile type (for CE-H scintillator)",
    "SipmType": "SiPM sensor type used with scintillator tiles",
    "TriggerCell": "Trigger or cell type",
    "RingIndex": "Radial ring index",
    "IphiIndex": "Azimuthal φ index",
    "Granularity": "Granularity of scintillator detector",
    "eta": "Pseudorapidity of the cell center",
    "phi": "Azimuthal angle of the cell center",
}

# === Parse arguments ===
parser = argparse.ArgumentParser(add_help=False)
parser.add_argument("--help", action="store_true", help="Show available tables and columns")
args = parser.parse_args()

# Connect to SQLite DB
conn = sqlite3.connect(db_file)
cursor = conn.cursor()

#List all tables
cursor.execute("SELECT name FROM sqlite_master WHERE type='table';")
tables = cursor.fetchall()

if args.help:
    print("\n Available tables:")
    for idx, tbl in enumerate(tables):
        table_name = tbl[0]
        print(f"\n{idx + 1}: {table_name}")

        # Get columns for that table
        cursor.execute(f"PRAGMA table_info({table_name});")
        columns_info = cursor.fetchall()

        print("  Columns with description:")
        for col in columns_info:
            col_name = col[1]
            desc = column_descriptions.get(col_name, "No description available")
            print(f"   - {col_name:12} : {desc}")

    conn.close()
    sys.exit(0)

# Choose a table
choice = 1   #change this number if you want to choose different table
table_name = tables[choice - 1][0]

print(f"\n Selected table: {table_name}")   # <-- NEW LINE ADDED

# === Get column names ===
cursor.execute(f"PRAGMA table_info({table_name});")
columns_info = cursor.fetchall()

# Build column name
column_map = {}
#print("\n Available columns:")
for col in columns_info:
    col_name = col[1]  # second field is column name
    column_map[col_name] = col_name
#    print(f"- {col_name}")

# === Hardcoded WHERE condition ===
# Example queries: (WaferType = 2 AND Zside = -1) OR Nlayer BETWEEN 5 AND 15
# Example queries: (WaferType = 1 AND Zside = 1) AND Nlayer > 10
# Example queries: (DetType = 8 OR DetType = 9) AND Nlayer BETWEEN 5 AND 20
# Example queries: (Zside = -1 AND LayerType = 2) OR (FrontBack = 1 AND WaferType = 3)
# Example queries: (eta > 1.5 AND eta < 2.5) AND (Granularity = 8 OR Granularity = 9)
# Example queries: (CassetteType = 2 AND Orient = 1) OR (AbsU BETWEEN 5 AND 15 AND AbsV BETWEEN 10 AND 20)

user_input = "X > 200"
#user_input = "(eta > 1.5 OR eta < 2.5) AND WaferType = 0"
#user_input = "(WaferType = 2 AND Zside = -1) OR Nlayer BETWEEN 5 AND 15"

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


print(f"\n Query condition: {user_input}")  

try:
    start_time = time.time()
    cursor.execute(query)
    results = cursor.fetchall()
    end_time = time.time()

    print(f"\n Query execution time: {end_time - start_time:.4f} seconds")
    print(f" Found {len(results)} matching entries:")

    for row in results[:10]:
        print(row)

    # Count DetType values
    det8 = sum(1 for r in results if r[1] == 8)
    det9 = sum(1 for r in results if r[1] == 9)
    det10 = sum(1 for r in results if r[1] == 10)

    print(f"\n Counts by DetType:")
    print(f"  DetType = 8 : {det8}")
    print(f"  DetType = 9 : {det9}")
    print(f"  DetType = 10: {det10}")

    # === Save to CSV ===
    output_file = "quried_detid_output.csv"
    with open(output_file, "w", newline="") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(selected_columns)
        writer.writerows(results)

    print(f"\n Results saved to {output_file}")

except Exception as e:
    print(f"\n Query failed: {e}")

# === Close connection ===
conn.close()
