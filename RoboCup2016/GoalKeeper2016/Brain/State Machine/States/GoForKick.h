#include "../State.h"


class GoForKick : public State
{
	public:
	GoForKick();
	virtual ~GoForKick();

	virtual void Play() = 0;
};

