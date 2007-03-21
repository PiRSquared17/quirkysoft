#ifndef Movement_h_seen
#define Movement_h_seen

#include "ScreenI.h"

class Movement : public ScreenI
{
  public:
    virtual void show();
    virtual void animate();
    virtual void vblank();
    virtual void handleKeys();

    //! Is the current movement of a flying creature.
    bool isFlying() const;

    //! Set the amount the selected thing can move.
    void setMovementAllowance(int ma);

    inline int movementAllowance() const
    {
      return m_movementAllowance;
    }

    //! Press A to select the creature.
    void a();
    void b();

    inline int selectedCreature() const
    {
      return m_selectedCreature;
    }
    inline int rangeAttack() const
    {
      return m_rangeAttack;
    }
    inline bool isEngaged() const
    {
      return m_engagedFlag;
    }

  private:
    unsigned char m_selectedCreature;
    unsigned char m_rangeAttack;
    unsigned char m_movementAllowance;
    unsigned char m_isFlying;
    unsigned char m_engagedFlag;
    unsigned char m_creatureId;
    unsigned char m_rangeAttackVal;
    unsigned char m_wizardMovementAllowance;
    unsigned char m_attacker;
    unsigned char m_highlightItem;
    void startMovementRound();
    void start();
    void end();
    void selectCreature();
    void moveFlyingCreature();
    void moveWalkingCreature();
    void moveCreature(int);
    void doRangeAttack();
    void checkEngaged(int index);
    void doSuccessfulMove(int distanceMoved);
    void makeAttack();
    void setEngagedToEnemy();
    void doAttackAnim(int index);
    void removeCreature(int creature);
};
#endif