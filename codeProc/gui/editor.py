from pxr.Usdviewq.qt import QtCore, QtGui, QtWidgets
from enum import IntFlag, auto

class Formatter:
    class BlockState(IntFlag):
        NONE = auto()
        START = auto()
        END = auto()

    def __init__(self):
        """Formatter for Qt made to format code. 
        The regex member will match for single line patterns.
        Multiline formatting is enabled when setting start and end regex and calling formatMultilineText().
        """
        self.format = QtGui.QTextCharFormat()
        self.regex = QtCore.QRegularExpression()

        # Multiline
        self.multiLineFormat = QtGui.QTextCharFormat()
        self.startRegex = QtCore.QRegularExpression()
        self.endRegex = QtCore.QRegularExpression()
        self.isMultiline = False

    def setColor(self, r, g, b):
        self.format.setForeground(QtGui.QColor(r, g, b))

    def setMultilineColor(self, r, g, b):
        self.multiLineFormat.setForeground(QtGui.QColor(r, g, b))
    
    def setMultiline(self, toggle):
        self.isMultiline = toggle
    
    def setRegex(self, expr):
        self.regex.setPattern(expr)
    
    def setStartRegex(self, expr):
        self.startRegex.setPattern(expr)

    def setEndRegex(self, expr):
        self.endRegex.setPattern(expr)
    
    def formatText(self, syntaxHighlighter, text):
        i = self.regex.globalMatch(text)
        while i.hasNext():
            match = i.next()
            start = match.capturedStart()
            count = match.capturedLength()
            QtGui.QSyntaxHighlighter.setFormat(syntaxHighlighter, start, count, self.format)
        

    def formatMultilineText(self, syntaxHighlighter, text):
        start = self.startRegex.globalMatch(text)
        end = self.endRegex.globalMatch(text)

        QtGui.QSyntaxHighlighter.setCurrentBlockState(syntaxHighlighter, self.BlockState.NONE)

        startIndex = 0
        if QtGui.QSyntaxHighlighter.previousBlockState(syntaxHighlighter) != self.BlockState.START and start.hasNext():
            QtGui.QSyntaxHighlighter.setCurrentBlockState(syntaxHighlighter, self.BlockState.START)
            startIndex = start.next().capturedStart()

        while startIndex >= 0:
            commentLength = 0
            if not end.hasNext() and QtGui.QSyntaxHighlighter.previousBlockState(syntaxHighlighter) == self.BlockState.START or \
                                     QtGui.QSyntaxHighlighter.currentBlockState(syntaxHighlighter) == self.BlockState.START:
                QtGui.QSyntaxHighlighter.setCurrentBlockState(syntaxHighlighter, self.BlockState.START)
                commentLength = len(text) - startIndex

            elif end.hasNext():
                QtGui.QSyntaxHighlighter.setCurrentBlockState(syntaxHighlighter, self.BlockState.END)
                endIndex = end.next().capturedEnd()
                commentLength = endIndex - startIndex

            QtGui.QSyntaxHighlighter.setFormat(syntaxHighlighter, startIndex, commentLength, self.multiLineFormat)

            if start.hasNext():
                startIndex = start.next().capturedStart()
            else:
                break

    def runFormatter(self, syntaxHighlighter, text):
        self.formatText(syntaxHighlighter, text)
        if self.isMultiline:
            self.formatMultilineText(syntaxHighlighter, text)


