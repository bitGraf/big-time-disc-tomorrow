# Level output addon for Blender 2.80

bl_info = {
    "name": "BigDisc Level Export Script",
    "author": "Andrew B",
    "version": (1, 0, 0),
    "blender": (2, 80, 0),
    "location": "File > Import-Export",
    "description": "Export blender level as panels",
    "warning": "",
    "wiki_url": "",
    "support": 'OFFICIAL',
    "category": "Import-Export"}

# To support reload properly, try to access a package var,
# if it's there, reload everything
if "bpy" in locals():
    import importlib
    if "export_lvl" in locals():
        importlib.reload(export_lvl)


import os
import bpy
from bpy.props import (
        CollectionProperty,
        StringProperty,
        BoolProperty,
        EnumProperty,
        FloatProperty,
        )
from bpy_extras.io_utils import (
        ImportHelper,
        ExportHelper,
        axis_conversion,
        orientation_helper
        )

@orientation_helper(axis_forward='Y', axis_up='Z')
class ExportLVL(bpy.types.Operator, ExportHelper):
    """Export list of panels, """
    bl_idname = "export_level.lvl"
    bl_label = "Export LVL"

    filename_ext = ".lvl"
    filter_glob: StringProperty(default="*.lvl", options={'HIDDEN'})

    #use_tangents: BoolProperty(
    #        name="Tangents",
    #        description="Export Tangents for smooth and "
    #                    "hard shaded faces "
    #                    "(hard shaded faces will be exported "
    #                    "as individual faces)",
    #        default=True,
    #        )

    global_scale: FloatProperty(
            name="Scale",
            min=0.01, max=1000.0,
            default=1.0,
            )

    @classmethod
    def poll(cls, context):
        return context.active_object is not None

    def execute(self, context):
        from . import export_lvl

        from mathutils import Matrix

        keywords = self.as_keywords(ignore=("axis_forward",
                                            "axis_up",
                                            "global_scale",
                                            "check_existing",
                                            "filter_glob",
                                            ))
        global_matrix = axis_conversion(to_forward=self.axis_forward,
                                        to_up=self.axis_up,
                                        ).to_4x4() @ Matrix.Scale(self.global_scale, 4)
        keywords["global_matrix"] = global_matrix

        filepath = self.filepath
        filepath = bpy.path.ensure_ext(filepath, self.filename_ext)

        return export_lvl.save(self, context, **keywords)

    def draw(self, context):
        layout = self.layout

        layout.prop(self, "axis_forward")
        layout.prop(self, "axis_up")
        layout.prop(self, "global_scale")


def menu_func_export(self, context):
    self.layout.operator(ExportLVL.bl_idname, text="Panel Level (.lvl)")


classes = (
    ExportLVL,
    )


def register():
    for cls in classes:
        bpy.utils.register_class(cls)

    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    for cls in classes:
        bpy.utils.unregister_class(cls)

    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
    register()
