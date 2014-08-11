#include "stdafx.h"

#include "common.h"

#include <string>
#include <fstream>

#include "ace/OS.h"
#include "ace/OS_Memory.h"

#define SPLOT_DATA_DIR_ENV_SYMBOL "SPLOT_DATA"

std::string
alterPathForPlatform (char* filename_in)
{
  char* walker = filename_in;
#ifdef macintosh
  while (*walker)
  {
    if (*walker == '/')
      *walker = ':';
    walker++;
  } // end IF
#endif
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  while (*walker)
  {
    if (*walker == '/')
      *walker = '\\';
    walker++;
  } // end IF
#endif

  return filename_in;
}

std::string
dataLoc (const std::string& filename_in, bool doCheck_in)
{
  char buffer[PATH_MAX + 1];
  ACE_OS::memset (buffer, 0, sizeof (buffer));

  const char* data_p =
   ACE_OS::getenv (ACE_TEXT_ALWAYS_CHAR (SPLOT_DATA_DIR_ENV_SYMBOL));
  if (data_p &&
      ((ACE_OS::strlen (data_p) + filename_in.size () + 1) > PATH_MAX))
  {
    sprintf (buffer,
             ACE_TEXT_ALWAYS_CHAR ("%s/%s"),
             data_p, filename_in.c_str ());

    goto check;
  } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  const char* home_dir_p = ACE_OS::getenv ("HOME");
  if (home_dir_p &&
      ((ACE_OS::strlen(home_dir_p) + filename_in.size() + 1) > PATH_MAX))
  {
    sprintf (buffer,
             ACE_TEXT_ALWAYS_CHAR ("%s/."PACKAGE"-data/%s"),
             home_dir_p, filename_in);

    goto check;
  } // end IF
#elif defined (PKGDATADIR)
  if (((ACE_OS::strlen (PKGDATADIR) + filename_in.size ()) < PATH_MAX))
  {
    sprintf (buffer,
             ACE_TEXT_ALWAYS_CHAR ("%s/%s"),
             PKGDATADIR, filename_in);

    goto check;
  } // end IF
#endif

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
  alterPathForPlatform (buffer);

  if (doCheck_in)
  {
    ACE_stat file_stat;
    if (ACE_OS::stat(buffer, &file_stat))
      return NULL;
  } // end IF

  return buffer;
}

void printExtensions (FILE* fstream, const char* extstr_in)
{
  char* walker = NULL;
  char* space = NULL;
  int len = ACE_OS::strlen (extstr_in);
  int cnt = 0;
  char* extstr = NULL;
  ACE_NEW (extstr,
           char[len + 1]);
  ACE_OS::strcpy (extstr, extstr_in);
  walker = extstr;

  fprintf (fstream, ACE_TEXT_ALWAYS_CHAR ("Extensions :"));
  while ((walker - extstr) < len)
  {
    space = ACE_OS::strchr (walker, ' ');
    if (space)
    {
      if ((cnt % 2) == 0)
        fprintf (fstream, "\n");
      *space = '\0';
      fprintf (fstream, ACE_TEXT_ALWAYS_CHAR ("%-31s "), walker);
      walker = space + 1;
      cnt++;
    } // end IF
    else
      break;
  } // end WHILE
  fprintf (fstream, ACE_TEXT_ALWAYS_CHAR ("\n"));

  delete [] extstr;
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

    ACE_OSTREAM_TYPE* log_stream;
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
                  logFile_in.c_str ()));

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
                programName_in.c_str (),
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
