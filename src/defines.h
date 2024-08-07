#ifndef DEFINES_H
#define DEFINES_H

#if defined (HAVE_CONFIG_H)
#include "splot_config.h"
#endif

#include "ace/config-lite.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
// On Windows SDL can only do 32x32 icons
#define SPLOT_WINDOW_ICON                   "icon32.png"
#else
// Elsewhere SDL can do any size icons
#define SPLOT_WINDOW_ICON                   "hero.png"
#endif
#define SPLOT_WINDOW_TITLE                  PACKAGE
#define SPLOT_ICON_CAPTION                  PACKAGE

#define SPLOT_DATA_DIR_ENV_SYMBOL           "SPLOT_DATA"
#define SPLOT_FONT_FILE                     "gothub__.ttf"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define SPLOT_HOME_ENV_SYMBOL               "USERPROFILE"
#else
#define SPLOT_HOME_ENV_SYMBOL               "HOME"
#endif

#define SPLOT_HIGHSCORE_HEADER_LINE          "# splot high scores file: skill rank score date time(UTC) name (do not remove this comment line)"
#define SPLOT_HIGHSCORE_MAXIMUM_LEVEL_PREFIX "maximum level attained: "
#define SPLOT_HIGHSCORE_DEFAULT_NAME         "nobody"
#define SPLOT_HIGHSCORE_ENV_SYMBOL           "SPLOT_SCORE"

#define CONFIGURATION_DEFAULT_SCREEN_WIDTH     1024
#define CONFIGURATION_DEFAULT_SCREEN_HEIGHT    786
#define CONFIGURATION_DEFAULT_SCREEN_FOV       30.0F
#define CONFIGURATION_DEFAULT_SCREEN_ZNEAR     10.0
#define CONFIGURATION_DEFAULT_SCREEN_ZFAR      70.0
#define CONFIGURATION_DEFAULT_SCREEN_BOUND_0   11.0
#define CONFIGURATION_DEFAULT_SCREEN_BOUND_1   9.0
#define CONFIGURATION_DEFAULT_Z_TRANSFORMATION -56.5

#define CONFIGURATION_DEFAULT_FULLSCREEN     false
#define CONFIGURATION_DEFAULT_BLEND_ENABLED  true
#define CONFIGURATION_DEFAULT_GRAPHICS_LEVEL 2
#define CONFIGURATION_DEFAULT_TRUE_COLOR     false
#define CONFIGURATION_DEFAULT_TEXTURE_BORDER true
#define CONFIGURATION_DEFAULT_SHOW_FPS       false
#define CONFIGURATION_DEFAULT_VIEW_GAMMA     1.1F
#ifdef USE_FTGL_TEXT
#define CONFIGURATION_DEFAULT_TEXTTOOLKITTYPE TEXTTOOLKIT_FTGL
#else
#define CONFIGURATION_DEFAULT_TEXTTOOLKITTYPE TEXTTOOLKIT_GLC
#endif

#define CONFIGURATION_DEFAULT_AUTO_SPEED     false
#define CONFIGURATION_DEFAULT_MOVEMENT_SPEED 0.03F

#define CONFIGURATION_DEFAULT_AUDIO_ENABLED  true
#define CONFIGURATION_DEFAULT_SWAP_STEREO    false
#define CONFIGURATION_DEFAULT_USE_PLAYLIST   false
#define CONFIGURATION_DEFAULT_USE_CDAUDIO    false
#define CONFIGURATION_DEFAULT_USE_MIDIMUSIC  false
#define CONFIGURATION_DEFAULT_VOLUME_SOUND   0.5F
#define CONFIGURATION_DEFAULT_VOLUME_MUSIC   0.5
#ifdef USE_SDLMIXER_AUDIO
#define CONFIGURATION_DEFAULT_AUDIOTOOLKITTYPE AUDIOTOOLKIT_SDL_MIXER
#else
#define CONFIGURATION_DEFAULT_AUDIOTOOLKITTYPE AUDIOTOOLKIT_OPENAL
#endif

#define CONFIGURATION_DEFAULT_DEBUG          false

#define CONFIGURATION_DEFAULT_SKILL_BASE     0.5

#define CONFIGURATION_INI_FILE_ROOT_SECTION  PACKAGE

#define OPTIONS_LONG_OPTION_NOAUDIO          "noaudio"
#define OPTIONS_LONG_OPTION_NOBLEND          "noblend"
#define OPTIONS_LONG_OPTION_CDMUSIC          "cdmusic"
#define OPTIONS_LONG_OPTION_MIDIMUSIC        "midimusic"
#define OPTIONS_LONG_OPTION_SHOWFPS          "showfps"
#define OPTIONS_LONG_OPTION_DEBUG            "debug"
#define OPTIONS_LONG_OPTION_FULLSCREEN       "fullscreen"
#define OPTIONS_LONG_OPTION_INIFILE          "inifile"
#define OPTIONS_LONG_OPTION_VIDEOMODE        "videomode"
#define OPTIONS_LONG_OPTION_NOTEXTUREBORDER  "notextureborder"
#define OPTIONS_LONG_OPTION_VERSION          "version"
#define OPTIONS_LONG_OPTION_WINDOWED         "window"

#define MAX_LEVEL                            4

#define MAX_PLAYER_NAME_LENGTH               100

