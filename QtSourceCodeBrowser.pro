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
    NoteRichTextEdit.cxx \
    SourceCodeEditor.cxx \
    SourcesAndOpenFiles.cxx

HEADERS += \
    MainWindow.hxx \
    BrowseSourceWidget.hxx \
    Highlighter.hxx \
    CodeEditor.hxx \
    SourceFileSystemModel.hxx \
    SourceFileSystemProxyModel.hxx \
    OpenDocumentsModel.hxx \
    NoteTextEdit.hxx \
    NoteRichTextEdit.hxx \
    SourceCodeEditor.hxx \
    SourcesAndOpenFiles.hxx

FORMS += \
    NoteRichTextEdit.ui \
    SourceCodeEditor.ui \
    SourcesAndOpenFiles.ui

QT += \
    widgets \

CONFIG += c++11
