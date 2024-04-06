# Form implementation generated from reading ui file 'box.ui'
#
# Created by: PyQt6 UI code generator 6.6.1
#
# WARNING: Any manual changes made to this file will be lost when pyuic6 is
# run again.  Do not edit this file unless you know what you are doing.

from collections import defaultdict
from pxr.Usdviewq.qt import QtCore, QtGui, QtWidgets
from pxr import Usd

from vik import CodeProc

def launchDialog(usdviewApi):
    dial = Dialog(usdviewApi.qMainWindow, usdviewApi)
    dial.show()


class CodeProcItemModel(QtCore.QAbstractTableModel):
    def __init__(self, parent):
        super().__init__(parent)

        self._data = []

        self.argModeOptions = ["Read", "Write", "Read + Write"]
        self.defaultRow = ["", "", self.argModeOptions[0]]


    def index(self, row, column, parent=QtCore.QModelIndex()):
        if row > len(self._data)-1:
            self._data += [[]] * (len(self._data)-1-row)

        return self.createIndex(row, column)
    

    def rowCount(self, parent=QtCore.QModelIndex()):
        return len(self._data)
    

    def columnCount(self, parent=QtCore.QModelIndex()):
        return 3


    def data(self, index, role):
        if index.row() >= len(self._data):
            return None
        if index.column() >= self.columnCount():
            return None


        if role == QtCore.Qt.ItemDataRole.DisplayRole or role == QtCore.Qt.ItemDataRole.EditRole:
            return self._data[index.row()][index.column()]

        return None


    def headerData(self, section, orientation, role):
        if orientation == QtCore.Qt.Orientation.Horizontal:
            if role == QtCore.Qt.ItemDataRole.DisplayRole:
                if section == 0:
                    return "Property Path"
                if section == 1:
                    return "Name"
                if section == 2:
                    return "Mode"
        if orientation == QtCore.Qt.Orientation.Vertical:
            if role == QtCore.Qt.ItemDataRole.DisplayRole:
                return "-"

        return super().headerData(section, orientation, role)


    def setData(self, index, value, role):
        if index.row() > len(self._data):
            return False
        elif index.row() == self.rowCount():
            self._data.append(self.defaultRow.copy())

        self._data[index.row()][index.column()] = value
        self.dataChanged.emit(index, index)
        return True


    def flags(self, index):
        return QtCore.Qt.ItemFlag.ItemIsEnabled | QtCore.Qt.ItemFlag.ItemIsEditable


    def insertRow(self, row, parent=QtCore.QModelIndex()):
        self.beginInsertRows(parent, row, row)
        self._data.append(self.defaultRow.copy())
        self.endInsertRows()

    def insertRows(self, row, count, parent=QtCore.QModelIndex()):
        self.beginInsertRows(parent, row, row+count)
        self._data.append(self.defaultRow.copy())
        self.endInsertRows()


    def removeRow(self, row, parent=QtCore.QModelIndex()):
        if 0 > row or row > self.rowCount()-1:
            return
    
        self.beginRemoveRows(parent, row, row)
        self._data.pop(row)
        self.endRemoveRows()
    
    def removeRows(self, row, count, parent=QtCore.QModelIndex()):
        if 0 > row or row > self.rowCount()-1:
            return
    
        self.beginRemoveRows(parent, row, count-1)
        for _ in range(count):
            self._data.pop(row)
        self.endRemoveRows()


class CodeProcItemView(QtWidgets.QTableView):
    def __init__(self, parent):
        super().__init__(parent)

    def mousePressEvent(self, event):
        if event.button() == QtCore.Qt.MouseButton.LeftButton:
            index = self.indexAt(event.position().toPoint())
            if index.isValid():
                self.edit(index)
        return super().mousePressEvent(event)


