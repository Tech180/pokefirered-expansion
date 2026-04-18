import os
import re
import json
import glob

def get_gen_from_natdex(num):
    if num <= 151: return 1
    if num <= 251: return 2
    if num <= 386: return 3
    if num <= 493: return 4
    if num <= 649: return 5
    if num <= 721: return 6
    if num <= 809: return 7
    if num <= 905: return 8
    return 9

def parse_pokedex_to_gens(header_path):
    species_to_gen = {}
    if not os.path.exists(header_path):
        return species_to_gen
    with open(header_path, 'r') as f:
        content = f.read()
    
    gen_markers = {
        '// Kanto': 1, '// Johto': 2, '// Hoenn': 3, '// Sinnoh': 4,
        '// Unova': 5, '// Kalos': 6, '// Alola': 7, '// Galar': 8,
        '// Unknown': 8, '// Paldea': 9,
        'NATIONAL_DEX_SPRIGATITO': 9
    }
    
    current_gen = 1
    
    # regex to find NATIONAL_DEX constants and comments
    # We want to iterate line by line to track current_gen
    lines = content.split('\n')
    for line in lines:
        line = line.strip()
        
        # Check for Gen marker (comment or specific constant)
        for marker, gen in gen_markers.items():
            if marker in line:
                current_gen = gen
                break
        
        # Match NATIONAL_DEX_...
        m = re.search(r'(NATIONAL_DEX_\w+)', line)
        if m:
            const = m.group(1)
            if const != 'NATIONAL_DEX_NONE' and const != 'NATIONAL_DEX_END':
                species_to_gen[const] = current_gen
                
    return species_to_gen

def main():
    species_to_gen = parse_pokedex_to_gens('include/constants/pokedex.h')
    base_dir = 'data/pokemon/species_info'
    
    # json_files across all folders
    json_files = glob.glob(os.path.join(base_dir, 'gen_*', '*.json'))
    
    for fpath in json_files:
        if fpath.endswith('family_order.json'): continue
        
        with open(fpath, 'r') as f:
            data = json.load(f)
            
        spec_const = data.get('species_constant', '')
        natdex_const = ""
        for entry in data.get('entries', []):
            if entry.get('name') == 'natDexNum':
                natdex_const = entry.get('value', '')
                break
        
        # Determine Gen
        target_gen = None
        
        # 1. Check for form keywords in constant
        # Mega/Primal: Gen 6
        if '_MEGA' in spec_const or '_PRIMAL' in spec_const:
            target_gen = 6
        elif '_ALOLA' in spec_const:
            target_gen = 7
        elif '_GALAR' in spec_const:
            target_gen = 8
        elif '_HISUI' in spec_const:
            target_gen = 8
        elif '_PALDEA' in spec_const:
            target_gen = 9
        elif '_GMAX' in spec_const:
            target_gen = 8
            
        # 2. Use National Dex mapping
        if target_gen is None:
            if natdex_const in species_to_gen:
                target_gen = species_to_gen[natdex_const]
                
        if target_gen is None:
             # Basic fallback for base species
             print(f"Could not determine Gen for {fpath}, staying put.")
             continue
             
        target_dir = os.path.join(base_dir, f'gen_{target_gen}')
        os.makedirs(target_dir, exist_ok=True)
        
        new_fpath = os.path.join(target_dir, os.path.basename(fpath))
        
        if fpath != new_fpath:
            print(f"Moving {fpath} -> {new_fpath}")
            os.rename(fpath, new_fpath)

if __name__ == "__main__":
    main()
