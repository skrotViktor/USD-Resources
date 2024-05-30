#ifndef CODEPROC_PROCEDURAL_DATA_SOURCES
#define CODEPROC_PROCEDURAL_DATA_SOURCES

#include "codeProceduralDeformer.h"
#include "codeProceduralDeformerArguments.h"

#include "pxr/base/vt/typeHeaders.h" // IWYU pragma: keep.
#include "pxr/base/vt/value.h"
#include "pxr/imaging/hd/dataSource.h"
#include "pxr/imaging/hd/primvarsSchema.h"
#include "pxr/pxr.h"

/*
Class declaration of the data sources used for CodeProc.

The structure of the dataSources will result in:

┌─────────────────────────────┐
│                             │
│ CodeProcDataSourceContainer │
│                             │
└───────────┬─────────────────┘
            │
┌───────────▼───────────────────────┐   ┌───────────────────────┐
│                                   │   │                       │
│ CodeProcPrimvarsOverrideDataSource├───►  CodeProcCodeDeformer │
│                                   │   │                       │
└───────────┬───────────────────────┘   └────────────┬──────────┘
            │                                        │
            │                                        │
 ┌──────────▼────────────────────────┐               │
 │                                   │               │
 │ CodeProcPrimvarOverrideDataSource │               │
 │                                   │               │
 └──────────┬────────────────────────┘               │
            │                                        │
 ┌──────────▼─────────────────────┐                  │
 │                                │                  │
 │ CodeProcPrimvarValueDataSource ◄──────────────────┘
 │                                │
 └────────────────────────────────┘

Where the CodeProcDataSourceContainer is the container data source attached to a
HdSceneIndexPrim with the CodeProcAPI schema attached.
The CodeProcPrimvarsOverrideDataSource is the primvar container data source
source responsible for handling primvar queries, and will return a
CodeProcPrimvarOverrideDataSource for the target primvars. If client requests
primvarValue from the CodeProcPrimvarOverrideDataSource, it will supply the
CodeProcPrimvarValueDataSource

The result value of CodeProcPrimvarValueDataSource is based on the
CodeProcCodeDeformer result

*/

PXR_NAMESPACE_OPEN_SCOPE

/* --------------------- CodeProcPrimvarValueDataSource --------------------- */
/*
This class is responsible for providing access to primvar values after invoking
OpenCL kernel
*/

template <class T>
class CodeProcPrimvarValueDataSource final
    : public HdTypedSampledDataSource<T> {
  public:
    HD_DECLARE_DATASOURCE(CodeProcPrimvarValueDataSource<T>);

    using Type = T;

    VtValue GetValue(const HdSampledDataSource::Time t = 0) override;
    Type GetTypedValue(const HdSampledDataSource::Time t = 0) override;

    bool GetContributingSampleTimesForInterval(
        HdSampledDataSource::Time startTime, HdSampledDataSource::Time endTime,
        std::vector<HdSampledDataSource::Time> *outSampleTimes) override;

  private:
    CodeProcPrimvarValueDataSource(
        const Deformer::Argument &argument,
        Deformer::CodeProcCodeProceduralDeformerSharedPtr deformer,
        const TfToken &interpolation, const TfToken &role)
        : _argument(argument), _interpolation(interpolation), _role(role),
          _deformer(deformer) {}

    Deformer::CodeProcCodeProceduralDeformerSharedPtr _deformer;
    TfToken _interpolation;
    TfToken _role;
    Deformer::Argument _argument;
};

/* -------------------- CodeProcPrimvarOverrideDataSource ------------------- */
/*
Override the data source for a single primvar
*/
class CodeProcPrimvarOverrideDataSource final : public HdContainerDataSource {
  public:
    HD_DECLARE_DATASOURCE(CodeProcPrimvarOverrideDataSource);

    TfTokenVector GetNames() override;
    HdDataSourceBaseHandle Get(const TfToken &name) override;

  private:
    CodeProcPrimvarOverrideDataSource(
        const HdContainerDataSourceHandle &inputDs,
        const HdSampledDataSourceHandle &primvarValueSrc,
        const TfToken &interpolation, const TfToken &role)
        : _valueDs(primvarValueSrc), _interpolation(interpolation),
          _role(role) {}

