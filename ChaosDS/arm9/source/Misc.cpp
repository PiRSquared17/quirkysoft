#include <ctype.h>
#include <nds.h>
#include "Misc.h"
#include "GameState.h"

// default seed
static unsigned int s_random(14071977);
void Misc::seedRand(unsigned int seed) {
  s_random = seed;
}

// churn the random number
void Misc::churnRand(void) {
  s_random *= 663608941;
}

unsigned int Misc::getRand(unsigned int maximum) {
  unsigned long long result = maximum;
  result *= s_random;
  churnRand();
  return (unsigned int)(result >> 32);
}

char * Misc::trim(char * str) {
  char *s = str;
  char *d = str;
  
  // eat trailing white space
  while (*s)
    s++;
  while (str < s and isspace(*(s-1)))
    *s-- = '\0';
  
  *s = '\0';
  
  s = str;
  // has leading space?
  if (isspace(*s))
  {
    // eat white space 
    while (isspace(*s))
      s++;
    
    // copy
    while (*s)
      *d++ = *s++;
    *d = '\0';
  } 
  return str;
}

// based on code at c64c
void Misc::orderTable(int count, unsigned char * table) {
  // orders "count" entries in the given table by priority
  // table contains priority, index pairs
  
  for (int i = 0; i < count; i++) {
    int index = 0;
    for (int j = 0; j < count; j++) {
      // priority 1, 2, value 1, 2
      unsigned char p1(table[index]);
      index++;  
      unsigned char val1(table[index]);
      index++;  
      unsigned char p2(table[index]);
      index++;
      unsigned char val2(table[index]);
      index--;   // points to prio 2
      if (p2 > p1) {
        index++;  // points to val 2
        table[index] = val1;
        index--;  // points to prio 2
        table[index] = p1;
        index--;  // points to val 1
        table[index] = val2;
        index--;  // points to prio 1
        table[index] = p2;
        index++;  // points to val 1
        index++;  // points to prio 2
      }
    }
  }
}

bool Misc::getKeypressWait()
{
    scanKeys();
    swiWaitForVBlank();
    u16 keysSlow = keysDownRepeat();
    if ( keysSlow & (KEY_A|KEY_B|KEY_L|KEY_R|
                     KEY_UP|KEY_DOWN|KEY_LEFT|KEY_RIGHT|
                     KEY_START|KEY_SELECT|KEY_X|KEY_Y)) 
      return true;
    return false;
}

void Misc::delay(int time) 
{
  GameState & state(GameState::instance());
  state.resetGameFrame();
  while(state.gameFrame() < time) 
  {
    swiWaitForVBlank();
    state.mainLoopExecute();
  }
}
