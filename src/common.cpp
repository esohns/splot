#include "stdafx.h"

#include "common.h"

#include <string>
#include <fstream>

#include "ace/OS.h"
#include "ace/OS_Memory.h"
#include "ace/ACE.h"

#include "types.h"

// init statics
unsigned int Bullet_t::count = 0;

std::string
alterPathForPlatform (char* filename_inout)
{
  char* walker = filename_inout;
#ifdef macintosh
  while (*walker)
  {
    if (*walker == '/')
      *walker = ':';
    walker++;
  } // end IF
#elif defined (ACE_WIN32) || defined (ACE_WIN64)
  while (*walker)
  {
    if (*walker == '/')
      *walker = '\\';
    walker++;
  } // end IF
#else
  ACE_UNUSED_ARG (walker);
#endif

  return filename_inout;
}

std::string
dataLoc (const std::string& filename_in,
         bool doCheck_in)
{
  std::string return_value;

  char buffer[PATH_MAX];
  ACE_OS::memset (buffer, 0, sizeof (buffer));

  const char* data_p =
    ACE_OS::getenv (ACE_TEXT_ALWAYS_CHAR (SPLOT_DATA_DIR_ENV_SYMBOL));
  if (data_p)
    return_value = data_p;
  else
  {
#ifdef PKGDATADIR
    return_value = ACE_TEXT_ALWAYS_CHAR (PKGDATADIR);
    return_value += ACE_DIRECTORY_SEPARATOR_STR;
    return_value += filename_in;
#else
    if (!ACE_OS::getcwd (buffer, sizeof (buffer)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to getcwd(): \"%m\", aborting\n")));

      return return_value;
    } // end IF
    return_value = buffer;
    return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
    return_value += ACE_TEXT_ALWAYS_CHAR (SPLOT_DATA_DIR);
  } // end IF
  return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
  return_value += filename_in;
#endif
  if (ACE_OS::strcmp (ACE::basename (filename_in.c_str (), ACE_DIRECTORY_SEPARATOR_CHAR),
                      filename_in.c_str ()))
    goto check;

  const char* home_directory_p =
    ACE_OS::getenv (ACE_TEXT_ALWAYS_CHAR (SPLOT_HOME_ENV_SYMBOL));
  if (home_directory_p)
  {
    return_value = home_directory_p;
    return_value += ACE_DIRECTORY_SEPARATOR_STR_A;
    return_value += filename_in;

    goto check;
  } // end IF

//#ifdef macintosh
//#define DATADIR "::data"
//#else
//#define DATADIR "../data"
//#endif
//
//  if ((ACE_OS::strlen (DATADIR) + filename_in.size ()) < PATH_MAX)
//  {
//    sprintf (buffer,
//             ACE_TEXT_ALWAYS_CHAR ("%s/%s"),
//             DATADIR, filename_in.c_str ());
//  } // end IF

check:
  ACE_OS::strcpy (buffer, return_value.c_str ());
  alterPathForPlatform (buffer);
  return_value = buffer;

  if (doCheck_in)
  {
    ACE_stat file_stat;
    if (ACE_OS::stat (return_value.c_str (), &file_stat))
      return_value.clear ();
  } // end IF

  return return_value;
}

void
printExtensions (FILE* fstream_in,
                 const char* extstr_in)
{
  char* walker = NULL;
  char* space = NULL;
  int len = static_cast<int> (ACE_OS::strlen (extstr_in));
  int cnt = 0;
  char* extstr = NULL;
  ACE_NEW (extstr,
           char[len + 1]);
  ACE_OS::strcpy (extstr, extstr_in);
  walker = extstr;

  ACE_OS::fprintf (fstream_in, ACE_TEXT_ALWAYS_CHAR ("Extensions :"));
  while ((walker - extstr) < len)
  {
    space = ACE_OS::strchr (walker, ' ');
    if (space)
    {
      if ((cnt % 2) == 0)
        ACE_OS::fprintf (fstream_in, "\n");
      *space = '\0';
      ACE_OS::fprintf (fstream_in, ACE_TEXT_ALWAYS_CHAR ("%-31s "), walker);
      walker = space + 1;
      cnt++;
    } // end IF
    else
      break;
  } // end WHILE
  ACE_OS::fprintf (fstream_in, ACE_TEXT_ALWAYS_CHAR ("\n"));

  delete [] extstr;
}

std::string
getHomeDirectory ()
{
  char home_directory[PATH_MAX];
  ACE_OS::memset (&home_directory, 0, sizeof (home_directory));
  const char* home_directory_p =
    ACE_OS::getenv (ACE_TEXT_ALWAYS_CHAR (SPLOT_HOME_ENV_SYMBOL));
  if (!home_directory_p)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to getenv(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (SPLOT_HOME_ENV_SYMBOL)));

    if (!ACE_OS::getcwd (home_directory, sizeof (home_directory)))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to getcwd(): \"%m\", aborting\n")));
  } // end IF
  else
    ACE_OS::strcpy (home_directory, home_directory_p);

  alterPathForPlatform (home_directory);

  return home_directory;
}

bool
initLogging (const std::string& programName_in,
             const std::string& logFile_in,
             bool logToSyslog_in,
             bool enableTracing_in,
             bool enableDebug_in,
             ACE_Log_Msg_Backend* backend_in)
{
  // *NOTE*: default log target is stderr
  u_long options_flags = ACE_Log_Msg::STDERR;
  if (logToSyslog_in)
    options_flags |= ACE_Log_Msg::SYSLOG;
  if (backend_in)
  {
    options_flags |= ACE_Log_Msg::CUSTOM;
    ACE_LOG_MSG->msg_backend (backend_in);
  } // end IF
  if (!logFile_in.empty ())
  {
    options_flags |= ACE_Log_Msg::OSTREAM;

    ACE_OSTREAM_TYPE* log_stream = NULL;
    std::ios_base::openmode open_mode = (std::ios_base::out |
                                         std::ios_base::trunc);
    ACE_NEW_NORETURN (log_stream,
                      std::ofstream (logFile_in.c_str (),
                      open_mode));
    if (!log_stream)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));

      return false;
    } // end IF
    //    if (log_stream->open (logFile_in.c_str (),
    //                         open_mode))
    if (log_stream->fail ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize logfile (was: \"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (logFile_in.c_str ())));

      // clean up
      delete log_stream;

      return false;
    } // end IF

    // *NOTE*: the logger singleton assumes ownership of the stream lifecycle
    ACE_LOG_MSG->msg_ostream (log_stream, 1);
  } // end IF
  if (ACE_LOG_MSG->open (ACE_TEXT (programName_in.c_str ()),
                         options_flags,
                         NULL) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Log_Msg::open(\"%s\", %u): \"%m\", aborting\n"),
                ACE_TEXT (programName_in.c_str ()),
                options_flags));

    return false;
  } // end IF

  // set new mask...
  u_long process_priority_mask = (LM_SHUTDOWN |
                                  LM_TRACE |
                                  LM_DEBUG |
                                  LM_INFO |
                                  LM_NOTICE |
                                  LM_WARNING |
                                  LM_STARTUP |
                                  LM_ERROR |
                                  LM_CRITICAL |
                                  LM_ALERT |
                                  LM_EMERGENCY);
  if (!enableTracing_in)
    process_priority_mask &= ~LM_TRACE;
  if (!enableDebug_in)
    process_priority_mask &= ~LM_DEBUG;
  ACE_LOG_MSG->priority_mask (process_priority_mask,
                              ACE_Log_Msg::PROCESS);

  return true;
}
