# Unreal Groom Exporter
## Developer Notes

This document provides an overview of the Unreal Groom Exporter plugin architecture, source files, data flow, and implementation details. It is intended for developers who want to understand, maintain, or extend the exporter.

---

# Project Structure

```
UE58GroomExporter/
│
├── Source/
│   └── UE58GroomExporter/
│       ├── Public/
│       │   ├── GroomExporterLibrary.h
│       │   ├── UE58GroomExporter.h
│       │   └── GroomContextMenu.h
│       │
│       ├── Private/
│       │   ├── GroomExporterLibrary.cpp
│       │   ├── UE58GroomExporter.cpp
│       │   └── GroomContextMenu.cpp
│       │
│       └── UE58GroomExporter.Build.cs
│
└── UE58GroomExporter.uplugin
```

---

# Overview

The exporter consists of three major components.

```
Content Browser
       │
       ▼
Right Click Menu
       │
       ▼
Export Command
       │
       ▼
Save File Dialog
       │
       ▼
ExportGroomToOBJ()
       │
       ├── OBJ
       └── JSON
```

The context menu only handles user interaction.

All Groom processing and file generation is performed inside `ExportGroomToOBJ()`.

---

# Source Files

## UE58GroomExporter.cpp

Module entry point.

Responsibilities

- Module startup
- Module shutdown
- Register Content Browser menu
- Handle menu callbacks

This file should remain lightweight.

No export logic should be implemented here.

---

## GroomContextMenu.cpp

Responsible for integrating with Unreal's Content Browser.

Responsibilities

- Register the right click menu
- Add "Export Groom to Blender"
- Display the Save File dialog
- Load the selected Groom Asset
- Call the exporter

No file writing should occur here.

---

## GroomExporterLibrary.cpp

Core exporter implementation.

Responsibilities

- Read Groom data
- Generate OBJ
- Generate JSON
- Save files

All future export functionality should be implemented here.

---

# Export Pipeline

```
User selects Groom
        │
        ▼
Right Click
        │
        ▼
Export Groom to Blender
        │
        ▼
Save File Dialog
        │
        ▼
Load Groom Asset
        │
        ▼
Read Hair Groups
        │
        ▼
Read Curves
        │
        ▼
Read Points
        │
        ▼
Write OBJ
        │
        ▼
Write JSON
```

---

# Groom Data

Each Groom Asset contains one or more Hair Groups.

```
Groom
 ├── Group 0
 ├── Group 1
 └── Group N
```

Each group contains

- Curves
- Points

These are exposed through

```
FHairStrandsDatas
```

The exporter processes every Hair Group independently.

---

# Curves

Each curve stores

```
CurvesOffset
CurvesCount
CurvesRootUV
```

Example

```
Curve 0

Offset = 0
Count = 51

Curve 1

Offset = 51
Count = 11
```

Offsets reference the shared point array.

---

# Points

All point positions are stored inside one continuous array.

```
PointsPosition

0
1
2
3
4
5
...
```

The exporter reconstructs each strand using

```
Offset
Count
```

No curve owns its own vertex array.

---

# OBJ Export

Only two OBJ record types are written.

Vertex

```
v x y z
```

Polyline

```
l 1 2 3 4 5
```

The OBJ intentionally contains no

- normals
- texture coordinates
- faces
- materials

The OBJ is used only as a compact point container.

---

# JSON Export

Each exported strand produces one JSON entry.

Example

```json
{
    "first": 108,
    "count": 46,
    "group": 0,
    "uv": [0.604328930, 0.957199752]
}
```

Fields

| Field | Description |
|--------|-------------|
| first | First OBJ vertex index |
| count | Number of vertices in strand |
| group | Hair Group index |
| uv | Root UV coordinate |

The JSON intentionally contains only data unavailable in OBJ.

---

# Root UV

Root UVs are read from

```
CurvesRootUV
```

One UV exists per curve.

The exporter does not generate UVs.

They are copied directly from the Groom Asset.

---

# Coordinate System

No coordinate conversion is performed.

The exporter writes Unreal coordinates exactly as stored.

Coordinate conversion is handled by the Blender importer.

This keeps the exporter simple and avoids maintaining multiple coordinate conventions.

---

# Save File Dialog

The exporter uses Unreal's desktop file dialog.

Workflow

```
User selects destination

↓

Hair.obj

↓

Hair.json
```

The JSON is automatically written beside the OBJ.

---

# Error Handling

The exporter validates

- Groom Asset exists
- Hair Group can be read
- Files can be written

If any step fails

- export is cancelled
- an error is logged
- no partial files are produced

---

# Dependencies

Runtime

- HairStrandsCore

Editor

- HairStrands
- ToolMenus
- DesktopPlatform
- ContentBrowser

Optional

- AlembicImporter

---

# Current Limitations

Currently exported

- Curve topology
- Point positions
- Hair Groups
- Root UV

Currently not exported

- Width
- Color
- Radius
- Vertex attributes
- Guide information
- Groom physics settings
- Materials
- LOD information

---

# Design Philosophy

The exporter intentionally performs only one task:

Convert a Groom Asset into a lightweight interchange format.

It does not attempt to recreate Blender data structures or perform coordinate conversion. Those responsibilities belong entirely to the Blender importer.

Keeping the exporter focused makes it easier to maintain and allows the exported files to be used by other applications in the future.

---

# Future Improvements

Potential future additions

- Progress bar during export
- Batch export of multiple Groom Assets
- Export selected Hair Groups only
- Width attribute export
- Vertex color export
- Curve IDs
- Custom metadata
- Additional export formats
- Export settings dialog
- Automatic export location memory

---
