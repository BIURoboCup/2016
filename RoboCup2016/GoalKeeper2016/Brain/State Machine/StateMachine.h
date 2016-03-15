#include "State.h"

class StateMachine
{
public:
	StateMachine();
	~StateMachine();

private:
	State& currentState;
};


