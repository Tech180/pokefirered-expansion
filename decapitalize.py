#!/usr/bin/env python3
import os
import re
import json
import argparse

# Static replacements for core terminology and badges
STATIC_REPLACEMENTS = {
    # Specific compound terms first
    r"POKéMON CENTER": "Pokémon Center",
    r"POKéMON CENTERS": "Pokémon Centers",
    r"POKéMON LEAGUE": "Pokémon League",
    r"BIKE SHOP": "Bike Shop",
    r"GYM LEADER": "Gym Leader",
    r"GYM LEADERS": "Gym Leaders",
    r"BUG CATCHER": "Bug Catcher",
    r"BUG CATCHERS": "Bug Catchers",
    r"YOUNGSTER": "Youngster",
    r"YOUNGSTERS": "Youngsters",
    r"LASS": "Lass",
    r"LASSES": "Lasses",
    r"POKéMART": "Pokémart",
    r"POKé MARTS": "Pokémarts",
    r"POKé MART": "Pokémart",

    # Partially-decapitalized compound terms
    r"Pokémon CENTER": "Pokémon Center",
    r"Pokémon CENTERS": "Pokémon Centers",
    r"Pokémon LEAGUE": "Pokémon League",
    r"Pokémon MART": "Pokémon Mart",
    r"Pokémon MARTS": "Pokémon Marts",
    r"Pokémon TOWER": "Pokémon Tower",
    r"Pokémon MANSION": "Pokémon Mansion",
    r"Pokémon FAN CLUB": "Pokémon Fan Club",
    r"Pokémon RESEARCH LAB": "Pokémon Research Lab",

    # Signposts & Other terms
    r"OAK'S PARCEL": "Oak's Parcel",
    r"OAK'S PC": "Oak's PC",
    r"OAK'S Pokémon SEMINAR": "Oak's Pokémon Seminar",
    r"TRAINER'S SCHOOL": "Trainer's School",
    r"TRAINER'S MARKET": "Trainer's Market",
    r"MR. PSYCHIC'S HOUSE": "Mr. Psychic's House",
    r"WARDEN'S HOME": "Warden's Home",
    r"SELPHY'S HOUSE": "Selphy's House",
    r"LORELEI'S HOUSE": "Lorelei's House",
    r"TODAY'S SMART SHOPPER": "Today's Smart Shopper",
    r"TODAY'S RIVAL TRAINER": "Today's Rival Trainer",
    r"WHAT'S NO. 1 IN HOENN TODAY": "What's No. 1 in Hoenn Today",
    r"KING'S ROCK": "King's Rock",
    r"CHILD'S PLAY": "Child's Play",
    r"Diglett'S Cave": "Diglett's Cave",

    # Standard terms
    r"POKéMON": "Pokémon",
    r"POKéDEXES": "Pokédexes",
    r"POKéDEX": "Pokédex",
    r"POKéBALL": "Poké Ball",
    r"POKéBALLS": "Poké Balls",
    r"POKé BALL": "Poké Ball",
    r"POKé BALLS": "Poké Balls",
    r"PREMIER BALL": "Premier Ball",
    r"PREMIER BALLS": "Premier Balls",
    r"POKéNAV": "Pokénav",
    r"POKéBLOCK": "Pokéblock",
    r"POKéBLOCKS": "Pokéblocks",
    r"POKé GEAR": "Pokégear",
    r"POKéGEAR": "Pokégear",
    r"COIN CASE": "Coin Case",
    r"TM CASE": "TM Case",
    r"BERRY POUCH": "Berry Pouch",
    r"FAME CHECKER": "Fame Checker",
    r"TEACHY TV": "Teachy TV",
    r"POKé FLUTE": "Poké Flute",
    r"POKéFLUTE": "Poké Flute",
    r"VS SEEKER": "VS Seeker",
    r"TRI-PASS": "Tri-Pass",
    r"RAINBOW PASS": "Rainbow Pass",
    r"MYSTERY GIFT": "Mystery Gift",
    r"MYSTERY EVENT": "Mystery Event",
    r"BATTLE SEARCHER": "Battle Searcher",
    
    # Gym Badges
    r"BOULDERBADGE": "Boulder Badge",
    r"CASCADEBADGE": "Cascade Badge",
    r"THUNDERBADGE": "Thunder Badge",
    r"RAINBOWBADGE": "Rainbow Badge",
    r"SOULBADGE": "Soul Badge",
    r"MARSHBADGE": "Marsh Badge",
    r"VOLCANOBADGE": "Volcano Badge",
    r"EARTHBADGE": "Earth Badge",
    
    r"BAG": "Bag",
    r"COINS": "Coins",

    # Gym Names
    r"PEWTER GYM": "Pewter Gym",
    r"CERULEAN GYM": "Cerulean Gym",
    r"VERMILION GYM": "Vermilion Gym",
    r"CELADON GYM": "Celadon Gym",
    r"FUCHSIA GYM": "Fuchsia Gym",
    r"SAFFRON GYM": "Saffron Gym",
    r"CINNABAR GYM": "Cinnabar Gym",
    r"VIRIDIAN GYM": "Viridian Gym",
    r"GYMS": "Gyms",
    r"GYM": "Gym",

    # Trainer terms
    r"TRAINER TIPS": "Trainer Tips",
    r"Trainer TIPS": "Trainer Tips",
    r"TRAINERS": "Trainers",
    r"TRAINER": "Trainer",

    # Key Character Names
    r"BROCK": "Brock",
    r"MISTY": "Misty",
    r"LT. SURGE": "Lt. Surge",
    r"ERIKA": "Erika",
    r"KOGA": "Koga",
    r"SABRINA": "Sabrina",
    r"BLAINE": "Blaine",
    r"GIOVANNI": "Giovanni",
    r"LORELEI": "Lorelei",
    r"BRUNO": "Bruno",
    r"AGATHA": "Agatha",
    r"LANCE": "Lance",
}

