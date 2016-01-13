#include "SharedMemory.h"

SharedMemory::SharedMemory() {
	// TODO Auto-generated constructor stub

}

SharedMemory::~SharedMemory() {
	// TODO Auto-generated destructor stub
}


void SharedMemory::SafeWrite(DetectedObject& objectToWrite)
{
	if (sem_wait(&m_semaphore) != 0)
	{
		cout << "Couldn't sem_wait\n";
	}

	// m_shraedObject = objectToWrite;

	if (sem_post(&m_semaphore) != 0)
	{
		cout << "Couldn't sem_post\n";
	}
}

DetectedObject* SharedMemory::SafeRead()
{
	DetectedObject* objectToReturn;

	if (sem_wait(&m_semaphore) != 0)
	{
		cout << "Couldn't sem_wait\n";
	}

	objectToReturn = m_shraedObject;

	if (sem_post(&m_semaphore) != 0)
	{
		cout << "Couldn't sem_post\n";
	}

	return objectToReturn;
}
