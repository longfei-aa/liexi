import unreal


ROOT = "/Game/RiftSquad"
INPUT_DIR = f"{ROOT}/Input"
MAP_DIR = f"{ROOT}/Maps"
MAP_PATH = f"{MAP_DIR}/M_Greybox_CombatRoom"


def log(message):
    unreal.log(f"[RiftInit] {message}")


def ensure_dir(path):
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)
        log(f"Created directory {path}")


def enum_value(enum_type, *names):
    for name in names:
        if hasattr(enum_type, name):
            return getattr(enum_type, name)
    raise RuntimeError(f"Cannot resolve enum value from {enum_type}: {names}")


def create_or_load_asset(asset_path, asset_class, factory_class=None):
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        log(f"Loaded {asset_path}")
        return asset

    package_path, asset_name = asset_path.rsplit("/", 1)
    factory = factory_class() if factory_class else None
    asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name,
        package_path,
        asset_class,
        factory,
    )
    if not asset:
        raise RuntimeError(f"Failed to create asset {asset_path}")

    log(f"Created {asset_path}")
    return asset


def create_input_assets():
    ensure_dir(INPUT_DIR)

    input_action_factory = getattr(unreal, "InputAction_Factory", None)
    input_mapping_context_factory = getattr(unreal, "InputMappingContext_Factory", None)

    move_action = create_or_load_asset(f"{INPUT_DIR}/IA_Move", unreal.InputAction, input_action_factory)
    move_action.set_editor_property(
        "value_type",
        enum_value(unreal.InputActionValueType, "AXIS2D", "Axis2D"),
    )

    fire_action = create_or_load_asset(f"{INPUT_DIR}/IA_Fire", unreal.InputAction, input_action_factory)
    fire_action.set_editor_property(
        "value_type",
        enum_value(unreal.InputActionValueType, "BOOLEAN", "Boolean"),
    )

    mapping_context = create_or_load_asset(
        f"{INPUT_DIR}/IMC_Player",
        unreal.InputMappingContext,
        input_mapping_context_factory,
    )

    # Keyboard fallback is defined in Config/DefaultInput.ini. Keep this IMC empty for now
    # to avoid version-sensitive Enhanced Input modifier setup through Python.
    mapping_context.set_editor_property("mappings", [])

    for asset in (move_action, fire_action, mapping_context):
        unreal.EditorAssetLibrary.save_loaded_asset(asset)

    log("Input assets are ready")


def get_level_subsystem():
    return unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)


def create_or_load_level():
    ensure_dir(MAP_DIR)
    subsystem = get_level_subsystem()

    if unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        subsystem.load_level(MAP_PATH)
        log(f"Loaded level {MAP_PATH}")
        return

    if hasattr(subsystem, "new_level"):
        subsystem.new_level(MAP_PATH)
    else:
        unreal.EditorLevelLibrary.new_level(MAP_PATH)

    log(f"Created level {MAP_PATH}")


def spawn_actor_once(label, actor_class, location, rotation=unreal.Rotator(0.0, 0.0, 0.0), scale=None):
    existing = [actor for actor in unreal.EditorLevelLibrary.get_all_level_actors() if actor.get_actor_label() == label]
    if existing:
        actor = existing[0]
    else:
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(actor_class, location, rotation)
        actor.set_actor_label(label)
        log(f"Spawned {label}")

    if scale:
        actor.set_actor_scale3d(scale)
    return actor


def spawn_mesh_once(label, mesh, location, scale, rotation=unreal.Rotator(0.0, 0.0, 0.0)):
    existing = [actor for actor in unreal.EditorLevelLibrary.get_all_level_actors() if actor.get_actor_label() == label]
    if existing:
        actor = existing[0]
    else:
        actor = unreal.EditorLevelLibrary.spawn_actor_from_object(mesh, location, rotation)
        actor.set_actor_label(label)
        log(f"Spawned {label}")

    actor.set_actor_scale3d(scale)
    return actor


def create_greybox_room():
    create_or_load_level()

    cube = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cube.Cube")
    if not cube:
        raise RuntimeError("Cannot load /Engine/BasicShapes/Cube.Cube")

    spawn_mesh_once("RS_Floor", cube, unreal.Vector(0.0, 0.0, -60.0), unreal.Vector(18.0, 18.0, 0.2))
    spawn_mesh_once("RS_Wall_North", cube, unreal.Vector(0.0, 950.0, 100.0), unreal.Vector(18.0, 0.4, 3.0))
    spawn_mesh_once("RS_Wall_South", cube, unreal.Vector(0.0, -950.0, 100.0), unreal.Vector(18.0, 0.4, 3.0))
    spawn_mesh_once("RS_Wall_East", cube, unreal.Vector(950.0, 0.0, 100.0), unreal.Vector(0.4, 18.0, 3.0))
    spawn_mesh_once("RS_Wall_West", cube, unreal.Vector(-950.0, 0.0, 100.0), unreal.Vector(0.4, 18.0, 3.0))

    spawn_actor_once(
        "RS_PlayerStart",
        unreal.PlayerStart,
        unreal.Vector(-600.0, 0.0, 60.0),
        unreal.Rotator(0.0, 0.0, 0.0),
    )

    # Runtime systems are spawned by ARiftGameMode. Keeping the map mostly static
    # avoids null-RHI editor placement crashes for native C++ actors on macOS.

    if hasattr(unreal.EditorLevelLibrary, "save_current_level"):
        unreal.EditorLevelLibrary.save_current_level()
    else:
        unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)

    unreal.EditorAssetLibrary.save_directory(ROOT, only_if_is_dirty=False, recursive=True)
    log("Greybox combat room is ready")


def main():
    create_input_assets()
    create_greybox_room()
    log("Rift Squad CLI content initialization complete")


main()