def title_case_map_name(name):
    if not name:
        return name
    if name == "S.S. ANNE":
        return "S.S. Anne"
    if name == "SILPH CO.":
        return "Silph Co."
    
    res = name.title()
    res = res.replace("'S", "'s")
    res = res.replace("S.S. ", "S.S. ")
    res = res.replace("S.s. ", "S.S. ")
    res = res.replace("Ss ", "S.S. ")
    res = res.replace("Mt. ", "Mt. ")
    res = res.replace("Mt ", "Mt. ")
    res = res.replace("Silph Co.", "Silph Co.")
    res = res.replace("Silph Co", "Silph Co.")
    res = res.replace("Pokéball", "Poké Ball")
    res = res.replace("Poké Ball", "Poké Ball")
    res = res.replace("And ", "and ")
    res = res.replace("Of ", "of ")
    res = res.replace("The ", "the ")
    res = res.replace("To ", "to ")
    res = res.replace("In ", "in ")
    return res

def title_case_trainer_name(res):
    if not res:
        return res
    res = res.replace("POKéMANIAC", "Pokémaniac")
    res = res.replace("COOLTRAINER", "Cooltrainer")
    
    parts = []
    current = ""
    in_bracket = False
    for char in res:
        if char == '{':
            if current:
                parts.append((current, in_bracket))
            current = "{"
            in_bracket = True
        elif char == '}':
            current += "}"
            parts.append((current, in_bracket))
            current = ""
            in_bracket = False
        else:
            current += char
    if current:
        parts.append((current, in_bracket))
        
    new_parts = []
    for text, is_bracket in parts:
        if is_bracket:
            new_parts.append(text)
        else:
            t = text.title()
            t = t.replace("'S", "'s")
            t = t.replace(" And ", " and ")
            t = t.replace(" Of ", " of ")
            t = t.replace(" Or ", " or ")
            t = t.replace(" Sis ", " Sis ")
            t = t.replace(" Bro ", " Bro ")
            t = t.replace(" Jr. ", " Jr. ")
            t = t.replace(" Sr. ", " Sr. ")
            t = t.replace(" Prof. ", " Prof. ")
            new_parts.append(t)
            
    res = "".join(new_parts)
    res = res.replace("  ", " ")
    res = res.strip()
    return res

def build_replacements(region_map_path):
    replacements = []
    
    # 1. Add static replacements
    for k, v in STATIC_REPLACEMENTS.items():
        pattern = r'(?<![a-zA-Z0-9é])' + re.escape(k) + r'(?![a-zA-Z0-9é])'
        replacements.append((re.compile(pattern), v))
        
    # 2. Extract map names from JSON
    map_names = {}
    if os.path.exists(region_map_path):
        with open(region_map_path, "r", encoding="utf-8") as f:
            data = json.load(f)
            for item in data.get("map_sections", []):
                name = item.get("name")
                if name:
                    upper_name = name.upper().replace("É", "é").replace("é", "é")
                    title_name = title_case_map_name(name)
                    map_names[upper_name] = title_name
                    
    # Sort map names by length descending
    sorted_map_names = sorted(list(map_names.keys()), key=len, reverse=True)
    for name in sorted_map_names:
        # Avoid overriding static replacements if any overlap
        if name in STATIC_REPLACEMENTS:
            continue
        # Only process names that are strictly uppercase/spaces/punctuation/é
        if not re.match(r'^[A-Z0-9\.\-\x27\s\u00e9\u00c9]+$', name):
            continue
        title_name = map_names[name]
        pattern = r'(?<![a-zA-Z0-9é])' + re.escape(name) + r'(?![a-zA-Z0-9é])'
        replacements.append((re.compile(pattern), title_name))
        
    return replacements

