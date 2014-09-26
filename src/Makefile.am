MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = splot

splot_CPPFLAGS = \
	$(GL_CFLAGS) \
	$(GLU_CFLAGS) \
	$(TEXT_CFLAGS) \
	$(IMAGE_CFLAGS) \
	$(WINDOW_CFLAGS) \
	$(AUDIO_CFLAGS) \
	"-DPACKAGE=\"$(PACKAGE)\"" \
	"-DLOCALEDIR=\"$(localedir)\"" \
	"-DPKGDATADIR=\"$(pkgdatadir)\""

splot_LDADD = \
	$(GL_LIBS) \
	$(GLU_LIBS) \
	$(TEXT_LIBS) \
	$(IMAGE_LIBS) \
	$(WINDOW_LIBS) \
	$(AUDIO_LIBS) \
	$(INTLLIBS)

noinst_HEADERS = stdafx.h
include_HEADERS = \
	audio.h \
	audio_SDLmixer.h \
	background.h \
	background_metal.h \
	background_metal_segment.h \
  background_sea.h \
  background_sea_segment.h \
	background_segment.h \
	common.h \
	configuration.h \
	defines.h \
	enemies.h \
	enemy_aircraft.h \
	enemy_bullets.h \
	enemy_fleet.h \
	explosion.h \
	game_element.h \
	highscore.h \
	image.h \
	main.h \
	main_SDL.h \
	menu.h \
	OpenGL_common.h \
	player_aircraft.h \
	player_bullets.h \
	powerup.h \
	screen.h \
	state.h \
	status_display.h \
	text.h \
	text_FTGL.h \
	textGeometry.h \
	types.h

splot_SOURCES = \
	audio.cpp \
	audio.h \
	audio_SDLmixer.cpp \
	audio_SDLmixer.h \
	background.cpp \
	background.h \
	background_metal.cpp \
	background_metal.h \
	background_metal_segment.cpp \
	background_metal_segment.h \
  background_sea.cpp \
  background_sea.h \
  background_sea_segment.cpp \
  background_sea_segment.h \
	background_segment.cpp \
	background_segment.h \
	common.cpp \
	common.h \
	configuration.cpp \
	configuration.h \
	defines.h \
	enemies.cpp \
	enemies.h \
	enemy_aircraft.cpp \
	enemy_aircraft.h \
	enemy_bullets.cpp \
	enemy_bullets.h \
	enemy_fleet.cpp \
	enemy_fleet.h \
	explosion.cpp \
	explosion.h \
	game_element.cpp \
	game_element.h \
	highscore.cpp \
	highscore.h \
	image.cpp \
	image.h \
	main.cpp \
	main.h \
	main_SDL.cpp \
	main_SDL.h \
	main_SDL_event.cpp \
	menu.cpp \
	menu.h \
	OpenGL_common.cpp \
	OpenGL_common.h \
	player_aircraft.cpp \
	player_aircraft.h \
	player_bullets.cpp \
	player_bullets.h \
	powerup.cpp \
	powerup.h \
	screen.cpp \
	screen.h \
	splot.cpp \
	state.cpp \
	state.h \
	status_display.cpp \
	status_display.h \
	stdafx.h \
	text.cpp \
	text.h \
	text_FTGL.cpp \
	text_FTGL.h \
	textGeometry.h \
	textGeometryBSU.cpp \
	textGeometryChromium.cpp \
	types.h

