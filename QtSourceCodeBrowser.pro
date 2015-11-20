SOURCES += \
    main.cxx \
    MainWindow.cxx \
    BrowseSourceWidget.cxx \
    Highlighter.cxx \
    CodeEditor.cxx \
    SourceFileSystemModel.cxx \
    SourceFileSystemProxyModel.cxx \
    OpenDocumentsModel.cxx \
    NoteTextEdit.cxx \
    NoteRichTextEdit.cxx

HEADERS += \
    MainWindow.hxx \
    BrowseSourceWidget.hxx \
    Highlighter.hxx \
    CodeEditor.hxx \
    SourceFileSystemModel.hxx \
    SourceFileSystemProxyModel.hxx \
    OpenDocumentsModel.hxx \
    NoteTextEdit.hxx \
    NoteRichTextEdit.hxx

FORMS += \
    NoteRichTextEdit.ui

QT += \
    widgets \

CONFIG += c++11
