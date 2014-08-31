#include "stdafx.h"

#include "highscore.h"

#include "ace/OS.h"
#include "ace/Log_Msg.h"

//#include "gettext.h"
#include "common.h"
#include "defines.h"
//#include "HeroAircraft.h"
//#include <cstdio>
//#include <cstdlib>
//#include <cstring>
//#include <ctime>
//
//#ifdef HAVE_LOCALE_H
//#include <locale.h>
//#endif

Splot_HighScore::Splot_HighScore (const std::string& filename_in)
 : initialized_ (false)
{
  for (int i = 0; i < MAX_LEVEL; i++)
    for (int j = 0; j < HI_SCORE_HIST; j++)
    {
      // default high scores
      switch (j)
      {
        case 0:
         highScore_[i][j] = 250000.0;
          break;
        case 1:
         highScore_[i][j] = 200000.0;
          break;
        case 2:
         highScore_[i][j] = 150000.0;
          break;
        case 3:
         highScore_[i][j] = 100000.0;
          break;
        case 4:
         highScore_[i][j] = 50000.0;
          break;
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("highscore index out of range (was: %d), continuing\n"),
                      j));

          break;
        }
      } // end SWITCH

      // default player
      ACE_OS::memset (highScoreName_[i][j], 0, sizeof (highScoreName_[i][j]));
      ACE_OS::strcpy (highScoreName_[i][j], ACE_TEXT_ALWAYS_CHAR (SPLOT_HIGHSCORE_DEFAULT_NAME));

      // default date (01/01/1970);
      highScoreDate_[i][j] = 0;
    } // end FOR

  initialized_ = load (filename_in);
}

Splot_HighScore::~Splot_HighScore()
{
  if (initialized_) ;
}

float
Splot_HighScore::getScore (int skill_in,
                           int index_in)
{
  float return_value = -1.0;

  if (skill_in > 0 && skill_in < 10)
    if (index_in >= 0 && index_in < HI_SCORE_HIST)
      return_value = highScore_[skill_in][index_in];

  return return_value;
}

std::string
Splot_HighScore::getName (int skill_in,
                          int index_in)
{
  std::string return_value =
    ACE_TEXT_ALWAYS_CHAR (SPLOT_HIGHSCORE_DEFAULT_NAME);

  if (skill_in > 0 && skill_in < 10)
    if (index_in >= 0 && index_in < HI_SCORE_HIST)
      return_value = highScoreName_[skill_in][index_in];

  return return_value;
}

time_t
Splot_HighScore::getDate (int skill_in,
                          int index_in)
{
  time_t return_value = 0;

  if (skill_in > 0 && skill_in < 10)
    if (index_in >= 0 && index_in < HI_SCORE_HIST)
      return_value = highScoreDate_[skill_in][index_in];

  return return_value;
}

std::string
Splot_HighScore::getFileName ()
{
  char highscore_filename[PATH_MAX];
  ACE_OS::memset (&highscore_filename, 0, sizeof (highscore_filename));
  const char* env_filename_p =
   ACE_OS::getenv (ACE_TEXT_ALWAYS_CHAR (SPLOT_HIGHSCORE_ENV_SYMBOL));
  if (env_filename_p &&
      (ACE_OS::strlen (env_filename_p) < PATH_MAX))
    ACE_OS::strcpy (highscore_filename, env_filename_p);
  else
  {
    char home_directory[PATH_MAX];
    ACE_OS::memset (&home_directory, 0, sizeof (home_directory));
    const char* home_directory_p =
      ACE_OS::getenv (ACE_TEXT_ALWAYS_CHAR (SPLOT_HOME_ENV_SYMBOL));
    if (!home_directory_p)
      ACE_OS::getcwd (home_directory, PATH_MAX);
    else
      ACE_OS::strcpy (home_directory, home_directory_p);
    ACE_OS::sprintf (highscore_filename,
                     ACE_TEXT_ALWAYS_CHAR ("%s%s%s"),
                     home_directory, ACE_DIRECTORY_SEPARATOR_STR, CONFIG_SCORE_FILE);
    alterPathForPlatform (highscore_filename);
  } // end ELSE

  return highscore_filename;
}

