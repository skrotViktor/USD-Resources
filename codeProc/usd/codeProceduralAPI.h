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
#ifndef CODEPROC_GENERATED_CODEPROCEDURALAPI_H
#define CODEPROC_GENERATED_CODEPROCEDURALAPI_H

/// \file codeProc/codeProceduralAPI.h

#include "pxr/pxr.h"
#include "./api.h"
#include "pxr/usd/usd/apiSchemaBase.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/stage.h"
#include "./tokens.h"

#include "pxr/base/vt/value.h"

#include "pxr/base/gf/vec3d.h"
#include "pxr/base/gf/vec3f.h"
#include "pxr/base/gf/matrix4d.h"

#include "pxr/base/tf/token.h"
#include "pxr/base/tf/type.h"

PXR_NAMESPACE_OPEN_SCOPE

class SdfAssetPath;

// -------------------------------------------------------------------------- //
// CODEPROCEDURALAPI                                                          //
// -------------------------------------------------------------------------- //

/// \class CodeProcCodeProceduralAPI
///
/// API for writing opencl code directly on a prim and invoke at render time:
/// - codepr oc:target: The animated geometry
/// 
///
class CodeProcCodeProceduralAPI : public UsdAPISchemaBase
{
public:
    /// Compile time constant representing what kind of schema this class is.
    ///
    /// \sa UsdSchemaKind
    static const UsdSchemaKind schemaKind = UsdSchemaKind::SingleApplyAPI;

    /// Construct a CodeProcCodeProceduralAPI on UsdPrim \p prim .
    /// Equivalent to CodeProcCodeProceduralAPI::Get(prim.GetStage(), prim.GetPath())
    /// for a \em valid \p prim, but will not immediately throw an error for
    /// an invalid \p prim
    explicit CodeProcCodeProceduralAPI(const UsdPrim& prim=UsdPrim())
        : UsdAPISchemaBase(prim)
    {
    }

    /// Construct a CodeProcCodeProceduralAPI on the prim held by \p schemaObj .
    /// Should be preferred over CodeProcCodeProceduralAPI(schemaObj.GetPrim()),
    /// as it preserves SchemaBase state.
    explicit CodeProcCodeProceduralAPI(const UsdSchemaBase& schemaObj)
        : UsdAPISchemaBase(schemaObj)
    {
    }

    /// Destructor.
    CODEPROC_API
    virtual ~CodeProcCodeProceduralAPI();

    /// Return a vector of names of all pre-declared attributes for this schema
    /// class and all its ancestor classes.  Does not include attributes that
    /// may be authored by custom/extended methods of the schemas involved.
    CODEPROC_API
    static const TfTokenVector &
    GetSchemaAttributeNames(bool includeInherited=true);

    /// Return a CodeProcCodeProceduralAPI holding the prim adhering to this
    /// schema at \p path on \p stage.  If no prim exists at \p path on
    /// \p stage, or if the prim at that path does not adhere to this schema,
    /// return an invalid schema object.  This is shorthand for the following:
    ///
    /// \code
    /// CodeProcCodeProceduralAPI(stage->GetPrimAtPath(path));
    /// \endcode
    ///
    CODEPROC_API
    static CodeProcCodeProceduralAPI
    Get(const UsdStagePtr &stage, const SdfPath &path);


    /// Returns true if this <b>single-apply</b> API schema can be applied to 
    /// the given \p prim. If this schema can not be a applied to the prim, 
    /// this returns false and, if provided, populates \p whyNot with the 
    /// reason it can not be applied.
    /// 
    /// Note that if CanApply returns false, that does not necessarily imply
    /// that calling Apply will fail. Callers are expected to call CanApply
    /// before calling Apply if they want to ensure that it is valid to 
    /// apply a schema.
    /// 
    /// \sa UsdPrim::GetAppliedSchemas()
    /// \sa UsdPrim::HasAPI()
    /// \sa UsdPrim::CanApplyAPI()
    /// \sa UsdPrim::ApplyAPI()
    /// \sa UsdPrim::RemoveAPI()
    ///
    CODEPROC_API
    static bool 
    CanApply(const UsdPrim &prim, std::string *whyNot=nullptr);

    /// Applies this <b>single-apply</b> API schema to the given \p prim.
    /// This information is stored by adding "CodeProceduralAPI" to the 
    /// token-valued, listOp metadata \em apiSchemas on the prim.
    /// 
    /// \return A valid CodeProcCodeProceduralAPI object is returned upon success. 
    /// An invalid (or empty) CodeProcCodeProceduralAPI object is returned upon 
    /// failure. See \ref UsdPrim::ApplyAPI() for conditions 
    /// resulting in failure. 
    /// 
    /// \sa UsdPrim::GetAppliedSchemas()
    /// \sa UsdPrim::HasAPI()
    /// \sa UsdPrim::CanApplyAPI()
    /// \sa UsdPrim::ApplyAPI()
    /// \sa UsdPrim::RemoveAPI()
    ///
    CODEPROC_API
    static CodeProcCodeProceduralAPI 
    Apply(const UsdPrim &prim);

protected:
    /// Returns the kind of schema this class belongs to.
    ///
    /// \sa UsdSchemaKind
    CODEPROC_API
    UsdSchemaKind _GetSchemaKind() const override;

private:
    // needs to invoke _GetStaticTfType.
    friend class UsdSchemaRegistry;
    CODEPROC_API
    static const TfType &_GetStaticTfType();

    static bool _IsTypedSchema();

