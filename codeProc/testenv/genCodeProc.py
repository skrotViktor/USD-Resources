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


def transform_pts(stage, prim, nframes=10, speed=0.1):
    """
    Transforms each point idividually.
    """
    frames = range(nframes)
    stage.SetStartTimeCode(frames[0])
    stage.SetEndTimeCode(frames[-1])
    stage.SetFramesPerSecond(24)

    pts_attr = prim.GetPointsAttr()
    pts = np.array(pts_attr.Get())

    pts_attr.Set(pts, frames[0])

    def _func(v, t):
        x = v[0] * np.cos(t) - v[2] * np.sin(t)
        y = v[1]
        z = v[0] * np.sin(t) + v[2] * np.cos(t)
        return np.array((x, y, z))

    vf = np.vectorize(_func, signature="(n)->(n)")
    vf.excluded.add("t")

    for f in frames:
        pts = vf(v=pts, t=speed)
        pts += np.array((np.sin(f / nframes) * speed, 0, np.cos(f / nframes) * speed))
        pts_attr.Set(pts, f)


def do_stuffs(stage):
    """
    Build the stage for testing hair procedural
    """
    plane0 = build_plane(stage, "/plane0",  w=5, h=5)
    plane1 = build_plane(stage, "/plane1",  w=5, h=5)
    transform_pts(stage, plane1, 20)
    CodeProc.CodeProceduralAPI.Apply(plane0.GetPrim())


if __name__ == "__main__":
    stage_name = os.path.join(os.path.dirname(__file__), "codeProc.usda")

    stage = open_stage(stage_name)
    do_stuffs(stage)
    stage.Export(stage_name)