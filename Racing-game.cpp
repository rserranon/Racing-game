#include "ncurses-game-engine.h"
#include <math.h>
#include <vector>
#include <utility>
#include <string>

class RacingGame: public ConsoleGameEngine
{
  public:
    RacingGame(){
      m_sAppName = "Racing game";
    }

  private:
    float fCarPos = 0.0f;
    float fDistance = 0.0f;
    float fCurvature = 0.0f;
    float fTrackCurvature = 0.0f;
    float fPlayerCurvature = 0.0f;
    float fSpeed = 0.0f;
    float fTrackDistance = 0.0f;
    float fCurrentLapTime = 0.0f;

    std::vector<std::pair<float, float>> vTrack;

  protected:
    bool OnUserCreate()
    {
      vTrack.push_back(std::make_pair(0.0f, 10.0f));  // Start/Finish line
      vTrack.push_back(std::make_pair(0.0f, 200.0f));  // Start/Finish line
      vTrack.push_back(std::make_pair(1.0f, 200.0f));  // Start/Finish line
      vTrack.push_back(std::make_pair(0.0f, 400.0f));  // Start/Finish line
      vTrack.push_back(std::make_pair(-1.0f, 200.0f));  // Start/Finish line
      vTrack.push_back(std::make_pair(0.0f, 200.0f));  // Start/Finish line
      vTrack.push_back(std::make_pair(-1.0f, 200.0f));  // Start/Finish line
      vTrack.push_back(std::make_pair(1.0f, 200.0f));  // Start/Finish line
      vTrack.push_back(std::make_pair(0.0f, 200.0f));  // Start/Finish line
      vTrack.push_back(std::make_pair(0.2f, 500.0f));  // Start/Finish line
      vTrack.push_back(std::make_pair(0.0f, 200.0f));  // Start/Finish line

      for (auto t: vTrack)
        fTrackDistance += t.second;
      return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime)
    {

      fElapsedTime *= 4;
      switch (m_nKeyPressed) {
        case KEY_UP:
          fSpeed += 2.0f * fElapsedTime;
          break;
        case KEY_DOWN:
          fSpeed -= 1.0f * fElapsedTime;
          break;
        case KEY_LEFT:
          fPlayerCurvature -= 1.9 * fElapsedTime * (1.0f - fSpeed / 2.0f);
          break;
        case KEY_RIGHT:
          fPlayerCurvature += 1.9 * fElapsedTime * (1.0f - fSpeed / 2.0f);
          break;
      }
      if (fabs(fPlayerCurvature - fTrackCurvature)  >= 0.8f)
        fSpeed -= 5.0f * fElapsedTime;

      if (fSpeed < 0.0f) fSpeed = 0.0f;
      if (fSpeed > 1.0f) fSpeed = 1.0f;


      fDistance += (100.0f * fSpeed) * fElapsedTime;

      float fOffset = 0.0f;
      int nTracSection = 0;

      fCurrentLapTime += fElapsedTime;

      if (fDistance > fTrackDistance){
        fDistance -= fTrackDistance;
        fCurrentLapTime = 0.0;;
      }

      // Find position on track
      while (nTracSection < vTrack.size() && fOffset <= fDistance)
      {
        fOffset += vTrack[nTracSection].second;
        nTracSection++;
      }

      // Interpolate towards target track curvature
      float fTargetCurvature = vTrack[nTracSection - 1].first;
      float fTrackCurvDiff = (fTargetCurvature - fCurvature) * fElapsedTime * fSpeed;

      // Accumulate player curvature
      fCurvature += fTrackCurvDiff;

      // Accumulate track curvature
      fTrackCurvature += (fCurvature) * fElapsedTime * fSpeed;


      for (int y = 0; y < m_nScreenHeight / 2; y++ )
        for (int x = 0; x < m_nScreenWidth; x++)
        {
            float fPerspective = (float)y / (m_nScreenHeight / 2.0f);

            float fRoadWidth    = 0.1f + fPerspective * 0.8f; // Min 10% max 90%
            float fClipWidth    = fRoadWidth * 0.15f;
            fRoadWidth    *= 0.5f;

            float fMiddlePoint  = 0.5f + fCurvature * powf((1.0f - fPerspective), 3);



            int nLeftGrass = (fMiddlePoint - fRoadWidth - fClipWidth) * m_nScreenWidth;
            int nLeftClip  = (fMiddlePoint - fRoadWidth) * m_nScreenWidth;
            int nRightClip = (fMiddlePoint + fRoadWidth) * m_nScreenWidth;
            int nRightGrass = (fMiddlePoint + fRoadWidth + fClipWidth) * m_nScreenWidth;

            // Draw road
            int nRow = m_nScreenHeight / 2 + y;

            int nGrassColour = sinf(20.0f *  powf(1.0f - fPerspective,3) + fDistance * 0.1f) > 0.0f ? PAIR_GREEN_LIGHT_BLACK : PAIR_GREEN_BLACK;
            int nClipColour = sinf(80.0f *  powf(1.0f - fPerspective,2) + fDistance ) > 0.0f ? PAIR_RED_BLACK : PAIR_WHITE_BLACK;

            int nRoadColour = (nTracSection - 1 == 0) ? PAIR_WHITE_BLACK : PAIR_GRAY_BLACK;

            if (x >= 0 && x < nLeftGrass)
              Draw(nRow, x, 0x2588, nGrassColour );
            if (x >= nLeftGrass && x < nLeftClip)
              Draw(nRow, x, 0x2588, nClipColour);
            if (x >= nLeftClip && x < nRightClip)
              Draw(nRow, x, 0x2588, nRoadColour);
            if (x >= nRightClip && x < nRightGrass)
              Draw(nRow, x, 0x2588, nClipColour);
            if (x >= nRightGrass && x < m_nScreenWidth)
              Draw(nRow, x, 0x2588, nGrassColour);

        }
            // Draw Car
            fCarPos = fPlayerCurvature - fTrackCurvature;
            int nCarPos = (int)(m_nScreenWidth / 2) + ((int)(m_nScreenWidth * fCarPos) / 2.0f) - 7;

            DrawWStringAlpha(nCarPos, 45, L"   ||####||    ");
            DrawWStringAlpha(nCarPos, 46, L"      ##       ");
            DrawWStringAlpha(nCarPos, 47, L"     ####      ");
            DrawWStringAlpha(nCarPos, 48, L"|||  ####  ||| ");
            DrawWStringAlpha(nCarPos, 49, L"|||########||| ");
            DrawWStringAlpha(nCarPos, 50, L"|||  ####  ||| ");

            // Draw Stats
            DrawWString(1, 1, L"Target Curvature: " + std::to_wstring(fCurvature));
            DrawWString(1, 2, L"Player Curvature: " + std::to_wstring(fPlayerCurvature));
            DrawWString(1, 3, L"Player Speed    : " + std::to_wstring(fSpeed));
            DrawWString(1, 4, L"Track Curvature : " + std::to_wstring(fTrackCurvature));
            DrawWString(1, 5, L"Distance        : " + std::to_wstring(fDistance));
            DrawWString(1, 6, L"Section         : " + std::to_wstring(nTracSection));
            DrawWString(1, 7, L"Elapsed time    : " + std::to_wstring(fElapsedTime));
            DrawWString(1, 8, L"fCarPos         : " + std::to_wstring(fCarPos));
            DrawWString(1, 9, L"nCarPos         : " + std::to_wstring(nCarPos));

            auto disp_time = [](float t)
            {
              int nMinutes = t / 60;
              int nSeconds = t - (nMinutes * 60.0f);
              int nMiliSeconds = (t - (float)nSeconds) * 1000.0f;
              return std::to_wstring(nMinutes) + L"." + std::to_wstring(nSeconds) + L":" + std::to_wstring(nMiliSeconds);
            };

            DrawWString(1, 10, L"CurrentLapTime  : " + disp_time(fCurrentLapTime));

      return true;
    }
};

int main(){

  RacingGame game;

  int status;

  status = game.ConstructConsole(1, 1, 160, 63, true, "Racing Game" );
  if (status == 0)
  {
      game.print_str("hello world", 1, 1, PAIR_YELLOW_BLACK);
      game.Start();
  } else {
    return -1;
  }

  return 0;
}
