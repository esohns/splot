#include "stdafx.h"

#include "state.h"

#include "ace/Global_Macros.h"
#include "ace/OS_Memory.h"

//#include "gettext.h"
//
//#include "extern.h"
#include "highscore.h"
#include "configuration.h"

#include "Audio.h"
#include "Ground.h"
#include "MenuGL.h"
#include "MainGL.h"
#include "PowerUps.h"
#include "HeroAmmo.h"
#include "EnemyAmmo.h"
#include "EnemyFleet.h"
#include "Explosions.h"
#include "HeroAircraft.h"
#include "ScreenItemAdd.h"
#include "StatusDisplay.h"
#include "GroundMetal.h"
#if defined (USE_GLUT)
#include "MainGLUT.h"
#elif defined (USE_SDL)
#include "MainSDL.h"
#else
#error "USE_SDL or USE_GLUT must be defined"
#endif

//#if defined (AUDIO_OPENAL)
//#include "AudioOpenAL.h"
//#endif
//#if defined (AUDIO_SDLMIXER)
//#include "AudioSDLMixer.h"
//#endif

Splot_State::Splot_State ()
{
  state_.text             = NULL;

  state_.toolkit          = NULL;
  state_.highscore        = NULL;

  state_.mouse_active     = false;

  state_.fps              = STATE_DEFAULT_FPS;
  state_.frame            = 0;
  state_.game_frame       = 0;
  state_.game_speed       = STATE_DEFAULT_GAME_SPEED;
  state_.game_skill       = STATE_DEFAULT_GAME_SKILL;
  state_.game_level       = 1;
  state_.max_level        = 1;
  state_.speed_adj        = STATE_DEFAULT_SPEED_ADJUSTMENT;
  state_.game_mode        = GAMEMODE_MENU;

  state_.hero_death       = 0;
  state_.hero_success     = 0;

  state_.scroll_speed     = STATE_DEFAULT_SCROLL_SPEED;

  state_.hero             = NULL;
  state_.enemy_fleet      = NULL;
  state_.hero_ammo        = NULL;
  state_.enemy_ammo       = NULL;
  state_.item_add         = NULL;
  state_.explosions       = NULL;
  state_.power_ups        = NULL;
  state_.audio            = NULL;
  state_.ground           = NULL;
  state_.ground_game      = NULL;
  state_.ground_menu      = NULL;
  state_.menu             = NULL;
  state_.main_GL          = NULL;
  state_.status_display   = NULL;

  state_.cursor_pos[0]    = 0.0;
  state_.cursor_pos[1]    = 0.0;
  state_.cursor_pos[2]    = HERO_Z;

  state_.event_file       = NULL;

  state_.tip_ship_past    = 0;
  state_.tip_super_shield = 0;

  state_.game_pause       = false;
  state_.game_quit        = false;

//	readConfigFile();
}

Splot_State::~Splot_State()
{
//	saveConfigFile();
}

bool
Splot_State::init ()
{
  ACE_ASSERT (!state_.highscore);
  ACE_NEW_RETURN (state_.highscore,
                  Splot_HighScore(Splot_HighScore::getFileName ()),
                  false);

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

  state_.game_skill = configuration->get ().skill_base + 0.5;
  state_.game_skill += (state_.game_level - 1) * 0.05;
  state_.game_frame = 0;

  ACE_ASSERT (state_.enemy_fleet);
  state_.enemy_fleet->clear ();
  ACE_ASSERT (state_.power_ups);
  state_.power_ups->clear ();
  ACE_ASSERT (state_.enemy_ammo);
  state_.enemy_ammo->clear ();
  ACE_ASSERT (state_.hero_ammo);
  state_.hero_ammo->clear ();
  ACE_ASSERT (state_.item_add);
  state_.item_add->clear ();
  ACE_ASSERT (state_.explosions);
  state_.explosions->clear ();
  game_state_.score = 0;
  ACE_ASSERT (state_.hero);
  state_.hero->newGame ();
  state_.hero->setLives (STATE_DEFAULT_HERO_LIVES);

  state_.game_pause = false;

  ACE_ASSERT (state_.item_add);
  state_.item_add->loadScreenItems ("");

  if (state_.ground_game != state_.ground_menu)
  {
    // delete game ground and set to menu/game common ground
    delete state_.ground_game;
    state_.ground_game = state_.ground_menu;
  } // end IF
  state_.ground = state_.ground_game;
  state_.ground->setVariation (state_.game_level - 1);

  state_.audio->setMusicIndex (state_.game_level - 1);

  if (state_.event_file)
    if (fclose (state_.event_file))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT("failed to fclose () event file: \"%m\", continuing\n")));
  //	//-- cheezy, partially functional record mechanism
  //	Splot_State::eventFile = fopen("EVENT_FILE.txt", "w");
  //	Splot_State::eventFile = fopen("EVENT_FILE.txt", "r");

  //	Splot_State::scrollSpeed = -Splot_State::gameSkill*0.05;
  //	fprintf(stderr, "%f\n", Splot_State::scrollSpeed);
}

void
Splot_State::gotoNextLevel ()
{
  Splot_Configuration* configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance();
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

  state_.enemy_fleet->clear ();
  state_.power_ups->clear ();
  state_.enemy_ammo->clear ();
  state_.hero_ammo->clear ();
  state_.item_add->clear ();

  state_.hero->fullHealth ();

  state_.item_add->loadScreenItems ("");

  // when more than one ground is used, check here if it need to be created
  state_.ground->nextVariation ();

  state_.audio->nextMusicIndex ();
}

