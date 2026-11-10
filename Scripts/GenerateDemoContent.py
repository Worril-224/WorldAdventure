import unreal

ROOT = "/Game/WA"
INPUT_PATH = f"{ROOT}/Characters/Input"
ASSET_TOOLS = unreal.AssetToolsHelpers.get_asset_tools()


def log(message):
    unreal.log(f"[WorldAdventure] {message}")


def get_or_create_asset(name, package_path, asset_class, factory_class):
    path = f"{package_path}/{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        return unreal.EditorAssetLibrary.load_asset(path)
    return ASSET_TOOLS.create_asset(name, package_path, asset_class, factory_class())


def add_modifier(context, mapping, modifier_class, **properties):
    modifier = unreal.new_object(modifier_class, context)
    for name, value in properties.items():
        modifier.set_editor_property(name, value)
    modifiers = list(mapping.get_editor_property("modifiers"))
    modifiers.append(modifier)
    mapping.set_editor_property("modifiers", modifiers)


def map_key(context, action, key_name, modifiers=()):
    key = unreal.Key()
    key.set_editor_property("key_name", key_name)
    mapping = context.map_key(action, key)
    for modifier_class, properties in modifiers:
        add_modifier(context, mapping, modifier_class, **properties)
    mapping_data = context.get_editor_property("default_key_mappings")
    mappings = list(mapping_data.get_editor_property("mappings"))
    mappings[-1] = mapping
    mapping_data.set_editor_property("mappings", mappings)
    context.set_editor_property("default_key_mappings", mapping_data)
    return mapping


def create_inputs():
    move = get_or_create_asset("IA_Move", INPUT_PATH, unreal.InputAction, unreal.InputAction_Factory)
    look = get_or_create_asset("IA_Look", INPUT_PATH, unreal.InputAction, unreal.InputAction_Factory)
    jump = get_or_create_asset("IA_Jump", INPUT_PATH, unreal.InputAction, unreal.InputAction_Factory)
    attack = get_or_create_asset("IA_Attack", INPUT_PATH, unreal.InputAction, unreal.InputAction_Factory)
    interact = get_or_create_asset("IA_Interact", INPUT_PATH, unreal.InputAction, unreal.InputAction_Factory)
    context = get_or_create_asset("IMC_ThirdPerson", INPUT_PATH, unreal.InputMappingContext, unreal.InputMappingContext_Factory)

    move.set_editor_property("value_type", unreal.InputActionValueType.AXIS2D)
    look.set_editor_property("value_type", unreal.InputActionValueType.AXIS2D)
    for action in (jump, attack, interact):
        action.set_editor_property("value_type", unreal.InputActionValueType.BOOLEAN)

    context.unmap_all()
    swizzle = (unreal.InputModifierSwizzleAxis, {"order": unreal.InputAxisSwizzle.YXZ})
    negate = (unreal.InputModifierNegate, {})
    map_key(context, move, "W", (swizzle,))
    map_key(context, move, "S", (swizzle, negate))
    map_key(context, move, "D")
    map_key(context, move, "A", (negate,))
    map_key(context, look, "Mouse2D")
    map_key(context, jump, "SpaceBar")
    map_key(context, attack, "LeftMouseButton")
    map_key(context, interact, "E")

    for asset in (move, look, jump, attack, interact, context):
        unreal.EditorAssetLibrary.save_asset(asset.get_path_name())
    log("Enhanced Input assets generated.")


def create_map(target_path, world_partition):
    created = not unreal.EditorAssetLibrary.does_asset_exist(target_path)
    if created:
        if not unreal.WAContentFactory.create_map(target_path, world_partition):
            raise RuntimeError(f"Could not create {target_path}")
        unreal.EditorAssetLibrary.save_asset(target_path)
    unreal.EditorLevelLibrary.load_level(target_path)
    return created


def get_world_settings(game_mode_path):
    world = unreal.EditorLevelLibrary.get_editor_world()
    settings = world.get_world_settings()
    settings.set_editor_property("default_game_mode", unreal.load_class(None, game_mode_path))
    return settings


def spawn_once(actor_class, label, location, scale=None):
    existing = [actor for actor in unreal.EditorLevelLibrary.get_all_level_actors() if actor.get_actor_label() == label]
    actor = existing[0] if existing else unreal.EditorLevelLibrary.spawn_actor_from_class(actor_class, unreal.Vector(*location))
    actor.set_actor_label(label)
    actor.set_actor_location(unreal.Vector(*location), False, False)
    if scale is not None:
        actor.set_actor_scale3d(unreal.Vector(*scale))
    return actor


def create_main_menu():
    path = f"{ROOT}/Maps/L_MainMenu"
    create_map(path, False)
    get_world_settings("/Script/WAGame.WAMainMenuGameMode")
    unreal.EditorLevelLibrary.save_current_level()
    log("Main menu map generated.")


def create_demo_map():
    path = f"{ROOT}/Maps/L_Demo"
    if not create_map(path, True):
        log("Demo map already exists; preserving its actors and World Partition packages.")
        return
    get_world_settings("/Script/WAGame.WAGameMode")

    ground = spawn_once(unreal.StaticMeshActor, "WA_GrayboxGround", (0, 0, -50), (1000, 1000, 1))
    ground.static_mesh_component.set_static_mesh(unreal.load_asset("/Engine/BasicShapes/Cube"))
    ground.static_mesh_component.set_mobility(unreal.ComponentMobility.STATIC)

    light = spawn_once(unreal.DirectionalLight, "WA_DirectionalLight", (0, 0, 1000))
    light.set_actor_rotation(unreal.Rotator(pitch=-45.0, yaw=45.0, roll=0.0), False)
    spawn_once(unreal.SkyLight, "WA_SkyLight", (0, 0, 500))

    explore_class = unreal.load_class(None, "/Script/WAWorld.WAExplorePoint")
    spawn_once(explore_class, "WA_ExplorePoint_A", (0, 2000, 75))
    spawn_once(explore_class, "WA_ExplorePoint_B", (30000, -20000, 75))
    spawn_once(explore_class, "WA_ExplorePoint_C", (-30000, 15000, 75))

    for index, location in enumerate(((0, 0, 120), (250, 0, 120), (0, 250, 120), (250, 250, 120)), start=1):
        spawn_once(unreal.PlayerStart, f"WA_PlayerStart_{index}", location)

    unreal.EditorLevelLibrary.save_current_level()
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    log("World Partition demo map generated with 3 exploration points and 4 player starts.")


def main():
    create_inputs()
    create_main_menu()
    create_demo_map()
    log("Content generation complete.")


main()
