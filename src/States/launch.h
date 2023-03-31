/*
lauches rocket
*/

#pragma once

#include "state.h"

class Launch : public State
{
  public:
    Launch(stateMachine *sm);
    void initialise();
    State *update();
    void exitstate();


};
