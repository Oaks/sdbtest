# CONFIG += QT console
QT += sql
TEMPLATE = app
HEADERS = window.h \
    sdb.h \
    rllib/rldefine.h \
    rllib/rlserial.h \
    rllib/rlsocket.h \
    rllib/rlthread.h \
    rllib/rlwthread.h \
    rllib/rlmodbus.h
SOURCES += main.cpp \
    rb_button.cpp \
    read_keys.cpp \
    bn_switch.cpp \
    kp_select.cpp \
    param_select.cpp
SOURCES += window.cpp
SOURCES += rllib/rlmodbus.cpp
SOURCES += rllib/rlserial.cpp
