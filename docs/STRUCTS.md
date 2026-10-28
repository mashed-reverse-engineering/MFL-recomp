# Record layouts

The curated structs of the header `src/mfl_tier2.h` (32-bit x86 layout of `MFL.exe`; sizes and offsets below are computed from the
header, not typed by hand). Field names follow what the code does with them. Confidence column: **high** = field used consistently
by several functions or seen in a live game, **medium** = single function or arithmetic evidence, **inferred** = a plausible reading.
Gaps (`_gap_xx`) and `field_0xNN` members are bytes whose meaning is not known yet. How the structs are used: [GAME_STRUCTURE.md](GAME_STRUCTURE.md).

| struct | size | what it is |
|---|---|---|
| `Car` | 0xd04 | physics state of one car (16 slots) |
| `Wheel` | 0xc4 | one wheel, 4 per `Car` |
| `ContactPoint` | 0x40 | collision probe point, 18 per `Car` |
| `VehicleRecord` | 0x2ac | visual side of a car: part frames, spheres, shadow |
| `PlayerInput` | 0x4c | one controller's input bytes and rumble request |
| `PlayerSlot` | 0x10 | which controller/team plays in a slot |
| `PlayerStatRecord` | 0x138 | per player scoring statistics |
| `CourseEntry` | 0x48 | name, id and callbacks of one course |
| `CourseFiles` | 0x3ae0 | file names filled in by the course script |
| `CourseCounts` | 0xd20 | counts and fog/light settings from the course script |
| `CourseLap` | 0x16300 | lap lines and AI polygon data of a course |
| `CourseWorld` | 0xca50 | loaded world, clumps, splines and lights of the current course |
| `MenuRecord` | 0x40 | one open menu page |
| `MenuItemAnim` | 0x34 | animation state of one menu item |
| `TeamPanelEntry` | 0x16c | team panel of the HUD / team select |
| `GameState` | 0xb64 | career progress, records and options |
| `Powerup` | 0x50 | one pickup |
| `WeaponSlot` | 0xb4 | a player's current weapon |
| `RocketRecord` | 0x60 | one flying rocket |
| `MachinegunState` | 0x48 | machine gun of a player |
| `ExplosionEntry` | 0x24 | one queued explosion |
| `Trigger` | 0x8c | overlap sphere with callback |
| `FxSceneObject` | 0xec | effect object (animated scene effect) |
| `FxTrailParticle` | 0x48 | one trail particle |
| `Listener3D` | 0x74 | 3D audio listener |
| `AiBotRecord` | 0x78 | state of one AI bot |
| `ControllerCfg` / `VideoCfg` | 0x200 each | saved controller and video settings |

## Car (0xd04)

| offset | field | meaning | confidence |
|---|---|---|---|
| 0x000 | `index` | slot number | high |
| 0x004 | `active` | slot in use | high |
| 0x008 | `vehicle_id` | chosen vehicle (set from the menu choice) | medium |
| 0x010 | `controls_disabled` | input ignored while set | medium |
| 0x014 | `rwp_object` | non-zero while the physics library simulates the car | medium |
| 0x018 | `rwp_timer` | ms countdown, 600 when entering that mode | medium |
| 0x020 | `effect_level[6]` | 0..100 per effect kind, decays when `effect_hold` runs out | medium |
| 0x038 | `effect_hold[6]` | ms an effect level is held | medium |
| 0x050 | `mass` | 1000.0 by default | high |
| 0x058 | `inertia_sum`, `inertia_inverse` | from the wheel positions | medium |
| 0x060 | `wheel_points[12]` / 0x090 `bounds_points[24]` / 0x0f0 `body_points[18]` | 18 probe points (4 wheels, 8 box corners, 6 derived) | high |
| 0x144 | `velocity_input[3]` | velocity added from outside (impulses) | high (seen live) |
| 0x154 | `speed_scale` | AI rubber band factor | medium |
| 0x15c | `com_offset[3]` | centre of mass offset | medium |
| 0x168 | `wheels[4]` | 4 x `Wheel` (front left/right, rear left/right) | high |
| 0x478 | `handling_params[6]` | copied from the handling tables | medium |
| 0x490 | `gear`, 0x494 `gear_shift_timer` | gearbox state | medium |
| 0x4a0 | `wheel_extent`, 0x4a4 `bounds_radius` | 1.05 x largest wheel / body point distance | medium |
| 0x4ac | `contacts[17]` | `ContactPoint` records (the 18th overlaps the matrices) | medium |
| 0x928 | `matrices[32]` | two `RwMatrix` (double buffer); 0x9a8 `matrix_current`, 0x9ac `matrix_previous` | high |
| 0x9b0 | `velocity[3]`, 0x9bc `angular_velocity[3]` | motion | high |
| 0x9c8 | `up[3]`, 0x9d4 `forward[3]` | unit vectors from the current matrix | high |
| 0x9e0 | `wheels_in_contact` | float count of wheels touching ground | high |
| 0x9e4 | `speed`, 0x9e8 `angular_speed` | vector lengths | high |
| 0x9ec | `contact_count` | ground contact points from the world test | inferred |
| 0x9f0 | `mode`, 0x9f4 `mode2` | 2 halves the velocity each step | medium |
| 0xad0 | `velocity_history_index`, 0xad4 `velocity_history[6]` | 2 velocity samples for the acceleration | medium |
| 0xb14 | `force_accum[3]` | extra force of this step | medium |
| 0xb20 | `driven` | set when the car was driven this step | medium |
| 0xbf4 | `turbo_charge` (0..3000), 0xbf8 `turbo_state` | turbo | medium |
| 0xbfc | `contact_key[32]`, 0xc7c `contact_valid[32]` | wheel contact cache keyed by triangle | medium |
| 0xcfc | `handbrake_held` | both handbrake inputs pressed | medium |
| 0xd00 | `is_bot` | 0 human, 1 AI | high (seen live) |