class SyntaxHighlighter(QtGui.QSyntaxHighlighter):
    formatters = []

    comments = Formatter()
    comments.setColor(255,255,0)
    comments.setMultilineColor(255,255,0)
    comments.setRegex("//.*")
    comments.setStartRegex("/\\*")
    comments.setEndRegex("\\*/")
    comments.setMultiline(True)
    formatters.append(comments)

    keywords = Formatter()
    keywords.setColor(200, 0, 200)
    _keywords = ["global", "const", "inline", "struct"]
    keywords.setRegex("({}) ".format('|'.join(_keywords)))
    formatters.append(keywords)

    types = Formatter()
    types.setColor(200, 0, 200)
    _types = ["float", "int", "void", "bool", "long", "double", "half", "size_t", "char"]
    types.setRegex("(^|\s)({0})(2|3|4|8|16)?".format('|'.join(_types)))
    formatters.append(types)

    controlFlow = Formatter()
    controlFlow.setColor(200, 0, 200)
    _controlFlow = ["if", "while", "do", "for", "else", "return"]
    controlFlow.setRegex("({0})".format('|'.join(_controlFlow)))
    formatters.append(controlFlow)

    macro = Formatter()
    macro.setColor(255, 228, 181)
    macro.setRegex("#.*")
    formatters.append(macro)

    def highlightBlock(self, text: str) -> None:
        for formatter in self.formatters:
            formatter.runFormatter(self, text)


class OpenCLTextEditor(QtWidgets.QTextEdit):
    def __init__(self, parent=None):
        """Yanky code editor for opencl with very basic opencl code highlight

        Args:
            parent (QtWidgets.QWidget, optional): Parent of this widget. Defaults to None.
        """
        super().__init__(parent)

        self.syntaxHighlighter = SyntaxHighlighter(self.document())
        self.tabCount = 4
        
        font = QtGui.QFont()
        font.setPixelSize(20)
        self.setFont(font)

        self.setStyleSheet("background-color:BLACK")

    def _tabFunc(self, functor):
        cursor = self.textCursor()

        selStart = cursor.selectionStart()
        selEnd = cursor.selectionEnd()

        blockStart = self.document().findBlock(selStart)
        lineStart = blockStart.position()

        blockEnd = self.document().findBlock(selEnd)
        lineEnd = blockEnd.position() + blockEnd.length()-1

        cursor.setPosition(lineStart, QtGui.QTextCursor.MoveMode.MoveAnchor)
        cursor.setPosition(lineEnd, QtGui.QTextCursor.MoveMode.KeepAnchor)

        lines = cursor.selectedText().splitlines()

        if not lines:
            lines.append("")

        for i in range(len(lines)):
            lines[i] = functor(lines[i])

        newText = "\n".join(lines)
        cursor.insertText(newText)

        if len(lines) > 1:
            cursor.setPosition(lineStart, QtGui.QTextCursor.MoveMode.MoveAnchor)
        cursor.setPosition(lineStart+len(newText), QtGui.QTextCursor.MoveMode.KeepAnchor)

        self.setTextCursor(cursor)
        
    
    def keyPressEvent(self, event: QtGui.QKeyEvent):
        if event.key() == QtCore.Qt.Key.Key_Tab:
            self._tabFunc(lambda x: " " * self.tabCount + x)
            event.accept()
            return

        if event.key() == QtCore.Qt.Key.Key_Backtab:
            reg = QtCore.QRegularExpression()
            reg.setPattern("^[ ]+")

            self._tabFunc(lambda x: x[min(self.tabCount, reg.globalMatch(x).next().capturedEnd()):] if x.startswith(" ") else x)
            event.accept()
            return
        
        if event.key() == QtCore.Qt.Key.Key_Return:
            cursor = self.textCursor()
            block = cursor.block()
            text = block.text()

            idx = text.find(" " * self.tabCount)
            cursor.insertBlock()

            if idx != -1:
                cursor.insertText(" " * self.tabCount * (idx+1))

            self.setTextCursor(cursor)
            event.accept()
            return

        return super().keyPressEvent(event)


if __name__ == "__main__":
    import os

    app = QtWidgets.QApplication()
    win = QtWidgets.QMainWindow()

    wid = OpenCLTextEditor(win)
    wid.setText("""/*
multiline
comment
*/

// single line comment
points hello;

#define

int hello = 0;
float3 vec = (1,1,0);
float5 = 0;
__kernel void hehe(
                const int globalSize,
                global int* hello
                )
{
    for (int i = 0; i<0; i++) {
        int j = 1;       
    }
}
""")

    win.setCentralWidget(wid)


    win.show()
    app.exec()
