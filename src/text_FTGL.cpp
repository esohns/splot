#include "stdafx.h"

#include "text_ftgl.h"

//#ifdef HAVE_CONFIG_H
//#include "splot-config.h"
//#endif

#ifdef USE_FTGL_TEXT

#include "ace/OS.h"
#include "ace/OS_Memory.h"
#include "ace/Log_Msg.h"

//#include "gettext.h"
//
#include "defines.h"
//
//#include <sys/stat.h>
//#include <FTGL/ftgl.h>
//
//#ifdef HAVE_FONTCONFIG
//#include <fontconfig/fontconfig.h>
//#endif

//using namespace std;

Splot_TextFTGL::Splot_TextFTGL ()
 : inherited ()
 , ftFont_ (NULL)
 , fontFile_ (NULL)
{
  fontFile_ = findFont ();
  ACE_NEW (ftFont_,
           FTBufferFont (fontFile_));
  free ((void*)fontFile_);
  if (ftFont_->Error ())
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to FTBufferFont(\"%s\"), continuing\n"),
                ACE_TEXT (fontFile_)));

  ftFont_->FaceSize (24);
}

Splot_TextFTGL::~Splot_TextFTGL ()
{
  delete ftFont_;
}

void
Splot_TextFTGL::Render (const char* string_in, const int length_in)
{
  // FTGL renders the whole string when len == 0
  // but we don't want any text rendered then.
  if (length_in != 0)
    ftFont_->Render (string_in,
                     length_in,
                     FTPoint (),
                     FTPoint (),
                     FTGL::RENDER_ALL);
}

float
Splot_TextFTGL::Advance (const char* string_in, const int length_in)
{
  return ftFont_->Advance (string_in,
                           length_in,
                           FTPoint ());
}

float
Splot_TextFTGL::LineHeight (const char* string_in, const int length_in)
{
  return ftFont_->LineHeight ();
}

const char*
checkFont (const char* filename_in)
{
  ACE_stat stat;
  const char* path = filename_in;
  if (path &&
      (0 == ACE_OS::stat (path, &stat)))
    return ACE_OS::strdup (path);

  return NULL;
}
const char*
Splot_TextFTGL::findFont ()
{
  const char* font = NULL;

  // Get user-specified font path
  font = checkFont (ACE_OS::getenv (ACE_TEXT_ALWAYS_CHAR (SPLOT_FONT_DIR_ENV_SYMBOL)));
  if (font) return font;

#ifdef FONT_PATH
  // Get distro-specified font path
  font = checkFont (FONT_PATH);
  if (font) return font;
#endif

#ifdef HAVE_FONTCONFIG
  // Get default font via fontconfig
  if (!FcInit ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to FcInit(), aborting\n")));

    return NULL;
  } // end IF

  FcResult result;
  FcFontSet* fs;
  FcPattern* pat;
  FcPattern* match;

  /*
  TRANSLATORS: If using the FTGL backend, this should be the font
  name of a font that contains all the Unicode characters in use in
  your translation.
  */
  pat = FcNameParse ((FcChar8*)ACE_TEXT_ALWAYS_CHAR ("Gothic Uralic"));
  FcConfigSubstitute (0, pat, FcMatchPattern);

  FcPatternDel (pat, FC_WEIGHT);
  FcPatternAddInteger (pat, FC_WEIGHT, FC_WEIGHT_BOLD);

  FcDefaultSubstitute (pat);
  fs = FcFontSetCreate ();
  match = FcFontMatch (0, pat, &result);

  if (match) FcFontSetAdd (fs, match);
  if (pat) FcPatternDestroy (pat);
  if (fs)
  {
    FcChar8* file;
    if (FcPatternGetString (fs->fonts[0], FC_FILE, 0, &file) == FcResultMatch)
      font = checkFont ((const char*)file)
    FcFontSetDestroy (fs);
  } // end IF
  FcFini ();
  if (font) return font;
#endif

#ifdef __linux__
  // Check a couple of common paths for Gothic Uralic/bold as a last resort
  // Debian
  /*
  TRANSLATORS: If using the FTGL backend, this should be the path of a bold
  font that contains all the Unicode characters in use in	your translation.
  If the font is available in Debian it should be the Debian path.
  */
  font = checkFont (ACE_TEXT_ALWAYS_CHAR ("/usr/share/fonts/truetype/uralic/gothub__.ttf"));
  if (font) return font;
  /*
  TRANSLATORS: If using the FTGL backend, this should be the path of a
  font that contains all the Unicode characters in use in	your translation.
  If the font is available in Debian it should be the Debian path.
  */
  font = checkFont (ACE_TEXT_ALWAYS_CHAR ("/usr/share/fonts/truetype/uralic/gothu___.ttf"));
  if (font) return font;

  // Mandrake
  /*
  TRANSLATORS: If using the FTGL backend, this should be the path of a bold
  font that contains all the Unicode characters in use in	your translation.
  If the font is available in Mandrake it should be the Mandrake path.
  */
  font = checkFont (ACE_TEXT_ALWAYS_CHAR ("/usr/share/fonts/TTF/uralic/GOTHUB__.TTF"));
  if (font) return font;

  /*
  TRANSLATORS: If using the FTGL backend, this should be the path of a
  font that contains all the Unicode characters in use in	your translation.
  If the font is available in Mandrake it should be the Mandrake path.
  */
  font = checkFont (ACE_TEXT_ALWAYS_CHAR ("/usr/share/fonts/TTF/uralic/GOTHU___.TTF"));
  if (font) return font;

  // Check the non-translated versions of the above
  font = checkFont ("/usr/share/fonts/truetype/uralic/gothub__.ttf");
  if (font) return font;
  font = checkFont ("/usr/share/fonts/truetype/uralic/gothu___.ttf");
  if (font) return font;
  font = checkFont ("/usr/share/fonts/TTF/uralic/GOTHUB__.TTF");
  if (font) return font;
  font = checkFont ("/usr/share/fonts/TTF/uralic/GOTHU___.TTF");
  if (font) return font;
#endif

  return NULL;
}

#endif // USE_FTGL_TEXT