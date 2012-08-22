#CONFIG += QT console
QT +=    sql
TEMPLATE = app
HEADERS =   window.h \
            sdb.h \
            rllib/rldefine.h \
            rllib/rlserial.h \
            rllib/rlsocket.h \
            rllib/rlthread.h \
            rllib/rlwthread.h \
            rllib/rlmodbus.h
SOURCES +=  main.cpp
SOURCES +=  window.cpp
SOURCES +=  rllib/rlmodbus.cpp
SOURCES +=  rllib/rlserial.cpp
