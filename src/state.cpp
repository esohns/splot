#include "stdafx.h"

#include "state.h"

#include "ace/Global_Macros.h"
#include "ace/OS_Memory.h"
#include "ace/OS.h"

//#include "gettext.h"

#include "highscore.h"
#include "configuration.h"

#include "audio.h"
#include "background.h"
#include "menu.h"
#include "OpenGL_common.h"
#include "powerup.h"
#include "player_bullets.h"
#include "enemy_bullets.h"
#include "enemies.h"
#include "explosion.h"
#include "player_aircraft.h"
#include "screen.h"
#include "status_display.h"
#include "background_metal.h"
#if defined (USE_GLUT)
#include "MainGLUT.h"
#elif defined (USE_SDL)
#include "main_SDL.h"
#else
#error "USE_SDL or USE_GLUT must be defined"
#endif

#if defined (USE_OPENAL_AUDIO)
#include "AudioOpenAL.h"
#endif
#if defined (USE_SDLMIXER_AUDIO)
#include "audio_SDLmixer.h"
#endif

Splot_State::Splot_State ()
{
  state_.text               = NULL;

  state_.toolkit            = NULL;
  state_.highscore          = NULL;

  state_.mouse_active       = false;
  state_.joy_active         = false;

  state_.FPS                = STATE_DEFAULT_FPS;
  state_.frame              = 0;
  state_.game_frame         = 0;
  state_.game_speed         = STATE_DEFAULT_GAME_SPEED;
  state_.game_skill         = STATE_DEFAULT_GAME_SKILL;
  state_.game_level         = 1;
  state_.max_level          = 1;
  state_.speed_adj          = STATE_DEFAULT_SPEED_ADJUSTMENT;

  state_.CDROM_count        = 1;

  state_.game_mode          = GAMEMODE_MENU;

  state_.player_death       = 0;
  state_.player_success     = 0;

  state_.scroll_speed       = STATE_DEFAULT_SCROLL_SPEED;

  state_.player             = NULL;
  state_.enemies            = NULL;
  state_.player_bullets     = NULL;
  state_.enemy_bullets      = NULL;
  //state_.screen_elements.clear ();
  state_.explosions         = NULL;
  state_.power_ups          = NULL;
  state_.audio              = NULL;
  state_.background         = NULL;
  state_.background_game    = NULL;
  state_.background_menu    = NULL;
  state_.menu               = NULL;
  //state_.main_GL           = NULL;
  state_.status_display     = NULL;

  state_.cursor_position[0] = 0.0;
  state_.cursor_position[1] = 0.0;
  state_.cursor_position[2] = HERO_Z;

  state_.event_file         = NULL;

  state_.tip_ship_past      = 0;
  state_.tip_super_shield   = 0;

  state_.game_pause         = false;
  state_.game_quit          = false;

  ///////////////////////////////////////
  game_state_.damage               = PLAYER_DEFAULT_DAMAGE;
  game_state_.shields              = PLAYER_DEFAULT_SHIELDS;

  game_state_.score                = 0;
  game_state_.score_step           = SCORE_STEP;
  game_state_.score_target         = SCORE_STEP;

  game_state_.death_stereo         = 0.0F;

  game_state_.ships                = STATE_DEFAULT_PLAYER_SHIPS;
  game_state_.super_bomb_exploding = 0;

  ACE_OS::memset (game_state_.ammo_stock, 0, sizeof (game_state_.ammo_stock));

  ACE_OS::memset (game_state_.gun_pause, 0, sizeof (game_state_.gun_pause));
  game_state_.gun_swap             = false;
  game_state_.gun_trigger          = false;

  game_state_.current_item_index   = 0;
  game_state_.use_item_armed       = 0.0;
}

Splot_State::~Splot_State ()
{
  if (state_.highscore)
    delete state_.highscore;
}

void
Splot_State::initialize ()
{
  Splot_PlayerBullets::initialize ();
  Splot_EnemyBullets::initialize ();
  Splot_PowerUps::initialize ();
}

bool
Splot_State::initialize (GameToolkitType_t toolkit_in,
                         int argc_in,
                         ACE_TCHAR** argv_in)
{
  // step0: initialize randomness
  initializeRandomness ();

  // step1: initialize highscore table
  ACE_ASSERT (!state_.highscore);
  ACE_NEW_RETURN (state_.highscore,
                  Splot_HighScore (Splot_HighScore::getFileName ()),
                  false);

  // step2: initialize toolkit
  switch (toolkit_in)
  {
#if defined (USE_GLUT)
    case TOOLKIT_GLUT:
      ACE_NEW_RETURN (state_.toolkit,
                      MainGLUT (argc_in, argv_in),
                      false);
      break;
#elif defined (USE_SDL)
    case TOOLKIT_SDL:
      ACE_NEW_RETURN (state_.toolkit,
                      Splot_MainSDL (argc_in, argv_in),
                      false);
      break;
#else
#error "USE_SDL or USE_GLUT must be defined"
#endif
    case TOOLKIT_INVALID:
    default:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("invalid/unknown toolkit (was: %d), aborting\n"),
                  toolkit_in));

      return false;
    }
  } // end SWITCH

  return true;
}

