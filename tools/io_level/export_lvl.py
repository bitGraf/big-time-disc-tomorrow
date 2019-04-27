"""
This script exports LVL files from Blender.
"""

import bpy
import os

def save_level(filepath,collec, B2D):
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

    #B2D = Matrix()#[[0, 0, 1], [1, 0, 0], [0, 1, 0]])
    #B2D.invert()
    numPanels = 0

    for obj in collec.objects:
        mesh = obj.data
        mesh.calc_tangents()
        for face in mesh.polygons:
            numPanels = numPanels + 1

    fw("NumPanels: %d\n" % numPanels)
    for obj in collec.objects:
        mesh = obj.data
        mesh.calc_tangents()
        m = obj.matrix_world
        s = obj.scale
        for face in mesh.polygons:
            panelLocalPosition = face.center #-> in local coords, needs to be in world coords
            panelPosition = m @ Vector([panelLocalPosition.x, panelLocalPosition.y, panelLocalPosition.z, 1])
            for vert in [mesh.loops[i] for i in face.loop_indices]:
                T = vert.tangent
                B = vert.bitangent
                panelT_ = m @ Vector([T.x, T.y, T.z, 0])
                panelB_ = m @ Vector([B.x, B.y, B.z, 0])
            panelPos = Vector([panelPosition.x, panelPosition.y, panelPosition.z])
            panelT = Vector([panelT_.x, panelT_.y, panelT_.z])
            panelT.normalize()
            panelB = Vector([panelB_.x, panelB_.y, panelB_.z])
            panelB.normalize()
            panelN = panelT.cross(panelB)
            fw("%.6f %.6f %.6f" % (panelPos.x, panelPos.y, panelPos.z))
            fw(" %.6f %.6f %.6f"   % (panelT.x, panelB.x, panelN.x))
            fw(" %.6f %.6f %.6f"   % (panelT.y, panelB.y, panelN.y))
            fw(" %.6f %.6f %.6f"   % (panelT.z, panelB.z, panelN.z))
            fw(" %.6f %.6f %.6f\n" % (s.z, s.z, s.z))
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