std::string
Splot_HighScore::getOldFileName ()
{
  char highscore_filename[PATH_MAX];

  char home_dir[PATH_MAX];
  const char *home_dir_p = ACE_OS::getenv (ACE_TEXT_ALWAYS_CHAR ("HOME"));
  if (!home_dir_p)
    ACE_OS::getcwd (home_dir, PATH_MAX);
  else
    ACE_OS::strcpy(home_dir, home_dir_p);
  ACE_OS::sprintf (highscore_filename,
                   ACE_TEXT_ALWAYS_CHAR ("%s/.chromium-score")CONFIG_EXT,
                   home_dir);
  alterPathForPlatform (highscore_filename);

  return highscore_filename;
}

bool
Splot_HighScore::save ()
{
  FILE* file = NULL;
  file = ACE_OS::fopen (getFileName ().c_str (), ACE_TEXT_ALWAYS_CHAR ("w"));
  if (!file)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to fopen (\"%s\", \"w\"): \"%m\", aborting\n"),
                ACE_TEXT (getFileName ().c_str ())));

    return false;
  } // end IF

#ifdef HAVE_LOCALE_H
  char* locale = setlocale (LC_NUMERIC, ACE_TEXT_ALWAYS_CHAR ("C"));
#endif
  if (ACE_OS::fprintf (file,
                       ACE_TEXT_ALWAYS_CHAR ("%s"),
                       ACE_TEXT_ALWAYS_CHAR (SPLOT_HIGHSCORE_HEADER_LINE)) !=
      (ACE_OS::strlen (ACE_TEXT_ALWAYS_CHAR (SPLOT_HIGHSCORE_HEADER_LINE)+1)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to fprintf (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (getFileName ().c_str ())));

    // clean up
    if (ACE_OS::fclose (file))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to fclose (\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (getFileName ().c_str ())));

    return false;
  } // end IF
  struct tm* time = NULL;
  for (int i = 0; i < 10; i++)
    for (int j = 0; j < HI_SCORE_HIST; j++)
    {
      time = ACE_OS::gmtime (&highScoreDate_[i][j]);
      if (!time)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to gmtime (%d): \"%m\", aborting\n"),
                    highScoreDate_[i][j]));

        // clean up
        if (fclose (file))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to fclose (\"%s\"): \"%m\", continuing\n"),
                      ACE_TEXT (getFileName ().c_str ())));

        return false;
      } // end IF

      ACE_OS::fprintf (file, 
                       ACE_TEXT_ALWAYS_CHAR ("%d %d %f %04d-%02d-%02d %02d:%02d:%02d %s\n"),
                       i, j, 
                       highScore_[i][j],
                       1900 + time->tm_year,
                       1 + time->tm_mon,
                       time->tm_mday,
                       time->tm_hour,
                       time->tm_min,
                       time->tm_sec,
                       highScoreName_[i][j]);
    } // end FOR

  if (ACE_OS::fclose (file))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to fclose (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (getFileName ().c_str ())));

    return false;
  } // end IF

#ifdef HAVE_LOCALE_H
  setlocale (LC_NUMERIC, locale);
#endif

  return true;
}

bool
Splot_HighScore::load (const std::string& filename_in)
{
  FILE* file = NULL;
  file = ACE_OS::fopen (filename_in.c_str (), ACE_TEXT_ALWAYS_CHAR ("r"));
  if (!file)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to fopen (\"%s\", \"r\"): \"%m\", aborting\n"),
                ACE_TEXT (filename_in.c_str ())));

    return false;
  } // end IF

  // sanity check(s): empty/format ?
  int chr = ACE_OS::fgetc (file);
  if (chr == EOF)
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("empty score file \"%s\", aborting\n"),
                ACE_TEXT (filename_in.c_str ())));

    // clean up
    if (ACE_OS::fclose (file))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to fclose (\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (filename_in.c_str ())));

    return false;
  } // end IF
  if (ACE_OS::fseek (file, 0L, SEEK_SET))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to fseek (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (filename_in.c_str ())));

    // clean up
    if (ACE_OS::fclose (file))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to fclose (\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (filename_in.c_str ())));

    return false;
  } // end IF
  if ('#' != chr)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid score file format (\"%s\"), aborting\n"),
                ACE_TEXT (filename_in.c_str ())));

    // clean up
    if (ACE_OS::fclose (file))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to fclose (\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (filename_in.c_str ())));

    return false;
  } // end IF

  // save and reset locale/timezone info
