#include "ncurses-game-engine.h"

class RacingGame: public ConsoleGameEngine
{
  public:
    RacingGame(){
      m_sAppName = "Racing game";
    }

  protected:
    bool OnUserCreate()
    {
      return true;
    }

    bool OnUserUpdate()
    {
      for (int y = 0; y < m_nScreenHeight / 2; y++ )
        for (int x = 0; x < m_nScreenWidth; x++)
        {
            float fMiddlePoint  = 0.5f;
            float fRoadWidth    = 0.6f;
            float fClipWidth    = fRoadWidth * 0.15f;

            fRoadWidth    *= 0.5f;

            int nLeftGrass = (fMiddlePoint - fRoadWidth - fClipWidth) * m_nScreenWidth;
            int nLeftClip  = (fMiddlePoint - fRoadWidth) * m_nScreenWidth;
            int nRightClip = (fMiddlePoint + fRoadWidth) * m_nScreenWidth;
            int nRightGrass = (fMiddlePoint + fRoadWidth + fClipWidth) * m_nScreenWidth;

            int nRow = m_nScreenHeight / 2 + y;
            if (x >= 0 && x < nLeftGrass)
              Draw(nRow, x, 0x2588, PAIR_GREEN_BLACK);
            if (x >= nLeftGrass && x < nLeftClip)
              Draw(nRow, x, 0x2588, PAIR_RED_BLACK);
            if (x >= nLeftClip && x < nRightClip)
              Draw(nRow, x, 0x2588, PAIR_GRAY_BLACK);
            if (x >= nRightClip && x < nRightGrass)
              Draw(nRow, x, 0x2588, PAIR_RED_BLACK);
            if (x >= nRightGrass && x < m_nScreenWidth)
              Draw(nRow, x, 0x2588, PAIR_GREEN_BLACK);
        }
      return true;
    }
};

int main(){

  RacingGame game;

  int status;

  status = game.ConstructConsole(1, 1, 137, 37, true, "Racing Game" );
  if (status == 0)
  {
      game.print_str("hello world", 1, 1, PAIR_YELLOW_BLACK);
      game.Start();
  }

  return 0;
}
