//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#include "./hairProcedural.h"
#include "pxr/usd/usd/schemaRegistry.h"
#include "pxr/usd/usd/typed.h"

#include "pxr/usd/sdf/types.h"
#include "pxr/usd/sdf/assetPath.h"

PXR_NAMESPACE_OPEN_SCOPE

// Register the schema with the TfType system.
TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<HairProcHairProcedural,
        TfType::Bases< UsdProcGenerativeProcedural > >();
    
    // Register the usd prim typename as an alias under UsdSchemaBase. This
    // enables one to call
    // TfType::Find<UsdSchemaBase>().FindDerivedByName("HairProcedural")
    // to find TfType<HairProcHairProcedural>, which is how IsA queries are
    // answered.
    TfType::AddAlias<UsdSchemaBase, HairProcHairProcedural>("HairProcedural");
}

/* virtual */
HairProcHairProcedural::~HairProcHairProcedural()
{
}

/* static */
HairProcHairProcedural
HairProcHairProcedural::Get(const UsdStagePtr &stage, const SdfPath &path)
{
    if (!stage) {
        TF_CODING_ERROR("Invalid stage");
        return HairProcHairProcedural();
    }
    return HairProcHairProcedural(stage->GetPrimAtPath(path));
}

/* static */
HairProcHairProcedural
HairProcHairProcedural::Define(
    const UsdStagePtr &stage, const SdfPath &path)
{
    static TfToken usdPrimTypeName("HairProcedural");
    if (!stage) {
        TF_CODING_ERROR("Invalid stage");
        return HairProcHairProcedural();
    }
    return HairProcHairProcedural(
        stage->DefinePrim(path, usdPrimTypeName));
}

/* virtual */
UsdSchemaKind HairProcHairProcedural::_GetSchemaKind() const
{
    return HairProcHairProcedural::schemaKind;
}

/* static */
const TfType &
HairProcHairProcedural::_GetStaticTfType()
{
    static TfType tfType = TfType::Find<HairProcHairProcedural>();
    return tfType;
}

/* static */
bool 
HairProcHairProcedural::_IsTypedSchema()
{
    static bool isTyped = _GetStaticTfType().IsA<UsdTyped>();
    return isTyped;
}

/* virtual */
const TfType &
HairProcHairProcedural::_GetTfType() const
{
    return _GetStaticTfType();
}

UsdAttribute
HairProcHairProcedural::GetProceduralTypeAttr() const
{
    return GetPrim().GetAttribute(HairProcTokens->primvarsHdGpProceduralType);
}

UsdAttribute
HairProcHairProcedural::CreateProceduralTypeAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(HairProcTokens->primvarsHdGpProceduralType,
                       SdfValueTypeNames->Token,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
HairProcHairProcedural::GetProceduralSystemAttr() const
{
    return GetPrim().GetAttribute(HairProcTokens->proceduralSystem);
}

UsdAttribute
HairProcHairProcedural::CreateProceduralSystemAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(HairProcTokens->proceduralSystem,
                       SdfValueTypeNames->Token,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdRelationship
HairProcHairProcedural::GetTargetsRel() const
{
    return GetPrim().GetRelationship(HairProcTokens->hairprocTargets);
}

UsdRelationship
HairProcHairProcedural::CreateTargetsRel() const
{
    return GetPrim().CreateRelationship(HairProcTokens->hairprocTargets,
                       /* custom = */ false);
}

namespace {
static inline TfTokenVector
_ConcatenateAttributeNames(const TfTokenVector& left,const TfTokenVector& right)
{
    TfTokenVector result;
    result.reserve(left.size() + right.size());
    result.insert(result.end(), left.begin(), left.end());
    result.insert(result.end(), right.begin(), right.end());
    return result;
}
}

/*static*/
const TfTokenVector&
HairProcHairProcedural::GetSchemaAttributeNames(bool includeInherited)
{
    static TfTokenVector localNames = {
        HairProcTokens->primvarsHdGpProceduralType,
        HairProcTokens->proceduralSystem,
    };
    static TfTokenVector allNames =
        _ConcatenateAttributeNames(
            UsdProcGenerativeProcedural::GetSchemaAttributeNames(true),
            localNames);

    if (includeInherited)
        return allNames;
    else
        return localNames;
}

PXR_NAMESPACE_CLOSE_SCOPE

// ===================================================================== //
// Feel free to add custom code below this line. It will be preserved by
// the code generator.
//
// Just remember to wrap code in the appropriate delimiters:
// 'PXR_NAMESPACE_OPEN_SCOPE', 'PXR_NAMESPACE_CLOSE_SCOPE'.
// ===================================================================== //
// --(BEGIN CUSTOM CODE)--
