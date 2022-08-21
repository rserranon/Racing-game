#include "ncurses-game-engine.h"
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <chrono>
#include <ncurses.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <thread>
#include <iomanip>
#include <locale.h>



ConsoleGameEngine::ConsoleGameEngine()
{
}

int ConsoleGameEngine::ConstructConsole(int x, int y, int width, int height, bool border, std::string label)
  {
    setlocale(LC_ALL, "");
    m_bBorder = border;
    WINDOW *console;
    int lnConsoleX = 0;
    int lnConsoleY = 0;


    console = initscr();
    noecho();
    curs_set(0);
    border(0,0,0,0,0,0,0,0);
    getmaxyx(console, lnConsoleY, lnConsoleX);
    if (!console ) {
      endwin();
      std::cout << "Error initializing nCurses \n";
      return -1;
    }
    if (!has_colors()){
        puts("Cannot do colors");
        return -1;
    }
    if (start_color() != OK)
    {
        endwin();
        puts("Unable to start colors");
        return -1;
    }
    init_pair(PAIR_RED_BLACK, COLOR_RED, COLOR_BLACK);
		init_pair(PAIR_YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);
		init_pair(PAIR_BLUE_BLACK, COLOR_BLUE, COLOR_BLACK);
		init_pair(PAIR_GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);
		init_pair(PAIR_WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);
		init_pair(PAIR_GRAY_BLACK, COLOR_GRAY, COLOR_BLACK);
		init_pair(PAIR_CYAN_BLACK, COLOR_CYAN, COLOR_BLACK);
		init_pair(PAIR_GREEN_LIGHT_BLACK, COLOR_GREEN_LIGHT, COLOR_BLACK);

    move(0,3);
    printw("[ConsoleY:%d , ", lnConsoleY);
    printw("ConsoleX:%d]", lnConsoleX);
    if ( width > lnConsoleX ) {
      endwin();
      std::cout << "Error Console game is larger in (x,y) than Terminal or y or y are larger than height, width \n";
      return -1;
    }
    m_nScreenWidth  = width;
    m_nScreenHeight = height;
    m_pWindow = subwin(console, m_nScreenHeight, m_nScreenWidth, y, x);
    if (!m_pWindow) {
      endwin();
      std::cout << "Error creating main window \n";
      return  -1;
    }
    if (border)
      wborder(m_pWindow,0,0,0,0,0,0,0,0);

    std::string title = ("["+label+"]");
    int x_tit = (int)(m_nScreenWidth / 2 - title.length()/2);
    mvwaddstr(m_pWindow, 0, x_tit, title.c_str());

    m_pBufferScreen = new CHAR_INFO[m_nScreenWidth * m_nScreenHeight];
    memset(m_pBufferScreen, ' ', sizeof(CHAR_INFO) * m_nScreenWidth * m_nScreenHeight);

    refresh();
    return 0;
  }

  void ConsoleGameEngine::Draw(int y, int x, wchar_t wc, int color)
  {

    if ( x >= 0 && x <= m_nScreenWidth && y >= 0 && y <= m_nScreenHeight)
    {
      m_pBufferScreen[ y * m_nScreenWidth + x ].utf8char = wc;
      m_pBufferScreen[ y * m_nScreenWidth + x ].Attributes = color;
    }
  }

  void ConsoleGameEngine::DisplayFrame()
  {
    int coords_offset = 0;
    int limits_offset = 0;
    if (m_bBorder) {
      coords_offset = 1;
      limits_offset = -1;
    }
    for (int y = 0 + coords_offset; y < m_nScreenHeight + limits_offset; y++)
      for (int x = 0 + coords_offset; x < m_nScreenWidth + limits_offset; x++) {
        wattrset(m_pWindow, COLOR_PAIR(m_pBufferScreen[y * m_nScreenWidth + x].Attributes));
        cchar_t c = {
          A_NORMAL,
          {m_pBufferScreen[y * m_nScreenWidth + x].utf8char}
        };
        mvwadd_wch(m_pWindow, y, x, &c);
        wattrset(m_pWindow, COLOR_PAIR(PAIR_WHITE_BLACK));
      }
    wrefresh(m_pWindow);
  }


  void ConsoleGameEngine::DrawWString(int x, int y, std::wstring wstr, int color)
  {
    CHAR_INFO *ptr;
    ptr = m_pBufferScreen + (y * m_nScreenWidth + x);
    for (int i = 0; i < wstr.length(); i++)
    {
        ptr->utf8char = wstr[i];
        ptr->Attributes = color;
        ptr++;
    }
  }

  void ConsoleGameEngine::DrawWStringAlpha(int x, int y, std::wstring wstr, int color)
  {
    CHAR_INFO *ptr;
    ptr = m_pBufferScreen + (y * m_nScreenWidth + x);
    for (int i = 0; i < wstr.length(); i++)
    {
        if (wstr[i] != L' ')
        {
          ptr->utf8char = wstr[i];
          ptr->Attributes = color;
        }
        ptr++;
    }
  }

  void ConsoleGameEngine::print_str(std::string str, int x, int y, int color)
  {
    CHAR_INFO *ptr;

    ptr = m_pBufferScreen + ( y * m_nScreenWidth + x);

      for (int i = 0; i < str.length(); i++ ) {
        ptr->utf8char = str[i];
        ptr->Attributes = color;
        ptr++;
      }
  }

  void ConsoleGameEngine::Start()
  {
    m_bAtomicActive = true;
    std::thread t = std::thread(&ConsoleGameEngine::GameThread, this);

    t.join();
  }

  ConsoleGameEngine::~ConsoleGameEngine()
  {
    delete [] m_pBufferScreen;
    endwin();
  }

  void ConsoleGameEngine::GameThread()
  {
    if (!OnUserCreate())
			m_bAtomicActive = false;

    auto tp1 = std::chrono::system_clock::now();
    auto tp2 = std::chrono::system_clock::now();

    keypad(m_pWindow, TRUE);
    while (m_bAtomicActive)
    {
      tp2 = std::chrono::system_clock::now();
      std::chrono::duration<float> elapsedTime = tp2 - tp1;
      tp1 = tp2;
      float fElapsedTime = elapsedTime.count();

      // INPUT ==========================================================
      nodelay(m_pWindow,TRUE);
      m_nKeyPressed = wgetch(m_pWindow);

      // Game logic =====================================================
      switch(m_nKeyPressed){
          case 'q':
              m_bAtomicActive = false;
              break;
      }

      std::stringstream ss;
      ss << "[FPS:";
      ss.width(10);
      ss << (int)(1.0f / fElapsedTime) << "]";

      int x_tit = (int)(m_nScreenWidth / 2 - ss.str().length()/2);
      mvwaddstr(m_pWindow, m_nScreenHeight-1, x_tit, ss.str().c_str());

      if (!OnUserUpdate(fElapsedTime))   // if userUpdate fails
        m_bAtomicActive = false;

      this->DisplayFrame();
    }
  }

std::atomic<bool> ConsoleGameEngine::m_bAtomicActive(false);
std::condition_variable ConsoleGameEngine::m_cvGameFinished;
std::mutex ConsoleGameEngine::m_muxGame;

