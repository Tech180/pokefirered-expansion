import json
import os
import glob

def get_trainer_id(data):
    return data.get("identifier")

def sanitize_c_identifier(s):
    if not s:
        return "0"
    if not isinstance(s, str):
        return str(s)
    return s.strip().replace(" ", "_")

def format_moves(moves):
    if not moves:
        return "MOVE_NONE, MOVE_NONE, MOVE_NONE, MOVE_NONE"
    # GBA engine expects 4 moves or trailing zeroes
    formatted = []
    for i in range(4):
        if i < len(moves):
            formatted.append(sanitize_c_identifier(moves[i]))
        else:
            formatted.append("MOVE_NONE")
    return ", ".join(formatted)

def format_ivs(ivs):
    if not ivs:
        return "TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0)"
    return f"TRAINER_PARTY_IVS({ivs.get('hp', 0)}, {ivs.get('atk', 0)}, {ivs.get('def', 0)}, {ivs.get('spe', 0)}, {ivs.get('spa', 0)}, {ivs.get('spd', 0)})"

def format_evs(evs):
    if not evs:
        return "NULL"
    return f"TRAINER_PARTY_EVS({evs.get('hp', 0)}, {evs.get('atk', 0)}, {evs.get('def', 0)}, {evs.get('spe', 0)}, {evs.get('spa', 0)}, {evs.get('spd', 0)})"

def format_trainer_mon(mon):
    species = sanitize_c_identifier(mon.get("species", "SPECIES_NONE"))
    level = mon.get("level", 1)
    moves = format_moves(mon.get("moves", []))
    ivs = format_ivs(mon.get("ivs", {}))
    evs = format_evs(mon.get("evs", {}))
    ability = sanitize_c_identifier(mon.get("ability", "ABILITY_NONE"))
    nature = sanitize_c_identifier(mon.get("nature", "NATURE_HARDY"))
    item = sanitize_c_identifier(mon.get("held_item", "ITEM_NONE"))
    shiny = "TRUE" if mon.get("isShiny", False) else "FALSE"
    
    # We'll use a more compact formatting similar to the original trainers.h
    lines = [
        "            {",
        f"            .species = {species},",
        f"            .lvl = {level},",
        f"            .iv = {ivs},",
        f"            .ev = {evs},",
        f"            .ability = {ability},",
        f"            .nature = {nature},",
        f"            .heldItem = {item},",
        f"            .isShiny = {shiny},",
        "            .ball = POKEBALL_COUNT,",
        "            .dynamaxLevel = MAX_DYNAMAX_LEVEL,",
        "            .moves = {" + moves + "},",
        "            },"
    ]
    return "\n".join(lines)

def format_trainer_block(difficulty, identifier, data):
    name = data.get("trainer_name", "???")
    t_class = data.get("trainer_class", "TRAINER_CLASS_PKMN_TRAINER_1")
    t_pic = data.get("trainer_pic", "TRAINER_PIC_HIKER")
    gender = sanitize_c_identifier(data.get("gender", "TRAINER_GENDER_MALE"))
    music = sanitize_c_identifier(data.get("encounter_music", "TRAINER_ENCOUNTER_MUSIC_MALE"))
    battle_type = sanitize_c_identifier(data.get("battle_type", "TRAINER_BATTLE_TYPE_SINGLES"))
    ai_flags = " | ".join([sanitize_c_identifier(f) for f in data.get("ai_flags", ["AI_FLAG_CHECK_BAD_MOVE"])])
    if not ai_flags: ai_flags = "0"
    
    items = [sanitize_c_identifier(i) for i in data.get("items", [])]
    items_str = ", ".join(items + ["ITEM_NONE"] * (4 - len(items)))
    
    party = data.get("party", [])
    party_str = "\n".join([format_trainer_mon(m) for m in party])
    
    block = [
        f"    [{difficulty}][{identifier}] =",
        "    {",
        f'        .trainerName = _("{name}"),',
        f"        .trainerClass = {t_class},",
        f"        .trainerPic = {t_pic},",
        f"        .gender = {gender},",
        f"        .encounterMusic = {music},",
        f"        .battleType = {battle_type},",
        f"        .aiFlags = {ai_flags},",
        f"        .items = {{{items_str}}},",
        f"        .partySize = {len(party)},",
        "        .party = (const struct TrainerMon[])",
        "        {",
        party_str,
        "        },",
        "    },"
    ]
    return "\n".join(block)

def main():
    json_files = glob.glob("data/trainers/**/*.json", recursive=True)
    all_trainers = []
    
    for fpath in json_files:
        with open(fpath, 'r') as f:
            data = json.load(f)
            all_trainers.append(data)
            
    # Sort for deterministic output
    all_trainers.sort(key=lambda x: x["identifier"])
    
    output_path = "src/data/trainers.h"
    with open(output_path, 'w') as out:
        out.write("//\n// DO NOT MODIFY THIS FILE! It is auto-generated from data/trainers/\n//\n\n")
        
        for trainer in all_trainers:
            tid = trainer["identifier"]
            
            # Check for Option A: complexities or root data
            if "difficulties" in trainer:
                for diff, diff_data in trainer["difficulties"].items():
                    # Merge root data with difficulty data
                    merged = trainer.copy()
                    merged.update(diff_data)
                    out.write(format_trainer_block(diff, tid, merged))
                    out.write("\n")
            else:
                # Default to NORMAL
                out.write(format_trainer_block("DIFFICULTY_NORMAL", tid, trainer))
                out.write("\n")

if __name__ == "__main__":
    main()
