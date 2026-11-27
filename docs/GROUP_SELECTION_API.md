# Group-selection API and implementation

[User guide](https://github.com/Bezimienny111/SpacerNET_Interface/blob/feature/group-selection/docs/GROUP_SELECTION.md) · [Polski](https://github.com/Bezimienny111/SpacerNET_Interface/blob/feature/group-selection/docs/GROUP_SELECTION.pl.md) · [Validation](VALIDATION.md)

## Binary contract

The feature spans two x86 DLLs: `SpacerUnionNet.dll` exports native operations and
`SpacerUnionInterface.dll` supplies the group window, Objects tree integration and a viewport-input callback.
Use a matching pair. This feature does not introduce ABI negotiation with older interface DLLs.

The functions below use `cdecl`, 32-bit `int` values, 32-bit `float` values and 32-bit unsigned VOB addresses.
An address belongs to the current in-process world; it is not an object ID that survives deletion or world reload.
Invoke operations through the editor's serialized UI/game execution, not background worker threads.
The API does not add concurrent access protection.

Source locations:

- [Native exports](../SpacerNET_Union/Export.cpp)
- [Selector declarations and method contracts](../SpacerNET_Union/MultiSelect.h)
- [Selection, focus state, transforms and undo](../SpacerNET_Union/MultiSelect.cpp)
- [Engine-independent filtering and rotation rules](../SpacerNET_Union/MultiSelectionRules.hpp)
- [Managed declarations](https://github.com/Bezimienny111/SpacerNET_Interface/blob/feature/group-selection/SpacerNET_Interface/Common/Imports.cs)

## Native exports

| Function | Inputs | Result / behavior |
| --- | --- | --- |
| `Extern_GetMultiSelectionCount()` | None | Current count after stale/filter pruning; 0 outside group mode or without a world |
| `Extern_SetMultiSelectionFilter(int filter)` | Group filter ID 0..21 | Persist filter, finish active gesture, cancel rectangle, prune membership; invalid IDs are ignored |
| `Extern_ChangeMultiSelection(uint vob, int action)` | Current-world address; action 0 replace, 1 add, 2 remove | Group count on success, -1 on rejected state/address/action/filter |
| `Extern_GetMultiSelectionVobs(uint* vobs, int capacity)` | Caller-owned output buffer; null/0 for size query | Total group size, writing at most `capacity` addresses; 0 outside group mode or without a world |
| `Extern_SetMultiSelectionThroughWalls(int enabled)` | 0 off, nonzero on | Persist geometry-occlusion option and cancel the active rectangle |
| `Extern_SetMultiSelectionTransformMode(int mode)` | 0 translation, 1 rotation | Persist mode, finish active gesture and cancel rectangle; invalid modes are ignored |
| `Extern_MoveSelectedVobs(float x, float y, float z)` | World-axis offsets | Transform result described below; each call is a discrete undo operation |
| `Extern_RotateSelectedVobs(float x, float y, float z)` | World-axis angles in degrees | Transform result described below; applies X then Y then Z about the shared pivot |
| `Extern_CanUndoSelectionMove()` | None | 1 when a committed entry or changed active gesture exists, otherwise 0; this is an availability hint |
| `Extern_UndoSelectionMove()` | None | 1 restored, 0 empty, -1 invalid state/world/hierarchy/keyframes; covers translation and rotation despite the legacy name |
| `Extern_ClearMultiSelection()` | None | Finish gesture, cancel rectangle and clear membership; valid transform history is retained |
| `Extern_ReparentSelectedVobs(uint parent)` | Current-world parent address | Existing reparent operation: number of reparented objects, or 0 when rejected/no change; not part of transform undo |

`ChangeMultiSelection` checks world membership before dereferencing a received pointer.
Replacing or adding a filtered-out object is rejected before clearing the existing group.
Adding an existing member and removing a nonmember are idempotent.
Removing does not require the target to match the current filter.
Selection editing requires group mode, a loaded world, no exit, no play mode and no camera playback.
It intentionally does not require viewport focus, because Objects list is an input source.

### Transform results

| Code | Meaning |
| --- | --- |
| Positive | Number of selected members affected by the group operation |
| 0 | No operation: zero input or no capturable group |
| -1 | Editor/world/mode/playback state is invalid, or an existing snapshot no longer matches the world/hierarchy/keyframe counts |
| -2 | `checkBoxBlockComplexVob` prohibits transforming a selected object with children |
| -3 | Non-finite/out-of-range input, excessive accumulated translation or a non-finite target position |

Offsets are limited to ±1,000,000 per axis. Angles are limited to ±3,600 degrees per axis per request.
Numeric validation happens before the editor-state guard. A rejected stale transform clears its invalid history.
Rotation composition uses double-precision matrices, then writes engine float transforms.

### Reading the selection

First call `Extern_GetMultiSelectionVobs(nullptr, 0)` to obtain the required buffer length.
Allocate a buffer of that many 32-bit addresses and call again to fill it.
The result is the total size, not the number copied: only the first `min(result, capacity)` entries are valid.
If capacity is insufficient, query/retry before assuming a complete snapshot.
The managed UI uses this snapshot to build a `HashSet<uint>` for both Objects and Fast access.
Tree sorting and rebuilding therefore do not change membership.

## Managed callback

`int IsMultiSelectionInputBlocked()` is exported by `MainForm` in the interface DLL.
It returns 1 before interface initialization, while the group window owns input, when the pointer is outside the viewport,
or when another visible editor control blocks scene interaction; otherwise it returns 0.
`MultiSelect::InputBlocked()` resolves it with `GetProcAddress`. A missing callback blocks group viewport input.
Native foreground-window checks are evaluated separately.

The callback reports current UI blocking. The native selector owns the focus-transition state:

1. A blocked/unfocused frame marks viewport input as awaiting activation.
2. A successful Objects tree selection also marks this state, covering callbacks between render frames.
3. Returning with LMB held consumes that entire gesture through mouse-up.
4. Returning without LMB immediately permits the next selection gesture.
5. A zero-area rectangle never clears the group; a deliberate nonempty rectangle with no matches still can.

## Settings and IDs

New integer settings in the existing `SPACER` section of `spacer_net.ini`:

| Key | Default | Values |
| --- | --- | --- |
| `multiSelectionFilter` | 0 | Filter IDs 0..21 |
| `multiSelectionTransformMode` | 0 | 0 translation, 1 rotation |
| `multiSelectionThroughWalls` | 0 | 0 geometry occlusion, 1 through geometry |

The existing `bToggleWorkMode` uses 3 for group mode. Existing movement/rotation speed, controller and child-block settings remain in effect.
New default key bindings are `MULTI_UNDO` (Ctrl+Z) and `MULTI_PARENT_CUT` (Ctrl+X).
Existing bindings provide translation/rotation switching, movement, rotation, speed modifiers, clear and delete.

Filter mapping: 0 all, 1 without mover subtrees, 2 without WP/freepoints, 3 items, 4 VOBs excluding items/movers,
5 WP/freepoints, 6 waypoints, 7 freepoints, 8 oCMOB, 9 dynamic collision, 10 oCMobInter,
11 lights, 12 sounds, 13 without PFX, 14 invisible, 15 without decals, 16 without decals/PFX,
17 decals, 18 PFX, 19 triggers, 20 zones, 21 music zones.
These are group API IDs, independent of the legacy VOB List controls' indices.
Keep existing group IDs stable because they are persisted.

## Transform and undo lifecycle

`CaptureGroupMove` visits selected roots in parent-first order and records all their descendants.
Each entry contains its original position/matrix, membership, parent, immediate-child count, collision flags and any mover keyframes.
The snapshot does not own long-lived references to world objects; validation and deletion hooks protect its lifetime.

`ApplyGroupMove` temporarily disables recorded collision flags, applies selected transforms once,
restores unselected descendant world transforms and restores the flags.
Selected mover positions and keyframe orientations are transformed; unselected mover routes are restored unchanged.
Waypoint network positions, camera splines and PFX bounds are refreshed after the transforms settle.
The world is marked modified.

`Translate` and `Rotate` call `Transform`. Native keyboard input passes `continuous=true`, accumulating changes against one original snapshot.
Releasing the keys, changing selection/filter/mode or losing UI input finishes the gesture with `EndTranslation`.
The public numeric exports use discrete operations. History holds at most 32 operations, evicting the oldest.

`UndoTranslation` restores a recorded transform and mover keys even if membership has since changed.
It validates world identity, live pointers, parent relationships, child counts and mover-keyframe counts before applying.
`CanUndoTranslation` does not perform that final validation.
`InvalidateUndo` clears group history when leaving the mode; `Reset` also clears membership and activation state.
`ForgetVob` invalidates snapshots before an object is destroyed or its address can be reused.
`RemoveSelected` retains temporary object references while performing deletion to handle hierarchy side effects.

## Interface implementation

- `GroupSelectionForm.UpdateLanguage` preserves filter/mode/input values; switching transform modes deliberately resets numeric input and limits.
- `GroupSelectionForm.RefreshState` reads settings/count/history availability and disables edits during play mode.
- `MainForm.RefreshMultiSelectionWindow` refreshes every 200 ms and shows the owned window once per activation, preserving focus and tool-window hiding.
- `ObjTree.HandleGroupTreeClick` handles object labels/icons before native selection, drag/drop and single-object callbacks. Folders/expanders keep native navigation.
- `ObjTree.RefreshMultiSelection` shares one address snapshot between both trees.
- `GroupSelectionTreeView` highlights all group members, independently of the single native caret. Custom label drawing is enabled only in group mode and survives theme changes.
- `LeaveGroupSelection` retains the reusable window and restores ordinary tree drawing. The separate VOB List implementation is unchanged.

## Compatibility and scope

The managed target remains .NET Framework 4.0. Existing upstream C# 7 syntax still needs a compatible C# compiler.
The native target remains v140_xp with SDK 1.0l. The `Kind` constructor avoids aggregate/default-member initialization unsupported by VS 2015.
Optional `UnionSdkRoot`/`DirectX7SdkRoot` properties allow nonstandard SDK locations without changing default versions.
The DllExport restore condition also recognizes a missing generated metadata cache; native DLL exports remain enabled.

There is no group scale tool, external-controller group gizmo, general deletion/reparent undo or ABI-version handshake.
See [validation](VALIDATION.md) for the boundary between automated checks and actual-engine/runtime checks.
