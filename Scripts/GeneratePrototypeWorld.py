import math

import unreal

MAP_PATH = "/Game/WA/Maps/L_Demo"
MATERIAL_PATH = "/Game/WA/World/Prototype"
ACTOR_PREFIX = "WA_Prototype_"
ASSET_TOOLS = unreal.AssetToolsHelpers.get_asset_tools()

MATERIAL_DEFINITIONS = {
    "Grass": ((0.18, 0.32, 0.20), 0.95),
    "Path": ((0.42, 0.31, 0.20), 0.90),
    "Stone": ((0.36, 0.40, 0.43), 0.85),
    "StoneDark": ((0.18, 0.21, 0.27), 0.90),
    "Wood": ((0.24, 0.12, 0.06), 0.90),
    "Leaf": ((0.12, 0.36, 0.22), 0.85),
    "LeafLight": ((0.28, 0.50, 0.24), 0.85),
    "Canvas": ((0.65, 0.52, 0.32), 0.95),
    "Cloth": ((0.12, 0.36, 0.42), 0.80),
    "Gold": ((0.85, 0.55, 0.16), 0.45),
    "Fire": ((1.00, 0.24, 0.04), 0.60),
}

MESH_PATHS = {
    "Cube": "/Engine/BasicShapes/Cube",
    "Cylinder": "/Engine/BasicShapes/Cylinder",
    "Cone": "/Engine/BasicShapes/Cone",
    "Sphere": "/Engine/BasicShapes/Sphere",
}

EXPLORE_A = (0.0, 2000.0)
EXPLORE_B = (30000.0, -20000.0)
EXPLORE_C = (-30000.0, 15000.0)

GROUP_COUNTS = {}
EXISTING_ACTORS = {}
MESHES = {}
MATERIALS = {}


def log(message):
    unreal.log(f"[WorldAdventure] {message}")


def get_or_create_material(name, color, roughness):
    asset_name = f"M_Prototype_{name}"
    asset_path = f"{MATERIAL_PATH}/{asset_name}"
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        return unreal.EditorAssetLibrary.load_asset(asset_path)

    material = ASSET_TOOLS.create_asset(
        asset_name,
        MATERIAL_PATH,
        unreal.Material,
        unreal.MaterialFactoryNew(),
    )
    if material is None:
        raise RuntimeError(f"Could not create {asset_path}")

    color_expression = unreal.MaterialEditingLibrary.create_material_expression(
        material,
        unreal.MaterialExpressionConstant3Vector,
        -450,
        0,
    )
    color_expression.set_editor_property(
        "constant",
        unreal.LinearColor(color[0], color[1], color[2], 1.0),
    )
    unreal.MaterialEditingLibrary.connect_material_property(
        color_expression,
        "",
        unreal.MaterialProperty.MP_BASE_COLOR,
    )

    roughness_expression = unreal.MaterialEditingLibrary.create_material_expression(
        material,
        unreal.MaterialExpressionConstant,
        -450,
        220,
    )
    roughness_expression.set_editor_property("r", roughness)
    unreal.MaterialEditingLibrary.connect_material_property(
        roughness_expression,
        "",
        unreal.MaterialProperty.MP_ROUGHNESS,
    )
    unreal.MaterialEditingLibrary.recompile_material(material)
    return material


def place(group, mesh_name, material_name, location, scale=(1.0, 1.0, 1.0), yaw=0.0):
    index = GROUP_COUNTS.get(group, 0) + 1
    GROUP_COUNTS[group] = index
    label = f"{ACTOR_PREFIX}{group}_{index:03d}"

    actor = EXISTING_ACTORS.get(label)
    if actor is None:
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
            unreal.StaticMeshActor,
            unreal.Vector(*location),
        )
        EXISTING_ACTORS[label] = actor
    if actor is None:
        raise RuntimeError(f"Could not spawn {label}")

    actor.set_actor_label(label)
    actor.set_actor_location(unreal.Vector(*location), False, False)
    actor.set_actor_rotation(unreal.Rotator(pitch=0.0, yaw=yaw, roll=0.0), False)
    actor.set_actor_scale3d(unreal.Vector(*scale))

    component = actor.static_mesh_component
    component.set_mobility(unreal.ComponentMobility.STATIC)
    component.set_static_mesh(MESHES[mesh_name])
    component.set_material(0, MATERIALS[material_name])