void
Splot_State::run ()
{
  ACE_ASSERT (state_.toolkit);
  state_.toolkit->run ();
}

void
Splot_State::newGame ()
{
  Splot_Configuration* configuration =
   SPLOT_CONFIGURATION_SINGLETON::instance ();
  ACE_ASSERT (configuration);

  state_.highscore->set (configuration->intSkill (), game_state_.score);

  state_.game_skill = configuration->get ().skill_base+0.5F;
  state_.game_skill += (state_.game_level-1)*0.05F;
  state_.game_frame = 0;

  ACE_ASSERT (state_.enemies);
  state_.enemies->getFleet ()->clear ();
  ACE_ASSERT (state_.power_ups);
  state_.power_ups->clear ();
  ACE_ASSERT (state_.enemy_bullets);
  state_.enemy_bullets->clear ();
  ACE_ASSERT (state_.player_bullets);
  state_.player_bullets->clear ();
  Splot_Screen::clear ();
  ACE_ASSERT (state_.screen_elements.empty ());
  ACE_ASSERT (state_.explosions);
  state_.explosions->clear ();
  game_state_.score = 0;
  ACE_ASSERT (state_.player);
  state_.player->reset ();
  state_.player->setShips (STATE_DEFAULT_PLAYER_SHIPS);

  state_.game_pause = false;

  if (!Splot_Screen::load ())
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Screen::load (), continuing\n")));

  if (state_.background_game != state_.background_menu)
  {
    // delete game ground and set to menu/game common ground
    delete state_.background_game;
    state_.background_game = state_.background_menu;
  } // end IF
  state_.background = state_.background_game;
  state_.background->setVariation (state_.game_level-1);

  state_.audio->setMusicIndex (state_.game_level-1);

  if (state_.event_file)
    if (ACE_OS::fclose (state_.event_file))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to fclose () event file: \"%m\", continuing\n")));
  //	//-- cheezy, partially functional record mechanism
  //	Splot_State::eventFile = fopen("EVENT_FILE.txt", "w");
  //	Splot_State::eventFile = fopen("EVENT_FILE.txt", "r");

  //	Splot_State::scrollSpeed = -Splot_State::gameSkill*0.05;
  //	fprintf(stderr, "%f\n", Splot_State::scrollSpeed);

  ///////////////////////////////////////
  resetGame ();
}

void
Splot_State::resetGame (bool playerOnly_in)
{
  game_state_.damage = PLAYER_DEFAULT_DAMAGE;
  game_state_.shields = PLAYER_DEFAULT_SHIELDS;

  game_state_.current_item_index = 0;
  game_state_.use_item_armed = 0.0;

  ACE_OS::memset (game_state_.ammo_stock, 0, sizeof (game_state_.ammo_stock));
  // ammoStock_[i] = AMMO_REFILL;

  ACE_OS::memset (game_state_.gun_pause, 0, sizeof (game_state_.gun_pause));
  game_state_.gun_swap = false;
  game_state_.gun_trigger = false;

  if (playerOnly_in)
    return; // done

  game_state_.score = 0;
  game_state_.score_step = SCORE_STEP;
  game_state_.score_target = SCORE_STEP;

  game_state_.death_stereo = 0.0F;

  game_state_.ships = STATE_DEFAULT_PLAYER_SHIPS;
  game_state_.super_bomb_exploding = 0;
}

void
Splot_State::gotoNextLevel ()
{
  Splot_Configuration* configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ();
  ACE_ASSERT (configuration);

  if (state_.game_level > MAX_LEVEL)
    state_.game_level++;
  if (state_.max_level < state_.game_level)
    state_.max_level = state_.game_level;
  if (state_.max_level > MAX_LEVEL)
    state_.max_level = MAX_LEVEL;

  state_.game_skill += SKILL_LEVEL_INCREMENT;
  if (state_.game_skill > SKILL_MAX)
    state_.game_skill = SKILL_MAX;

  state_.game_frame = 0;

  state_.enemies->getFleet ()->clear ();
  state_.power_ups->clear ();
  state_.enemy_bullets->clear ();
  state_.player_bullets->clear ();
  Splot_Screen::clear ();
  ACE_ASSERT (state_.screen_elements.empty ());

  state_.player->fullRepair ();

  if (!Splot_Screen::load ())
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Screen::load (), continuing\n")));

  // when more than one ground is used, check here if it need to be created
  state_.background->nextVariation ();

  state_.audio->nextMusicIndex ();
}

