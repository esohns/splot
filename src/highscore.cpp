#include "stdafx.h"

#include "highscore.h"

#include <iostream>
#include <sstream>

#include "ace/OS.h"
#include "ace/Log_Msg.h"
#include "ace/Codecs.h"

//#include "gettext.h"
#include "common.h"
#include "defines.h"
#include "configuration.h"
#include "state.h"

Splot_HighScore::Splot_HighScore (const std::string& filename_in)
 : initialized_ (false)
{
  for (int i = 0; i < SKILL_LEVEL_MAX_INT; i++)
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

  if (!filename_in.empty ())
    initialized_ = load (filename_in);
}

Splot_HighScore::~Splot_HighScore ()
{
  if (!save ())
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to save highscore table, continuing\n")));
}

float
Splot_HighScore::getScore (int skill_in,
                           int index_in)
{
  float return_value = -1.0;

  if (skill_in > 0 && skill_in < SKILL_LEVEL_MAX_INT)
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

  if (skill_in > 0 && skill_in < SKILL_LEVEL_MAX_INT)
    if (index_in >= 0 && index_in < HI_SCORE_HIST)
      return_value = highScoreName_[skill_in][index_in];

  return return_value;
}

time_t
Splot_HighScore::getDate (int skill_in,
                          int index_in)
{
  time_t return_value = 0;

  if (skill_in > 0 && skill_in < SKILL_LEVEL_MAX_INT)
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
    std::string home_directory = getHomeDirectory ();
    ACE_OS::sprintf (highscore_filename,
                     ACE_TEXT_ALWAYS_CHAR ("%s%s%s"),
                     ACE_TEXT_ALWAYS_CHAR (home_directory.c_str ()),
                     ACE_DIRECTORY_SEPARATOR_STR,
                     CONFIG_SCORE_FILE);
    alterPathForPlatform (highscore_filename);
  } // end ELSE

  return highscore_filename;
}

bool
Splot_HighScore::save ()
{
  std::string filename = getFileName ();
  if (filename.empty ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid highscore filename, aborting\n")));

    return false;
  } // end IF

  FILE* file = NULL;
  file = ACE_OS::fopen (filename.c_str (), ACE_TEXT_ALWAYS_CHAR ("w"));
  if (!file)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to fopen(\"%s\", \"w\"): \"%m\", aborting\n"),
                ACE_TEXT (filename.c_str ())));

    return false;
  } // end IF

#ifdef HAVE_LOCALE_H
  char* locale = setlocale (LC_NUMERIC, ACE_TEXT_ALWAYS_CHAR ("C"));
#endif
  if (ACE_OS::fprintf (file,
                       ACE_TEXT_ALWAYS_CHAR ("%s\n"),
                       ACE_TEXT_ALWAYS_CHAR (SPLOT_HIGHSCORE_HEADER_LINE)) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to fprintf(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (filename.c_str ())));

    // clean up
    if (ACE_OS::fclose (file))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to fclose(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (filename.c_str ())));

    return false;
  } // end IF

  // write maximum attained level
  std::ostringstream converter;
  converter << SPLOT_STATE_SINGLETON::instance ()->get ().maximum_level;
  char buffer[BUFSIZ];
  ACE_OS::strcpy (buffer, converter.str ().c_str ());
  size_t length = 0;
  ACE_Byte* byte_stream = ACE_Base64::encode ((ACE_Byte*)buffer,
                                              ACE_OS::strlen (buffer),
                                              &length,
                                              false);
  if (!byte_stream)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Base64::encode(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (buffer)));

    // clean up
    if (ACE_OS::fclose (file))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to fclose(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (filename.c_str ())));

    return false;
  } // end IF
  if (ACE_OS::fprintf (file,
                       ACE_TEXT_ALWAYS_CHAR ("%s%s\n"),
                       ACE_TEXT_ALWAYS_CHAR (SPLOT_HIGHSCORE_MAXIMUM_LEVEL_PREFIX),
                       byte_stream) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to fprintf(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (filename.c_str ())));

    // clean up
    delete [] byte_stream;
    if (ACE_OS::fclose (file))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to fclose(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (filename.c_str ())));

    return false;
  } // end IF
  delete [] byte_stream;

  struct tm time;
  ACE_OS::memset (&time, 0, sizeof (time));
  for (int i = 0; i < SKILL_LEVEL_MAX_INT; i++)
    for (int j = 0; j < HI_SCORE_HIST; j++)
    {
      if (ACE_OS::gmtime_r (&highScoreDate_[i][j], &time) == NULL)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to gmtime_r(%:): \"%m\", aborting\n"),
                    highScoreDate_[i][j]));

        // clean up
        if (ACE_OS::fclose (file))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to fclose(\"%s\"): \"%m\", continuing\n"),
                      ACE_TEXT (filename.c_str ())));

        return false;
      } // end IF

      if (ACE_OS::fprintf (file,
                           ACE_TEXT_ALWAYS_CHAR ("%d %d %d %04d-%02d-%02d %02d:%02d:%02d %s\n"),
                           i, j,
                           (int)highScore_[i][j],
                           1900+time.tm_year,
                           1+time.tm_mon,
                           time.tm_mday,
                           time.tm_hour,
                           time.tm_min,
                           time.tm_sec,
                           highScoreName_[i][j]) < 0)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to fprintf(\"%s\"): \"%m\", aborting\n"),
                    ACE_TEXT (filename.c_str ())));

        // clean up
        if (ACE_OS::fclose (file))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to fclose(\"%s\"): \"%m\", continuing\n"),
                      ACE_TEXT (filename.c_str ())));

        return false;
      } // end IF
    } // end FOR

  if (ACE_OS::fclose (file))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to fclose(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (filename.c_str ())));

    return false;
  } // end IF