    HdSampledDataSourceHandle _valueDs;
    HdContainerDataSourceHandle _inputDs;
    TfToken _interpolation;
    TfToken _role;
};

/* -------------------- CodeProcPrimvarsOverrideDataSource -------------------
 */
/*
This class is resposible for providing values data sources based on requested
primvar name. Get() method will return CodeProcPrimvarValueDataSource<T> for all
writable attributes in the CodeProcAPI prim.
*/

class CodeProcPrimvarsOverrideDataSource final : public HdContainerDataSource {
  public:
    HD_DECLARE_DATASOURCE(CodeProcPrimvarsOverrideDataSource);

    TfTokenVector GetNames() override;
    HdDataSourceBaseHandle Get(const TfToken &name) override;

  private:
    CodeProcPrimvarsOverrideDataSource(
        const SdfPath &primPath, const HdContainerDataSourceHandle &inputDs,
        const HdPrimvarsSchema primvarSchema,
        Deformer::CodeProcCodeProceduralDeformerSharedPtr deformer)
        : _primPath(primPath), _inputDs(inputDs), _schema(primvarSchema),
          _deformer(deformer) {}

    SdfPath _primPath;
    HdContainerDataSourceHandle _inputDs;
    HdPrimvarsSchema _schema;
    Deformer::CodeProcCodeProceduralDeformerSharedPtr _deformer;
};

/* ----------------------- CodeProcDataSourceContainer ---------------------- */
/*
Container data source for handling target datasources and CodeProcAPI data
source
*/
class CodeProcDataSourceContainer final : public HdContainerDataSource {
  public:
    HD_DECLARE_DATASOURCE(CodeProcDataSourceContainer);

    TfTokenVector GetNames() override;
    HdDataSourceBaseHandle Get(const TfToken &name) override;

  private:
    CodeProcDataSourceContainer(
        const SdfPath &primPath,
        const HdContainerDataSourceHandle &primDataSource,
        Deformer::CodeProcCodeProceduralDeformerSharedPtr deformer)
        : _primPath(primPath), _primDs(primDataSource), _deformer(deformer) {}

    HdContainerDataSourceHandle _primDs;

    // TODO: Currently only supports one deformer for an instance of this ds,
    // which means a prim can only be affected by one deformer
    Deformer::CodeProcCodeProceduralDeformerSharedPtr _deformer;
    SdfPath _primPath;
};

/* ------------------------------ factory impl ------------------------------ */
namespace {

typedef HdSampledDataSourceHandle (*_DataSourceFactory)(
    const Deformer::Argument &argument,
    Deformer::CodeProcCodeProceduralDeformerSharedPtr deformer,
    const TfToken &interpolation, const TfToken &role);

using _FactoryMap = std::unordered_map<std::string, _DataSourceFactory>;

template <typename T>
HdSampledDataSourceHandle
_FactoryImpl(const Deformer::Argument &argument,
             Deformer::CodeProcCodeProceduralDeformerSharedPtr deformer,
             const TfToken &interpolation, const TfToken &role) {
    return CodeProcPrimvarValueDataSource<T>::New(argument, deformer,
                                                  interpolation, role);
}

#define MAP_CONSTR_FUNC(unused, elem)                                          \
    map[TF_PP_STRINGIZE(VT_TYPE(elem))] = _FactoryImpl<VT_TYPE(elem)>;         \
    map[TF_PP_STRINGIZE(VtArray<VT_TYPE(elem)>)] =                             \
        _FactoryImpl<VtArray<VT_TYPE(elem)>>;

#define MAP_CONSTR(X) TF_PP_SEQ_FOR_EACH(MAP_CONSTR_FUNC, ~, X);

static inline _FactoryMap _CreateFactoryMap() {
    _FactoryMap map;
    MAP_CONSTR(VT_SCALAR_VALUE_TYPES);

    return map;
}

} // namespace

PXR_NAMESPACE_CLOSE_SCOPE

#endif