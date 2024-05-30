#ifndef CODEPROC_PROCEDURAL_HYDRA_SCHEMA_H
#define CODEPROC_PROCEDURAL_HYDRA_SCHEMA_H

#include "./api.h"
#include "pxr/imaging/hd/schema.h"

#include <pxr/base/tf/token.h>
#include <pxr/imaging/hd/dataSourceTypeDefs.h>

PXR_NAMESPACE_OPEN_SCOPE

#define CODEPROC_SCHEMA_TOKENS                                                 \
    (codeProcedural)(read)(                                                    \
        write)(readWrite)(readNames)(writeNames)(readWriteNames)(code)(workgroupSize)

TF_DECLARE_PUBLIC_TOKENS(CodeProcCodeProceduralSchemaTokens, CODEPROC_API,
                         CODEPROC_SCHEMA_TOKENS);

class CodeProcCodeProceduralSchema : public HdSchema {
  public:
    CodeProcCodeProceduralSchema(HdContainerDataSourceHandle container)
        : HdSchema(container) {}

    CODEPROC_API
    HdPathArrayDataSourceHandle GetRead();

    CODEPROC_API
    HdPathArrayDataSourceHandle GetWrite();

    CODEPROC_API
    HdPathArrayDataSourceHandle GetReadWrite();

    CODEPROC_API
    HdTokenArrayDataSourceHandle GetReadNames();

    CODEPROC_API
    HdTokenArrayDataSourceHandle GetWriteNames();

    CODEPROC_API
    HdTokenArrayDataSourceHandle GetReadWriteNames();

    CODEPROC_API
    HdStringDataSourceHandle GetCode();

    CODEPROC_API
    HdIntDataSourceHandle GetWorkgroupSize();

    CODEPROC_API
    static CodeProcCodeProceduralSchema
    GetFromParent(const HdContainerDataSourceHandle &parent);

    CODEPROC_API
    static const TfToken &GetSchemaToken();

    CODEPROC_API
    static const HdDataSourceLocator &GetDefaultLocator();

    CODEPROC_API
    static const HdDataSourceLocator &GetReadLocator();

    CODEPROC_API
    static const HdDataSourceLocator &GetWriteLocator();

    CODEPROC_API
    static const HdDataSourceLocator &GetReadWriteLocator();

    CODEPROC_API
    static const HdDataSourceLocator &GetReadNamesLocator();

    CODEPROC_API
    static const HdDataSourceLocator &GetWriteNamesLocator();

    CODEPROC_API
    static const HdDataSourceLocator &GetReadWriteNamesLocator();

    CODEPROC_API
    static const HdDataSourceLocator &GetCodeLocator();

    CODEPROC_API
    static const HdDataSourceLocator &GetWorkgroupSizeLocator();

    CODEPROC_API
    void GetArgumentDataSource(const TfToken &name);

    CODEPROC_API
    TfTokenVector GetNames(const TfToken &name);

    CODEPROC_API
    static HdContainerDataSourceHandle
    BuildRetained(const HdPathArrayDataSourceHandle &read,
                  const HdPathArrayDataSourceHandle &write,
                  const HdPathArrayDataSourceHandle &readWrite,
                  const HdTokenArrayDataSourceHandle &readNames,
                  const HdTokenArrayDataSourceHandle &writeNames,
                  const HdTokenArrayDataSourceHandle &readWriteNames,
                  const HdStringDataSourceHandle &code,
                  const HdIntDataSourceHandle &workgroupSize);

    class Builder {
      public:
        CODEPROC_API
        Builder &SetRead(const HdPathArrayDataSourceHandle &ds);

        CODEPROC_API
        Builder &SetWrite(const HdPathArrayDataSourceHandle &ds);

        CODEPROC_API
        Builder &SetReadWrite(const HdPathArrayDataSourceHandle &ds);

        CODEPROC_API
        Builder &SetReadNames(const HdTokenArrayDataSourceHandle &ds);

        CODEPROC_API
        Builder &SetWriteNames(const HdTokenArrayDataSourceHandle &ds);

        CODEPROC_API
        Builder &SetReadWriteNames(const HdTokenArrayDataSourceHandle &ds);

        CODEPROC_API
        Builder &SetCode(const HdStringDataSourceHandle &ds);

        CODEPROC_API
        Builder &SetWorkgroupSize(const HdIntDataSourceHandle &ds);

        CODEPROC_API
        HdContainerDataSourceHandle Build();

      private:
        HdPathArrayDataSourceHandle _read;
        HdPathArrayDataSourceHandle _write;
        HdPathArrayDataSourceHandle _readWrite;
        HdTokenArrayDataSourceHandle _readNames;
        HdTokenArrayDataSourceHandle _writeNames;
        HdTokenArrayDataSourceHandle _readWriteNames;
        HdStringDataSourceHandle _code;
        HdIntDataSourceHandle _workgroupSize;
    };
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif