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
#ifndef HAIRPROC_GENERATED_HAIRPROCEDURAL_H
#define HAIRPROC_GENERATED_HAIRPROCEDURAL_H

/// \file hairProc/hairProcedural.h

#include "pxr/pxr.h"
#include "./api.h"
#include "pxr/usd/usdProc/generativeProcedural.h"
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
// HAIRPROCEDURAL                                                             //
// -------------------------------------------------------------------------- //

/// \class HairProcHairProcedural
///
/// Prim definition wrapping HydraGenerativeProcedural. Defines the prim which is run at render time
///
/// For any described attribute \em Fallback \em Value or \em Allowed \em Values below
/// that are text/tokens, the actual token is published and defined in \ref HairProcTokens.
/// So to set an attribute to the value "rightHanded", use HairProcTokens->rightHanded
/// as the value.
///
class HairProcHairProcedural : public UsdProcGenerativeProcedural
{
public:
    /// Compile time constant representing what kind of schema this class is.
    ///
    /// \sa UsdSchemaKind
    static const UsdSchemaKind schemaKind = UsdSchemaKind::ConcreteTyped;

    /// Construct a HairProcHairProcedural on UsdPrim \p prim .
    /// Equivalent to HairProcHairProcedural::Get(prim.GetStage(), prim.GetPath())
    /// for a \em valid \p prim, but will not immediately throw an error for
    /// an invalid \p prim
    explicit HairProcHairProcedural(const UsdPrim& prim=UsdPrim())
        : UsdProcGenerativeProcedural(prim)
    {
    }

    /// Construct a HairProcHairProcedural on the prim held by \p schemaObj .
    /// Should be preferred over HairProcHairProcedural(schemaObj.GetPrim()),
    /// as it preserves SchemaBase state.
    explicit HairProcHairProcedural(const UsdSchemaBase& schemaObj)
        : UsdProcGenerativeProcedural(schemaObj)
    {
    }

    /// Destructor.
    HAIRPROC_API
    virtual ~HairProcHairProcedural();

    /// Return a vector of names of all pre-declared attributes for this schema
    /// class and all its ancestor classes.  Does not include attributes that
    /// may be authored by custom/extended methods of the schemas involved.
    HAIRPROC_API
    static const TfTokenVector &
    GetSchemaAttributeNames(bool includeInherited=true);

    /// Return a HairProcHairProcedural holding the prim adhering to this
    /// schema at \p path on \p stage.  If no prim exists at \p path on
    /// \p stage, or if the prim at that path does not adhere to this schema,
    /// return an invalid schema object.  This is shorthand for the following:
    ///
    /// \code
    /// HairProcHairProcedural(stage->GetPrimAtPath(path));
    /// \endcode
    ///
    HAIRPROC_API
    static HairProcHairProcedural
    Get(const UsdStagePtr &stage, const SdfPath &path);

    /// Attempt to ensure a \a UsdPrim adhering to this schema at \p path
    /// is defined (according to UsdPrim::IsDefined()) on this stage.
    ///
    /// If a prim adhering to this schema at \p path is already defined on this
    /// stage, return that prim.  Otherwise author an \a SdfPrimSpec with
    /// \a specifier == \a SdfSpecifierDef and this schema's prim type name for
    /// the prim at \p path at the current EditTarget.  Author \a SdfPrimSpec s
    /// with \p specifier == \a SdfSpecifierDef and empty typeName at the
    /// current EditTarget for any nonexistent, or existing but not \a Defined
    /// ancestors.
    ///
    /// The given \a path must be an absolute prim path that does not contain
    /// any variant selections.
    ///
    /// If it is impossible to author any of the necessary PrimSpecs, (for
    /// example, in case \a path cannot map to the current UsdEditTarget's
    /// namespace) issue an error and return an invalid \a UsdPrim.
    ///
    /// Note that this method may return a defined prim whose typeName does not
    /// specify this schema class, in case a stronger typeName opinion overrides
    /// the opinion at the current EditTarget.
    ///
    HAIRPROC_API
    static HairProcHairProcedural
    Define(const UsdStagePtr &stage, const SdfPath &path);

protected:
    /// Returns the kind of schema this class belongs to.
    ///
    /// \sa UsdSchemaKind
    HAIRPROC_API
    UsdSchemaKind _GetSchemaKind() const override;

private:
    // needs to invoke _GetStaticTfType.
    friend class UsdSchemaRegistry;
    HAIRPROC_API
    static const TfType &_GetStaticTfType();

    static bool _IsTypedSchema();

    // override SchemaBase virtuals.
    HAIRPROC_API
    const TfType &_GetTfType() const override;

public:
    // --------------------------------------------------------------------- //
    // PROCEDURALTYPE 
    // --------------------------------------------------------------------- //
    /// The registered name of a HdGpGenerativeProceduralPlugin to
    /// be executed.
    ///
    /// | ||
    /// | -- | -- |
    /// | Declaration | `token primvars:hdGp:proceduralType = "HairProcedural"` |
    /// | C++ Type | TfToken |
    /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Token |
    HAIRPROC_API
    UsdAttribute GetProceduralTypeAttr() const;

    /// See GetProceduralTypeAttr(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
    /// If specified, author \p defaultValue as the attribute's default,
    /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
    /// the default for \p writeSparsely is \c false.
    HAIRPROC_API
    UsdAttribute CreateProceduralTypeAttr(VtValue const &defaultValue = VtValue(), bool writeSparsely=false) const;

public:
    // --------------------------------------------------------------------- //
    // PROCEDURALSYSTEM 
    // --------------------------------------------------------------------- //
    /// 
    /// This value should correspond to a configured instance of
    /// HdGpGenerativeProceduralResolvingSceneIndex which will evaluate the
    /// procedural. The default value of "hydraGenerativeProcedural" matches
    /// the equivalent default of HdGpGenerativeProceduralResolvingSceneIndex.
    /// Multiple instances of the scene index can be used to determine where
    /// within a scene index chain a given procedural will be evaluated.
    /// 
    ///
    /// | ||
    /// | -- | -- |
    /// | Declaration | `token proceduralSystem = "hydraGenerativeProcedural"` |
    /// | C++ Type | TfToken |
    /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Token |
    HAIRPROC_API
    UsdAttribute GetProceduralSystemAttr() const;

    /// See GetProceduralSystemAttr(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
    /// If specified, author \p defaultValue as the attribute's default,
    /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
    /// the default for \p writeSparsely is \c false.
    HAIRPROC_API
    UsdAttribute CreateProceduralSystemAttr(VtValue const &defaultValue = VtValue(), bool writeSparsely=false) const;

public:
    // --------------------------------------------------------------------- //
    // TARGETS 
    // --------------------------------------------------------------------- //
    /// The hair prims to deform. The targets should have HairProceduralAPI applied
    ///
    HAIRPROC_API
    UsdRelationship GetTargetsRel() const;

    /// See GetTargetsRel(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create
    HAIRPROC_API
    UsdRelationship CreateTargetsRel() const;

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
