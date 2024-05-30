#ifndef HAIRPROC_PROCEDURAL_DATA_SOURCES
#define HAIRPROC_PROCEDURAL_DATA_SOURCES

#include "pxr/imaging/hd/dataSource.h"
#include "pxr/imaging/hd/tokens.h"
#include "pxr/pxr.h"

#include "pxr/imaging/hd/basisCurvesSchema.h"
#include "pxr/imaging/hd/primvarSchema.h"
#include "pxr/imaging/hd/primvarsSchema.h"

#include "hairProceduralDeformer.h"
#include "tokens.h"

#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE

class _PrimvarDataSource final : public HdContainerDataSource {
  public:
    HD_DECLARE_DATASOURCE(_PrimvarDataSource);

    TfTokenVector GetNames() override;
    HdDataSourceBaseHandle Get(const TfToken &name) override;

  private:
    _PrimvarDataSource(const HdDataSourceBaseHandle &primvarValueSrc,
                       const TfToken &interpolation, const TfToken &role)
        : _primvarValueSrc(primvarValueSrc), _interpolation(interpolation),
          _role(role) {}

    HdDataSourceBaseHandle _primvarValueSrc;
    TfToken _interpolation;
    TfToken _role;
};

class _PointsDataSource : public HdVec3fArrayDataSource {
  public:
    HD_DECLARE_DATASOURCE(_PointsDataSource);

    VtValue GetValue(const Time shutterOffset) override;
    VtVec3fArray GetTypedValue(const Time shutterOffset) override;
    bool GetContributingSampleTimesForInterval(
        const Time startTime, const Time endTime,
        std::vector<Time> *const outSampleTimes) override;

  private:
    _PointsDataSource(HdPrimvarsSchema &primvarSchema,
                      HairProcHairProceduralDeformerSharedPtr deformer)
        : _schema(primvarSchema), _deformer(deformer) {}

    HdPrimvarsSchema _schema;
    HairProcHairProceduralDeformerSharedPtr _deformer;
};

class _PrimvarOverrideDataSource : public HdContainerDataSource {
  public:
    HD_DECLARE_DATASOURCE(_PrimvarOverrideDataSource);

    TfTokenVector GetNames() override;
    HdDataSourceBaseHandle Get(const TfToken &name) override;

  private:
    _PrimvarOverrideDataSource(const HdContainerDataSourceHandle &inputDs,
                               HdPrimvarsSchema &primvarSchema,
                               HairProcHairProceduralDeformerSharedPtr deformer)
        : _inputDs(inputDs), _schema(primvarSchema), _deformer(deformer) {}

    HdContainerDataSourceHandle _inputDs;
    HdPrimvarsSchema _schema;
    HairProcHairProceduralDeformerSharedPtr _deformer;
};

// class _HairProcDataSource
// used as override to access primvars on a HairProceduralAPI prim
class _HairProcDataSource : public HdContainerDataSource {
  public:
    HD_DECLARE_DATASOURCE(_HairProcDataSource);

    TfTokenVector GetNames() override;
    HdDataSourceBaseHandle Get(const TfToken &name) override;

  private:
    _HairProcDataSource(const SdfPath &primPath,
                        const HdContainerDataSourceHandle &primDataSource,
                        HairProcHairProceduralDeformerSharedPtr deformer)
        : _primPath(primPath), _primDs(primDataSource), _deformer(deformer) {}

    HdContainerDataSourceHandle _primDs;
    HairProcHairProceduralDeformerSharedPtr _deformer;
    const SdfPath &_primPath;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif