from io import BytesIO
from pathlib import Path
import re
from PIL import Image

source = Path("Averix.h").read_text(encoding="utf-8")

match = re.search(
    r"averix_indicator_image\s*\[\s*\]\s*=\s*\{(.*?)\};",
    source,
    re.S
)

if not match:
    raise SystemExit("Di mahanap ung image array sa Averix.h")

image_bytes = bytes(
    int(value, 16)
    for value in re.findall(r"0x[0-9a-fA-F]{2}", match.group(1))
)

image = Image.open(BytesIO(image_bytes)).convert("RGB")
image.save("logo_fixed.jpg", "JPEG", quality=90, progressive=False)

data = Path("logo_fixed.jpg").read_bytes()

with open("LogoNew.h", "w", encoding="utf-8") as output:
    output.write("#pragma once\n#include <stddef.h>\n\n")
    output.write("static const unsigned char LogoNew_data[] = {\n")

    for i in range(0, len(data), 16):
        chunk = data[i:i + 16]
        values = ", ".join(f"0x{byte:02X}" for byte in chunk)
        output.write(f"    {values},\n")

    output.write("};\n")

print("LogoNew.h generated")
