#ifndef VictoryScreen_h_seen
#define VictoryScreen_h_seen

#include "TouchScreen.h"

class VictoryScreen: public TouchScreen
{
  public:
    //!Types of possible end game.
    enum Victory_t {
      WIN,       //!< There was a winner.
      DRAW,      //!< No winner, game drawn.
      CONTINUE   //!< No human players, check continue state
    };
    //! Start the end game screen.
    VictoryScreen(Victory_t type);
    virtual void show();
    virtual void animate();
    virtual void handleKeys();

    /** Call this when only cpu players remain. Asks the player if they want to
     * continue. */
    static void endGameOption();
  private:
    Victory_t m_winOrDraw;
    int m_highlightItem;
    int m_cycleColour1;
    int m_cycleColour2;
    int m_cycleColour3;
    int m_cycleFrame;
    bool m_decided;

    static void exitCb(void * arg);
    static void continueYesCb(void * arg);
    static void continueNoCb(void * arg);

    void exit();

    void win();
    void draw();
    void continueGame();

    void continueKeys();
};
#endif
