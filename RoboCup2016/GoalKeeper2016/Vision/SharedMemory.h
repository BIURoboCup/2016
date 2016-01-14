#ifndef SHAREDMEMORY_H_
#define SHAREDMEMORY_H_

#include "DetectedObject.h"
#include <semaphore.h>

class SharedMemory {
public:
	SharedMemory(DetectedObject* object);
	SharedMemory();
	~SharedMemory();

	DetectedObject* SafeRead();
	void SafeWrite(DetectedObject* objectToWrite);

private:
	sem_t m_semaphore;
	DetectedObject* m_shraedObject;
};

#endif /* SHAREDMEMORY_H_ */
