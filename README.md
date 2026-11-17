# Unreal Groom Exporter

Unreal Groom Exporter is an Unreal Engine 5.8 plugin that exports native Groom Assets to a format that can be imported directly into Blender as Hair Curves.

The plugin exports two files:

- `.obj` — Hair strand vertex positions
- `.json` — Curve metadata, root UVs, per-point radii, and colors when available

These files can be imported using the Unreal Groom Importer Blender Extension.

---

## Features

- Right click export directly from the Content Browser
- Native Windows Save File dialog
- Exports all hair strands
- Preserves curve topology
- Preserves root UV coordinates
- Preserves per-point strand radii and colors in the JSON sidecar when present in the Groom
- Exports multiple hair groups
- Generates accompanying JSON metadata automatically
- Compatible with the Unreal Groom Blender Importer Extension
- Imports Blender Alembic grooms with their JSON group manifest
- Keeps Unreal's native `.abc` import unchanged; Blender groom import is a separate menu action

---

## Requirements

- Unreal Engine 5.8
- HairStrands plugin enabled
- Alembic Importer plugin enabled
- Alembic Groom Importer plugin enabled (enabled by this plugin dependency)

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

Import both files using the Unreal Groom Importer Blender extension.

## Import a Blender Groom into Unreal

1. Keep the `.abc` exported by the Blender extension beside its same-name `.json` manifest.
2. In Unreal Editor, choose **File → Import/Export → Import Blender Groom (.abc + .json)**.
3. Select the `.abc`. The plugin reads the companion manifest, validates curve order and point counts, and restores each strand's group ID and root UV plus per-point radius and color data when present.
4. The asset is created in the current Content Browser folder (or `/Game` if no project folder is active). Save the package after import.

The Blender exporter already bakes its configured scale and rotation into the Alembic geometry. This manual import action uses identity conversion to avoid applying those transforms a second time. This is an explicit plugin action; it does not replace or intercept Unreal's normal Alembic import.
