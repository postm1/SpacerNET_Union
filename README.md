# SpacerNET_Union

## Table of contents

- [About](#about)
- [Download](#download)
- [Usage requirements](#usage-requirements)
- [Build requirements](#build-requirements)
- [Group-selection documentation](#group-selection-documentation)
- [Group selection and movement](#group-selection-and-movement)
- [Regression tests](#group-selection-regression-tests)
- [Contributing](#contributing)

## About

### Spacer.NET

**Spacer.NET** is a world editor for **Gothic II: Night of the Raven** that allows you to edit .ZEN world files.  
It's mainly inspired by the original **Spacer** editor, it offers almost the same functionallity from the original editor and some additional features. The main purpose of it is to give a better vobbing experience without many inconveniences of original editor.

### SpacerNET_Union

This project is a back-end plugin written in **cpp** for **Spacer.NET** editor using **Union SDK**.  
It's responsibility is to integrate editor features into the game and communicate with the interface.

The **Spacer.NET** contains also two additional files that can be only obtained from **SpacerNET.mod**:
- **zwe_controller.dll**  
    Contains code responsible for implementing alternative controller for camera movement / picking vobs.
- **SpacerNET.patch**  
    Contains memory patches required to run the editor.

#### Project details

- **NOTE!** Do not add new Union Files via Visual Studio 2015 UI, because it won't work.
- **NOTE!** This project uses custom `vdfs.h` and `vdfs32g.lib` that contain additional function `vdf_getvolumename`.
- Most of the hooks are located in **Spacer_Hooks.cpp**.
- Hooks using `HOOK .... AS` syntax should work for all gothic engines versions.
- Hooks using `CInvoke` will work only for G2A (they contain a specific address of function)
- `void SpacerApp::PluginLoop()` is a main loop of SpacerNET
- **Export.cpp** contains export files for using them in **C#**
- All of the communication between Union plugin and interface work via **Stack system**, like `Stack_PushInt` function.
- The project might contain some test/unused code.
- For printing use `print.PrintRed("Some text")`;

## Download

You can download the latest release of Spacer.NET from [worldofplayers.de](https://forum.worldofplayers.de/forum/threads/1557793-WORLD-EDITOR-Spacer-NET) forum.  
Or you can use Google drive: [Link](https://drive.google.com/drive/folders/1shwV7oFBP2vQfxZkBCw9KjScDlx-Wueq)

## Manual (SpacerNET lessons)
It is in Russian, but all the screenshots are English, so just use any browser translator

https://worldofplayers.ru/forums/1179/?order=title&direction=asc

## Usage requirements

In order to use this project you need'll to install:
- [Visual C++ Redistributable Package](https://aka.ms/vs/17/release/vc_redist.x86.exe)  

- **Union 1.0l or newer**  
    Install it via [Union Resource Manager](https://worldofplayers.ru/threads/41415/).

## Build requirements

- **Visual Studio 2015-2019**, with **Desktop development with C++**, the **v140_xp toolset**, and **Windows XP support (Windows SDK 7.1A)**.
- **Union SDK 1.0l**, installed via Union Resource Manager. The default location is `%ALLUSERSPROFILE%\Union\SDK\Union\v1.0l`.
- **DirectX 7 SDK**, normally installed alongside Union SDK at `%ALLUSERSPROFILE%\Union\SDK\DirectX7`.

Build from a Visual Studio Developer Command Prompt in the repository root:

```bat
msbuild SpacerNET_Union.sln /t:Build /p:Configuration="G2A Release" /p:Platform=x86
```

The Gothic II: Night of the Raven plugin is written to `Bin\SpacerUnionNet.dll`.
The build uses the repository's custom `vdfs\Vdfs32g.lib`; keep it ahead of the SDK library directory.
For SDKs installed elsewhere, override the `UnionSdkRoot` and `DirectX7SdkRoot` MSBuild properties.
The Union pre-build generator runs automatically. This configuration keeps the original 32-bit Windows XP target.
Group selection and transforms do not require v143 or Union SDK 1.0m.
The SDK path overrides are optional and do not change these defaults.

The C# interface is maintained separately in [SpacerNET_Interface](https://github.com/postm1/SpacerNET_Interface).
A successful DLL build still needs a Gothic installation with Union and the remaining Spacer.NET runtime files to run.

## Group-selection documentation

The paired development forks are [Bezimienny111/SpacerNET_Union](https://github.com/Bezimienny111/SpacerNET_Union)
and [Bezimienny111/SpacerNET_Interface](https://github.com/Bezimienny111/SpacerNET_Interface).
Build and distribute both feature DLLs together.

- [Native API and implementation contracts](docs/GROUP_SELECTION_API.md)
- [Validation results and manual checks](docs/VALIDATION.md)
- [Complete user guide](https://github.com/Bezimienny111/SpacerNET_Interface/blob/feature/group-selection/docs/GROUP_SELECTION.md)
- [Instrukcja po polsku](https://github.com/Bezimienny111/SpacerNET_Interface/blob/feature/group-selection/docs/GROUP_SELECTION.pl.md)
- [Prepared pull-request description](docs/PULL_REQUEST.md)
- [Publikacja dwóch PR-ów krok po kroku](docs/PUBLISHING.pl.md)

## Group selection and movement

Enable **Multi selection mode** on the main toolbar. A separate modeless **Group selection and transform** window opens after a world is loaded.
The render viewport keeps its full size. Closing the tool window exits group-selection mode.
The window follows the editor's theme and participates in hiding/showing tool windows.

The initial five group filters are:

- **All objects**: editable scene objects, including objects without visible models.
- **Without movers**: exclude movers and their entire descendant hierarchies.
- **Without WP and freepoints**: exclude waypoints and freepoints.
- **Items only**: oCItem objects and derived classes.
- **VOBs (no items or movers)**: any editable VOB except oCItem and zCMover, including other specialized VOB classes.

The list now has **22 filters**, covering both lists in **VOB List** and the selection filter in the object tree:
WP + freepoints, WP only, freepoints only, oCMOB, dynamic collision enabled, oCMobInter, lights, sounds,
ignore PFX, invisible objects, ignore decals, ignore decals and PFX, decals only, PFX only, zCTrigger, zCZone, and oCZoneMusic.
The old selection list labels its oCMobInter test as "Trigger"; the group window explicitly offers oCMobInter and zCTrigger separately.
"Invisible" includes objects with no visual or with showVisual disabled.

The active viewport camera, editor helpers, temporary objects marked not to be archived, and level-composition geometry are excluded from every filter.
Changing the filter immediately removes nonmatching objects from the existing selection.

Drag a rectangle to replace the selection. Hold **Shift** to add or **Alt** to subtract.
The rectangle tests object origins, rather than requiring the whole model to fit.
Enable **Through obstacles**, or hold **Ctrl** during selection, to include objects hidden by world geometry.
The same selection algorithm is used with DX7 and DX11.

The **Objects list window** can also build the same group selection, from either
the **Objects** tree or its **Fast access** tab:
**Ctrl+left click** or **Shift+left click** adds an object, **Alt+left click** removes it,
and an unmodified left click replaces the group with that object.
Adding an already selected object keeps it selected. Group filters still apply;
objects rejected by the filter leave the group unchanged. Tree selection ignores camera visibility and obstacles.
Selected nodes follow selection changes in the viewport, including clearing and filter changes.
Folders and expand/collapse buttons navigate the tree without changing the group.
Sorting or rebuilding tree nodes preserves selection by object address.
These controls require Multi selection mode and are disabled in play mode.
Outside group mode, the tree retains its usual single-object selection behavior.
The separate **VOB List** search window retains its original behavior.

Returning from a tool window with a left click activates the viewport without changing the group.
That activating gesture is ignored until the button is released; the next drag selects normally.
A click without a rectangle also keeps the selection. A deliberate empty rectangle still clears it.
The group tool window includes a visible **Controls** section for mouse selection, the object tree,
camera movement, mode-specific keyboard transforms and undo. The descriptions follow the interface language.

Move the group with **WASD**, **Space/X**, **Shift** (faster), and **Ctrl** (slower), using the configured movement bindings.
Hold the right mouse button to move the camera instead.
Choose **Translation** or **Rotation** in the tool window, or use the configured tool shortcuts (default **1** / **2**) in the viewport.
In translation mode, enter **delta X/Y/Z** and click **Move selection**.
In rotation mode, enter world-axis angles in degrees and click **Rotate selection**.
The numeric rotation order is X, then Y, then Z.
Rotation uses the shared arithmetic center of all selected object origins; W/S rotate about X, A/D about Z, Q/E about Y.
The configured rotation speed, faster/slower modifiers, hierarchy protection and undo also apply.
Every selected object receives the same world-space offset, preserving relative positions.
A selected child is moved once even if its parent is selected. Unselected descendants retain their original world-space transforms.

Waypoint network positions, camera splines, and PFX bounds are refreshed.
Moving a selected mover also offsets all its saved keyframe positions. Rotating it rotates its keyframe positions about the group pivot and composes its keyframe orientations.
The existing option that blocks movement of objects with children is respected.

**Undo operation** or **Ctrl+Z** restores the complete translation or rotation, including object orientations and mover keyframes.
One uninterrupted keyboard movement is one operation; one numeric move is another.
History stores up to 32 group operations, and is cleared when leaving group mode or deleting objects.
Undo refuses changed worlds, changed hierarchies, or changed mover keyframe counts.
Parent-cut in group mode uses **Ctrl+X**; **Ctrl+V** applies the chosen parent.

The group window uses its own transformation implementation with either controller setting.
Group movement by dragging an external-controller gizmo is not part of this implementation.

## Group-selection regression tests

From an **x86 Visual Studio Developer Command Prompt**, run:

    tests\run-multiselection-tests.cmd

The tests compile the actual MultiSelect.cpp against an engine double that propagates parent transforms to children.
They cover filters, occlusion, DX7/DX11 candidate parity, mixed hierarchies, selected-only translation and rotation, mover keyframe positions and orientations, collision flags, keyboard gestures, bounded undo, deletion, changed worlds, and rejected input.
Twenty deterministic generated hierarchies exercise another 1,200 objects.
Objects-list selection cases also cover additive/removal/replacement operations, duplicate clicks,
stale addresses, filters, editor-state guards, and transform/undo integration without viewport focus.
Focus-return cases cover activation clicks, delayed input, held buttons, camera activation,
cancelled drags, zero-area clicks and normal rectangle selection after returning.
These tests do not replace an in-game check with the actual engine and renderer.

## Contributing

Any contribution to this repository is welcome.  
In order to add your own changes to the project, just open a pull request.