void
Splot_State::createGame ()
{
  const Configuration_t& configuration =
   SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();

  if (configuration.debug)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("starting up...\n")));

  ACE_NEW (state_.main_GL,
           MainGL ());
  ACE_NEW (state_.explosions,
           Explosions ());
  ACE_NEW (state_.enemy_fleet,
           EnemyFleet ());
  ACE_NEW (state_.hero,
           HeroAircraft ());
  ACE_NEW (state_.hero_ammo,
           HeroAmmo ());
  ACE_NEW (state_.enemy_ammo,
           EnemyAmmo ());
  ACE_NEW (state_.status_display,
           StatusDisplay ());
  ACE_NEW (state_.power_ups,
           PowerUps ());
  ACE_NEW (state_.ground,
           GroundMetal ());
  ACE_NEW (state_.menu,
           MenuGL ());
  ACE_NEW (state_.item_add,
           ScreenItemAdd ());

 #if defined (AUDIO_OPENAL) && defined (AUDIO_SDLMIXER) 
  if (configuration.audio_type == AUDIO_OPENAL)
    ACE_NEW (state_.audio,
             AudioOpenAL ());
  else
    ACE_NEW (state_.audio,
             AudioSDLMixer ());
#elif defined (AUDIO_OPENAL)
  ACE_NEW (state_.audio,
           AudioOpenAL ());
#elif defined(AUDIO_SDLMIXER)
  ACE_NEW (state_.audio,
           AudioSDLMixer ());
#else
  ACE_NEW (state_.audio,
           Audio ());
#endif

  state_.ground_game = state_.ground_menu = state_.ground;

  newGame ();

  state_.audio->setMusicMode (Audio::MusicMenu);

  if (configuration.debug)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("starting up...DONE\n")));
}

void
Splot_State::deleteGame ()
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();

  if (configuration.debug)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT("shutting down...\n")));
  
  delete state_.main_GL;
  delete state_.enemy_fleet;
  delete state_.hero;
  delete state_.hero_ammo;
  delete state_.enemy_ammo;
  delete state_.status_display;
  delete state_.explosions;
  delete state_.power_ups;
  delete state_.ground;
  delete state_.menu;
  delete state_.item_add;
  delete state_.audio;

  if (configuration.debug)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT("shutting down...DONE\n")));
}

void
Splot_State::deleteTextures ()
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.debug)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT("deleteTextures\n")));

  glFinish ();

  state_.main_GL->deleteTextures ();
  state_.enemy_ammo->deleteTextures ();
  state_.enemy_fleet->deleteTextures ();
  state_.explosions->deleteTextures ();
  state_.hero->deleteTextures ();
  state_.hero_ammo->deleteTextures ();
  state_.ground->deleteTextures ();
  state_.menu->deleteTextures ();
  state_.power_ups->deleteTextures ();
  state_.status_display->deleteTextures ();

  glFinish ();
}

void
Splot_State::loadTextures()
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.debug)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("loadTextures\n")));

  glFinish();

  state_.main_GL->loadTextures ();
  state_.enemy_ammo->loadTextures ();
  state_.enemy_fleet->loadTextures ();
  state_.explosions->loadTextures ();
  state_.hero->loadTextures ();
  state_.hero_ammo->loadTextures ();
  state_.ground->loadTextures ();
  state_.menu->loadTextures ();
  state_.power_ups->loadTextures ();
  state_.status_display->loadTextures ();

  glFinish ();
}

static int randData[256] = {
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
void
Splot_State::generateRandom (bool r)
{
  state_.rand_index = 0;

  if (r)
  {
    const Configuration_t& configuration =
      SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
    if (configuration.debug)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("generating random sequences...\n")));

    for (int i = 0; i < 256; i++)
    {
      state_.randI[i] = rand ();
      state_.randS[i] = (2.0 * (0.5 - (rand () / (double)RAND_MAX)));
      state_.randF[i] = (rand () / (double)RAND_MAX);
    } // end FOR
  } // end IF
  else
  {
    for(int i = 0; i < 256; i++)
    {
      state_.randI[i] = randData[i];
      state_.randS[i] = (2.0 * (0.5 - (randData[i] / (double)RAND_MAX)));
      state_.randF[i] = (randData[i] / (double)RAND_MAX);
    } // end FOR
  } // end ELSE
}

bool
Splot_State::setToolkit (GameToolkit_t toolkit_in,
                         int argc_in,
                         char** argv_in)
{
  switch (toolkit_in)
  {
    case TOOLKIT_GLUT:
#if defined (USE_GLUT)
      ACE_NEW_RETURN (state_.toolkit,
                      MainGLUT (argc_in, argv_in),
                      false);
#endif
      break;
    case TOOLKIT_SDL:
#if defined (USE_SDL)
      ACE_NEW_RETURN (state_.toolkit,
                      MainSDL (argc_in, argv_in),
                      false);
#endif
      break;
    case TOOLKIT_INVALID:
    default:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("invalid toolkit (was: %d), aborting\n"),
                  toolkit_in));

      return false;
    }
  } // end SWITCH

  return true;
}