#ifdef HAVE_LOCALE_H
  char* locale = setlocale (LC_NUMERIC, ACE_TEXT_ALWAYS_CHAR ("C"));
#endif
  char* tz = ACE_OS::getenv (ACE_TEXT_ALWAYS_CHAR ("TZ"));
  if (ACE_OS::setenv (ACE_TEXT_ALWAYS_CHAR ("TZ"), ACE_TEXT_ALWAYS_CHAR (""), 1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to setenv (\"TZ\"): \"%m\", aborting\n")));

    // clean up
    if (ACE_OS::fclose (file))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to fclose (\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (filename_in.c_str ())));

    return false;
  } // end IF
  ACE_OS::tzset ();

  // discard the comment line
  if (::fscanf (file, ACE_TEXT_ALWAYS_CHAR ("%*[^\n]")) == EOF)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT("invalid score file format (\"%s\"), aborting\n"),
                ACE_TEXT (filename_in.c_str ())));

    // clean up
    if (ACE_OS::fclose (file))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to fclose (\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (filename_in.c_str ())));
    if (tz)
    {
      if (ACE_OS::setenv (ACE_TEXT_ALWAYS_CHAR ("TZ"), tz, 1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to setenv (\"TZ\", \"%s\"): \"%m\", continuing\n"),
                    tz));
    } // end IF
    else
      if (ACE_OS::unsetenv (ACE_TEXT_ALWAYS_CHAR ("TZ")))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to unsetenv (\"TZ\"): \"%m\", continuing\n")));

    return false;
  } // end IF

  char name[MAX_PLAYER_NAME_LENGTH + 1];
  struct tm time;
  int i, j;
  double score;
  int fields;
  do
  {
    i = j = -1;
    ACE_OS::memset (&time, 0, sizeof (time));
    fields = ::fscanf (file,
                       ACE_TEXT_ALWAYS_CHAR ("%d %d %lf %d-%d-%d %d:%d:%d %100s%*[^\n]"), // *TODO*
                       &i, &j,
                       &score,
                       &time.tm_year,
                       &time.tm_mon,
                       &time.tm_mday,
                       &time.tm_hour,
                       &time.tm_min,
                       &time.tm_sec,
                       name);
    if (((fields != 10) && (fields != EOF)) ||
        (i < 0 || i > MAX_LEVEL)            ||
        (j < 0 || j > HI_SCORE_HIST))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid score file format (\"%s\"), aborting\n"),
                  ACE_TEXT (filename_in.c_str ())));

      // clean up
      if (ACE_OS::fclose (file))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT("failed to fclose (\"%s\"): \"%m\", continuing\n"),
                    ACE_TEXT(filename_in.c_str ())));
      if (tz)
      {
        if (ACE_OS::setenv (ACE_TEXT_ALWAYS_CHAR ("TZ"), tz, 1))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to setenv (\"TZ\", \"%s\"): \"%m\", continuing\n"),
                      tz));
      } // end IF
      else
      if (ACE_OS::unsetenv (ACE_TEXT_ALWAYS_CHAR ("TZ")))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to unsetenv (\"TZ\"): \"%m\", continuing\n")));

      return false;
    } // end IF

    highScore_[i][j] = (float)score;
    time.tm_year -= 1900;
    time.tm_mon--;
    highScoreDate_[i][j] = ACE_OS::mktime (&time);
    ACE_OS::strncpy(highScoreName_[i][j], name, MAX_PLAYER_NAME_LENGTH);
    highScoreName_[i][j][ACE_OS::strlen (name)] = '\0';
  } while (fields != EOF);

  if (ACE_OS::fclose (file))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT("failed to fclose (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT(filename_in.c_str ())));
    if (tz)
    {
      if (ACE_OS::setenv (ACE_TEXT_ALWAYS_CHAR ("TZ"), tz, 1))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to setenv (\"TZ\", \"%s\"): \"%m\", continuing\n"),
                    tz));
    } // end IF
    else
      if (ACE_OS::unsetenv (ACE_TEXT_ALWAYS_CHAR ("TZ")))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to unsetenv (\"TZ\"): \"%m\", continuing\n")));

    return false;
  } // end IF

  // reset locale/timezone info
