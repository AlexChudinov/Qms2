INCLUDEPATH += $$PWD \
               $$PWD/QCustomPlot/Include

HEADERS += $$PWD/zoomplot.h \
    $$PWD/zoomplottoolbar.h

SOURCES += $$PWD/zoomplot.cpp \
    $$PWD/zoomplottoolbar.cpp

LIBS += $$PWD/QCustomPlot/Lib/libqcustomplotd.a
