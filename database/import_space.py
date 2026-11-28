from pathlib import Path
import re
import xml.etree.ElementTree as ET

DB_DIR = Path(__file__).resolve().parent
REPO_DIR = DB_DIR.parent
IMAGE_DIR = DB_DIR / "media/images/space"
XML_DIR = DB_DIR / "descriptions/space"

MPEG7 = "urn:mpeg:mpeg7:schema:2001"
XSI = "http://www.w3.org/2001/XMLSchema-instance"

ET.register_namespace("", MPEG7)
ET.register_namespace("xsi", XSI)


def child(parent, name, attributes=None):
    return ET.SubElement(
        parent, f"{{{MPEG7}}}{name}", attributes or {}
    )


def sql_text(value):
    return "'" + value.replace("'", "''") + "'"


if not IMAGE_DIR.is_dir():
    raise FileNotFoundError(f"Missing folder: {IMAGE_DIR}")

XML_DIR.mkdir(parents=True, exist_ok=True)
statements = ["BEGIN TRANSACTION;"]
count = 0

for image in sorted(IMAGE_DIR.glob("*.png")):
    planet = re.fullmatch(r"planet(\d{2})", image.stem)
    player = re.fullmatch(
        r"playerShip([1-3])_(blue|green|orange|red)", image.stem
    )
    other_ship = re.fullmatch(
        r"enemy(Black|Blue|Green|Red)([1-5])", image.stem
    )

    if planet:
        title = f"Fantasy Planet {planet.group(1)}"
        annotation = (
            "A colorful illustrated fantasy planet from Kenney's "
            "Planets pack. Topic: space."
        )
        source = "https://kenney.nl/assets/planets"

    elif player:
        style, color = player.groups()
        title = f"{color.title()} Spaceship Style {style}"
        annotation = (
            f"A {color} cartoon spaceship, style {style}, from "
            "Kenney's Space Shooter Remastered pack. Topic: space."
        )
        source = "https://kenney.nl/assets/space-shooter-remastered"

    elif other_ship:
        color, style = other_ship.groups()
        title = f"{color} Spacecraft Design {style}"
        annotation = (
            f"A {color.lower()} cartoon spacecraft, design {style}, "
            "from Kenney's Space Shooter Remastered pack. Topic: space."
        )
        source = "https://kenney.nl/assets/space-shooter-remastered"

    else:
        print(f"Skipped unrecognized file: {image.name}")
        continue

    xml_path = XML_DIR / f"{image.stem}.xml"

    # Preserve any descriptions you have already edited.
    if not xml_path.exists():
        root = ET.Element(f"{{{MPEG7}}}Mpeg7")
        description = child(
            root, "Description",
            {f"{{{XSI}}}type": "ContentEntityType"}
        )
        content = child(
            description, "MultimediaContent",
            {f"{{{XSI}}}type": "ImageType"}
        )
        item = child(
            content, "Image", {"id": f"space-{image.stem}"}
        )
        locator = child(item, "MediaLocator")
        child(locator, "MediaUri").text = (
            f"../../media/images/space/{image.name}"
        )
        text = child(item, "TextAnnotation")
        child(
            text, "FreeTextAnnotation",
            {"{http://www.w3.org/XML/1998/namespace}lang": "en"}
        ).text = annotation

        tree = ET.ElementTree(root)
        ET.indent(tree, space="    ")
        tree.write(
            xml_path, encoding="UTF-8", xml_declaration=True
        )

    values = [
        title, "images", "space",
        image.relative_to(REPO_DIR).as_posix(),
        "Kenney",
        source,
        "CC0-1.0",
        xml_path.relative_to(REPO_DIR).as_posix()
    ]
    title_sql, type_sql, category_sql, path_sql, creator_sql, \
        source_sql, license_sql, xml_sql = map(sql_text, values)

    statements.append(f"""
INSERT INTO media (
    title, media_type, category, min_age, max_age,
    file_path, creator, source_url, license,
    has_audio, description_path
)
VALUES (
    {title_sql}, {type_sql}, {category_sql}, 0, 12,
    {path_sql}, {creator_sql}, {source_sql}, {license_sql},
    0, {xml_sql}
)
ON CONFLICT(file_path) DO UPDATE SET
    description_path = excluded.description_path;
""")
    count += 1
    print(f"Prepared: {title}")

if count == 0:
    raise RuntimeError("No recognized space images found.")

statements.append("COMMIT;")
(DB_DIR / "seed_space.sql").write_text(
    "\n".join(statements), encoding="UTF-8"
)

print(f"\nPrepared {count} records in database/seed_space.sql")