#ifdef HAVE_LOCALE_H
  setlocale (LC_NUMERIC, locale);
#endif

  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("saved highscore file: \"%s\"\n"),
                ACE_TEXT (filename.c_str ())));

  return true;
}

bool
Splot_HighScore::load (const std::string& filename_in)
{
  bool return_value = false;

  FILE* file = NULL;
  file = ACE_OS::fopen (filename_in.c_str (), ACE_TEXT_ALWAYS_CHAR ("r"));
  if (!file)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to fopen(\"%s\", \"r\"): \"%m\", aborting\n"),
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
                  ACE_TEXT ("failed to fclose(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (filename_in.c_str ())));

    return false;
  } // end IF
  if (ACE_OS::fseek (file, 0L, SEEK_SET))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to fseek(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (filename_in.c_str ())));

    // clean up
    if (ACE_OS::fclose (file))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to fclose(\"%s\"): \"%m\", continuing\n"),
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
                  ACE_TEXT ("failed to fclose(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (filename_in.c_str ())));

    return false;
  } // end IF

  // save (and reset) locale/timezone info
#ifdef HAVE_LOCALE_H
  char* locale = setlocale (LC_NUMERIC, ACE_TEXT_ALWAYS_CHAR ("C"));
#endif
  char* tz = ACE_OS::getenv (ACE_TEXT_ALWAYS_CHAR ("TZ"));
  int result = -1;
#if defined (_MSC_VER)
  std::string tz_environment = ACE_TEXT_ALWAYS_CHAR ("TZ=");
  result = ACE_OS::putenv (tz_environment.c_str ());
#else
  result = ACE_OS::setenv (ACE_TEXT_ALWAYS_CHAR ("TZ"), ACE_TEXT_ALWAYS_CHAR (""), 1);
#endif
  if (result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to putenv/setenv(\"TZ\"): \"%m\", aborting\n")));

    // clean up
    if (ACE_OS::fclose (file))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to fclose(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (filename_in.c_str ())));

    return false;
  } // end IF
  ACE_OS::tzset ();

  std::stringstream converter;
  std::string format_string;

  // discard the comment line
  if (::fscanf (file, ACE_TEXT_ALWAYS_CHAR ("%*[^\n]\n")) == EOF)
  {
    return_value = false;

    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid score file format (\"%s\"), aborting\n"),
                ACE_TEXT (filename_in.c_str ())));

    return_value = false;

    goto clean_up;
  } // end IF

  // read maximum attained level
  char buffer[BUFSIZ];
  ACE_OS::memset (&buffer, 0, sizeof (buffer));
  if (ACE_OS::fgets (buffer, BUFSIZ, file) != buffer)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to fgets(\"%s\", line: 1): \"%m\", aborting\n"),
                ACE_TEXT (filename_in.c_str ())));

    return_value = false;

    goto clean_up;
  } // end IF
  format_string = ACE_TEXT_ALWAYS_CHAR (SPLOT_HIGHSCORE_MAXIMUM_LEVEL_PREFIX);
  format_string += ACE_TEXT_ALWAYS_CHAR ("%s");
  char tmp_buffer[BUFSIZ];
  ACE_OS::memset (tmp_buffer, 0, sizeof (tmp_buffer));
  int fields = ::sscanf (buffer,
                         format_string.c_str (),
                         tmp_buffer);
  if (fields < 1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sscanf(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (buffer)));

    return_value = false;

    goto clean_up;
  } // end IF
  size_t length = 0;
  ACE_Byte* byte_stream = ACE_Base64::decode ((ACE_Byte*)tmp_buffer,
                                              &length);
  if (!byte_stream)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Base64::decode(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (tmp_buffer)));

    return_value = false;

    goto clean_up;
  } // end IF
  converter.str ((char*)byte_stream);
  int maximum_level = 1;
  converter >> maximum_level;
  SPLOT_STATE_SINGLETON::instance ()->get ().maximum_level = maximum_level;
  delete [] byte_stream;

  converter.clear ();
  converter << MAX_PLAYER_NAME_LENGTH;
  format_string = ACE_TEXT_ALWAYS_CHAR ("%d %d %d %d-%d-%d %d:%d:%d %");
  format_string += converter.str ();
  format_string += ACE_TEXT_ALWAYS_CHAR ("s%*s");
  char name[MAX_PLAYER_NAME_LENGTH + 1];
  struct tm time;
  int i, j, line = 2;
  int score;
  do
  {
    ACE_OS::memset (&time, 0, sizeof (time));
    ACE_OS::memset (&name, 0, sizeof (name));
    i = j = -1;
    line++;
    score = 0;
    fields = -1;

    ACE_OS::memset (&buffer, 0, sizeof (buffer));
    if (!ACE_OS::fgets (buffer, sizeof (buffer), file))
    {
      if (!::feof (file))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to fgets(\"%s\", line: %d): \"%m\", aborting\n"),
                    ACE_TEXT (filename_in.c_str ()), line));

        return_value = false;
      } // end IF

      break; // done
    } // end IF

    fields = ::sscanf (buffer,
                       format_string.c_str (),
                       &i, &j,
                       &score,
                       &time.tm_year,
                       &time.tm_mon,
                       &time.tm_mday,
                       &time.tm_hour,
                       &time.tm_min,
                       &time.tm_sec,
                       name);
    if ((fields < 10)                       ||
        (i < 0 || i >= SKILL_LEVEL_MAX_INT) ||
        (j < 0 || j >= HI_SCORE_HIST))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to sscanf(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (buffer)));

      return_value = false;

      break;
    } // end IF

    highScore_[i][j] = (float)score;
    time.tm_year -= 1900;
    time.tm_mon--;
    highScoreDate_[i][j] = ACE_OS::mktime (&time);
    ACE_OS::strncpy (highScoreName_[i][j], name, MAX_PLAYER_NAME_LENGTH);
    highScoreName_[i][j][ACE_OS::strlen (name)] = '\0';
  } while (true);

