TEMPLATE = app
TARGET = notepad

QT += widgets

qtHaveModule(printsupport): QT += printsupport
requires(qtConfig(fontdialog))

SOURCES += \
    main.cpp \
    qurannotepad.cpp \
    tewafuqeditor.cpp

HEADERS += \
    qurannotepad.h \
    tewafuqeditor.h

FORMS += \
    qurannotepad.ui

RESOURCES += \
    notepad.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/tutorials/notepad
INSTALLS += target