`Wheel` (0xc4): 0x00 `state`, 0x04..0x10 position in car space and its length, 0x2c `contact_distance` (>= 0.02 counts as lifted),
0x30 `in_contact`, 0x84 `contact_triangle` (-1 none), 0x88 `surface_color` (RGBA of the touched triangle; selects the surface effect),
0x98 `contact_normal[3]`, 0xa4 `load`. Confidence medium to high. `ContactPoint` (0x40): 0x00 `triangle`, 0x10 `lever_arm`.

## VehicleRecord (0x2ac)

| offset | field | meaning | confidence |
|---|---|---|---|
| 0x000 | `part_frames[140]` | frame handle per vehicle part id (`vehicle_part_frame(v, part)`) | high |
| 0x230 | `matrix6[6]` | 6 floats returned by `vehicle_get_matrix` | medium |
| 0x248 | `local_sphere[4]` | x y z radius of the clump bounding sphere | medium |
| 0x258 | `world_sphere[4]` | same in world space; used for trigger spheres and HUD markers | medium |
| 0x268 | `object`, 0x26c `frame` | vehicle object and frame handle | medium |
| 0x270 | `clump_part_count` | loop bound of the part scan in `vehicle_load` | medium |
| 0x274 | `half_scale` | scale * 0.5 | medium |
| 0x278 | `envmap_texture`, 0x27c `envmap_frame`, 0x280 `envmap_angle` | environment map | medium |
| 0x284 | `size_factor`, 0x288 `light_factor` | light factor = 1 - ground probe distance | medium |
| 0x290 | `index` | slot index 0..3 | high |
| 0x294 | `flags` | `VEHICLE_FLAG_*` | medium |
| 0x29c | `render_camera`, 0x2a0 `shadow_camera`, 0x2a4 `shadow_frame`, 0x2a8 `shadow_texture` | render and shadow pass | medium |

## Input, players, stats

| struct | offset | field | meaning | confidence |
|---|---|---|---|---|
| `PlayerInput` | 0x00..0x03 | `left right up down` | strength 0..255 | high |
| | 0x04, 0x05 | `accelerate`, `brake` | strength 0..255 | high |
| | 0x07, 0x08 | `fire`, `drop` | weapon buttons | high |
| | 0x09..0x0b | `pause menu_select menu_back` | | medium |
| | 0x14, 0x18 | `analog_x`, `analog_y` | stick (y negated) | medium |
| | 0x20.. | `rumble_request` and rumble parameters | consumed by the input backend | medium |
| | 0x3f | `insult` | | medium |
| `PlayerSlot` | 0x00 | `controller` | controller index, -1 = no player | high |
| | 0x04, 0x08, 0x0c | `state`, `team`, `extra` | `1 << team` is the HUD mask | medium |
| `PlayerStatRecord` | 0x00 | `place` | finishing place | medium |
| | 0x08..0x10 | turbo skill score, kind, timer | "turbo skills" | medium |
| | 0x9c.. | contact ticks, aggression, kills, unforced errors | scoring categories | medium |

