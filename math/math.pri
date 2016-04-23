INCLUDEPATH += $$PWD

HEADERS +=  $$PWD/matrix.hpp \
            $$PWD/calibration.hpp \
            $$PWD/smoothing_splines.h \
            $$PWD/Solvers.h \
            $$PWD/splines.h \
    $$PWD/cubicspline.h \
    $$PWD/exception.h \
    $$PWD/logsplines.h \
    $$PWD/idataanalysis.h

SOURCES += $$PWD/splines.cpp \
    $$PWD/cubicspline.cpp \
    $$PWD/logsplines.cpp \
    $$PWD/idataanalysis.cpp

LIBS += $$PWD/Lib/libCubicSpline.a
