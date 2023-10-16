from pxr import Usd, UsdGeom, Sdf

from vik import HairProc
import numpy as np

import os


def open_stage(stage_name):
    if os.path.isfile(stage_name):
        os.remove(stage_name)
    stage = Usd.Stage.CreateNew(stage_name)
    return stage


def build_plane(stage, w=1, h=1):
    pts = np.zeros((4, 3))
    pts[0] = (w, h, 0)
    pts[1] = (-w, h, 0)
    pts[2] = (-w, -h, 0)
    pts[3] = (w, -h, 0)

    mesh = UsdGeom.Mesh.Define(stage, Sdf.Path("/plane"))
    mesh.CreatePointsAttr(pts)
    mesh.CreateFaceVertexIndicesAttr([0,1,2,3])
    mesh.CreateFaceVertexCountsAttr([4])
    return mesh


def build_tube(stage, sides=5, radius=1, height=2, caps=True):
    pts = np.zeros((sides*2, 3))
    for i in range(sides*2):
        v = ((i % sides)) / sides * np.pi * 2
        y = height if i >= sides else 0
        p = (np.sin(v) * radius, y, np.cos(v) * radius)

        pts[i] = p

    indices = []
    counts = []

    if caps:
        pts.resize((sides*2 + 2, 3))
        pts[-2] = (0,0,0)
        pts[-1] = (0,height,0)
        # CAPS
        for i in range(sides):
            indices.append(i + 1 if i + 1 != sides else 0)
            indices.append(i)
            indices.append(sides * 2)
            counts.append(3)

        for i in range(sides, sides * 2 + 1):
            indices.append(sides * 2 + 1)
            indices.append(i)
            indices.append(i + 1 if i + 1 != sides * 2 else sides)
            counts.append(3)

    # SIDES
    for i in range(sides ):
        indices.append(i + 1 if i != sides-1 else 0)
        indices.append(i + sides + 1 if i != sides-1 else sides)
        indices.append(i + sides)
        indices.append(i)
        counts.append(4)


    mesh = UsdGeom.Mesh.Define(stage, Sdf.Path("/tube"))
    mesh.CreatePointsAttr(pts)
    mesh.CreateFaceVertexIndicesAttr(indices)
    mesh.CreateFaceVertexCountsAttr(counts)
    return mesh


def build_hair(stage, target, count=15, path="/curves", apply_api=True):
    counts = target.GetFaceVertexCountsAttr().Get()
    indices = target.GetFaceVertexIndicesAttr().Get()
    pts = target.GetPointsAttr().Get()

    l = count / len(counts)
    curve_cnt = [2] * count
    curve_pts = np.zeros((count * 2, 3))
    curve_w = [0.01] * count * 2
    curve_prm = []
    curve_uvs = []
    curve_ups = []

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

        r = np.random.random((li, 2))
        roots = np.zeros((li, 3), dtype=np.float64)

        for j in range(li):
            u = r[j][0]
            v = r[j][1]
            if c == 3:
                u /= 2
                v /= 2
                w = 1-u-v
                roots[j] = p[0]*w + p[1]*u + p[2]*v

            elif c == 4:
                roots[j] = p[0]*(1-u)*(1-v) + p[1]*(1-u)*v + p[2]*u*v + p[3]*u*(1-v)

            curve_uvs.append((u, v))
            curve_ups.append(n)
        tips = roots + n

        curve_pts.put(range(total, total + li * 6), np.stack((roots, tips), -2))        
        curve_prm += [i] * li

        offset += c
        total += li * 6

    hair = UsdGeom.BasisCurves.Define(stage, Sdf.Path(path))
    hair.CreatePointsAttr(curve_pts)
    hair.CreateCurveVertexCountsAttr(curve_cnt)
    hair.CreateWidthsAttr(curve_w)
    hair.CreateTypeAttr("linear")

    if apply_api:
        api = HairProc.HairProceduralAPI.Apply(hair.GetPrim())
        api.CreatePrimAttr(curve_prm)
        api.CreateParamuvAttr(curve_uvs)
        api.CreateRestAttr(pts)
        rel = api.CreateTargetRel()
        rel.SetTargets([target.GetPath()])

        assert(hair.GetPrim().HasAPI("HairProceduralAPI"))
        assert(hair.GetPrim().HasAPI(HairProc.HairProceduralAPI))

    return hair


def animate(stage, prim, samples=10, speed=2):
    frames = [1001, 1100]
    stage.SetStartTimeCode(frames[0])
    stage.SetEndTimeCode(frames[1])
    stage.SetFramesPerSecond(24)

    pts_attr = prim.GetPointsAttr()
    pts = np.array(pts_attr.Get())

    step = (frames[1] - frames[0]) / samples
    pts_attr.Set(pts, frames[0])

    frame = frames[0] + step

    def _func(v, t):
        x = v[0] * np.cos(t) - v[2] * np.sin(t)
        y = v[1]
        z = v[0] * np.sin(t) + v[2] * np.cos(t)
        return np.array((x, y, z))

    vf = np.vectorize(_func, signature="(n)->(n)")
    vf.excluded.add("t")

    for sample in range(samples+1):
        # s = (sample/samples) * np.pi * 2 * speed
        # rot = np.array((np.sin(s), 0, np.cos(s)))
        pts = vf(v=pts, t=0.5)
        # pts += np.array((np.sin(sample), 0, np.cos(sample)))
        pts_attr.Set(pts, frame)
        frame += step


def do_stuffs(stage):
    # mesh = build_tube(stage, sides=4, caps=False)
    mesh = build_plane(stage)
    build_hair(stage, mesh, count=1, path="/curves")
    # build_hair(stage, tube, path="/no_api_curves", apply_api=False)

    animate(stage, mesh)


if __name__ == "__main__":
    stage_name = os.path.join(os.path.dirname(__file__), "hairProc.usda")
    
    stage = open_stage(stage_name)
    do_stuffs(stage)
    stage.Export(stage_name)