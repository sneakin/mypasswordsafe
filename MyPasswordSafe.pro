TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on release

LIBS	+= uuid-1.0.0/.libs/libuuid.a
INCLUDEPATH	+= uuid-1.0.0

HEADERS	+= src/safe.hpp \
	src/safeserializer.hpp \
	src/securedstring.hpp \
	src/encryptedstring.hpp \
	src/serializers.hpp \
	src/pwsafe/BlowFish.h \
	src/pwsafe/Util.h \
	src/pwsafe/sha1.h \
	src/pwsafe/PwsPlatform.h \
	src/myutil.hpp \
	src/exception.hpp \
	src/startupdlg.hpp \
	src/config.h \
	src/smartptr.hpp \
	src/safelistview.hpp \
	src/uuid.hpp
SOURCES	+= src/main.cpp \
	src/securedstring.cpp \
	src/safe.cpp \
	src/safeserializer.cpp \
	src/serializers.cpp \
	src/encryptedstring.cpp \
	src/pwsafe/BlowFish.cpp \
	src/pwsafe/Util.cpp \
	src/pwsafe/sha1.cpp \
	src/myutil.cpp \
	src/startupdlg.cpp \
	src/smartptr.cpp \
	src/safelistview.cpp \
	src/uuid.cpp
FORMS	= src/mypasswordsafe.ui \
	src/pwordeditdlg.ui \
	src/passphrasedlg.ui \
	src/preferencesdlg.ui \
	src/aboutdlg.ui \
	src/manualdlg.ui \
	src/startupdlgbase.ui \
	src/newpassphrasedlg.ui
IMAGES	= images/searchfind \
	images/edit_add.png \
	images/edit_remove.png \
	images/editcopy_1.png \
	images/about_banner.png \
	images/edit.png \
	images/editcut.png \
	images/encrypted.png \
	images/exit.png \
	images/filenew.png \
	images/filenew2.png \
	images/fileopen.png \
	images/filesave.png \
	images/filesaveas.png \
	images/info.png \
	images/configure.png \
	images/contexthelp.png \
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
	images/folder_blue_open.png
















unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
TRANSLATIONS = locale/mypasswordsafe_en.ts \
	locale/mypasswordsafe_c.ts \
	locale/mypasswordsafe_fr.ts \
        locale/mypasswordsafe_pig.ts