void
Splot_State::initGame ()
{
  const Configuration_t& configuration =
   SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();

  if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("starting up...\n")));

  //ACE_NEW (state_.main_GL,
  //         MainGL ());
  ACE_NEW (state_.explosions,
           Splot_Explosions ());
  ACE_NEW (state_.enemies,
           Splot_Enemies ());
  ACE_NEW (state_.player,
           Splot_PlayerAircraft ());
  ACE_NEW (state_.player_bullets,
           Splot_PlayerBullets ());
  ACE_NEW (state_.enemy_bullets,
           Splot_EnemyBullets ());
  ACE_NEW (state_.status_display,
           Splot_StatusDisplay ());
  ACE_NEW (state_.power_ups,
           Splot_PowerUps ());
  ACE_NEW (state_.background,
           Splot_BackgroundMetal ());
  ACE_NEW (state_.menu,
           Splot_Menu ());
  //Splot_Screen::clear ();

#if defined (USE_OPENAL_AUDIO) && defined (USE_SDLMIXER_AUDIO) 
  if (configuration.audio_type == AUDIO_OPENAL)
    ACE_NEW (state_.audio,
             Splot_AudioOpenAL ());
  else
    ACE_NEW (state_.audio,
             Splot_AudioSDLMixer ());
#elif defined (USE_OPENAL_AUDIO)
  ACE_NEW (state_.audio,
           Splot_AudioOpenAL ());
#elif defined (USE_SDLMIXER_AUDIO)
  ACE_NEW (state_.audio,
           Splot_AudioSDLMixer ());
#elif defined (USE_SDL)
  ACE_NEW (state_.audio,
           Splot_Audio ()); // (with SDL) this can only do CD audio
#endif

  state_.background_game = state_.background_menu = state_.background;

  //newGame ();

  state_.audio->setMusicMode (MUSIC_MENU);

  if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("starting up...DONE\n")));
}

void
Splot_State::finiGame ()
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();

  if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT("shutting down...\n")));
  
  //delete state_.main_GL;
  delete state_.enemies;
  delete state_.player;
  delete state_.player_bullets;
  delete state_.enemy_bullets;
  delete state_.status_display;
  delete state_.explosions;
  delete state_.power_ups;
  delete state_.background;
  delete state_.menu;
  //state_.screen_elements.clear ();
  delete state_.audio;

  if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT("shutting down...DONE\n")));
}

void
Splot_State::deleteTextures ()
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("deleteTextures\n")));

  glFinish ();

  state_.enemy_bullets->deleteTextures ();
  state_.enemies->deleteTextures ();
  state_.explosions->deleteTextures ();
  state_.player->deleteTextures ();
  state_.player_bullets->deleteTextures ();
  state_.background->deleteTextures ();
  state_.menu->deleteTextures ();
  state_.power_ups->deleteTextures ();
  state_.status_display->deleteTextures ();

  glFinish ();
}

void
Splot_State::loadTextures ()
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.debug)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("loadTextures\n")));

  glFinish ();

  state_.enemy_bullets->loadTextures ();
  state_.enemies->loadTextures ();
  state_.explosions->loadTextures ();
  state_.player->loadTextures ();
  state_.player_bullets->loadTextures ();
  state_.background->loadTextures ();
  state_.menu->loadTextures ();
  state_.power_ups->loadTextures ();
  state_.status_display->loadTextures ();

  glFinish ();
}

