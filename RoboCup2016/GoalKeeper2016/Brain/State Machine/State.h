#include "../../Motion/Motion.h"

class State
{
protected:
	State();
	void WriteStateLog(string message);

public:
	virtual ~State();

	virtual void Play() = 0;

	virtual string GetName() = 0;
};