    // override SchemaBase virtuals.
    CODEPROC_API
    const TfType &_GetTfType() const override;

public:
    // --------------------------------------------------------------------- //
    // WORKGROUPSIZE 
    // --------------------------------------------------------------------- //
    /// The global workgroup size. If not specified, will use first writeable argument
    ///
    /// | ||
    /// | -- | -- |
    /// | Declaration | `int codeproc:workgroupSize` |
    /// | C++ Type | int |
    /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Int |
    CODEPROC_API
    UsdAttribute GetWorkgroupSizeAttr() const;

    /// See GetWorkgroupSizeAttr(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
    /// If specified, author \p defaultValue as the attribute's default,
    /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
    /// the default for \p writeSparsely is \c false.
    CODEPROC_API
    UsdAttribute CreateWorkgroupSizeAttr(VtValue const &defaultValue = VtValue(), bool writeSparsely=false) const;

public:
    // --------------------------------------------------------------------- //
    // READWRITENAMES 
    // --------------------------------------------------------------------- //
    /// Opencl argument names coresponding to the attributes in the readWrite rel
    ///
    /// | ||
    /// | -- | -- |
    /// | Declaration | `string[] codeproc:readWriteNames` |
    /// | C++ Type | VtArray<std::string> |
    /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->StringArray |
    CODEPROC_API
    UsdAttribute GetReadWriteNamesAttr() const;

    /// See GetReadWriteNamesAttr(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
    /// If specified, author \p defaultValue as the attribute's default,
    /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
    /// the default for \p writeSparsely is \c false.
    CODEPROC_API
    UsdAttribute CreateReadWriteNamesAttr(VtValue const &defaultValue = VtValue(), bool writeSparsely=false) const;

public:
    // --------------------------------------------------------------------- //
    // READNAMES 
    // --------------------------------------------------------------------- //
    /// Opencl argument names coresponding to the attributes in the read rel
    ///
    /// | ||
    /// | -- | -- |
    /// | Declaration | `string[] codeproc:readNames` |
    /// | C++ Type | VtArray<std::string> |
    /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->StringArray |
    CODEPROC_API
    UsdAttribute GetReadNamesAttr() const;

    /// See GetReadNamesAttr(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
    /// If specified, author \p defaultValue as the attribute's default,
    /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
    /// the default for \p writeSparsely is \c false.
    CODEPROC_API
    UsdAttribute CreateReadNamesAttr(VtValue const &defaultValue = VtValue(), bool writeSparsely=false) const;

public:
    // --------------------------------------------------------------------- //
    // WRITENAMES 
    // --------------------------------------------------------------------- //
    /// Opencl argument names coresponding to the attributes in the write rel
    ///
    /// | ||
    /// | -- | -- |
    /// | Declaration | `string[] codeproc:writeNames` |
    /// | C++ Type | VtArray<std::string> |
    /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->StringArray |
    CODEPROC_API
    UsdAttribute GetWriteNamesAttr() const;

    /// See GetWriteNamesAttr(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
    /// If specified, author \p defaultValue as the attribute's default,
    /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
    /// the default for \p writeSparsely is \c false.
    CODEPROC_API
    UsdAttribute CreateWriteNamesAttr(VtValue const &defaultValue = VtValue(), bool writeSparsely=false) const;

public:
    // --------------------------------------------------------------------- //
    // CODE 
    // --------------------------------------------------------------------- //
    /// The opencl kernel code
    ///
    /// | ||
    /// | -- | -- |
    /// | Declaration | `string codeproc:code` |
    /// | C++ Type | std::string |
    /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->String |
    CODEPROC_API
    UsdAttribute GetCodeAttr() const;

    /// See GetCodeAttr(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
    /// If specified, author \p defaultValue as the attribute's default,
    /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
    /// the default for \p writeSparsely is \c false.
    CODEPROC_API
    UsdAttribute CreateCodeAttr(VtValue const &defaultValue = VtValue(), bool writeSparsely=false) const;

public:
    // --------------------------------------------------------------------- //
    // READWRITE 
    // --------------------------------------------------------------------- //
    /// The target properties which opencl can read from and write to
    ///
    CODEPROC_API
    UsdRelationship GetReadWriteRel() const;

    /// See GetReadWriteRel(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create
    CODEPROC_API
    UsdRelationship CreateReadWriteRel() const;

public:
    // --------------------------------------------------------------------- //
    // WRITE 
    // --------------------------------------------------------------------- //
    /// The target properties which opencl can only write to
    ///
    CODEPROC_API
    UsdRelationship GetWriteRel() const;

    /// See GetWriteRel(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create
    CODEPROC_API
    UsdRelationship CreateWriteRel() const;

public:
    // --------------------------------------------------------------------- //
    // READ 
    // --------------------------------------------------------------------- //
    /// The target properties which opencl can only read from
    ///
    CODEPROC_API
    UsdRelationship GetReadRel() const;

    /// See GetReadRel(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create
    CODEPROC_API
    UsdRelationship CreateReadRel() const;

public:
    // ===================================================================== //
    // Feel free to add custom code below this line, it will be preserved by 
    // the code generator. 
    //
    // Just remember to: 
    //  - Close the class declaration with }; 
    //  - Close the namespace with PXR_NAMESPACE_CLOSE_SCOPE
    //  - Close the include guard with #endif
    // ===================================================================== //
    // --(BEGIN CUSTOM CODE)--

};

PXR_NAMESPACE_CLOSE_SCOPE

#endif