#ifdef HAVE_LOCALE_H
  setlocale (LC_NUMERIC, locale);
#endif
  if (tz)
  {
    if (ACE_OS::setenv (ACE_TEXT_ALWAYS_CHAR ("TZ"), tz, 1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to setenv (\"TZ\", \"%s\"): \"%m\", aborting\n"),
                  tz));

      return false;
    } // end IF
  } // end IF
  else
    if (ACE_OS::unsetenv (ACE_TEXT_ALWAYS_CHAR ("TZ")))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to unsetenv (\"TZ\"): \"%m\", aborting\n")));

      return false;
    } // end IF
  ACE_OS::tzset ();

  return true;
}

void
Splot_HighScore::insert (int skill_in, int rank_in, float score_in)
{
  int i = HI_SCORE_HIST - 2;
  while (i >= rank_in)
  {
    highScore_[skill_in][i + 1] = highScore_[skill_in][i];
    ACE_OS::strcpy (highScoreName_[skill_in][i + 1], highScoreName_[skill_in][i]);
    ACE_OS::memcpy (&highScoreDate_[skill_in][i + 1], &highScoreDate_[skill_in][i], sizeof (time_t));
    i--;
  } // end WHILE
  highScore_[skill_in][rank_in] = score_in;
  char *name = ACE_OS::getenv ("USER");
  if (name)
    ACE_OS::strcpy (highScoreName_[skill_in][rank_in], name);
  else
    ACE_OS::strcpy (highScoreName_[skill_in][rank_in], ACE_TEXT_ALWAYS_CHAR ("player"));
  time (&highScoreDate_[skill_in][rank_in]);
}

int
Splot_HighScore::set (int skill_in, float score_in)
{
  if (skill_in < 0 || skill_in > MAX_LEVEL)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid skill level (was: %d), aborting\n"),
                skill_in));

    return -1;
  } // end IF

  if (!load (getFileName ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to load (\"%s\"), aborting\n"),
                getFileName ().c_str ()));

    return -1;
  } // end IF

  int rank = -1;
  for (int i = HI_SCORE_HIST - 1; i >= 0; i--)
  {
    if (score_in > highScore_[skill_in][i])
      rank = i;
    else
      break;
  } // end FOR
  if (rank > -1)
  {
    insert (skill_in, rank, score_in);
    if (!save ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to save (), aborting\n")));

      return -1;
    } // end IF
  } // end IF

  return (rank + 1);
}

int
Splot_HighScore::check (int skill_in, float score_in)
{
  if (skill_in < 0 || skill_in > MAX_LEVEL)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid skill level (was: %d), aborting\n"),
                skill_in));

    return 0;
  } // end IF

  int rank = -1;
  for (int i = HI_SCORE_HIST - 1; i >= 0; i--)
    if (score_in > highScore_[skill_in][i])
      rank = i;

  return (rank + 1);
}

void
Splot_HighScore::print (int skill_in)
{
  struct tm* tm_p = NULL;
  fprintf (stderr, ACE_TEXT_ALWAYS_CHAR ("%s"),
           ACE_TEXT ("high scores:\n"));
  for (int i = 0; i < HI_SCORE_HIST; i++)
  {
    tm_p = ACE_OS::localtime (&highScoreDate_[skill_in][i]);
    if (!tm_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to localtime (%d), continuing\n"),
                  highScoreDate_[skill_in][i]));

      continue;
    } // end IF
    fprintf (stderr, ACE_TEXT_ALWAYS_CHAR ("%02d/%02d/%04d %16s %d\n"),
             1 + tm_p->tm_mon, tm_p->tm_mday, 1900 + tm_p->tm_year,
             highScoreName_[skill_in][i], (int)(highScore_[skill_in][i]));
  } // end FOR
}
