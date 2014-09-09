#ifndef TYPES_H
#define TYPES_H

enum AudioToolkitType_t
{
 AUDIOTOOLKIT_INVALID = -1,
 AUDIOTOOLKIT_OPENAL = 0,
 AUDIOTOOLKIT_SDL_MIXER,
 ////////////////////////////////////////
 MAX_AUDIOTOOLKIT_TYPES
};

enum TextToolkitType_t
{
 TEXTTOOLKIT_INVALD = -1,
 TEXTTOOLKIT_GLC = 0,
 TEXTTOOLKIT_FTGL,
 ////////////////////////////////////////
 MAX_TEXTTOOLKIT_TYPES
};

struct Configuration_t
{
  int    screen_width;
  int    screen_height;
  float  screen_FOV;
  float  screen_z_near;
  float  screen_z_far;
  float  screen_bound[2];
  float  z_transformation;

  bool   full_screen;
  bool   blend_enabled;
  int    graphics_level;
  bool   true_color;
  bool   texture_border;
  bool   show_FPS;
  float  view_gamma;
  TextToolkitType_t text_toolkit_type;

  bool   auto_speed;
  float  movement_speed;

  bool   audio_enabled;
  bool   swap_stereo;
  bool   use_playlist;
  bool   use_CD_audio;
  bool   use_MIDI_music;
  int    CDROM_device;
  float  volume_sound;
  float  volume_music;
  AudioToolkitType_t audio_toolkit_type;

  bool   debug;

  float  skill_base;
};

enum Mode_t
{
  MODE_INVALID = -1,
  MODE_SHOW_VERSION = 0,
  MODE_GAME,
  ///////////////////////////////////////
  MAX_MODE_TYPES
};

enum GameMode_t
{
  GAMEMODE_GAME_INVALID = -1,
  GAMEMODE_GAME = 0,
  GAMEMODE_GAME_OVER,
  GAMEMODE_LEVEL_COMPLETE,
  GAMEMODE_MENU,
  ///////////////////////////////////////
  MAX_GAMEMODE_TYPES
};

#include <cstdio>

// forward declarations
class Splot_PlayerAircraft;
class Splot_PlayerBullets;
class Splot_Enemies;
class Splot_EnemyBullets;
class Splot_Explosions;
class Splot_PowerUps;
class Splot_Audio;
class Splot_Background;
class Splot_Menu;
class Splot_StatusDisplay;
class Splot_Text;
class Splot_Main;
class Splot_HighScore;
class Splot_GameElement;

#include <list>

struct ScreenElement_t
{
  int                release_time;
  Splot_GameElement* game_element;
};
typedef std::list<ScreenElement_t> ScreenElements_t;
typedef ScreenElements_t::iterator ScreenElementsIterator_t;

struct State_t
{
  Splot_HighScore* highscore;
  Splot_Main*      toolkit;

  bool             mouse_active;
  bool             joy_active;

  Splot_Text*      text;

  float            FPS;
  int              frame;
  int              game_frame;
  float            game_speed;
  float            game_skill;
  unsigned int     game_level;
  unsigned int     max_level;
  float            speed_adj;

  int              CDROM_count;

  float            scroll_speed;

  bool             game_pause;
  bool             game_quit;

  int              player_death;
  int              player_success;

  GameMode_t       game_mode;

  Splot_PlayerAircraft* player;
  Splot_Enemies*        enemies;
  Splot_PlayerBullets*  player_bullets;
  Splot_EnemyBullets*   enemy_bullets;
  ScreenElements_t      screen_elements;

  Splot_Explosions*     explosions;
  Splot_PowerUps*       power_ups;
  Splot_Audio*          audio;
  Splot_Background*     background;
  Splot_Background*     background_game;
  Splot_Background*     background_menu;
  Splot_Menu*           menu;
  Splot_StatusDisplay*  status_display;

  float            cursor_position[3];

  // *TODO*: save randomness/state with static savegames
  // *TODO*: keep randomness/state here to support re-entrant code
  //int              randomI[256];
  //float            randomF[256];
  //float            randomS[256];
  //int              random_index;

  FILE*            event_file;

  int              tip_ship_past;
  int              tip_super_shield;
};

#include "defines.h"

struct GameState_t
{
  float damage;
  float shields;

  float score;
  float score_step;
  float score_target;

  float death_stereo;

  int ships;
  int super_bomb_exploding;

  float ammo_stock[NUM_PLAYER_AMMUNITION_TYPES];
  float gun_pause[NUM_PLAYER_AMMUNITION_TYPES];
  bool gun_swap;
  bool gun_trigger;

  int current_item_index;
  float use_item_armed;
};

enum GameToolkitType_t
{
  TOOLKIT_INVALID = -1,
#if defined (USE_GLUT)
  TOOLKIT_GLUT = 0,
#elif defined (USE_SDL)
  TOOLKIT_SDL,
#else
#error "USE_SDL or USE_GLUT must be defined"
#endif
  ///////////////////////////////////////
  MAX_TOOLKIT_TYPES
};

struct Bullet_t
{
  float position[3];
  float translation_step[3];
  float damage;

  static unsigned int count;
};

#include <vector>

typedef std::vector<Bullet_t> Bullets_t;
typedef Bullets_t::iterator BulletsIterator_t;
typedef Bullets_t::const_iterator BulletsConstIterator_t;

struct Explosion_t
{
  // *TODO*: remove these
  Explosion_t ()
   : age (0)
   , size (1)
  {
    position[0] = 0.0; position[1] = 0.0; position[2] = 0.0;
    translation_step[0] = 0.0; translation_step[1] = 0.0; translation_step[2] = 0.0;
    color[0] = 1.0; color[1] = 1.0; color[2] = 1.0; color[3] = 1.0;
    count++;
  }
  ~Explosion_t () { count--; }
  void init (const float (&)[3], // position
             int = 0,            // age
             float = 1.0);       // size
  void init (const float (&)[3], // position
             const float (&)[3], // translation step
             const float (&)[4], // color
             int = 0,            // age
             float = 1.0);       // size

  float position[3];
  float translation_step[3];
  float color[4];
  int   age;
  float size;

  static unsigned int count;
};

typedef std::vector<Explosion_t*> Explosions_t;
typedef Explosions_t::iterator ExplosionsIterator_t;

enum Key_t
{
  KEY_INVALID = -1,
  KEY_DOWN = 0,
  KEY_ENTER,
  KEY_ESCAPE,
  KEY_LEFT,
  KEY_RIGHT,
  KEY_SPACE,
  KEY_TAB,
  KEY_UP,
  ///////////////////////////////////////
  MAX_KEY_TYPES
};

enum Button_t
{
  BUTTON_INVALID = -1,
  BUTTON_LEFT = 0,
  BUTTON_MIDDLE,
  BUTTON_RIGHT,
  ///////////////////////////////////////
  MAX_BUTTON_TYPES
};

#endif // TYPES_H
