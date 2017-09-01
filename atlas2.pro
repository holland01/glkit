TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    stb_image.c

QMAKE_CXXFLAGS += -std=c++14 -stdlib=libc++

INCLUDEPATH += /usr/local/include

LIBS += -L/usr/local/lib -lglfw3 -lGLEW -F/System/Library/Frameworks -framework OpenGL

HEADERS += \
    stb_image.h \
    glk_include_gl.h \
    glk_util.h \
    glk_atlas.h \
    glk_main_def.h \
    glk_core.h
