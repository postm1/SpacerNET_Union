# Group-selection validation

Last automated validation: **2026-11-27**.

## Confirmed automatically

| Check | Result | Evidence / scope |
| --- | --- | --- |
| Native release build | Passed | G2A Release x86; MSVC 19.00.24247.2, v140_xp, Windows SDK 7.1A, Union SDK 1.0l |
| Managed release build | Passed | Release x86 against .NET Framework 4.0 reference assemblies; existing source language checked with C# 7.3 |
| Native target metadata | Passed | x86 PE; OS and subsystem versions 5.01 |
| Managed target metadata | Passed | `.NETFramework,Version=v4.0` |
| Native regression suite | 119 scenarios passed | Actual `MultiSelect.cpp`, compiled with v140, against a deterministic engine double |
| Managed/interface suite | 70 checks passed | Real forms, native test DLL compiled with v140, and a test executable compiled against .NET 4.0 references |

The workstation has a newer installed runtime. A .NET 4.0 compilation target and XP-compatible PE headers do not constitute execution on original CLR 4.0 or Windows XP.
The native build used the workstation's installed Universal CRT headers/libraries alongside the original Windows SDK 7.1A and the v140 compiler.

Native coverage includes all 22 filters, occlusion, identical DX7/DX11 candidate enumeration,
selected/unselected hierarchies, positions and orientations, mover keyframes, collision restoration,
waypoint/camera/PFX updates, continuous keyboard gestures, bounded undo, deletion, stale worlds and rejected input.
Twenty deterministic generated hierarchies exercise 1,200 additional objects.
Focus tests cover Objects-list callbacks between render frames, first-click activation, held buttons,
cancelled drags, RMB activation, zero-area clicks and normal rectangle selection afterward.

Interface coverage includes localization/value preservation, numeric limits, count/undo state,
play-mode disabling, owned-window lifecycle, unchanged viewport size and visible control instructions.
Real queued Windows mouse messages exercise Ctrl/Shift selection in Objects and Fast access,
folder expanders, duplicates, membership synchronization, sorting/rebuilding and native single-selection behavior outside group mode.
The separate VOB List control remains the original single-selection control.

## Reproduce

Build using the requirements and commands in each repository's README, then run from an x86 developer prompt:

```bat
rem In SpacerNET_Union:
tests\run-multiselection-tests.cmd

rem In SpacerNET_Interface, after its Release x86 build:
tests\run-interface-selection-tests.cmd
```

The interface test requires .NET Framework 4.0 reference assemblies. For a nonstandard targeting-pack location:

```bat
set "NET40_REFERENCE_PATH=C:\path\to\v4.0"
tests\run-interface-selection-tests.cmd
```

Tests do not require Gothic. The native suite substitutes engine behavior; the interface suite substitutes native API behavior.
They test the two sides separately rather than loading the actual Gothic backend into the interface test process.
Preview files `group-selection-window.png` and `objects-list-group-selection.png` are written under the interface's `bin\tests` directory.

The original development workspace also has a local `python .compat/build_legacy.py all` helper.
That helper and its downloaded toolchain live outside both repositories and are not prerequisites for contributors.

## Manual release checks

The user reported the feature working in-game before the final rebuild with the legacy toolchain.
The final legacy-built DLL pair has not yet been confirmed in-game or on Windows XP.
The following cases remain release checks on the actual engine and renderers:

1. Install both matching DLLs, load a representative world and open group mode. Verify viewport size, theme changes, tool-window hiding and closing/reopening.
2. In Objects and Fast access, add objects with Ctrl/Shift, remove with Alt and return to the viewport with an ordinary LMB click. Verify that all members remain selected.
3. Select by rectangle in DX7 and DX11: replace, add, subtract, behind geometry, invisible objects, waypoints and freepoints. Check a zero-area click and an intentionally empty rectangle.
4. Move and rotate a mixed group numerically and with configured keys. Check Shift/Ctrl speed, RMB camera control and one-entry undo for a held-key gesture.
5. Include a parent and child, unselected descendants and a mover with multiple position/orientation keyframes. Verify each selected object transforms once and unselected descendants/routes stay fixed.
6. Check waypoint network updates, camera splines, PFX bounds and the option that blocks objects with children.
7. Change filter, remove an object, reparent, unload/reload a world and enter play mode. Verify stale operations are rejected and editing is blocked where expected.
8. Save/reload the edited world and verify transforms and mover keyframes persist.
9. If Windows XP is a release requirement, execute the matching pair on XP with its supported .NET/VC runtime and Union installation.

Passing an engine double does not establish actual renderer integration, game ABI behavior, saved-world behavior or Windows XP runtime support.
