idle {
	HEADERS += $$IDLE_CPP/idle.h
	SOURCES += $$IDLE_CPP/idle.cpp

	unix:!mac {
		SOURCES += $$IDLE_CPP/idle_x11.cpp
	}
	win32: {
		SOURCES += $$IDLE_CPP/idle_win.cpp
	}
	mac: {
		SOURCES += $$IDLE_CPP/idle_mac.cpp
	}
}
