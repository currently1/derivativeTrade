# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
TEMPLATE = vcapp
TARGET = derivativeTrade
DESTDIR = Release/Win32
QT += core sql widgets gui
CONFIG += qt release
=======
=======
>>>>>>> parent of 77ddc69... Revert "fix"
TEMPLATE = app
TARGET = TraderQtApp
DESTDIR = Release/TraderQtApp/Win32
QT += core sql widgets gui
CONFIG += release
<<<<<<< HEAD
>>>>>>> parent of 77ddc69... Revert "fix"
=======
>>>>>>> parent of 77ddc69... Revert "fix"
=======
TEMPLATE = vcapp
TARGET = derivativeTrade
DESTDIR = Release/Win32
QT += core sql widgets gui
CONFIG += qt release
>>>>>>> parent of 3239b9d... Revert "fix"
DEFINES += WIN64 QT_DLL QT_SQL_LIB QT_WIDGETS_LIB
INCLUDEPATH += ./GeneratedFiles \
    . \
    ./GeneratedFiles/Release \
    src/ctpLib \
    src/common \
    src/header \
    src/OCILink \
    src/OptionModel \
    src/resources \
    src/source \
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    src/utility/instantclient_11_2/sdk/include \k
=======
    src/utility/instantclient_11_2/sdk/include \
>>>>>>> parent of 77ddc69... Revert "fix"
=======
    src/utility/instantclient_11_2/sdk/include \
>>>>>>> parent of 77ddc69... Revert "fix"
=======
    src/utility/instantclient_11_2/sdk/include \k
>>>>>>> parent of 3239b9d... Revert "fix"
    src/utility/easygoing \
    src/view
LIBS += -l src/ctpLib \
    -l src/instantclient_11_2/sdk/lib/msvc \
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

SUBDIRS += src

=======
>>>>>>> parent of 77ddc69... Revert "fix"
=======
>>>>>>> parent of 77ddc69... Revert "fix"
=======

SUBDIRS += src

>>>>>>> parent of 3239b9d... Revert "fix"
MOC_DIR += ./GeneratedFiles/release
OBJECTS_DIR += release
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

#message($$PWD)
include(src/derivativeTrade.pri)
=======
include(TraderQtApp.pri)
>>>>>>> parent of 77ddc69... Revert "fix"
=======
include(TraderQtApp.pri)
>>>>>>> parent of 77ddc69... Revert "fix"
=======

#message($$PWD)
include(src/derivativeTrade.pri)
>>>>>>> parent of 3239b9d... Revert "fix"
