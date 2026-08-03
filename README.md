# Unreal Groom Exporter

Unreal Groom Exporter is an Unreal Engine 5.8 plugin that exports native Groom Assets to a format that can be imported directly into Blender as Hair Curves.

The plugin exports two files:

- `.obj` — Hair strand vertex positions
- `.json` — Curve metadata including strand layout and root UV coordinates

These files can be imported using the Unreal Groom Importer Blender Extension.

---

## Features

- Right click export directly from the Content Browser
- Native Windows Save File dialog
- Exports all hair strands
- Preserves curve topology
- Preserves root UV coordinates
- Exports multiple hair groups
- Generates accompanying JSON metadata automatically
- Compatible with the Unreal Groom Blender Importer Extension

---

## Requirements

- Unreal Engine 5.8
- HairStrands plugin enabled
- Alembic Importer plugin enabled

---

## Installation

1. Copy the `UE58GroomExporter` plugin into your project's `Plugins` folder.

2. Open the project in Unreal Engine.

3. Enable the plugin if it is not already enabled.

4. Restart Unreal Engine.

---

## Quick Start

1. Open the Content Browser.

2. Right click a Groom Asset.

3. Select **Export Groom to Blender**.

4. Choose the export location.

The exporter creates two files:

```
Hair_S_Pixie.obj
Hair_S_Pixie.json
```

These files can now be imported using the Blender 
