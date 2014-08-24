#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include <string>

#include "defines.h"

class Splot_HighScore
{
 public:
  ~Splot_HighScore ();

  Splot_HighScore (const std::string&); // FQ filename
  bool initialized () const;

  float       getScore (int,  // skill level
                        int); // index
  std::string getName  (int,  // skill level
                        int); // index
  time_t      getDate  (int,  // skill level
                        int); // index

  static std::string getFileName ();
  static std::string getOldFileName();
  bool save ();
  int     set (int skill, float score);
  int     check (int skill, float score);
  void    print (int skill);

 private:
  bool load (const std::string&); // FQ filename
  void insert (int skill, int rank, float score);

  float  highScore_[MAX_LEVEL][HI_SCORE_HIST];
  char   highScoreName_[MAX_LEVEL][HI_SCORE_HIST][MAX_PLAYER_NAME_LENGTH + 1];
  time_t highScoreDate_[MAX_LEVEL][HI_SCORE_HIST];

  bool initialized_;
};

#endif // HIGHSCORE_H
