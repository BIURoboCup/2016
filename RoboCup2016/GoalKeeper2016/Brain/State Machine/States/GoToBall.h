#include "../State.h"


class GoToBall : public State
{
	public:
	GoToBall();
	virtual ~GoToBall();

	virtual void Play() = 0;
};