// init statics
int Splot_State::randomI[256];
float Splot_State::randomF[256];
float Splot_State::randomS[256];
int Splot_State::random_index = 0;
#if defined (USE_STATIC_RANDOMNESS)
int Splot_State::randomData[256] = {
   248102004,  328117286,  993377775,  854015073, 1439374375, 1767159396,   70069201, 1907231987, 
   211119157,  479401751,  622212282,    9231838,  734458600, 1262181023, 1588506079, 1045482140, 
  1339687125, 1560344577, 1213849032,  633406194,  614125827, 1554327504, 1806655959,    9023993, 
  1078727242, 1176063139,  976187378,  810931292,  746270205, 1562136994, 2087884076,  424129806, 
  1193958768, 1095781440,  983612896,  149343863,  360133739, 1686051724, 1739362510,  731185254, 
  1288405356, 1885751450,  384971725, 1815492077,  997042586, 1665612979,  571718101, 1202528724, 
   823422130,  648247351, 1310410582, 1276554002, 2101005116, 1295452586, 1016404054,  195103114, 
     7612905, 1430120805,  978590810, 1546776775,  401848397, 1779173400, 1807914296, 1682029851, 
   349817859,  942374862, 1461561077,  120026595,  751306787,  799796867,  996682091,  482884166, 
  1144154542, 1217981297,  372372860, 1751855466,  579591149, 1722265308, 2087439369,  351051816, 
  1456317295,  271195445,  219723916, 1255424177,  378781518,  753683640,  427750009, 1556077179, 
  1273818659,  559565258,  960093753,  729768891, 1647764074, 1227026302, 1149585425, 1765259304, 
  1868939152,  774702399,  750059288,  398832958,  520820276, 2112703334, 1362399002, 1561545469, 
  1780640386,  244572965,  852682320, 2037075953, 1225051875,  504049456,  225120512, 1844582338, 
  1338663083, 1074570311,  160244553, 1121950973,  418405917,  624843041, 1149855877, 1119779436, 
  1094178976, 2033148251,  371488856, 2051932162, 1859053526,  942438374, 1067346010, 1955231938, 
    85831261,  504170563, 1461599154, 1331453662,   78953775, 1213982141, 2003116935, 2079514895, 
  1687748987, 1680501530, 1947364831, 1565907115, 1040177840,  595297516, 2075848483,  378042878, 
   871418310, 1568735684, 1444462324,  917004149,  611204198, 1676918350,  938958743, 1638988003, 
  1515187085,  939044846, 1962493447,    2135470,  295376586, 1762079157, 2062507824, 1282493848, 
  2073691441, 1602732461, 1201070569,  947659542, 2056481520,   88202495, 1417304658, 1289086053, 
  1384048210, 1502072312, 1061581827, 1102754594,   73404482,   49379974, 1408723948,  727887420, 
  1723089720, 1543825878, 1107100043, 1024116412,  824309689, 1446638616,  235123901,  852436277, 
  2090136943, 1615752050, 1578900715,  127475878, 1171040344,  329650992, 1033285319, 1060667470, 
  2014529907,  800975903,  515148648, 1066046014,  394386848, 2064959457,  924398949,  504237184, 
   159062826,  950457702, 1833692758,  734688828, 1997793182,  589278825, 1532000472,  106596900, 
     4377635,  184959073, 1672457626,  996215687, 1750355520, 2022517378,  517653891, 1473385649, 
    63013760,   12922586, 1450588006, 1252083068, 1894542412,  116284061, 1272942781,  936597537, 
  2055707879,  850455160,  534369042, 1501836375, 1522849318, 1521806822, 2002946071,  977846244, 
  1544448690, 1241478873,  803625621,   81198469, 1810959889, 1082459409,  149777578, 1164775679, 
    17445490,  527728284, 1544356102,  522717168, 1632812509, 1097328719,  389651448, 1720278104, 
   807163977, 1248380721,    1044108, 1107180198,  388105756, 1264247211,  377234805, 1930260065
};
#endif // USE_STATIC_RANDOMNESS

void
Splot_State::initializeRandomness ()
{
  Splot_State::random_index = 0;
  //state_.random_index = 0;

#if defined (USE_STATIC_RANDOMNESS)
  for (int i = 0; i < 256; i++)
  {
    Splot_State::randomI[i] = Splot_State::randomData[i];
    Splot_State::randomS[i] = (2.0*(0.5-(Splot_State::randomData[i]/(double)RAND_MAX)));
    Splot_State::randomF[i] = (Splot_State::randomData[i]/(double)RAND_MAX);
    //state_.randomI[i] = Splot_State::randomData[i];
    //state_.randomS[i] = (2.0*(0.5-(Splot_State::randomData[i]/(double)RAND_MAX)));
    //state_.randomF[i] = (Splot_State::randomData[i]/(double)RAND_MAX);
  } // end FOR
#else
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.debug)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("generating randomness...\n")));

  for (int i = 0; i < 256; i++)
  {
    Splot_State::randomI[i] = ACE_OS::rand ();
    Splot_State::randomS[i] = (2.0F*(0.5F-(ACE_OS::rand ()/(float)RAND_MAX)));
    Splot_State::randomF[i] = (ACE_OS::rand ()/(float)RAND_MAX);
    //state_.randomI[i] = ACE_OS::rand ();
    //state_.randomS[i] = (2.0F*(0.5F-(ACE_OS::rand ()/(float)RAND_MAX)));
    //state_.randomF[i] = (ACE_OS::rand ()/(float)RAND_MAX);
  } // end FOR
#endif
}