def process_text_content(content, Compiled_replacements):
    if not content:
        return content, 0
        
    parts = []
    current = ""
    in_bracket = False
    for char in content:
        if char == '{':
            if current:
                parts.append((current, in_bracket))
            current = "{"
            in_bracket = True
        elif char == '}':
            current += "}"
            parts.append((current, in_bracket))
            current = ""
            in_bracket = False
        else:
            current += char
    if current:
        parts.append((current, in_bracket))
        
    new_parts = []
    total_replacements = 0
    for part_text, is_bracket in parts:
        if is_bracket:
            new_parts.append(part_text)
        else:
            new_text = part_text
            for pattern, replacement in Compiled_replacements:
                matches = pattern.findall(new_text)
                if matches:
                    total_replacements += len(matches)
                    new_text = pattern.sub(replacement, new_text)
            new_parts.append(new_text)
            
    return "".join(new_parts), total_replacements

def decapitalize_dialogue_file(filepath, Compiled_replacements, dry_run=False):
    with open(filepath, "r", encoding="utf-8") as f:
        lines = f.readlines()
        
    string_re = re.compile(r'^(\s*\.string\s+")([^"]*)(".*)$')
    new_lines = []
    file_changes = 0
    
    for idx, line in enumerate(lines):
        match = string_re.match(line)
        if match:
            prefix = match.group(1)
            str_val = match.group(2)
            suffix = match.group(3)
            
            new_val, count = process_text_content(str_val, Compiled_replacements)
            if count > 0:
                file_changes += count
                new_lines.append(f"{prefix}{new_val}{suffix}\n")
            else:
                new_lines.append(line)
        else:
            new_lines.append(line)
            
    if file_changes > 0 and not dry_run:
        with open(filepath, "w", encoding="utf-8") as f:
            f.writelines(new_lines)
            
    return file_changes

def decapitalize_c_file(filepath, Compiled_replacements, dry_run=False):
    with open(filepath, "r", encoding="utf-8") as f:
        lines = f.readlines()
        
    c_string_re = re.compile(r'((?:_|\bCOMPOUND_STRING)\(")([^"]*)("\))')
    new_lines = []
    file_changes = 0
    
    for line in lines:
        new_line = line
        matches = c_string_re.findall(line)
        for prefix, str_val, suffix in matches:
            new_val, count = process_text_content(str_val, Compiled_replacements)
            if count > 0:
                file_changes += count
                # Replace exact match in line
                target = f'{prefix}{str_val}{suffix}'
                replacement = f'{prefix}{new_val}{suffix}'
                new_line = new_line.replace(target, replacement)
        new_lines.append(new_line)
        
    if file_changes > 0 and not dry_run:
        with open(filepath, "w", encoding="utf-8") as f:
            f.writelines(new_lines)
            
    return file_changes

def decapitalize_battle_main_classes(filepath, dry_run=False):
    with open(filepath, "r", encoding="utf-8") as f:
        lines = f.readlines()
        
    file_changes = 0
    new_lines = []
    c_string_re = re.compile(r'(_\(")([^"]*)("\))')
    
    for idx, line in enumerate(lines):
        line_num = idx + 1
        # Limit to the gTrainerClasses array definition range (roughly 298-416)
        if 298 <= line_num <= 416:
            new_line = line
            matches = c_string_re.findall(line)
            for prefix, str_val, suffix in matches:
                title_val = title_case_trainer_name(str_val)
                if title_val != str_val:
                    file_changes += 1
                    target = f'{prefix}{str_val}{suffix}'
                    replacement = f'{prefix}{title_val}{suffix}'
                    new_line = new_line.replace(target, replacement)
            new_lines.append(new_line)
        else:
            new_lines.append(line)
            
    if file_changes > 0 and not dry_run:
        with open(filepath, "w", encoding="utf-8") as f:
            f.writelines(new_lines)
            
    return file_changes

