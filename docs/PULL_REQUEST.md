# Add safe group translation, rotation and Objects tree selection

Multi-selection now supports shared-center rotation and world-space translation, from keyboard gestures or numeric UI input.
Transforms preserve relative positions, apply selected parent/child transforms once, retain unselected descendant world transforms
and update selected mover keyframe positions/orientations. Waypoint positions, camera splines and PFX bounds are refreshed.

The selector adds 22 group filters, consistent DX7/DX11 candidate enumeration, 32-entry group-transform undo,
and an API for selection changes from Objects/Fast access. Selection changes validate current-world addresses before dereferencing them.
Focus-acquisition clicks and zero-area clicks preserve the group; subsequent deliberate rectangles retain normal replace/add/subtract behavior.
World/hierarchy/keyframe validation and deletion hooks invalidate stale snapshots.

This change requires the corresponding interface implementation in
[Bezimienny111/SpacerNET_Interface](https://github.com/Bezimienny111/SpacerNET_Interface).
The paired interface supplies the group window, tree highlighting and viewport-input callback.
Build and distribute both DLLs together.

The project keeps **v140_xp**, **Union SDK 1.0l** and the original x86 target.
An explicit `Kind` constructor supports the VS 2015 compiler.
SDK paths can be overridden with `UnionSdkRoot` and `DirectX7SdkRoot`; their default versions remain unchanged.

## Validation

- G2A Release x86 build passed with MSVC 19.00.24247.2, Windows SDK 7.1A and Union SDK 1.0l.
- The resulting PE targets x86 with OS/subsystem versions 5.01.
- 119 regression scenarios passed on the v140 compiler, compiling the actual selector against an engine double.
- Generated hierarchy cases exercise 1,200 additional objects; coverage includes mixed hierarchies, mover rotations, undo, filters, stale input and viewport focus transitions.
- Companion interface: .NET Framework 4.0 build and 70 interface/interop checks passed.
- Actual-engine/DX7/DX11, save/reload and Windows XP execution of the rebuilt legacy pair remain release checks.

The scope is group editing. It does not add scaling, an external-controller group gizmo or general deletion/reparent undo.
The max-FPS fix is already part of the upstream base and is not introduced by this change.

See [API contracts](GROUP_SELECTION_API.md) and [validation details](VALIDATION.md).
