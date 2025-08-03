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

