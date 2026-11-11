import unreal

MATERIAL_PATH = "/Game/WA/Characters/Prototype"
ASSET_TOOLS = unreal.AssetToolsHelpers.get_asset_tools()

MATERIAL_DEFINITIONS = {
    "Tunic": ((0.08, 0.34, 0.30), 0.88),
    "Pants": ((0.055, 0.09, 0.16), 0.92),
    "Leather": ((0.20, 0.09, 0.035), 0.82),
    "Skin": ((0.72, 0.43, 0.27), 0.78),
    "Hair": ((0.08, 0.035, 0.02), 0.95),
    "Accent": ((0.82, 0.48, 0.08), 0.72),
}


def log(message):
    unreal.log(f"[WorldAdventure] {message}")


def get_or_create_material(name, color, roughness):
    asset_name = f"M_Prototype_{name}"
    asset_path = f"{MATERIAL_PATH}/{asset_name}"
    material = unreal.EditorAssetLibrary.load_asset(asset_path) if unreal.EditorAssetLibrary.does_asset_exist(asset_path) else None
    if material is None:
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

    if not unreal.EditorAssetLibrary.save_asset(asset_path):
        raise RuntimeError(f"Could not save {asset_path}")
    return unreal.EditorAssetLibrary.load_asset(asset_path)


def main():
    materials = []
    for name, (color, roughness) in MATERIAL_DEFINITIONS.items():
        material = get_or_create_material(name, color, roughness)
        if not isinstance(material, unreal.Material):
            raise RuntimeError(f"Generated asset is not a material: {material.get_path_name()}")
        materials.append(material.get_path_name())

    log(f"Prototype character materials ready: {len(materials)} assets in {MATERIAL_PATH}.")
    for material_path in materials:
        log(material_path)


main()
