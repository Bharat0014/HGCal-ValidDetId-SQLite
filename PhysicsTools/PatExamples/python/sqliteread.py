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

