from pxr import Usd, UsdGeom, Sdf

from vik import HairProc
import numpy as np

import os


def open_stage(stage_name):
    if os.path.isfile(stage_name):
        os.remove(stage_name)
    stage = Usd.Stage.CreateNew(stage_name)
    return stage


def bend(stage, prim, height=2, bend=90):
    frames = [0, 10]
    stage.SetStartTimeCode(frames[0])
    stage.SetEndTimeCode(frames[-1])
    stage.SetFramesPerSecond(24)
    mesh = UsdGeom.PointBased.Get(stage, prim.GetPath())
    attr = mesh.GetPointsAttr()
    pts = np.array(attr.Get())
    attr.Set(pts, frames[0])

    for p in pts:
        y = p[1]
        if y == 0:
            continue

        bend_amt = (p[1] / height) * bend
        print(bend_amt)
        p[0] += np.cos(bend_amt)
        p[1] -= np.cos(bend_amt)

    attr.Set(pts, frames[-1])

def build_plane(stage, w=2, h=2):
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


def build_tube(stage, rows=5, columns=5, radius=1, height=2, caps=True):
    rows = max(rows, 2)
    columns = max(columns, 3)

    rc = rows * columns

    pts = np.zeros((rc, 3))
    for i in range(rows):
        y = (height / (rows-1)) * i
        for j in range(columns):
            v = ((j % columns)) / columns * np.pi * 2
            p = (np.sin(v) * radius, y, np.cos(v) * radius)

            pts[i*columns+j] = p

    indices = []
    counts = []

    # CAPS
    if caps:
        pts.resize((rc + 2, 3))
        pts[-2] = (0,0,0)
        pts[-1] = (0,height,0)
        for i in range(columns):
            indices.append(i + 1 if i + 1 != columns else 0)
            indices.append(i)
            indices.append(rc)
            counts.append(3)

        for i in range(rc-columns, rc):
            indices.append(rc + 1)
            indices.append(i)
            indices.append(i + 1 if i != rc-1 else rc-columns)
            counts.append(3)

    # SIDES
    for i in range(rows-1):
        for j in range(columns):
            idx = i*columns+j
            indices.append(idx + 1 if j != columns-1 else i*columns)
            indices.append(idx + columns + 1 if j != columns-1 else i*columns+columns)
            indices.append(idx + columns)
            indices.append(idx)
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
            offset += c
            continue

        p = np.zeros((c, 3))
        for j in range(c):
            p[j] = pts[indices[j + offset]]

        e = p[1:] - p[0]
        n = np.cross(e[0], e[1])
        d = np.dot(n, n)
        if d != 0.0:
            n /= np.sqrt(d)

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

        # print(total)
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


def animate(stage, prim, nframes=10, speed=0.1):
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
    mesh = build_tube(stage, rows=2, columns=5, height=10, caps=True)
    # mesh = build_plane(stage)

    for i in range(200):
        build_hair(stage, mesh, count=10, path=f"/curves{i}", apply_api=True)
    # build_hair(stage, mesh, count=9000, path="/curves2")

    animate(stage, mesh, nframes=200)
    # bend(stage, mesh, height=10)


if __name__ == "__main__":
    stage_name = os.path.join(os.path.dirname(__file__), "hairProc.usda")
    
    stage = open_stage(stage_name)
    do_stuffs(stage)
    stage.Export(stage_name)