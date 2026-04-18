import os
import re
import json
import glob

def parse_species_constants(header_path):
    species_map = {}
    with open(header_path, 'r') as f:
        content = f.read()
    
    # Simple regex for enum or #define
    # Matches: SPECIES_BULBASAUR = 1,
    pattern = re.compile(r'(SPECIES_\w+)\s*=\s*(\d+)')
    for match in pattern.finditer(content):
        species_map[match.group(1)] = int(match.group(2))
    
    return species_map

def main():
    header_path = 'include/constants/species.h'
    species_map = parse_species_constants(header_path)
    
    json_path_pattern = 'data/pokemon/species_info/gen_*/*.json'
    files = glob.glob(json_path_pattern)
    
    for fpath in files:
        if fpath.endswith('family_order.json'):
            continue
            
        with open(fpath, 'r') as f:
            try:
                data = json.load(f)
            except json.JSONDecodeError:
                print(f"Skipping {fpath}: invalid JSON.")
                continue
        
        spec_const = data.get('species_constant')
        if not spec_const:
            print(f"Skipping {fpath}: no species_constant found.")
            continue
            
        index = species_map.get(spec_const)
        if index is None:
            # Try to handle cases without explicit = in the same line or if it's a #define
            # (Though our regex is pretty good for this species.h)
            print(f"Warning: Could not find index for {spec_const} in {fpath}")
            continue
            
        dirname = os.path.dirname(fpath)
        basename = os.path.basename(fpath)
        
        # Avoid double prefixing if already prefixed
        if re.match(r'^\d{4}_', basename):
             # Update if number changed? 
             clean_name = re.sub(r'^\d{4}_', '', basename)
        else:
             clean_name = basename
             
        new_basename = f"{index:04}_{clean_name}"
        new_fpath = os.path.join(dirname, new_basename)
        
        if fpath != new_fpath:
            print(f"Renaming {fpath} -> {new_fpath}")
            os.rename(fpath, new_fpath)

if __name__ == "__main__":
    main()
