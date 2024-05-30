#ifndef CODEPROC_CODEPROCEDURAL_ARGUMENTS_H
#define CODEPROC_CODEPROCEDURAL_ARGUMENTS_H

#include "pxr/base/tf/token.h"
// #include "pxr/base/tf/type.h"
#include "pxr/imaging/hd/types.h"
#include "pxr/usd/sdf/path.h"
#include <set>

#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE

namespace Deformer {

/* ---------------------------------- Flags --------------------------------- */

/*
Bitwise enum learned from
https://www.sandordargo.com/blog/2022/06/22/bitwise-enums
*/

enum class ArgumentFlags : char { readable = 1 << 0, writable = 1 << 1 };

inline ArgumentFlags operator|(const ArgumentFlags &lhs,
                               const ArgumentFlags &rhs) {
    using ArgumentFlagsType = std::underlying_type<ArgumentFlags>::type;
    return ArgumentFlags(static_cast<ArgumentFlagsType>(lhs) |
                         static_cast<ArgumentFlagsType>(rhs));
}

inline ArgumentFlags operator&(const ArgumentFlags &lhs,
                               const ArgumentFlags &rhs) {
    using ArgumentFlagsType = std::underlying_type<ArgumentFlags>::type;
    return ArgumentFlags(static_cast<ArgumentFlagsType>(lhs) &
                         static_cast<ArgumentFlagsType>(rhs));
}

inline ArgumentFlags operator^(const ArgumentFlags &lhs,
                               const ArgumentFlags &rhs) {
    using ArgumentFlagsType = std::underlying_type<ArgumentFlags>::type;
    return ArgumentFlags(static_cast<ArgumentFlagsType>(lhs) ^
                         static_cast<ArgumentFlagsType>(rhs));
}

/* --------------------------------- Agument -------------------------------- */
/*
C++ representation of arguments passed from the CodeProcAPI schema prim.
Provides a ArgumentSet (std::set<Argument>) for storing multiple arguments.
Filter arguments by using one of the getter functions.
*/

struct Argument {
    pxr::SdfPath primvarPath = pxr::SdfPath::EmptyPath();
    ArgumentFlags argumentflags = ArgumentFlags::readable;
    TfType type = TfType::GetUnknownType();
    TfToken name = TfToken();

    size_t arraySize;
    size_t vectorSize;
    size_t dataSize;

    // constexpr size_t TotalSize() const {
    //     return arraySize * vectorSize * dataSize;
    // }

    bool operator<(const Argument &other) const {
        return other.primvarPath < primvarPath;
    }

    Argument operator=(const Argument &other) const { return other; }

    bool operator==(const Argument &other) const {
        return other.primvarPath == primvarPath;
    }
    bool operator==(const SdfPath &other) const {
        return other == primvarPath.GetAbsoluteRootOrPrimPath();
    }
    bool operator==(const TfToken &other) const { return other == name; }
    bool operator==(const TfType &other) const { return other == type; }
    bool operator==(const ArgumentFlags &other) const {
        return other == argumentflags;
    }

    constexpr bool HasFlags(const ArgumentFlags &flags) const {
        return static_cast<bool>(argumentflags & flags);
    }

    constexpr bool IsFlags(const ArgumentFlags &flags) const {
        return argumentflags == flags;
    }
};

using ArgumentSet = std::set<Argument>;

/* --------------------------------- Getters -------------------------------- */
/*
Getter functions for filtering ArgumentSet objects. Enables filtering by flags,
name or path. Since the flags are bitwise enums, one can get all readable and
writable args by doing

    GetArguments(argumentSet, ArgumentFlags::readable &
ArgumentFlags::writable);

*/
template <class T> struct _GetArgsFunc {
    _GetArgsFunc(const T &compArg) : _compArg(compArg){};

    inline bool operator()(const Argument &arg) const {
        return arg == _compArg;
    }
    T _compArg;
};

template <class T>
inline ArgumentSet _GetArguments(const ArgumentSet &inputArgs,
                                 const T &compArg) {

    ArgumentSet result;
    std::copy_if(inputArgs.cbegin(), inputArgs.cend(),
                 std::inserter(result, result.begin()),
                 _GetArgsFunc<T>(compArg));

    return result;
}

} // namespace Deformer

PXR_NAMESPACE_CLOSE_SCOPE

#endif