#define MOVEMENT_SPEED_MIN                   0.01F
#define MOVEMENT_SPEED_MAX                   0.1F

#define SKILL_BASE_MIN                       0.2F
#define SKILL_BASE_MAX                       0.9F
#define SKILL_LEVEL_INCREMENT                0.05F
#define SKILL_MAX                            1.9F
#define SKILL_LEVEL_MAX_INT                  10

#define STATE_DEFAULT_FPS                    50.0

#define STATE_DEFAULT_GAME_SPEED             0.5
#define STATE_DEFAULT_GAME_SKILL             1.0
#define STATE_DEFAULT_SPEED_ADJUSTMENT       1.0
#define STATE_DEFAULT_SCROLL_SPEED           -0.045F
#define STATE_DEFAULT_TOOLKIT                TOOLKIT_SDL

#define STATE_DEFAULT_PLAYER_SHIPS           4
#define MAX_PLAYER_SHIPS                     9

#define SPLOT_DATA_DIR                       "data"
#define SPLOT_FONT_DATA_DIR                  "fnt"
#define SPLOT_IMAGE_DATA_DIR                 "png"
#define SPLOT_AUDIO_DATA_DIR                 "wav"

#if defined (USE_DYNAMIC_RANDOMNESS)
#define SRAND (2.0*(0.5-(ACE_OS::rand ()/(double)RAND_MAX)))
#define FRAND (ACE_OS::rand ()/(double)RAND_MAX)
#define IRAND (ACE_OS::rand ())
#else
#define SRAND (Splot_State::randomS[(++Splot_State::random_index)%256])
#define FRAND (Splot_State::randomF[(++Splot_State::random_index)%256])
#define IRAND (Splot_State::randomI[(++Splot_State::random_index)%256])
#endif

#define AMMUNITION_REFILL                    150
#define HERO_Z                               25.0F

#define MIN_SCREEN_SIZE                      0 // 320x200
#define MAX_SCREEN_SIZE                      8 // 1280x1024

#define NUM_ENEMY_AMMUNITION_TYPES           5
#define NUM_PLAYER_AMMUNITION_TYPES          3
#define MAX_PLAYER_ITEMS                     1

#define PLAYER_DEFAULT_DAMAGE                -500.0F
#define PLAYER_DEFAULT_SHIELDS               500.0F

#define DEATH_SPIKES                         7
#define DEATH_TIME                           50

#define SUPER_BOMB_DAMAGE                    5000.0F
#define MAX_COLLISION_DAMAGE                 35.0F

#define SCORE_STEP                           50000.0F
#define SCORE_END_OF_LEVEL                   5000.0F
#define SCORE_KILL_DEFAULT                   75.0F
#define SCORE_KILL_OMNI                      25.0F
#define SCORE_KILL_RAYGUN                    1000.0F
#define SCORE_KILL_TANK                      1500.0F
#define SCORE_KILL_GNAT                      10.0F
#define SCORE_KILL_BOSS_0                    SCORE_END_OF_LEVEL
#define SCORE_KILL_BOSS_1                    SCORE_END_OF_LEVEL
#define SCORE_POWERUP_SHIELD_SUPER           2500.0F
#define SCORE_POWERUP_SHIELD                 1000.0F
#define SCORE_POWERUP_REPAIR                 1000.0F
#define SCORE_POWERUP_AMMUNITION             100.0F

#ifdef _APPLE_
#define CONFIG_EXT                           ".cfg"
#define CONFIG_FILE                          PACKAGE".cfg"
#define CONFIG_LOG_FILE                      PACKAGE"-log.txt"
#define CONFIG_SCORE_FILE                    PACKAGE"-score.cfg"
#define CONFIG_MUSIC_FILE                    PACKAGE"-music.cfg"
#elif defined (_MSC_VER)
#define CONFIG_EXT
#define CONFIG_FILE                          PACKAGE"-configuration"
#define CONFIG_LOG_FILE                      PACKAGE"-log"
#define CONFIG_SCORE_FILE                    PACKAGE"-score"
#define CONFIG_MUSIC_FILE                    PACKAGE"-music"
#else
#define CONFIG_EXT
#define CONFIG_FILE                          "."PACKAGE
#define CONFIG_LOG_FILE                      "."PACKAGE"-log"
#define CONFIG_SCORE_FILE                    "."PACKAGE"-score"
#define CONFIG_MUSIC_FILE                    "."PACKAGE"-music"
#endif // _APPLE_

#define HI_SCORE_HIST                        5

#define NUM_EXPLO                            2
#define NUM_EXPLO_POP                        6
#define MAX_MUSIC                            32
#define MAX_MIXING_CHANNELS                  32

#define AUDIO_FREQUENCY                      22050
#define AUDIO_SDL_FORMAT                     AUDIO_S16
#define AUDIO_CHANNELS                       2
#define AUDIO_CHUNKSIZE                      512

#define NUM_HELP_MESSAGE_LINES               7

#define DEBUG_NO_ENEMIES                     0
#define DEBUG_NO_ENEMY_FIRE                  0
#define DEBUG_NO_AMMUNITION                  0
#define DEBUG_NO_POWERUPS                    0
#define DEBUG_LAZY_LOADING                   0
#define DEBUG_BACKGROUND                     1

#define WOBBLE_0                             45
#define WOBBLE_1                             75

#define OMNI_SWAP                            138

#endif // DEFINES_H
