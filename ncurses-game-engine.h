#define _XOPEN_SOURCE_EXTENDED 1 // Activate wide char functionality
#include <mutex>
#include <ncurses.h>
#include <string>
#include <thread>
#include <atomic>
#include <condition_variable>


#define COLOR_GRAY 8
#define COLOR_GREEN_LIGHT 10
enum ColourPairs
	{
		PAIR_RED_BLACK = 1,
		PAIR_YELLOW_BLACK,
		PAIR_BLUE_BLACK,
		PAIR_GREEN_BLACK,
		PAIR_WHITE_BLACK,
    PAIR_GRAY_BLACK,
    PAIR_CYAN_BLACK,
    PAIR_GREEN_LIGHT_BLACK
	};

typedef struct _CHAR_INFO {
  wchar_t   utf8char;
  int       Attributes;
} CHAR_INFO;

class ConsoleGameEngine
{
  public:
    ConsoleGameEngine();

    int ConstructConsole(int x, int y, int width, int height, bool border, std::string label);

    virtual ~ConsoleGameEngine();

    virtual void Draw(int y, int x, wchar_t wc = 0x2588, int color = PAIR_WHITE_BLACK);

    void DrawWString(int x, int y, std::wstring str, int color = PAIR_WHITE_BLACK);

    void DrawWStringAlpha(int x, int y, std::wstring str, int color = PAIR_WHITE_BLACK);

    void print_str(std::string, int x, int y, int color = PAIR_WHITE_BLACK);

    void DisplayFrame();

    void Start();

    int m_nProgramStatus;


  protected:
    std::string   m_sAppName;
    WINDOW        *m_pWindow = nullptr;
    bool          m_bBorder = true;
    CHAR_INFO     *m_pBufferScreen = nullptr;
    int           m_nScreenWidth;
    int           m_nScreenHeight;
    int           m_nKeyPressed;

    static std::atomic<bool> m_bAtomicActive;
    static std::condition_variable m_cvGameFinished;
	  static std::mutex m_muxGame;

    virtual bool OnUserCreate() = 0;
    virtual bool OnUserUpdate(float fElapsedTime) = 0;

  private:
    void GameThread();
};

