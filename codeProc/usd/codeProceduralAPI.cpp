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
#include "./codeProceduralAPI.h"
#include "pxr/usd/usd/schemaRegistry.h"
#include "pxr/usd/usd/typed.h"

#include "pxr/usd/sdf/types.h"
#include "pxr/usd/sdf/assetPath.h"

PXR_NAMESPACE_OPEN_SCOPE

// Register the schema with the TfType system.
TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<CodeProcCodeProceduralAPI,
        TfType::Bases< UsdAPISchemaBase > >();
    
}

/* virtual */
CodeProcCodeProceduralAPI::~CodeProcCodeProceduralAPI()
{
}

/* static */
CodeProcCodeProceduralAPI
CodeProcCodeProceduralAPI::Get(const UsdStagePtr &stage, const SdfPath &path)
{
    if (!stage) {
        TF_CODING_ERROR("Invalid stage");
        return CodeProcCodeProceduralAPI();
    }
    return CodeProcCodeProceduralAPI(stage->GetPrimAtPath(path));
}


/* virtual */
UsdSchemaKind CodeProcCodeProceduralAPI::_GetSchemaKind() const
{
    return CodeProcCodeProceduralAPI::schemaKind;
}

/* static */
bool
CodeProcCodeProceduralAPI::CanApply(
    const UsdPrim &prim, std::string *whyNot)
{
    return prim.CanApplyAPI<CodeProcCodeProceduralAPI>(whyNot);
}

/* static */
CodeProcCodeProceduralAPI
CodeProcCodeProceduralAPI::Apply(const UsdPrim &prim)
{
    if (prim.ApplyAPI<CodeProcCodeProceduralAPI>()) {
        return CodeProcCodeProceduralAPI(prim);
    }
    return CodeProcCodeProceduralAPI();
}

/* static */
const TfType &
CodeProcCodeProceduralAPI::_GetStaticTfType()
{
    static TfType tfType = TfType::Find<CodeProcCodeProceduralAPI>();
    return tfType;
}

/* static */
bool 
CodeProcCodeProceduralAPI::_IsTypedSchema()
{
    static bool isTyped = _GetStaticTfType().IsA<UsdTyped>();
    return isTyped;
}

/* virtual */
const TfType &
CodeProcCodeProceduralAPI::_GetTfType() const
{
    return _GetStaticTfType();
}

UsdAttribute
CodeProcCodeProceduralAPI::GetWorkgroupSizeAttr() const
{
    return GetPrim().GetAttribute(CodeProcTokens->codeprocWorkgroupSize);
}

UsdAttribute
CodeProcCodeProceduralAPI::CreateWorkgroupSizeAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(CodeProcTokens->codeprocWorkgroupSize,
                       SdfValueTypeNames->Int,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
CodeProcCodeProceduralAPI::GetMappingsAttr() const
{
    return GetPrim().GetAttribute(CodeProcTokens->codeprocMappings);
}

UsdAttribute
CodeProcCodeProceduralAPI::CreateMappingsAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(CodeProcTokens->codeprocMappings,
                       SdfValueTypeNames->String,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
CodeProcCodeProceduralAPI::GetCodeAttr() const
{
    return GetPrim().GetAttribute(CodeProcTokens->codeprocCode);
}

UsdAttribute
CodeProcCodeProceduralAPI::CreateCodeAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(CodeProcTokens->codeprocCode,
                       SdfValueTypeNames->String,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdRelationship
CodeProcCodeProceduralAPI::GetReadWriteRel() const
{
    return GetPrim().GetRelationship(CodeProcTokens->codeprocReadWrite);
}

UsdRelationship
CodeProcCodeProceduralAPI::CreateReadWriteRel() const
{
    return GetPrim().CreateRelationship(CodeProcTokens->codeprocReadWrite,
                       /* custom = */ false);
}

UsdRelationship
CodeProcCodeProceduralAPI::GetWriteRel() const
{
    return GetPrim().GetRelationship(CodeProcTokens->codeprocWrite);
}

UsdRelationship
CodeProcCodeProceduralAPI::CreateWriteRel() const
{
    return GetPrim().CreateRelationship(CodeProcTokens->codeprocWrite,
                       /* custom = */ false);
}

UsdRelationship
CodeProcCodeProceduralAPI::GetReadRel() const
{
    return GetPrim().GetRelationship(CodeProcTokens->codeprocRead);
}

UsdRelationship
CodeProcCodeProceduralAPI::CreateReadRel() const
{
    return GetPrim().CreateRelationship(CodeProcTokens->codeprocRead,
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
CodeProcCodeProceduralAPI::GetSchemaAttributeNames(bool includeInherited)
{
    static TfTokenVector localNames = {
        CodeProcTokens->codeprocWorkgroupSize,
        CodeProcTokens->codeprocMappings,
        CodeProcTokens->codeprocCode,
    };
    static TfTokenVector allNames =
        _ConcatenateAttributeNames(
            UsdAPISchemaBase::GetSchemaAttributeNames(true),
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