class ArgComboBoxItemDelegate(QtWidgets.QStyledItemDelegate):
    def __init__(self, parent=None):
        super().__init__(parent=parent)

        self.currentOptionIndex = 0
        self.editing = None


    def paint(self, painter, option, index):
        if self.editing == index.row():
            return
        box = QtWidgets.QComboBox()
        data = index.data(QtCore.Qt.ItemDataRole.DisplayRole)

        box.addItem(data)
        box.setGeometry(option.rect)

        box.setAttribute(QtCore.Qt.WidgetAttribute.WA_TranslucentBackground)
        painter.drawPixmap(option.rect, box.grab())


    def createEditor(self, parent, option, index):
        box = QtWidgets.QComboBox(parent)
        
        box.addItems(index.model().argModeOptions)
        box.setCurrentText(index.data(QtCore.Qt.ItemDataRole.EditRole))
        box.show()
        self.editing = index.row()
        return box


    def setModelData(self, editor, model, index):
        self.editing = None
        return super().setModelData(editor, model, index)


    def updateEditorGeometry(self, editor, option, index):
        editor.setGeometry(option.rect)
        return super().updateEditorGeometry(editor, option, index)


class Dialog(QtWidgets.QDialog):
    def __init__(self, parent, api, stage=None):
        super().__init__(parent)
        
        self.api = api
        if self.api:
            self.stage = self.api.stage
        else:
            self.stage = stage

        self.verticalLayout = QtWidgets.QVBoxLayout(self)
        self.verticalLayout.setContentsMargins(3,3,3,3)


        self.loadSelectedPrimButton = QtWidgets.QPushButton("Load Selected Prim", parent=self)
        self.loadSelectedPrimButton.clicked.connect(self.loadSelectedPrim)
        self.verticalLayout.addWidget(self.loadSelectedPrimButton)
    
        self.selectedPrim = QtWidgets.QLineEdit(self)
        self.verticalLayout.addWidget(self.selectedPrim)

        self.addArgButton = QtWidgets.QPushButton("Add Argument", parent=self)
        self.addArgButton.clicked.connect(self.addArg)
        self.verticalLayout.addWidget(self.addArgButton)

        self.model = CodeProcItemModel(self)
        self.argModeEditor = ArgComboBoxItemDelegate(self)

        self.argsTable = CodeProcItemView(self)
        self.argsTable.setEditTriggers(QtWidgets.QAbstractItemView.EditTrigger.AllEditTriggers)
        self.argsTable.setSizePolicy(QtWidgets.QSizePolicy.Policy.Expanding,
                                     QtWidgets.QSizePolicy.Policy.Expanding)

        self.argsTable.setModel(self.model)
        self.argsTable.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.ResizeMode.Stretch)
        self.argsTable.setItemDelegateForColumn(2, self.argModeEditor)
        self.argsTable.verticalHeader().sectionClicked.connect(self.removeArg)

        self.verticalLayout.addWidget(self.argsTable)

        self.generateKernelButton = QtWidgets.QPushButton("Geneate Kernel", parent=self)
        self.generateKernelButton.clicked.connect(self.generateKernel)
        self.verticalLayout.addWidget(self.generateKernelButton)

        self.textEdit = QtWidgets.QTextEdit(parent=self)
        self.textEdit.setMinimumSize(QtCore.QSize(309, 0))
        self.verticalLayout.addWidget(self.textEdit)

        self.saveButton = QtWidgets.QPushButton("Save", parent=self)
        self.saveButton.clicked.connect(self.saveAttributes)
        self.verticalLayout.addWidget(self.saveButton)

        # Fill UI from selected prim
        self.loadSelectedPrim()


    def addArg(self, path=None, name=None, mode=None):
        row = self.model.rowCount()
        self.model.insertRow(row)

        if self.api and path is None:
            selectedProperty = self.api.property

            if not selectedProperty:
                return

            index = self.model.index(row, 0)
            self.model.setData(index, str(selectedProperty.GetPath()), QtCore.Qt.ItemDataRole.DisplayRole)
            return
        
        if path:
            index = self.model.index(row, 0)
            self.model.setData(index, path, QtCore.Qt.ItemDataRole.DisplayRole)
        if name:
            index = self.model.index(row, 1)
            self.model.setData(index, name, QtCore.Qt.ItemDataRole.DisplayRole)
        if mode:
            index = self.model.index(row, 2)
            self.model.setData(index, mode, QtCore.Qt.ItemDataRole.DisplayRole)


    def removeArg(self, row=-1):
        if row == -1:
            row = self.model.rowCount()-1
        self.model.removeRow(row)


    def generateKernel(self):
        kernelUtils = CodeProc.KernelUtils()
        
        for i in range(self.model.rowCount()):
            index = self.model.index(i, 0)
            path = index.data(QtCore.Qt.ItemDataRole.DisplayRole)

            index = self.model.index(i, 1)
            name = index.data(QtCore.Qt.ItemDataRole.DisplayRole)

            prop = self.stage.GetAttributeAtPath(path)
            argType = str(prop.GetTypeName().scalarType)
            isArray = prop.GetTypeName().isArray
            kernelUtils.addArg(name, argType, isArray)


        self.textEdit.setText(kernelUtils.generateKernelCode())
    
    def loadSelectedPrim(self):
        if self.api:
            selectedPrim = self.api.prim

        elif self.stage and __name__ == "__main__":
            selectedPrim = self.stage.GetPrimAtPath("/plane")

        if not selectedPrim:
            return

        self.selectedPrim.setText(str(selectedPrim.GetPath()))
        self.model.removeRows(0, self.model.rowCount())

        if not selectedPrim.HasAPI("CodeProceduralAPI"):
            return

        api = CodeProc.CodeProceduralAPI(selectedPrim)
    
        if api.GetReadWriteNamesAttr().Get():
            for path, name in zip(api.GetReadWriteRel().GetForwardedTargets(), api.GetReadWriteNamesAttr().Get()):
                self.addArg(str(path), name, "ReadWrite")

        if api.GetReadNamesAttr().Get():
            for path, name in zip(api.GetReadRel().GetForwardedTargets(), api.GetReadNamesAttr().Get()):
                self.addArg(str(path), name, "Read")

        if api.GetWriteNamesAttr().Get():
            for path, name in zip(api.GetWriteRel().GetForwardedTargets(), api.GetWriteNamesAttr().Get()):
                self.addArg(str(path), name, "Write")

        code = api.GetCodeAttr().Get()
        if code:
            self.textEdit.setText(code)

        print(self.model.rowCount())

    def saveAttributes(self):
        mappings = defaultdict(list)
        rels = defaultdict(list)
        for i in range(self.model.rowCount()):
            index = self.model.index(i, 0)
            path = index.data(QtCore.Qt.ItemDataRole.DisplayRole)

            index = self.model.index(i, 1)
            name = index.data(QtCore.Qt.ItemDataRole.DisplayRole)
            
            index = self.model.index(i, 2)
            mode = index.data(QtCore.Qt.ItemDataRole.DisplayRole)

            mappings[mode].append(name)
            rels[mode].append(path)

        prim = self.stage.GetPrimAtPath(self.selectedPrim.text())
        api = CodeProc.CodeProceduralAPI(prim)

        api.CreateReadWriteRel().SetTargets(rels["Read + Write"])
        api.CreateWriteRel().SetTargets(rels["Write"])
        api.CreateReadRel().SetTargets(rels["Read"])

        api.CreateReadWriteNamesAttr().Set(mappings["Read + Write"])
        api.CreateWriteNamesAttr().Set(mappings["Write"])
        api.CreateReadNamesAttr().Set(mappings["Read"])

        api.CreateCodeAttr().Set(self.textEdit.toPlainText())

        self.stage.Export(os.path.join(os.path.dirname(__file__), "../testenv/codeProc.usda"))


if __name__ == "__main__":
    import os
    stage_name = os.path.join(os.path.dirname(__file__), "../testenv/codeProc.usda")

    stage = Usd.Stage.Open(stage_name)

    app = QtWidgets.QApplication()
    dial = Dialog(parent=None, api=None, stage=stage)

    dial.show()
    app.exec()