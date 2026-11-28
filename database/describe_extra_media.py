from pathlib import Path
import os
import sqlite3
import xml.etree.ElementTree as ET

DB_DIR = Path(__file__).resolve().parent
REPO_DIR = DB_DIR.parent
OUTPUT_DIR = DB_DIR / "descriptions"

MPEG7 = "urn:mpeg:mpeg7:schema:2001"
XSI = "http://www.w3.org/2001/XMLSchema-instance"

ET.register_namespace("", MPEG7)
ET.register_namespace("xsi", XSI)

descriptions = {
    "Gentle Rain": (
        "gentle-rain",
        "An outdoor recording of gentle rainfall by rasunter255. "
        "Topic: nature."
    ),
    "Water Lily": (
        "water-lily",
        "Water Lily, an instrumental composition by Kevin MacLeod "
        "featuring piano and strings. Topic: music."
    ),
    "Jellyfish": (
        "jellyfish",
        "A close-up video of a jellyfish swimming underwater. "
        "Topic: animals. No audio track."
    ),
    "Forest Waterfall": (
        "forest-waterfall",
        "An aerial video of a forest and waterfall. "
        "Topic: nature. No audio track."
    ),
    "Solar System": (
        "solar-system",
        "An animated rendering of planets in the solar system. "
        "Topic: space. Contains an audio track."
    )
}


def child(parent, name, attributes=None):
    return ET.SubElement(
        parent, f"{{{MPEG7}}}{name}", attributes or {}
    )


def sql_text(value):
    return "'" + value.replace("'", "''") + "'"


OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

with sqlite3.connect(DB_DIR / "media.db") as db:
    rows = db.execute(
        "SELECT title, media_type, file_path FROM media"
    ).fetchall()

records = {}
for title, media_type, file_path in rows:
    if title in descriptions:
        if title in records:
            raise RuntimeError(f"Duplicate title: {title}")
        records[title] = (media_type, file_path)

# Check all required records and files before generating anything.
for title in descriptions:
    if title not in records:
        raise RuntimeError(f"Missing database record: {title}")
    path = REPO_DIR / records[title][1]
    if not path.is_file():
        raise FileNotFoundError(f"Missing media file: {path}")

updates = ["BEGIN TRANSACTION;"]

for title, (identifier, annotation) in descriptions.items():
    media_type, file_path = records[title]
    element_name = {"music": "Audio", "video": "Video"}[media_type]
    xml_path = OUTPUT_DIR / f"{identifier}.xml"

    if not xml_path.exists():
        root = ET.Element(f"{{{MPEG7}}}Mpeg7")
        description = child(
            root, "Description",
            {f"{{{XSI}}}type": "ContentEntityType"}
        )
        content = child(
            description, "MultimediaContent",
            {f"{{{XSI}}}type": f"{element_name}Type"}
        )
        item = child(content, element_name, {"id": identifier})

        locator = child(item, "MediaLocator")
        child(locator, "MediaUri").text = Path(
            os.path.relpath(REPO_DIR / file_path, OUTPUT_DIR)
        ).as_posix()

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

    relative_xml = xml_path.relative_to(REPO_DIR).as_posix()
    updates.append(
        f"UPDATE media SET description_path = {sql_text(relative_xml)} "
        f"WHERE file_path = {sql_text(file_path)};"
    )
    print(f"Prepared description: {title}")

updates.append("COMMIT;")
(DB_DIR / "seed_extra_descriptions.sql").write_text(
    "\n".join(updates), encoding="UTF-8"
)