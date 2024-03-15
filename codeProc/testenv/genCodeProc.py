from pxr import Usd, UsdGeom, Sdf

from vik import CodeProc
import numpy as np
import os


def open_stage(stage_name):
    if os.path.isfile(stage_name):
        os.remove(stage_name)
    stage = Usd.Stage.CreateNew(stage_name)
    return stage


def build_plane(stage, name="/plane", w=2, h=2, pos=(0,0,0)):
    """
    Build a simple plane for testing hair procedural
    """
    pts = np.zeros((4, 3))
    pts[0] = (pos[0] + w, pos[1] + h*2, pos[2])
    pts[1] = (pos[0] - w, pos[1] + h*2, pos[2])
    pts[3] = (pos[0] + w, 0, pos[2])
    pts[2] = (pos[0] - w, 0, pos[2])

    mesh = UsdGeom.Mesh.Define(stage, Sdf.Path(name))
    mesh.CreatePointsAttr(pts)
    mesh.CreateFaceVertexIndicesAttr([0,1,2,3])
    mesh.CreateFaceVertexCountsAttr([4])
    return mesh


def do_stuffs(stage):
    """
    Build the stage for testing hair procedural
    """
    plane = build_plane(stage, "/plane",  w=5, h=5)
    api = CodeProc.CodeProceduralAPI.Apply(plane.GetPrim())


if __name__ == "__main__":
    stage_name = os.path.join(os.path.dirname(__file__), "codeProc.usda")

    stage = open_stage(stage_name)
    do_stuffs(stage)
    stage.Export(stage_name)