#include "QueueWrapper.h"

QueueWrapper::QueueManager* QueueWrapper::QueueManager::pQueueManagerInstance = nullptr;

QueueWrapper::QueueManager::QueueManager()
{
	return;
}

QueueWrapper::QueueManager::~QueueManager()
{
	return;
}

QueueWrapper::QueueManager* QueueWrapper::QueueManager::Instance()
{
	if (pQueueManagerInstance == nullptr)
		pQueueManagerInstance = new QueueManager();

	return pQueueManagerInstance;
}