#include "pxr/imaging/hd/retainedDataSource.h"

#include "codeProceduralSchema.h"

#include <iostream>
#include <pxr/imaging/hd/dataSourceTypeDefs.h>

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(CodeProcCodeProceduralSchemaTokens,
                        CODEPROC_SCHEMA_TOKENS);

HdPathArrayDataSourceHandle CodeProcCodeProceduralSchema::GetRead() {
    return _GetTypedDataSource<HdPathArrayDataSource>(
        CodeProcCodeProceduralSchemaTokens->read);
}

HdPathArrayDataSourceHandle CodeProcCodeProceduralSchema::GetWrite() {
    return _GetTypedDataSource<HdPathArrayDataSource>(
        CodeProcCodeProceduralSchemaTokens->write);
}

HdPathArrayDataSourceHandle CodeProcCodeProceduralSchema::GetReadWrite() {
    return _GetTypedDataSource<HdPathArrayDataSource>(
        CodeProcCodeProceduralSchemaTokens->readWrite);
}

HdTokenArrayDataSourceHandle CodeProcCodeProceduralSchema::GetReadNames() {
    return _GetTypedDataSource<HdTokenArrayDataSource>(
        CodeProcCodeProceduralSchemaTokens->readNames);
}

HdTokenArrayDataSourceHandle CodeProcCodeProceduralSchema::GetWriteNames() {
    return _GetTypedDataSource<HdTokenArrayDataSource>(
        CodeProcCodeProceduralSchemaTokens->writeNames);
}

HdTokenArrayDataSourceHandle CodeProcCodeProceduralSchema::GetReadWriteNames() {
    return _GetTypedDataSource<HdTokenArrayDataSource>(
        CodeProcCodeProceduralSchemaTokens->readWriteNames);
}

HdStringDataSourceHandle CodeProcCodeProceduralSchema::GetCode() {
    return _GetTypedDataSource<HdStringDataSource>(
        CodeProcCodeProceduralSchemaTokens->code);
}

HdIntDataSourceHandle CodeProcCodeProceduralSchema::GetWorkgroupSize() {
    return _GetTypedDataSource<HdIntDataSource>(
        CodeProcCodeProceduralSchemaTokens->workgroupSize);
}

HdContainerDataSourceHandle CodeProcCodeProceduralSchema::BuildRetained(
    const HdPathArrayDataSourceHandle &read,
    const HdPathArrayDataSourceHandle &write,
    const HdPathArrayDataSourceHandle &readWrite,
    const HdTokenArrayDataSourceHandle &readNames,
    const HdTokenArrayDataSourceHandle &writeNames,
    const HdTokenArrayDataSourceHandle &readWriteNames,
    const HdStringDataSourceHandle &code,
    const HdIntDataSourceHandle &workgroupSize) {

    TfToken names[4];
    HdDataSourceBaseHandle values[4];

    size_t count = 0;
    if (read) {
        names[count] = CodeProcCodeProceduralSchemaTokens->read;
        values[count++] = read;
    }
    if (write) {
        names[count] = CodeProcCodeProceduralSchemaTokens->write;
        values[count++] = write;
    }
    if (readWrite) {
        names[count] = CodeProcCodeProceduralSchemaTokens->readWrite;
        values[count++] = readWrite;
    }
    if (readNames) {
        names[count] = CodeProcCodeProceduralSchemaTokens->readNames;
        values[count++] = readNames;
    }
    if (writeNames) {
        names[count] = CodeProcCodeProceduralSchemaTokens->writeNames;
        values[count++] = writeNames;
    }
    if (readWriteNames) {
        names[count] = CodeProcCodeProceduralSchemaTokens->readWriteNames;
        values[count++] = readWriteNames;
    }
    if (code) {
        names[count] = CodeProcCodeProceduralSchemaTokens->code;
        values[count++] = code;
    }
    if (workgroupSize) {
        names[count] = CodeProcCodeProceduralSchemaTokens->workgroupSize;
        values[count++] = workgroupSize;
    }

    return HdRetainedContainerDataSource::New(count, names, values);
}

const TfToken &CodeProcCodeProceduralSchema::GetSchemaToken() {
    return CodeProcCodeProceduralSchemaTokens->codeProcedural;
}

CodeProcCodeProceduralSchema CodeProcCodeProceduralSchema::GetFromParent(
    const HdContainerDataSourceHandle &parent) {
    return CodeProcCodeProceduralSchema(
        parent ? HdContainerDataSource::Cast(parent->Get(
                     CodeProcCodeProceduralSchemaTokens->codeProcedural))
               : nullptr);
}

const HdDataSourceLocator &CodeProcCodeProceduralSchema::GetDefaultLocator() {
    static const HdDataSourceLocator locator(
        CodeProcCodeProceduralSchemaTokens->codeProcedural);
    return locator;
}

