QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# SDL stuff
SDL2_PATH = D:\SDL
LIBS += -L$${SDL2_PATH}\lib\x64 -lSDL2 -lSDL2main -lSDL2_image
INCLUDEPATH += $${SDL2_PATH}\include

QMAKE_LFLAGS += $$QMAKE_LFLAGS_WINDOWS
QMAKE_LFLAGS += -lSDL2main -lSDL2 -lSDL2_image -mwindows
QMAKE_LINK += -lSDL2main -lSDL2 -lSDL2_image -mwindows

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/CPU/alu.cpp \
    src/CPU/branches.cpp \
    src/CPU/cop0.cpp \
    src/CPU/cpu.cpp \
    src/CPU/exceptions.cpp \
    src/CPU/loads_stores.cpp \
    src/bus.cpp \
    src/dma.cpp \
    src/main.cpp \
    src/psx.cpp

HEADERS += \
    include/bus.h \
    include/cop0.h \
    include/cpu.h \
    include/dma.h \
    include/gpu.h \
    include/helpers.h \
    include/psx.h \
    include/termcolor.hpp \
    include/types.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
