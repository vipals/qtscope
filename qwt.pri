#-------------------------------------------------
#
# Qwt Library configuration file
#
#-------------------------------------------------

QWT_VER = 6.1.3

contains(QWT_VER,^5\\..*\\..*) {
    VER_SFX     = 5
    UNIX_SFX    = -qt4
} else {
    VER_SFX     =
    UNIX_SFX    = -qt5
}

unix {
    QWT_PATH = /usr
    QWT_INC_PATH = $${QWT_PATH}/include/qwt
    QWT_LIB  = qwt$${UNIX_SFX}
}

INCLUDEPATH += $${QWT_INC_PATH}
LIBS += -L$${QWT_PATH}/lib -l$${QWT_LIB} -lliquid