const HdDataSourceLocator &CodeProcCodeProceduralSchema::GetReadLocator() {
    static const HdDataSourceLocator locator(
        CodeProcCodeProceduralSchemaTokens->codeProcedural,
        CodeProcCodeProceduralSchemaTokens->read);
    return locator;
}

const HdDataSourceLocator &CodeProcCodeProceduralSchema::GetWriteLocator() {
    static const HdDataSourceLocator locator(
        CodeProcCodeProceduralSchemaTokens->codeProcedural,
        CodeProcCodeProceduralSchemaTokens->write);
    return locator;
}

const HdDataSourceLocator &CodeProcCodeProceduralSchema::GetReadWriteLocator() {
    static const HdDataSourceLocator locator(
        CodeProcCodeProceduralSchemaTokens->codeProcedural,
        CodeProcCodeProceduralSchemaTokens->readWrite);
    return locator;
}

const HdDataSourceLocator &CodeProcCodeProceduralSchema::GetReadNamesLocator() {
    static const HdDataSourceLocator locator(
        CodeProcCodeProceduralSchemaTokens->codeProcedural,
        CodeProcCodeProceduralSchemaTokens->readNames);
    return locator;
}

const HdDataSourceLocator &
CodeProcCodeProceduralSchema::GetWriteNamesLocator() {
    static const HdDataSourceLocator locator(
        CodeProcCodeProceduralSchemaTokens->codeProcedural,
        CodeProcCodeProceduralSchemaTokens->writeNames);
    return locator;
}

const HdDataSourceLocator &
CodeProcCodeProceduralSchema::GetReadWriteNamesLocator() {
    static const HdDataSourceLocator locator(
        CodeProcCodeProceduralSchemaTokens->codeProcedural,
        CodeProcCodeProceduralSchemaTokens->readWriteNames);
    return locator;
}

const HdDataSourceLocator &CodeProcCodeProceduralSchema::GetCodeLocator() {
    static const HdDataSourceLocator locator(
        CodeProcCodeProceduralSchemaTokens->codeProcedural,
        CodeProcCodeProceduralSchemaTokens->code);
    return locator;
}

const HdDataSourceLocator &
CodeProcCodeProceduralSchema::GetWorkgroupSizeLocator() {
    static const HdDataSourceLocator locator(
        CodeProcCodeProceduralSchemaTokens->codeProcedural,
        CodeProcCodeProceduralSchemaTokens->workgroupSize);
    return locator;
}

CodeProcCodeProceduralSchema::Builder &
CodeProcCodeProceduralSchema::Builder::SetRead(
    const HdPathArrayDataSourceHandle &readRel) {
    _read = readRel;
    return *this;
}

CodeProcCodeProceduralSchema::Builder &
CodeProcCodeProceduralSchema::Builder::SetWrite(
    const HdPathArrayDataSourceHandle &writeRel) {
    _write = writeRel;
    return *this;
}

CodeProcCodeProceduralSchema::Builder &
CodeProcCodeProceduralSchema::Builder::SetReadWrite(
    const HdPathArrayDataSourceHandle &readWriteRel) {
    _readWrite = readWriteRel;
    return *this;
}

CodeProcCodeProceduralSchema::Builder &
CodeProcCodeProceduralSchema::Builder::SetReadNames(
    const HdTokenArrayDataSourceHandle &readNames) {
    _readNames = readNames;
    return *this;
}

CodeProcCodeProceduralSchema::Builder &
CodeProcCodeProceduralSchema::Builder::SetWriteNames(
    const HdTokenArrayDataSourceHandle &writeNames) {
    _writeNames = writeNames;
    return *this;
}

CodeProcCodeProceduralSchema::Builder &
CodeProcCodeProceduralSchema::Builder::SetReadWriteNames(
    const HdTokenArrayDataSourceHandle &readWriteNames) {
    _readWriteNames = readWriteNames;
    return *this;
}

CodeProcCodeProceduralSchema::Builder &
CodeProcCodeProceduralSchema::Builder::SetCode(
    const HdStringDataSourceHandle &code) {
    _code = code;
    return *this;
}

CodeProcCodeProceduralSchema::Builder &
CodeProcCodeProceduralSchema::Builder::SetWorkgroupSize(
    const HdIntDataSourceHandle &workgroupSize) {
    _workgroupSize = workgroupSize;
    return *this;
}

void
CodeProcCodeProceduralSchema::GetArgumentDataSource(const TfToken& name) {
    std::cout<<name<<std::endl;
}

HdContainerDataSourceHandle CodeProcCodeProceduralSchema::Builder::Build() {
    return CodeProcCodeProceduralSchema::BuildRetained(
        _read, _write, _readWrite, _readNames, _writeNames, _readWriteNames,
        _code, _workgroupSize);
}

PXR_NAMESPACE_CLOSE_SCOPE