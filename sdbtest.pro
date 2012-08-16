QT += core \
    sql qui testlib
TEMPLATE = app
HEADERS = window.h \
    sdb.h \
    rllib/rldefine.h \
    rllib/rlserial.h \
    rllib/rlsocket.h \
    rllib/rlthread.h \
    rllib/rlwthread.h \
    rllib/rlmodbus.h
SOURCES = main.cpp \
    window.cpp \
    rllib/rlmodbus.cpp \
    rllib/rlserial.cpp
