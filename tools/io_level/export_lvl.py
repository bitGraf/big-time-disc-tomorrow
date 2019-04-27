"""
This script exports LVL files from Blender.
"""

import bpy
import os

def save_level(filepath,collec, global_matrix):
    from mathutils import Matrix
    from mathutils import Vector

    def rvec3d(v):
        return round(v[0], 6), round(v[1], 6), round(v[2], 6)

    def rvec2d(v):
        return round(v[0], 6), round(v[1], 6)

    file = open(filepath, "w", encoding="utf8", newline="\n")
    fw = file.write

    fw("comment Created by Blender %s - "
       "www.blender.org, source file: %r\n" %
       (bpy.app.version_string, os.path.basename(bpy.data.filepath)))

    numPanels = 0

    for obj in collec.objects:
        mesh = obj.data
        for face in mesh.polygons:
            numPanels = numPanels + 1

    fw("NumPanels: %d\n" % numPanels)
    for obj in collec.objects:
        mesh = obj.data.copy()
        mesh.transform(global_matrix @ obj.matrix_world)
        for face in mesh.polygons:
            for vert in [mesh.loops[i] for i in face.loop_indices]:
                vidx = vert.vertex_index
                pos_ = mesh.vertices[vidx].co
                pos = Vector([pos_.x, pos_.y, pos_.z, 1])
                fw("%.9f %.9f %.9f " % (pos.x, pos.y, pos.z))
            fw("\n")
    fw("DONE\n")

    file.close()
    print("writing %r done" % filepath)

    return {'FINISHED'}


def save(operator,
         context,
         filepath="",
         global_matrix=None
         ):

    collec = bpy.data.collections['EXPORT']

    if global_matrix is None:
        from mathutils import Matrix
        global_matrix = Matrix()

    if bpy.ops.object.mode_set.poll():
        bpy.ops.object.mode_set(mode='OBJECT')

    #mesh.transform(global_matrix @ obj.matrix_world)

    ret = save_level(filepath, collec, global_matrix)

    return ret
