#!/usr/bin/env python3

"""
Usage: python3 make_teaching_types.py OUTPUT_FILE

Build a primary store of "teaching-types" for each enabled species in the repository as an
input for make_teachables.py.

UPDATED: Reads from data/pokemon/species_info/ manifest.json files.
"""

import json
import pathlib
import os
import sys

def enabled() -> bool:
    with open("./include/config/pokemon.h", "r") as cfg_pokemon_fp:
        cfg_pokemon = cfg_pokemon_fp.read()
        return "#define P_LEARNSET_HELPER_TEACHABLE" in cfg_pokemon and "TRUE" in cfg_pokemon

def extract_repo_species_data() -> list:
    species_data = []
    pokemon_list = []
    
    base_dir = "data/pokemon/species_info"
    
    # Iterate through all generations
    for gen in range(1, 10):
        manifest_path = os.path.join(base_dir, f"gen_{gen}", "manifest.json")
        if not os.path.exists(manifest_path):
            continue
            
        with open(manifest_path, "r") as f:
            manifest = json.load(f)
            
        for item in manifest:
            if item["type"] == "family":
                species_data.append(f"\n#if {item['family_constant']}\n")
                
                for component in item["components"]:
                    if component["type"] == "species":
                        spec_filename = component["filename"]
                        spec_gen = component["gen"]
                        spec_path = os.path.join(base_dir, f"gen_{spec_gen}", spec_filename)
                        
                        if not os.path.exists(spec_path):
                            continue
                            
                        with open(spec_path, "r") as sf:
                            spec = json.load(sf)
                            
                        spec_const = spec.get('species_constant', '')
                        clean_const = spec_const.replace('SPECIES_', '')
                        
                        teaching_type = "DEFAULT_LEARNING"
                        found_name = None
                        
                        if "entries" in spec:
                            for entry in spec["entries"]:
                                if entry.get("name") == "teachableLearnset":
                                    val = entry.get("value", "")
                                    if val.startswith("s") and val.endswith("TeachableLearnset"):
                                        found_name = val[1:-17]
                                if entry.get("name") == "teachingType":
                                    teaching_type = entry.get("value", "DEFAULT_LEARNING")
                                if entry.get("type") == "macro" and entry.get("name", "").endswith("_INFO"):
                                    # Extract base name from macro like BULBASAUR_SPECIES_INFO
                                    base_parts = entry.get("name", "").split('_')
                                    if len(base_parts) > 1:
                                        found_name = base_parts[0].capitalize()
                        
                        if "macro_call" in spec:
                            macro_start = spec["macro_call"].split('(')[0]
                            base_parts = macro_start.split('_')
                            if len(base_parts) > 1:
                                found_name = base_parts[0].capitalize()
                        
                        final_name = found_name if found_name else "".join(x.capitalize() for x in clean_const.split('_'))
                        
                        if final_name not in pokemon_list:
                            species_data.append({"name": final_name, "teaching_type": teaching_type})
                            pokemon_list.append(final_name)
                
                species_data.append(f"#endif // {item['family_constant']}\n")
                
    return species_data

def add_whitesspaces(parsed_list) -> list:
    for i, item in enumerate(parsed_list):
        if i == 0: continue
        if not isinstance(item, str): continue
        if item.startswith("#if"):
            if not isinstance(parsed_list[i - 1], str):
                parsed_list[i] = "\n" + item
            elif parsed_list[i - 1].startswith("#endif"):
                parsed_list[i] = "\n" + item
    return parsed_list

def main():
    if not enabled(): quit()
    if len(sys.argv) < 2: quit(1)

    OUTPUT_FILE = pathlib.Path(sys.argv[1])
    teaching_types_list = extract_repo_species_data()
    teaching_types_list = add_whitesspaces(teaching_types_list)
    
    with open(OUTPUT_FILE, "w") as fp:
        fp.write(json.dumps(teaching_types_list, indent=2))
        
    pathlib.Path("./tools/learnset_helpers/make_teachables.py").touch()

if __name__ == "__main__":
    main()