def add_road(group, start, end, width=450.0, segment_length=1400.0):
    dx = end[0] - start[0]
    dy = end[1] - start[1]
    length = math.hypot(dx, dy)
    segment_count = max(1, math.ceil(length / segment_length))
    segment_size = length / segment_count
    yaw = math.degrees(math.atan2(dy, dx))

    for index in range(segment_count):
        t = (index + 0.5) / segment_count
        place(
            group,
            "Cube",
            "Path",
            (start[0] + dx * t, start[1] + dy * t, 6.0),
            (segment_size / 100.0, width / 100.0, 0.12),
            yaw,
        )


def add_lamp(group, location):
    x, y = location
    place(group, "Cylinder", "StoneDark", (x, y, 160.0), (0.18, 0.18, 3.2))
    place(group, "Sphere", "Gold", (x, y, 345.0), (0.42, 0.42, 0.42))


def add_route_lamps(group, start, end, parameters):
    dx = end[0] - start[0]
    dy = end[1] - start[1]
    length = math.hypot(dx, dy)
    normal_x = -dy / length
    normal_y = dx / length
    for t, side in parameters:
        add_lamp(
            group,
            (
                start[0] + dx * t + normal_x * side * 400.0,
                start[1] + dy * t + normal_y * side * 400.0,
            ),
        )


def add_tree(group, location, light_leaves=False):
    x, y = location
    place(group, "Cylinder", "Wood", (x, y, 140.0), (0.70, 0.70, 2.8))
    place(
        group,
        "Sphere",
        "LeafLight" if light_leaves else "Leaf",
        (x, y, 400.0),
        (2.8, 2.8, 2.4),
    )


def add_rock(group, location, scale, yaw):
    x, y = location
    place(
        group,
        "Sphere",
        "StoneDark",
        (x, y, 45.0 * scale[2]),
        scale,
        yaw,
    )


def build_camp():
    center = (-650.0, -600.0)
    place("CampPlaza", "Cylinder", "Stone", (*center, 3.0), (7.0, 7.0, 0.06))
    place("Campfire", "Cylinder", "StoneDark", (*center, 12.5), (0.9, 0.9, 0.25))
    place("Campfire", "Cone", "Fire", (*center, 70.0), (0.55, 0.55, 0.9))

    for location in ((-1100.0, -1000.0), (-1400.0, -300.0), (-700.0, -1250.0)):
        place("CampTent", "Cone", "Canvas", (*location, 125.0), (2.5, 2.5, 2.5), 45.0)

    for index, location in enumerate(((-1200.0, -50.0), (-100.0, -600.0), (-650.0, -1150.0), (-1200.0, -1150.0))):
        x, y = location
        group = f"CampBanner_{index + 1}"
        place(group, "Cylinder", "Wood", (x, y, 150.0), (0.12, 0.12, 3.0))
        place(group, "Cube", "Cloth" if index % 2 == 0 else "Gold", (x + 55.0, y, 260.0), (1.1, 0.08, 0.65))

    for location, yaw in (((-350.0, -850.0), 15.0), ((-250.0, -900.0), -10.0), ((-400.0, -970.0), 30.0), ((-950.0, -250.0), 0.0)):
        place("CampCrate", "Cube", "Wood", (*location, 35.0), (0.7, 0.7, 0.7), yaw)


def build_wind_ruins():
    cx, cy = EXPLORE_A
    place("RuinBase", "Cylinder", "Stone", (cx, cy, 2.0), (14.0, 14.0, 0.04))

    for index in range(6):
        angle = math.radians(index * 60.0)
        x = cx + math.cos(angle) * 550.0
        y = cy + math.sin(angle) * 550.0
        place("RuinColumn", "Cylinder", "Stone", (x, y, 225.0), (0.55, 0.55, 4.5))
        place("RuinColumn", "Cube", "StoneDark", (x, y, 460.0), (0.85, 0.85, 0.20), index * 60.0)

    for location, yaw in (((cx - 800.0, cy + 250.0), 20.0), ((cx + 750.0, cy - 350.0), -35.0)):
        place("RuinBroken", "Cylinder", "Stone", (*location, 100.0), (0.60, 0.60, 2.0), yaw)

    arch_y = cy + 700.0
    for x in (cx - 250.0, cx + 250.0):
        place("RuinArch", "Cube", "StoneDark", (x, arch_y, 250.0), (0.45, 0.55, 5.0))
    place("RuinArch", "Cube", "Stone", (cx, arch_y, 510.0), (5.5, 0.65, 0.35))

    place("WindSpire", "Cone", "StoneDark", (cx, cy + 450.0, 250.0), (1.3, 1.3, 5.0))
    place("WindSpire", "Sphere", "Gold", (cx, cy + 450.0, 580.0), (0.55, 0.55, 0.85))