## Course records

| struct | offset | field | meaning | confidence |
|---|---|---|---|---|
| `CourseEntry` | 0x00 | `name[16]` | course name | high |
| | 0x10 | `course_id` | +1000 when mirrored | medium |
| | 0x14 | `callbacks[13]` | per course hooks (`cb0` after load, `cb12` before world creation) | medium |
| `CourseFiles` | various | file names set by the course script (texture dictionary, world BSP, collision BSP, clumps x 128, MTS x 17, splines x 16, lights, UVA, copters) | strings of 0x40 bytes each | high |
| `CourseCounts` | 0x00..0x14 | `course_id`, object BSP / clump / MTS / spline / animation counts | | high |
| | 0xcd4 | `powerup_file_count` | | medium |
| | 0xcd8, 0xce8 | `ambient_rgba[4]`, `fog_color[4]` | | high |
| | 0xd14, 0xd18 | `uva_count`, `copter_count` | | medium |
| `CourseWorld` | 0x0000 | `mts_matrices` | 16 matrix lists of 0xc00 bytes | high |
| | 0xc000 | `mts_matrix_counts[16]` | | high |
| | 0xc040..0xc0f0 | object BSP worlds, AI worlds, collision BSPs, hoppy BSPs, splines | loaded handles | high |
| | 0xc110 | `clumps[128]` | loaded clump handles | high |
| | 0xc9d0..0xc9f0 | texture dictionary, `world`, occluder, lights, ambient and directional light, PVS, camera animation | | medium |
| | 0xc9f4, 0xca00 | `fog_a[3]`, `fog_b[3]` | blended by lap fraction | inferred |
| | 0xca38 | `flag_outdoor` | 0 for indoor event types | medium |
| `CourseLap` | (see header) | `lap_lines[512]`, split sectors, AI polygon count | `Lap_Line` entries (-1 end, 0x100 change) | medium |

## Frontend

| struct | offset | field | meaning | confidence |
|---|---|---|---|---|
| `MenuRecord` | 0x00 | `item_list` | pointer to the token list of the page | high |
| | 0x04 | `page_id` | `MENU_PAGE_*` | high |
| | 0x08 | `selected` | -1 none | high |
| | 0x0c | `entry_enabled[12]` | set by `menu_page_update_entry_states` | high |
| | 0x3c | `entry_count` | | high |
| `MenuItemAnim` | 0x00 | `item_id` | token, 0 = unused | high |
| | 0x04 | `state` | `MENU_ITEM_APPEARING/IDLE/DISAPPEARING/FINISHED` | high |
| | 0x0c, 0x10 | `colour`, `anim_counter` | RGBA, 0x1ff = done | medium |
| | 0x14, 0x18, 0x1c | `scale`, `x`, `y` | defaults 0.6, 64, 48 | medium |
| `TeamPanelEntry` | 0x00 | `player[3]`, 0x0c `player_count` | players of the team | medium |
| | 0x10 | `visible[40]` | per part, rewritten every frame | medium |
| | 0xb0 | `part[40]` | part objects (drawn through a vtable slot) | medium |
| | 0x150 | `origin` | panel translation | medium |
| | 0x15c..0x164 | `clump`, `frame`, `team` | | medium |
| | 0x168 | `flags` | button group bits | inferred |
| `GameState` | 0x000 | `progress[270]` | [15][18]: 0 locked, 1 open, 2 done | high |
| | 0x438 | `records[75]` | [15][5], -1 empty | medium |
| | 0x7bc, 0x870, 0x924 | best time minutes, seconds, fraction (x 45) | | high |
| | 0x9d8 | `enabled[270]` | | medium |
| | 0xb00, 0xb2c | `option_sliders[5]`, `option_flags[5]` | | medium |
| | 0xb54 | `mirror_mode` | course id + 1000 | medium |

## Weapons and effects

