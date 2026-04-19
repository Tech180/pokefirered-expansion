import os
import json
import glob

def build_species_info():
    base_dir = 'data/pokemon/species_info'
    output_base = 'src/data/pokemon/species_info'
    
    # Process each generation based on its manifest
    for gen in range(1, 10):
        manifest_path = os.path.join(base_dir, f'gen_{gen}', 'manifest.json')
        if not os.path.exists(manifest_path):
            continue
            
        with open(manifest_path, 'r') as f:
            manifest = json.load(f)
            
        lines = []
        lines.append("#ifdef __INTELLISENSE__")
        lines.append(f"const struct SpeciesInfo gSpeciesInfoGen{gen}[] =")
        lines.append("{")
        lines.append("#endif")
        lines.append("")

        for item in manifest:
            if item["type"] == "raw":
                lines.append(item["content"])
                lines.append("")
            elif item["type"] == "family":
                lines.append(f"#if {item['family_constant']}")
                
                for component in item["components"]:
                    if component["type"] == "raw":
                        lines.append(component["content"])
                        lines.append("")
                    elif component["type"] == "species":
                        # Load species JSON
                        species_path = os.path.join(base_dir, f"gen_{component['gen']}", component["filename"])
                        if not os.path.exists(species_path):
                            print(f"Warning: Missing species file {species_path}")
                            continue
                            
                        with open(species_path, 'r') as sf:
                            spec = json.load(sf)
                            
                        # Output species initializer
                        lines.append(f"    [{spec['species_constant']}] =")
                        if "macro_call" in spec:
                            lines.append(f"        {spec['macro_call']},")
                        else:
                            lines.append("    {")
                            for entry in spec.get("entries", []):
                                if entry["type"] == "preprocessor":
                                    lines.append(f"    {entry['line']}")
                                elif entry["type"] == "field":
                                    lines.append(f"        .{entry['name']} = {entry['value']},")
                                elif entry["type"] == "macro":
                                    args = entry["args"].strip()
                                    comma = "," if entry.get("has_comma") else ""
                                    lines.append(f"        {entry['name']}({args}){comma}")
                            lines.append("    },")
                            
                lines.append(f"#endif // {item['family_constant']}")
                lines.append("")

        lines.append("#ifdef __INTELLISENSE__")
        lines.append("};")
        lines.append("#endif")

        output_file = os.path.join(output_base, f'gen_{gen}_families.h')
        with open(output_file, 'w') as f:
            f.write("\n".join(lines))
        print(f"Generated {output_file}")

if __name__ == "__main__":
    build_species_info()