def decapitalize_trainer_jsons(trainers_dir, dry_run=False):
    total_changes = 0
    for root, dirs, files in os.walk(trainers_dir):
        for file in files:
            if file.endswith(".json"):
                filepath = os.path.join(root, file)
                with open(filepath, "r", encoding="utf-8") as f:
                    try:
                        data = json.load(f)
                    except Exception as e:
                        print(f"Error reading JSON {filepath}: {e}")
                        continue
                        
                changed = False
                if "trainer_name" in data:
                    orig_name = data["trainer_name"]
                    title_name = title_case_trainer_name(orig_name)
                    if title_name != orig_name:
                        data["trainer_name"] = title_name
                        changed = True
                        total_changes += 1
                        
                if changed and not dry_run:
                    with open(filepath, "w", encoding="utf-8") as f:
                        json.dump(data, f, indent=2, ensure_ascii=False)
                        f.write("\n")
                        
    return total_changes

def decapitalize_region_map_json(filepath, dry_run=False):
    if not os.path.exists(filepath):
        return 0
        
    with open(filepath, "r", encoding="utf-8") as f:
        data = json.load(f)
        
    total_changes = 0
    for item in data.get("map_sections", []):
        name = item.get("name")
        if name and not any(c.islower() for c in name if ord(c) < 128):
            title_name = title_case_map_name(name)
            if title_name != name:
                item["name"] = title_name
                total_changes += 1
                
    if total_changes > 0 and not dry_run:
        with open(filepath, "w", encoding="utf-8") as f:
            json.dump(data, f, indent=2, ensure_ascii=False)
            f.write("\n")
            
    return total_changes

def main():
    parser = argparse.ArgumentParser(description="Decapitalize game text")
    parser.add_argument("--dry-run", action="store_true", help="Print changes without modifying files")
    args = parser.parse_args()
    
    region_map_path = "src/data/region_map/region_map_sections.json"
    trainers_dir = "data/trainers"
    maps_dir = "data/maps"
    text_dir = "data/text"
    scripts_dir = "data/scripts"
    src_dir = "src"
    battle_main_path = "src/battle_main.c"
    
    print("Loading region map and compiling replacements...")
    replacements = build_replacements(region_map_path)
    print(f"Compiled {len(replacements)} text replacement patterns.")
    
    print("\nProcessing region map JSON...")
    map_changes = decapitalize_region_map_json(region_map_path, dry_run=args.dry_run)
    print(f"Region map name changes: {map_changes}")
    
    print("\nProcessing trainer JSON files...")
    trainer_changes = decapitalize_trainer_jsons(trainers_dir, dry_run=args.dry_run)
    print(f"Trainer name changes: {trainer_changes}")
    
    print("\nProcessing C and H files in src/...")
    strings_changes = 0
    for root, dirs, files in os.walk(src_dir):
        for file in files:
            if file.endswith((".c", ".h")):
                filepath = os.path.join(root, file)
                # Skip certain autogenerated files if any, but in this case let's process all.
                changes = decapitalize_c_file(filepath, replacements, dry_run=args.dry_run)
                strings_changes += changes
    print(f"src/ C/H file changes: {strings_changes}")
    
    print("\nProcessing battle_main.c trainer classes...")
    battle_changes = decapitalize_battle_main_classes(battle_main_path, dry_run=args.dry_run)
    print(f"battle_main.c trainer class changes: {battle_changes}")
    
    print("\nProcessing map scripts (.inc)...")
    map_script_changes = 0
    for root, dirs, files in os.walk(maps_dir):
        for file in files:
            if file.endswith(".inc"):
                filepath = os.path.join(root, file)
                changes = decapitalize_dialogue_file(filepath, replacements, dry_run=args.dry_run)
                map_script_changes += changes
    print(f"Map script changes: {map_script_changes}")
    
    print("\nProcessing global dialogue texts (.inc)...")
    text_changes = 0
    for root, dirs, files in os.walk(text_dir):
        for file in files:
            if file.endswith(".inc"):
                filepath = os.path.join(root, file)
                changes = decapitalize_dialogue_file(filepath, replacements, dry_run=args.dry_run)
                text_changes += changes
    print(f"Global dialogue text changes: {text_changes}")

    print("\nProcessing data/scripts (.inc)...")
    script_changes = 0
    for root, dirs, files in os.walk(scripts_dir):
        for file in files:
            if file.endswith(".inc"):
                filepath = os.path.join(root, file)
                changes = decapitalize_dialogue_file(filepath, replacements, dry_run=args.dry_run)
                script_changes += changes
    print(f"data/scripts changes: {script_changes}")
    
    total = map_changes + trainer_changes + strings_changes + battle_changes + map_script_changes + text_changes + script_changes
    print(f"\nTotal changes logged: {total} (Dry run: {args.dry_run})")

if __name__ == "__main__":
    main()
