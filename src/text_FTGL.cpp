#include "stdafx.h"

#include "text_FTGL.h"

#ifdef USE_FTGL_TEXT
#include <string>

#include "ace/OS.h"
#include "ace/OS_Memory.h"
#include "ace/Log_Msg.h"

#include "defines.h"
#include "common.h"

#ifdef HAVE_FONTCONFIG
#include "fontconfig/fontconfig.h"
#endif

Splot_TextFTGL::Splot_TextFTGL ()
 : inherited ()
 , ftFont_ (NULL)
{
  char* font_file = findFont ();
  if (!font_file)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_TextFTGL::findFont(), returning\n")));

    return;
  } // end IF

  ACE_NEW (ftFont_,
           FTBufferFont (font_file));
  FT_Error ft_error = ftFont_->Error ();
  if (ft_error)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to FTBufferFont(\"%s\"): %d, continuing\n"),
                ACE_TEXT (font_file),
                ft_error));
  free ((void*)font_file);

  if (!ftFont_->FaceSize (24, 72))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to FTFont::FaceSize(): %d, continuing\n"),
                ftFont_->Error ()));
}

Splot_TextFTGL::~Splot_TextFTGL ()
{
  if (ftFont_)
    delete ftFont_;
}

void
Splot_TextFTGL::Render (const char* string_in, const int length_in)
{
  ACE_ASSERT (ftFont_);

  // *NOTE*: FTGL renders the whole string when len == 0
  if (length_in != 0)
  {
    FTPoint new_pen_point = ftFont_->Render (string_in,
                                             length_in,
                                             FTPoint (),
                                             FTPoint (),
                                             FTGL::RENDER_ALL);
    ACE_UNUSED_ARG (new_pen_point);
    FT_Error ft_error = ftFont_->Error ();
    if (ft_error)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to FTFont::Render(\"%s\"): %d, continuing\n"),
                  ACE_TEXT (string_in),
                  ft_error));
  } // end IF
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
  ACE_UNUSED_ARG (string_in);
  ACE_UNUSED_ARG (length_in);

  return ftFont_->LineHeight ();
}

char*
checkFont (const char* filename_in)
{
  ACE_stat stat;
  const char* path = filename_in;
  if (path &&
      (0 == ACE_OS::stat (path, &stat)))
    return ACE_OS::strdup (path);

  return NULL;
}
char*
Splot_TextFTGL::findFont ()
{
  char* font = NULL;

  // Get user-specified font path
  std::string path_base = ACE_TEXT_ALWAYS_CHAR (SPLOT_FONT_DATA_DIR);
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  std::string filename = path_base + ACE_TEXT_ALWAYS_CHAR (SPLOT_FONT_FILE);
  font = checkFont (dataLoc (filename).c_str ());
  if (font) return font;

#ifdef FONT_PATH
  // Get distro-specified font path
  path_base = ACE_TEXT_ALWAYS_CHAR (FONT_PATH);
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  filename = path_base + ACE_TEXT_ALWAYS_CHAR (SPLOT_FONT_FILE);
  font = checkFont (filename.c_str ());
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
      font = checkFont ((const char*)file);
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
