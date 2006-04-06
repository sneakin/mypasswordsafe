TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on

LIBS	+= uuid-1.0.0/.libs/libuuid.a
INCLUDEPATH	+= uuid-1.0.0

# idle
CONFIG += idle
IDLE_CPP = src/tools/idle
INCLUDEPATH += $$IDLE_CPP
unix {
	LIBS += -lXss
}
include($$IDLE_CPP/idle.pri)

HEADERS	+= src/safe.hpp \
	src/safeserializer.hpp \
	src/securedstring.hpp \
	src/encryptedstring.hpp \
	src/serializers.hpp \
	src/pwsafe/PW_BlowFish.h \
	src/pwsafe/Util.h \
	src/pwsafe/PW_sha1.h \
	src/pwsafe/PwsPlatform.h \
	src/myutil.hpp \
	src/exception.hpp \
	src/startupdlg.hpp \
	src/config.h \
	src/smartptr.hpp \
	src/safelistview.hpp \
	src/uuid.hpp \
	src/xmlserializer.hpp \
	src/plaintextlizer.hpp \
	src/safedragobject.hpp \
	src/myendian.h \
	src/crypto/sha1.h \
	src/crypto/blowfish.h \
        src/crypto/cryptointerface.hpp \
        src/crypto/bfproxy.hpp \
        src/clipboard.hpp

SOURCES	+= src/main.cpp \
	src/securedstring.cpp \
	src/safe.cpp \
	src/safeserializer.cpp \
	src/serializers.cpp \
	src/encryptedstring.cpp \
	src/pwsafe/PW_BlowFish.cpp \
	src/pwsafe/Util.cpp \
	src/pwsafe/PW_sha1.cpp \
	src/myutil.cpp \
	src/startupdlg.cpp \
	src/smartptr.cpp \
	src/safelistview.cpp \
	src/uuid.cpp \
	src/xmlserializer.cpp \
	src/plaintextlizer.cpp \
	src/safedragobject.cpp \
	src/crypto/sha1.cpp \
	src/crypto/blowfish.cpp \
        src/crypto/bfproxy.cpp \
        src/clipboard.cpp

FORMS	= src/mypasswordsafe.ui \
	src/pwordeditdlg.ui \
	src/passphrasedlg.ui \
	src/preferencesdlg.ui \
	src/aboutdlg.ui \
	src/manualdlg.ui \
	src/startupdlgbase.ui \
	src/newpassphrasedlg.ui
IMAGES	= \
	images/edit_add.png \
	images/edit_remove.png \
	images/editcopy.png \
	images/about_banner.png \
	images/edit.png \
	images/editcut.png \
	images/exit.png \
	images/filenew.png \
	images/filenew2.png \
	images/fileopen.png \
	images/filesave.png \
	images/filesaveas.png \
	images/info.png \
	images/Padlock.xpm \
	images/fetch_user.png \
	images/help.png \
	images/back.png \
	images/forward.png \
	doc/sshots/editdialog.jpg \
	doc/sshots/mainwindow.jpg \
	doc/sshots/prefsdialog.jpg \
	images/password.png \
	images/undo.png \
	images/redo.png \
	images/folder_new.png \
	images/folder_blue.png \
	images/folder_blue_open.png \
	images/keys.png \
	images/button_ok.png \
	images/configure.png \
	images/contexthelp.png \
	images/lock.png \
	images/fileclose.png \
	images/button_cancel.png \
	images/file_locked.png

DEBUG = $$(DEBUG)

isEmpty(DEBUG) {
	config += release
} else {
	config += debug
	DEFINES += DEBUG
}

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
TRANSLATIONS = locale/mypasswordsafe_en.ts \
	locale/mypasswordsafe_c.ts \
	locale/mypasswordsafe_fr.ts \
        locale/mypasswordsafe_pig.ts
