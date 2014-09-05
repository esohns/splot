#include "stdafx.h"

#include "game_element.h"

#include "ace/Assert.h"
#include "ace/OS.h"

Splot_GameElementBase::Splot_GameElementBase ()
 : prev_ (NULL),
   next_ (NULL)
{

}

Splot_GameElementBase::~Splot_GameElementBase ()
{

}

void
Splot_GameElementBase::set_next (Splot_GameElement* next_in)
{
//  Splot_GameElement* temp = next_;
  next_ = next_in;
  if (next_)
  {
    next_->prev_ = (Splot_GameElement*)this;
//    next_->prev_ = dynamic_cast<Splot_GameElement*> (this);
//    if (temp)
//      next_->next_ = temp;
  } // end IF
}

// init statics
unsigned int Splot_GameElement::count = 0;

Splot_GameElement::Splot_GameElement ()
 : inherited ()
// , position_ ()
// , translationVector_ ()
 , age_ (0)
 , type_ (GAMEELEMENT_INVALID)
{
  ACE_OS::memset (&position_, 0, sizeof (position_));
  ACE_OS::memset (&translationVector_, 0, sizeof (translationVector_));

  Splot_GameElement::count++;
}

Splot_GameElement::~Splot_GameElement ()
{
  Splot_GameElement::count--;
}

//void
//Splot_GameElement::reset ()
//{
//  type_ = GAMEELEMENT_INVALID;
//  age_ = 0;
//  ACE_OS::memset (&position_, 0, sizeof (position_));
//  ACE_OS::memset (&velocity_, 0, sizeof (velocity_));
//}
