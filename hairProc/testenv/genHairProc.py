from pxr import Usd, UsdGeom, Sdf

from vik import HairProc
import numpy as np

import os


def open_stage(stage_name):
    if os.path.isfile(stage_name):
        os.remove(stage_name)
    stage = Usd.Stage.CreateNew(stage_name)
    return stage


def build_plane(stage, name="plane", w=2, h=2, pos=(0,0,0)):
    """
    Build a simple plane for testing hair procedural
    """
    pts = np.zeros((4, 3))
    pts[0] = (pos[0] + w, pos[1] + h, pos[2])
    pts[1] = (pos[0] - w, pos[1] + h, pos[2])
    pts[2] = (pos[0] - w, pos[1] - h, pos[2])
    pts[3] = (pos[0] + w, pos[1] - h, pos[2])

    mesh = UsdGeom.Mesh.Define(stage, Sdf.Path("/" + name))
    mesh.CreatePointsAttr(pts)
    mesh.CreateFaceVertexIndicesAttr([0,1,2,3])
    mesh.CreateFaceVertexCountsAttr([4])
    return mesh


def build_tube(stage, name="tube", rows=5, columns=5, radius=1, height=2, caps=True, pos=(0,0,0)):
    """
    Build a tube for testing hair procedural
    """
    rows = max(rows, 2)
    columns = max(columns, 3)
    rc = rows * columns

    pts = np.zeros((rc, 3))
    for i in range(rows):
        y = (height / (rows-1)) * i
        for j in range(columns):
            v = ((j % columns)) / columns * np.pi * 2
            p = (pos[0] + np.sin(v) * radius, pos[1] + y, pos[2] + np.cos(v) * radius)

            pts[i*columns+j] = p

    indices = []
    counts = []

    # CAPS
    if caps:
        pts.resize((rc + 2, 3))
        pts[-2] = pos
        pts[-1] = (pos[0], pos[1] + height, pos[2])
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

    mesh = UsdGeom.Mesh.Define(stage, Sdf.Path("/"+name))
    mesh.CreatePointsAttr(pts)
    mesh.CreateFaceVertexIndicesAttr(indices)
    mesh.CreateFaceVertexCountsAttr(counts)
    return mesh


def build_hair(stage, target, count=15, path="/curves", faces=[], apply_api=True):
    """
    Create hair primitives on a target UsdPrim. Choose what faces to apply the hair to 
    for testing face indexes.
    """
    counts = target.GetFaceVertexCountsAttr().Get()
    indices = target.GetFaceVertexIndicesAttr().Get()
    pts = target.GetPointsAttr().Get()

    if not faces:
        faces = range(len(counts))

    l = count / len(faces)

    curve_cnt = [2] * count
    curve_pts = np.zeros((count * 2, 3))
    curve_w = np.full((1, count*2), 0.01)
    curve_prm = []
    curve_uvs = []
    curve_ups = []

    total = 0
    carried = 0

    for i in faces:
        c = counts[i]
        li = int(np.rint(l + carried))
        carried += l - li
        if li == 0:
            continue

        offset = 0
        if i > 0:
            offset = sum(counts[0:i])

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

        curve_pts.put(range(total, total + li * 6), np.stack((roots, tips), -2))        
        curve_prm += [i] * li

        # total += li * 6

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


def transform(stage, prim, nframes=10, speed=1, translate=True, rotate=False):
    """
    Set the xform of the UsdPrim
    """
    frames = range(nframes)
    stage.SetStartTimeCode(frames[0])
    stage.SetEndTimeCode(frames[-1])
    stage.SetFramesPerSecond(24)

    xform = UsdGeom.Xformable(prim)

    if rotate:
        op = xform.AddRotateYOp()
        for f in frames:
            op.Set((f / nframes) * speed * 360, f)

    if translate:
        op = xform.AddTranslateOp()
        for f in frames:
            op.Set((np.sin(f / nframes * 10) * 10, 0, 0), f)

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


def twist(stage, prim, nframes=10, speed=0.1):
    """
    twists the geometry to test skewing of faces
    """
    frames = range(nframes)
    stage.SetStartTimeCode(frames[0])
    stage.SetEndTimeCode(frames[-1])
    stage.SetFramesPerSecond(24)

    pts_attr = prim.GetPointsAttr()
    pts = np.array(pts_attr.Get())

    pts_attr.Set(pts, frames[0])

    def _func(v, t):
        x = v[0] * np.cos(t * v[1]) - v[2] * np.sin(t * v[1])
        y = v[1]
        z = v[0] * np.sin(t * v[1]) + v[2] * np.cos(t * v[1])
        return np.array((x, y, z))

    vf = np.vectorize(_func, signature="(n)->(n)")
    vf.excluded.add("t")

    for f in frames:
        pts = vf(v=pts, t=speed)
        pts_attr.Set(pts, f)


def do_stuffs(stage):
    """
    Build the stage for testing hair procedural
    """

    # for i in range(100):
    i = 0
    # tube0 = build_tube(stage, f"tube{i*3}", rows=3, columns=5, height=10, caps=True, pos=(0,0,0))
    # tube1 = build_tube(stage, f"tube{i*3+1}", rows=5, columns=10, height=10, caps=True, pos=(6,0,0))
    plane = build_plane(stage, f"plane1",  w=10, h=10)
    # for i in range(100):
    #     build_hair(stage, tube0, count=10, path=f"/curves{i*3}")
    #     build_hair(stage, tube1, count=10, path=f"/curves{i*3 + 1}")

    build_hair(stage, plane, count=100, path=f"/curves1", apply_api=True)
    transform(stage, plane, nframes=200, translate=False, rotate=True)
    # twist(stage, tube0, nframes=200, speed=0.01)
    # transform_pts(stage, tube1, nframes=200)



if __name__ == "__main__":
    stage_name = os.path.join(os.path.dirname(__file__), "hairProc.usda")

    stage = open_stage(stage_name)
    do_stuffs(stage)
    stage.Export(stage_name)