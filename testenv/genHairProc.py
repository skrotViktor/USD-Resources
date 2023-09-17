from pxr import Usd, UsdGeom, Sdf
from pxr import UsdProc 

from vik import HairProc
import numpy as np

import os


def open_stage(stage_name):
    if os.path.isfile(stage_name):
        os.remove(stage_name)
    stage = Usd.Stage.CreateNew(stage_name)
    return stage


def build_tube(stage, sides=5, radius=1, height=2):
    pts = np.zeros((sides*2 + 2, 3))
    for i in range(sides*2):
        v = ((i % sides)) / sides * np.pi * 2
        y = height if i >= sides else 0
        p = (np.sin(v) * radius, y, np.cos(v) * radius)

        pts[i+1] = p

    pts[-1] = (0,height,0)
    indices = []
    counts = []

    # CAPS
    for i in range(1, sides + 1):
        indices.append(i + 1 if i != sides else 1)
        indices.append(i)
        indices.append(0)
        counts.append(3)

    for i in range(sides+1, sides * 2 + 1):
        indices.append(sides * 2 + 1)
        indices.append(i)
        indices.append(i + 1 if i != sides * 2 else sides+1)
        counts.append(3)

    # SIDES
    for i in range(1, sides + 1):
        indices.append(i + 1 if i != sides else 1)
        indices.append(i + sides + 1 if i != sides else sides+1)
        indices.append(i + sides)
        indices.append(i)
        counts.append(4)


    mesh = UsdGeom.Mesh.Define(stage, Sdf.Path("/tube"))
    mesh.CreatePointsAttr(pts)
    mesh.CreateFaceVertexIndicesAttr(indices)
    mesh.CreateFaceVertexCountsAttr(counts)
    return mesh


def build_hair(stage, target, count=100):
    pts = target.GetPointsAttr().Get()
    indices = target.GetFaceVertexIndicesAttr().Get()
    counts = target.GetFaceVertexCountsAttr().Get()
    
    l = count / len(counts)
    curve_cnt = [2] * count
    curve_pts = np.zeros((count * 2, 3))
    curve_w = [5.0] * count * 2
    curve_prm = []
    curve_uvs = []

    total = 0
    carried = 0
    offset = 0
    for i, c in enumerate(counts):
        li = int(np.rint(l + carried))
        carried += l - li
        if not li:
            continue
        p = np.zeros((c, 3))
        for j in range(c):
            p[j] = pts[indices[j + offset]]

        e = p[1:] - p[0]
        n = np.cross(e[0], e[1])
        n /= np.linalg.norm(n)

        r = np.random.rand(li, c-1)

        roots = np.zeros((li, 3), dtype=np.float64)
        for j in range(li):
            curve_uvs.append((r[j][0], r[j][1]))
            roots[j] = np.sum((e.T * r[j]).T + p[0], -2) / (c-1)
        tips = roots + n

        curve_pts.put(range(total, total + li * 6), np.stack((roots, tips), -2))        
        curve_prm += [i] * li

        offset += c
        total += li * 6

    hair = UsdGeom.BasisCurves.Define(stage, Sdf.Path("/hair/curves"))
    hair.CreatePointsAttr(curve_pts)
    hair.CreateCurveVertexCountsAttr(curve_cnt)
    hair.CreateWidthsAttr(curve_w)
    hair.CreateTypeAttr("linear")

    proc = HairProc.HairProcedural.Define(stage, Sdf.Path("/hair/proc"))
    proc_rel = proc.CreateTargetsRel()
    proc_rel.AddTarget(hair.GetPath())

    api = HairProc.HairProceduralAPI.Apply(hair.GetPrim())
    api.CreatePrimAttr(curve_prm)
    api.CreateUvAttr(curve_uvs)
    rel = api.CreateTargetRel()
    rel.SetTargets([target.GetPath()])
    return hair


def do_stuffs(stage):
    tube = build_tube(stage)
    hair = build_hair(stage, tube)


if __name__ == "__main__":
    stage_name = "hairProc.usda"

    stage = open_stage(stage_name)
    do_stuffs(stage)
    stage.Export(stage_name)