| struct | offset | field | meaning | confidence |
|---|---|---|---|---|
| `Powerup` | 0x00 | `clump` | pickup model | high |
| | 0x18, 0x1c | `respawn_time`, `respawn_timer` | seconds, default 120 | high |
| | 0x20 | `state` | 0 unused, 1 available, 2 taken, 3 disabled | high |
| | 0x24, 0x28 | `type`, `special` | | medium |
| | 0x2c, 0x38 | `position[3]`, `spawn_position[3]` | | high |
| `WeaponSlot` | 0x20 | `aim_vector[3]` | passed to fire callbacks | medium |
| | 0x30 | `muzzle_position[3]` | sound / effect position | medium |
| | 0xa8, 0xac, 0xb0 | `descriptor`, `state`, `player` | entry of `weapon_descriptors`, per player state | high |
| `RocketRecord` | 0x0c | `pos` | | high |
| | 0x18 | `age` | drives the straight flight wobble | medium |
| | 0x1c | `vel` | per frame movement | high |
| | 0x28, 0x2c, 0x30 | `lift`, `accel`, `speed` | accel grows 1e-4 per frame to 0.005 | medium |
| | 0x40, 0x44 | `pitch_rate`, `yaw_rate` | homing steering | medium |
| | 0x54 | `target_car` | -1 none | high |
| | 0x5c | `frame_holder` | its +4 is the model's frame | medium |
| `MachinegunState` | 0x00..0x18 | slot, `cooldown`, clump, `active`, `ammo` (50 on equip), `fired`, `spin` | | medium |
| `ExplosionEntry` | 0x00..0x10 | `damage`, `push_horizontal`, `push_vertical`, `frames`, `frames_left` | | medium |
| | 0x14 | `position_radius[4]` | | high |
| `Trigger` | 0x00 | `position_radius[4]` | sphere | high |
| | 0x10 | `type` | 7 powerup, 0xB rocket, 0x19 explosion | high |
| | 0x14..0x1c | `owner`, `flags`, `callback` | callback(trigger, other) per overlap | high |
| | 0x24, 0x64 | `cells[16]`, `cell_count` | grid cells entered | medium |
| `FxSceneObject` | 0x10..0x24 | frame handles, owner, spin | | medium |
| | 0x4c | `age` | += tick seconds | high |
| | 0x5c | `health` | percent | medium |
| | 0xb0..0xcc | `choice_count`, `choice[5]`, `choice_index`, `cur` | random walk over the course effect table | inferred |
| | 0xd4, 0xdc | `gravity`, `vel` | | medium |
| `FxTrailParticle` | 0x00 | `pos` | | high |
| | 0x0c | `colour[4]` | RGBA bytes | high |
| | 0x10, 0x14 | `size_start`, `size_end` | | high |
| | 0x18, 0x24 | `velocity`, `accel` | | high |
| | 0x38 | `scale` | | inferred |
| | 0x40, 0x45 | `flags`, `kind` | | medium |
| `Listener3D` | 0x28..0x58 | `position`, `velocity`, `front`, `top`, `side` | `side` = front x top | inferred |
| | 0x64..0x6c | distance, doppler, rolloff factors | DirectSound3D style | inferred |
| | 0x70 | `dirty` | `LISTENER3D_DIRTY_*` bits | inferred |

## AI

`AiBotRecord` (0x78): stage state machine fields (`stage`, `stage_counter`), the chosen target (`target_valid`, `target_car`), distance to the
human leader (`leader_distance`, `leader_far_ms`), last steering (`steer_side`, `steer_amount`), cooldown timers, weapon hold and fire delay
(`weapon_held_ms`, `fire_delay`, `must_fire`). Confidence medium (written and read by the `ai_*` functions). `AiData` (0x11884) holds the 128 x 128
`grid` of tile indices, a pool of 512 tiles of 8 x 8 danger bytes and the route sets; confidence medium.

## Voice record offsets

Named in `mfl_tier2.h` (raw offsets, confidence medium): `VOICE_OFF_HANDLE` 0x30 (sound object / sub voice), `VOICE_OFF_FLAGS` 0x34, `VOICE_OFF_FREQ` 0x3c (float), `VOICE_OFF_POS_X/Y/Z` 0x58 / 0x5c / 0x60. Car ids in collider and event parameters are `CAR_ID_BASE + n` (0xff00 + n). Several functions that keep a local struct on the stack (probe, hit record, wave state) now name its fields; those layouts are file-local and not shared.
