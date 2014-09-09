#include "stdafx.h"

#include "background.h"

#include "ace/OS.h"
#include "ace/Log_Msg.h"

#include "background_segment.h"
#include "background_metal.h"
#include "background_sea.h"

Splot_Background::Splot_Background (BackgroundType_t type_in)
 : type_ (type_in)
 , variation_ (0)
 //, position_ ()
 //, tex_ ()
 //, segments_ ()
{
  ACE_OS::memset (position_, 0, sizeof (position_));
  ACE_OS::memset (tex_, 0, sizeof (tex_));
}

Splot_Background::~Splot_Background ()
{
  //glDeleteTextures (MAX_BACKGROUND_TYPES, &(tex_[0]));

  for (segmentsIterator_t iterator = segments_.begin ();
       iterator != segments_.end ();
       iterator++)
    delete *iterator;
}

bool
Splot_Background::init ()
{
  bool result = Splot_BackgroundMetal::init ();
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_BackgroundMetal::init(), aborting\n")));

    return false;
  } // end IF
  result = Splot_BackgroundMetal::init ();
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_BackgroundSea::init(), aborting\n")));

    return false;
  } // end IF

  return result;
}

void
Splot_Background::fini ()
{
  Splot_BackgroundMetal::fini ();
  Splot_BackgroundSea::fini ();
}

Splot_Background*
Splot_Background::make (BackgroundType_t type_in)
{
  Splot_Background* result = NULL;

  switch (type_in)
  {
    case BACKGROUND_METAL:
      ACE_NEW_NORETURN (result,
                        Splot_BackgroundMetal ());
      break;
    case BACKGROUND_SEA:
      ACE_NEW_NORETURN (result,
                        Splot_BackgroundSea ());
      break;
    default:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown background type (was: %d), aborting\n"),
                  type_in));

      return NULL;
  } // end SWITCH
  if (!result)
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));

  return result;
}

void
Splot_Background::nextVariation ()
{
  setVariation (variation_+1);
}