clean_up:
  if (ACE_OS::fclose (file))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to fclose(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (filename_in.c_str ())));

    return_value = false;
  } // end IF

  // reset locale/timezone info
#ifdef HAVE_LOCALE_H
  setlocale (LC_NUMERIC, locale);
#endif
  if (tz)
  {
#if defined (_MSC_VER)
    tz_environment = ACE_TEXT_ALWAYS_CHAR ("TZ=");
    tz_environment += tz;
    result = ACE_OS::putenv (tz_environment.c_str ());
#else
    result = ACE_OS::setenv (ACE_TEXT_ALWAYS_CHAR ("TZ"), tz, 1);
#endif
    if (result)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to putenv/setenv(\"TZ\", \"%s\"): \"%m\", aborting\n"),
                  tz));

      return false;
    } // end IF
  } // end IF
  else
  {
#if defined (_MSC_VER)
    tz_environment = ACE_TEXT_ALWAYS_CHAR ("TZ=");
    result = ACE_OS::putenv (tz_environment.c_str ());
#else
    result = ACE_OS::unsetenv (ACE_TEXT_ALWAYS_CHAR ("TZ"));
#endif
    if (result)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to putenv/unsetenv(\"TZ\"): \"%m\", aborting\n")));

      return false;
    } // end IF
  } // end ELSE
  ACE_OS::tzset ();

  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (return_value && configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("loaded highscore file: \"%s\"\n"),
                ACE_TEXT (filename_in.c_str ())));

  return return_value;
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
  std::cout << ACE_TEXT ("high scores:\n");
  struct tm time_struct;
  char buffer[BUFSIZ];
  int width, max_width = 0;
  for (int i = 0; i < HI_SCORE_HIST; i++)
  {
    width = ACE_OS::strlen (highScoreName_[skill_in][i]);
    if (width > max_width)
      max_width = width;
  } // end FOR
  std::string format_string;
  std::ostringstream converter;
  format_string = ACE_TEXT_ALWAYS_CHAR ("%02d/%02d/%04d %");
  converter << max_width;
  format_string += converter.str ();
  format_string += ACE_TEXT_ALWAYS_CHAR ("s %d\n");
  for (int i = 0; i < HI_SCORE_HIST; i++)
  {
    if (ACE_OS::localtime_r (&highScoreDate_[skill_in][i], &time_struct) != &time_struct)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to localtime_r (%d): \"%m\", continuing\n"),
                  highScoreDate_[skill_in][i]));

      continue;
    } // end IF

    ACE_OS::memset (&buffer, 0, sizeof (buffer));
    if (ACE_OS::sprintf (buffer, format_string.c_str (),
                         time_struct.tm_mday, 1+time_struct.tm_mon, 1900+time_struct.tm_year,
                         highScoreName_[skill_in][i], (int)(highScore_[skill_in][i])) < 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to sprintf (): \"%m\", continuing\n")));

      continue;
    } // end IF

    std::cout << buffer;
  } // end FOR
}