def build_forest_landmark():
    cx, cy = EXPLORE_B
    tree_center = (cx, cy + 900.0)
    place("AncientTree", "Cylinder", "Wood", (*tree_center, 350.0), (1.8, 1.8, 7.0))
    for offset, scale in (((-220.0, 0.0), 4.2), ((220.0, 40.0), 4.0), ((0.0, 180.0), 4.8)):
        place(
            "AncientTree",
            "Sphere",
            "LeafLight",
            (tree_center[0] + offset[0], tree_center[1] + offset[1], 760.0),
            (scale, scale, scale * 0.75),
        )

    for index in range(6):
        angle = math.radians(index * 60.0 + 30.0)
        x = cx + math.cos(angle) * 650.0
        y = cy + math.sin(angle) * 650.0
        place("ForestStone", "Cube", "Stone", (x, y, 80.0), (0.55, 0.35, 1.6), index * 60.0)


def build_rock_highland():
    cx, cy = EXPLORE_C
    place("HighlandBase", "Cylinder", "StoneDark", (cx, cy, 2.0), (15.0, 15.0, 0.04))

    heights = (600.0, 900.0, 700.0, 1100.0, 750.0, 950.0, 650.0)
    for index, height in enumerate(heights):
        angle = math.radians(index * (360.0 / len(heights)))
        x = cx + math.cos(angle) * 950.0
        y = cy + math.sin(angle) * 950.0
        width = 1.5 + (index % 3) * 0.25
        place("HighlandPillar", "Cylinder", "StoneDark", (x, y, height / 2.0), (width, width, height / 100.0), index * 17.0)

    spire = (cx, cy + 1350.0)
    place("HighlandSpire", "Cone", "Stone", (*spire, 450.0), (2.4, 2.4, 9.0))
    place("HighlandSpire", "Sphere", "Gold", (*spire, 950.0), (0.65, 0.65, 0.65))

    for index in range(4):
        place(
            "HighlandStep",
            "Cube",
            "Stone",
            (cx + 700.0 + index * 130.0, cy - 500.0 - index * 100.0, 10.0 + index * 5.0),
            (1.4, 0.8, 0.20 + index * 0.10),
            -35.0,
        )


def configure_lighting(actors):
    actors_by_label = {actor.get_actor_label(): actor for actor in actors}

    def get_or_spawn(actor_class, label):
        actor = actors_by_label.get(label) or EXISTING_ACTORS.get(label)
        if actor is None:
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(actor_class, unreal.Vector(0.0, 0.0, 0.0))
            EXISTING_ACTORS[label] = actor
        if actor is None:
            raise RuntimeError(f"Could not create environment actor: {label}")
        actor.set_actor_label(label)
        return actor

    light = actors_by_label.get("WA_DirectionalLight")
    if light is not None:
        light.light_component.set_editor_property("intensity", 3.0)
        light.light_component.set_editor_property("atmosphere_sun_light", True)

    sky_light = actors_by_label.get("WA_SkyLight")
    if sky_light is not None:
        sky_light.light_component.set_editor_property("real_time_capture", True)
        sky_light.light_component.set_editor_property("intensity", 1.0)

    get_or_spawn(unreal.SkyAtmosphere, "WA_Prototype_SkyAtmosphere")
    fog = get_or_spawn(unreal.ExponentialHeightFog, "WA_Prototype_HeightFog")
    fog_component = fog.get_component_by_class(unreal.ExponentialHeightFogComponent)
    fog_component.set_editor_property("fog_density", 0.0005)
    fog_component.set_editor_property("fog_height_falloff", 0.2)
    fog_component.set_editor_property("start_distance", 1000.0)


