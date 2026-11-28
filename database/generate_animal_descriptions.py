from pathlib import Path
import xml.etree.ElementTree as ET

DATABASE_DIR = Path(__file__).resolve().parent
OUTPUT_DIR = DATABASE_DIR / "descriptions"
IMAGE_DIR = DATABASE_DIR / "media" / "images" / "animals"

MPEG7 = "urn:mpeg:mpeg7:schema:2001"
XSI = "http://www.w3.org/2001/XMLSchema-instance"
XML = "http://www.w3.org/XML/1998/namespace"

ET.register_namespace("", MPEG7)
ET.register_namespace("xsi", XSI)

animals = [
    "giraffe", "hippo", "monkey", "panda", "parrot",
    "penguin", "pig", "rabbit", "snake"
]


def element(parent, name, attributes=None):
    return ET.SubElement(
        parent, f"{{{MPEG7}}}{name}", attributes or {}
    )


OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

for animal in animals:
    image_path = IMAGE_DIR / f"{animal}.png"
    output_path = OUTPUT_DIR / f"{animal}.xml"

    if not image_path.is_file():
        raise FileNotFoundError(f"Missing image: {image_path}")

    if output_path.exists():
        print(f"Kept existing: {output_path.name}")
        continue

    root = ET.Element(f"{{{MPEG7}}}Mpeg7")

    description = element(
        root, "Description",
        {f"{{{XSI}}}type": "ContentEntityType"}
    )
    content = element(
        description, "MultimediaContent",
        {f"{{{XSI}}}type": "ImageType"}
    )
    image = element(content, "Image", {"id": animal})

    locator = element(image, "MediaLocator")
    element(locator, "MediaUri").text = (
        f"../media/images/animals/{animal}.png"
    )

    annotation = element(image, "TextAnnotation")
    element(
        annotation, "FreeTextAnnotation",
        {f"{{{XML}}}lang": "en"}
    ).text = (
        f"A cartoon {animal} illustration from Kenney's "
        "Animal Pack. Topic: animals."
    )

    tree = ET.ElementTree(root)
    ET.indent(tree, space="    ")
    tree.write(output_path, encoding="UTF-8", xml_declaration=True)

    print(f"Created: {output_path.name}")