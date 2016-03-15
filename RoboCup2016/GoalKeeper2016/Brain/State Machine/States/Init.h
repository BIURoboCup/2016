#include "../State.h"


class Init : public State
{
	public:
			Init();
	virtual ~Init();

	virtual void Play() = 0;
};

