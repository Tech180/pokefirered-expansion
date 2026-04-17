import json
import glob
import os

def main():
    # Load group metadata
    with open('data/encounters/groups.json', 'r') as f:
        groups_metadata = json.load(f)

    # Find all modular encounter files
    encounter_files = glob.glob('data/encounters/**/*.json', recursive=True)
    
    # Exclude groups.json itself
    encounter_files = [f for f in encounter_files if 'groups.json' not in f]

    # Organize encounters by group label
    collected_encounters = {}
    for fpath in encounter_files:
        with open(fpath, 'r') as f:
            data = json.load(f)
            label = data["group_label"]
            if label not in collected_encounters:
                collected_encounters[label] = []
            collected_encounters[label].extend(data["encounters"])

    # Re-assemble the original structure
    output_data = {"wild_encounter_groups": []}
    for group_meta in groups_metadata:
        label = group_meta["label"]
        group = group_meta.copy()
        
        # Get encounters for this group
        encs = collected_encounters.get(label, [])
        
        # Sort encounters by base_label for deterministic output
        # (This helps prevent unnecessary git diffs if files are loaded in different orders)
        encs.sort(key=lambda x: x.get("base_label", ""))
        
        group["encounters"] = encs
        output_data["wild_encounter_groups"].append(group)

    # Output to src/data/wild_encounters.json
    output_path = 'src/data/wild_encounters.json'
    with open(output_path, 'w') as f:
        json.dump(output_data, f, indent=2)
    print(f"Re-generated {output_path} from modular encounters.")

if __name__ == "__main__":
    main()