def build_world():
    routes = (
        ("Road_A", (0.0, 400.0), EXPLORE_A),
        ("Road_B", (250.0, -100.0), EXPLORE_B),
        ("Road_C", (-250.0, 100.0), EXPLORE_C),
    )
    for group, start, end in routes:
        add_road(group, start, end)

    build_camp()

    for location in ((-1800.0, 300.0), (-1800.0, -1400.0), (900.0, -1300.0), (1300.0, -500.0)):
        add_tree("CampTree", location, True)
    for location in ((-900.0, 1400.0), (900.0, 1500.0)):
        add_tree("RuinTree", location)

    forest_offsets = (
        (-1800.0, -900.0),
        (-1200.0, 1600.0),
        (-500.0, 2200.0),
        (900.0, 2000.0),
        (1900.0, 1200.0),
        (2300.0, -300.0),
        (1800.0, -1700.0),
        (700.0, -2300.0),
        (-900.0, -2200.0),
        (-2300.0, -1800.0),
        (-2800.0, 300.0),
        (3000.0, 1800.0),
        (-3200.0, 1800.0),
        (3200.0, -1800.0),
    )
    for index, offset in enumerate(forest_offsets):
        add_tree(
            "ForestTree",
            (EXPLORE_B[0] + offset[0], EXPLORE_B[1] + offset[1]),
            index % 3 == 0,
        )

    build_wind_ruins()
    build_forest_landmark()
    build_rock_highland()

    add_route_lamps("Lamp_A", routes[0][1], routes[0][2], ((0.55, 1.0),))
    lamp_parameters = tuple((t, 1.0 if index % 2 == 0 else -1.0) for index, t in enumerate((0.18, 0.36, 0.54, 0.72, 0.90)))
    add_route_lamps("Lamp_B", routes[1][1], routes[1][2], lamp_parameters)
    add_route_lamps("Lamp_C", routes[2][1], routes[2][2], lamp_parameters)

    rock_offsets = (
        (-1800.0, -500.0),
        (-1500.0, 900.0),
        (-800.0, 1800.0),
        (1400.0, 1400.0),
        (2000.0, -600.0),
    )
    rock_scales = ((1.8, 1.3, 1.1), (1.2, 1.8, 1.5), (2.0, 1.4, 1.0), (1.4, 1.2, 1.8), (2.2, 1.5, 1.2))
    for index, (offset, scale) in enumerate(zip(rock_offsets, rock_scales)):
        add_rock(
            "HighlandRock",
            (EXPLORE_C[0] + offset[0], EXPLORE_C[1] + offset[1]),
            scale,
            index * 27.0,
        )

    boulder_offsets = ((-2600.0, 700.0), (-2400.0, 1100.0), (2500.0, 2500.0), (2900.0, 2200.0), (2600.0, -2500.0))
    for index, offset in enumerate(boulder_offsets):
        add_rock(
            "ForestRock",
            (EXPLORE_B[0] + offset[0], EXPLORE_B[1] + offset[1]),
            (1.4 + index * 0.1, 1.2, 1.0 + (index % 2) * 0.4),
            index * 31.0,
        )


def load_all_world_partition_actors():
    result = unreal.WorldPartitionBlueprintLibrary.get_actor_descs()
    succeeded, descriptors = result if isinstance(result, tuple) else (True, result)
    if not succeeded:
        raise RuntimeError("Could not enumerate World Partition actor descriptors.")
    unreal.WorldPartitionBlueprintLibrary.load_actors([descriptor.guid for descriptor in descriptors])
    actors = unreal.EditorLevelLibrary.get_all_level_actors()
    log(f"Loaded {len(actors)} actors from {len(descriptors)} World Partition descriptors.")
    return actors


def main():
    if not unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        raise RuntimeError(f"Required map does not exist: {MAP_PATH}")
    unreal.EditorLevelLibrary.load_level(MAP_PATH)

    for key, path in MESH_PATHS.items():
        MESHES[key] = unreal.load_asset(path)
        if MESHES[key] is None:
            raise RuntimeError(f"Could not load engine mesh: {path}")
    for name, (color, roughness) in MATERIAL_DEFINITIONS.items():
        MATERIALS[name] = get_or_create_material(name, color, roughness)

    actors = load_all_world_partition_actors()
    unique_actors = []
    duplicate_count = 0
    for actor in actors:
        label = actor.get_actor_label()
        if label.startswith(ACTOR_PREFIX) and label in EXISTING_ACTORS:
            unreal.EditorLevelLibrary.destroy_actor(actor)
            duplicate_count += 1
            continue
        unique_actors.append(actor)
        if label.startswith(ACTOR_PREFIX):
            EXISTING_ACTORS[label] = actor
    actors = unique_actors
    if duplicate_count:
        log(f"Removed {duplicate_count} duplicate prototype actors by deterministic label.")

    ground = next((actor for actor in actors if actor.get_actor_label() == "WA_GrayboxGround"), None)
    if ground is not None:
        ground.static_mesh_component.set_material(0, MATERIALS["Grass"])
    else:
        log("WA_GrayboxGround was not found; no replacement ground was spawned.")

    configure_lighting(actors)
    build_world()
    actor_count = sum(GROUP_COUNTS.values())
    if not 100 <= actor_count <= 200:
        raise RuntimeError(f"Prototype actor count outside intended range: {actor_count}")

    unreal.EditorLevelLibrary.save_current_level()
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    log(f"Prototype world generated with {actor_count} deterministic static actors across {len(GROUP_COUNTS)} groups.")
    log("Existing GameMode, World Partition, exploration points, and player starts were preserved; prototype atmosphere was configured.")


main()
