import os
import re
import json
import glob
import subprocess

def parse_species_constants(header_path):
    species_map = {}
    if not os.path.exists(header_path):
        return species_map
    with open(header_path, 'r') as f:
        content = f.read()
    pattern = re.compile(r'(SPECIES_\w+)\s*=\s*(\d+)')
    for match in pattern.finditer(content):
        species_map[match.group(1)] = int(match.group(2))
    return species_map

def parse_species_block(block_text):
    entries = []
    lines = block_text.split('\n')
    i = 0
    while i < len(lines):
        line = lines[i].strip()
        if not line: i += 1; continue
        if line.startswith('#'):
            entries.append({"type": "preprocessor", "line": line})
            i += 1; continue
        field_match = re.match(r'\.(\w+)\s*=\s*(.*)', line)
        if field_match:
            field_name = field_match.group(1)
            value = field_match.group(2)
            full_value = value
            depth_p = full_value.count('(') - full_value.count(')')
            depth_b = full_value.count('{') - full_value.count('}')
            while (depth_p > 0 or depth_b > 0 or not full_value.endswith(',')) and i + 1 < len(lines):
                i += 1
                next_line = lines[i].strip()
                full_value += "\n" + next_line
                depth_p += next_line.count('(') - next_line.count(')')
                depth_b += next_line.count('{') - next_line.count('}')
            full_value = full_value.strip()
            if full_value.endswith(','): full_value = full_value[:-1].strip()
            entries.append({"type": "field", "name": field_name, "value": full_value})
            i += 1; continue
        macro_match = re.match(r'(\w+)\s*\((.*)', line)
        if macro_match:
            macro_name = macro_match.group(1)
            args = macro_match.group(2)
            full_args = args
            depth_p = full_args.count('(') - full_args.count(')') + 1
            while depth_p > 0 and i + 1 < len(lines):
                i += 1
                next_line = lines[i].strip()
                full_args += "\n" + next_line
                depth_p += next_line.count('(') - next_line.count(')')
            full_args = full_args.strip()
            has_comma = False
            if full_args.endswith('),'): full_args = full_args[:-2].rstrip(); has_comma = True
            elif full_args.endswith(')'): full_args = full_args[:-1].rstrip()
            entries.append({"type": "macro", "name": macro_name, "args": full_args, "has_comma": has_comma})
            i += 1; continue
        i += 1
    return entries

def parse_pokedex_to_gens(header_path):
    species_to_gen = {}
    if not os.path.exists(header_path):
        return species_to_gen
    with open(header_path, 'r') as f:
        content = f.read()
    gen_markers = {
        '// Kanto': 1, '// Johto': 2, '// Hoenn': 3, '// Sinnoh': 4,
        '// Unova': 5, '// Kalos': 6, '// Alola': 7, '// Galar': 8,
        '// Unknown': 8, '// Paldea': 9
    }
    current_gen = 1
    lines = content.split('\n')
    for line in lines:
        line = line.strip()
        for marker, gen in gen_markers.items():
            if marker in line: current_gen = gen; break
        m = re.search(r'(NATIONAL_DEX_\w+)', line)
        if m:
            const = m.group(1)
            if const != 'NATIONAL_DEX_NONE' and const != 'NATIONAL_DEX_END':
                species_to_gen[const] = current_gen
    return species_to_gen

def clean_gap_text(text):
    lines = text.split('\n')
    clean = []
    i = 0
    while i < len(lines):
        l = lines[i]
        s = l.strip()
        if s.startswith('#ifdef __INTELLISENSE__'):
            if i + 1 < len(lines) and ('gSpeciesInfo' in lines[i+1] or '};' in lines[i+1]):
                while i < len(lines) and not lines[i].strip().startswith('#endif'): i += 1
                i += 1; continue
        if 'gSpeciesInfo' in s or s == '{' or s == '};' or s == '},':
            if not s.startswith('#'): # Don't skip if it's #define
                i += 1; continue
        if re.search(r'^#if\s+P_FAMILY_|^#endif //\s*P_FAMILY_|^#endif //P_FAMILY_', s):
            i += 1; continue
        clean.append(l)
        i += 1
    return "\n".join(clean).strip()

