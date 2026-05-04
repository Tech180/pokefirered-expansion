with open("/home/tech180/.gemini/antigravity/brain/55123f0f-c608-404c-8a60-c568b52c4f7a/task.md", "r") as f:
    text = f.read()

text = text.replace("[/] Resolve Compilation Constants", "[x] Resolve Compilation Constants")
text = text.replace("[ ] Resolve remaining compilation errors in `new_shop.c`", "[x] Resolve remaining compilation errors in `new_shop.c`")
text = text.replace("[ ] Verify Shop GUI Functions In-Game", "[x] Verify Shop GUI Functions In-Game")

with open("/home/tech180/.gemini/antigravity/brain/55123f0f-c608-404c-8a60-c568b52c4f7a/task.md", "w") as f:
    f.write(text)
