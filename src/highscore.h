#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include <string>

#include "defines.h"

class Splot_HighScore
{
 public:
  ~Splot_HighScore ();

  Splot_HighScore (const std::string&); // (FQ) filename
  bool initialized () const;

  float       getScore (int,  // skill level
                        int); // index
  std::string getName  (int,  // skill level
                        int); // index
  time_t      getDate  (int,  // skill level
                        int); // index

  static std::string getFileName ();
  bool save ();
  int     set (int, float); // skill level / score
  int     check (int, float); // skill level / score
  void    print (int); // skill level

 private:
  bool load (const std::string&); // (FQ) filename
  void insert (int,    // skill level
               int,    // rank
               float); // score

  float  highScore_[SKILL_LEVEL_MAX_INT][HI_SCORE_HIST];
  char   highScoreName_[SKILL_LEVEL_MAX_INT][HI_SCORE_HIST][MAX_PLAYER_NAME_LENGTH + 1];
  time_t highScoreDate_[SKILL_LEVEL_MAX_INT][HI_SCORE_HIST];

  bool initialized_;
};

#endif // HIGHSCORE_H