def main():
    species_map = parse_species_constants('include/constants/species.h')
    dex_to_gen = parse_pokedex_to_gens('include/constants/pokedex.h')
    
    for target_gen in range(1, 10):
        input_file = f'src/data/pokemon/species_info/gen_{target_gen}_families.h'
        output_base = f'data/pokemon/species_info'
        
        try:
            content = subprocess.check_output(["git", "show", f"origin/master:{input_file}"]).decode('utf-8')
        except:
            if not os.path.exists(input_file): continue
            with open(input_file, 'r') as f: content = f.read()

        manifest = []
        
        match_first_fam = re.search(r'(^#if\s+P_FAMILY_)', content, flags=re.MULTILINE)
        if match_first_fam:
            header_area = content[:match_first_fam.start()]
            clean_head = clean_gap_text(header_area)
            if clean_head: manifest.append({"type": "raw", "content": clean_head})
        else:
            clean_head = clean_gap_text(content)
            if clean_head: manifest.append({"type": "raw", "content": clean_head})

        # Improved pattern to catch: [X] = { ... }, [X] = MACRO(...), and [X] = MACRO,
        species_pattern = re.compile(r'\[(SPECIES_\w+)\]\s*=\s*(?:\{\s*(.*?)\n\s*\}|([A-Z0-9_]+\s*\([\s\S]*?\)|[A-Z0-9_]+))(?:,)?', re.DOTALL)
        
        family_blocks = re.split(r'(^#if\s+P_FAMILY_\w+.*)', content, flags=re.MULTILINE)
        for i in range(1, len(family_blocks), 2):
            family_header = family_blocks[i]
            family_body = family_blocks[i+1]
            f_match = re.search(r'P_FAMILY_\w+', family_header)
            if not f_match: continue
            family_const = f_match.group(0)
            
            fam_manifest = {"type": "family", "family_constant": family_const, "components": []}
            manifest.append(fam_manifest)
            
            last_pos = 0
            species_matches = list(species_pattern.finditer(family_body))
            for s_match in species_matches:
                gap = family_body[last_pos:s_match.start()]
                raw_gap = clean_gap_text(gap)
                if raw_gap: fam_manifest["components"].append({"type": "raw", "content": raw_gap})
                
                species_const = s_match.group(1)
                struct_body = s_match.group(2)
                macro_body = s_match.group(3)
                
                species_data = {
                    "species_constant": species_const,
                    "family_constant": family_const,
                }
                
                if struct_body:
                    species_data["entries"] = parse_species_block(struct_body)
                elif macro_body:
                    m_val = macro_body.strip()
                    if m_val.endswith(','): m_val = m_val[:-1].strip()
                    species_data["macro_call"] = m_val

                real_gen = target_gen
                if '_MEGA' in species_const or '_PRIMAL' in species_const: real_gen = 6
                elif '_ALOLA' in species_const: real_gen = 7
                elif '_GALAR' in species_const or '_GMAX' in species_const: real_gen = 8
                elif '_HISUI' in species_const: real_gen = 8
                elif '_PALDEA' in species_const: real_gen = 9
                else:
                    natdex = ""
                    if struct_body:
                        for e in species_data.get("entries", []):
                            if e.get('name') == 'natDexNum': natdex = e.get('value'); break
                    real_gen = dex_to_gen.get(natdex, target_gen)

                os.makedirs(os.path.join(output_base, f"gen_{real_gen}"), exist_ok=True)
                index = species_map.get(species_const)
                prefix = f"{index:04}_" if index is not None else ""
                json_filename = f"{prefix}{species_const[8:].lower()}.json"
                
                with open(os.path.join(output_base, f"gen_{real_gen}", json_filename), 'w') as f:
                    json.dump(species_data, f, indent=2)
                
                fam_manifest["components"].append({"type": "species", "constant": species_const, "filename": json_filename, "gen": real_gen})
                last_pos = s_match.end()

            post_gap = family_body[last_pos:]
            raw_pg = clean_gap_text(post_gap)
            if raw_pg: fam_manifest["components"].append({"type": "raw", "content": raw_pg})

        os.makedirs(os.path.join(output_base, f"gen_{target_gen}"), exist_ok=True)
        with open(os.path.join(output_base, f"gen_{target_gen}", "manifest.json"), 'w') as f:
            json.dump(manifest, f, indent=2)

if __name__ == "__main__":
    main()
