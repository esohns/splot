#ifndef TYPES_H
#define TYPES_H

enum AudioType_t
{
 AUDIO_INVALID = -1,
 AUDIO_OPENAL = 0,
 AUDIO_SDL_MIXER,
 ////////////////////////////////////////
 MAX_AUDIO_TYPES
};

enum TextType_t
{
 TEXT_INVALD = -1,
 TEXT_GLC = 0,
 TEXT_FTGL,
 ////////////////////////////////////////
 MAX_TEXT_TYPES
};

struct Configuration_t
{
  int    screen_width;
  int    screen_height;
  float  screen_aspect;
  float  screen_FOV;
  float  screen_z_near;
  float  screen_z_far;
  float  screen_bound[2];
  float  z_trans;

  bool   full_screen;
  bool   blend_enabled;
  int    gfx_level;
  bool   true_color;
  bool   tex_border;
  bool   show_fps;
  float  view_gamma;
  TextType_t text_type;

  bool   auto_speed;
  float  movement_speed;

  bool   audio_enabled;
  bool   swap_stereo;
  bool   use_playList;
  bool   use_cdrom;
  int    cdrom_device;
  float  vol_sound;
  float  vol_music;
  AudioType_t audio_type;

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

  float            fps;
  int              frame;
  int              game_frame;
  float            game_speed;
  float            game_skill;
  unsigned int     game_level;
  unsigned int     max_level;
  float            speed_adj;

  int              cdrom_count;

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

  float            cursor_pos[3];

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

  float ammo_stock[NUM_PLAYER_AMMO_TYPES];
  float gun_pause[NUM_PLAYER_AMMO_TYPES];
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
    velocity[0] = 0.0; velocity[1] = 0.0; velocity[2] = 0.0;
    clr[0] = 1.0; clr[1] = 1.0; clr[2] = 1.0; clr[3] = 1.0;
    count++;
  }
  ~Explosion_t () { count--; }
  void init (const float (&)[3], // position
             int = 0,            // age
             float = 1.0);       // size
  void init (const float (&)[3], // position
             const float (&)[3], // direction
             const float (&)[4], // color
             int = 0,            // age
             float = 1.0);       // size

  float position[3];
  float velocity[3];
  float clr[4];
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
