#include "TestObject.h"
#include "QueueWrapper.h"
unsigned long long TestObject::event_count = 0;

TestObject::TestObject()
{
	queueName = "test_queue";
	QueueWrapper::QueueManager::Instance()->CreateQueue<std::string>(&queueName, nullptr, nullptr);
	workerThreadWrapper.SetProcessthread(processFunc);
	workerThreadWrapper.StartProcessing(10, this);
	workerThreadWrapperWriter.SetProcessthread(processFuncWrite);
	workerThreadWrapperWriter.StartProcessing(30, this);
}

TestObject::~TestObject()
{
	
}

int TestObject::GetQueueSize()
{
	return mainQueue.size();
}

void TestObject::IncrementEventCount()
{
	unsigned long long prev_event_count = TestObject::event_count;
	TestObject::event_count++;
	if (TestObject::event_count == 0 && prev_event_count == 0xFFFFFFFFFFFFFFFF)
		TestObject::event_count = 0xFFFFFFFFFFFFFFFF;//this means data is being put on the queue faster than we can read... by this point we possibly would run out of memory and other alarms should go off  
	/* but what this does is make it so we can at least take off all the data */

}

void TestObject::DecrementEventCount()
{
	TestObject::event_count--;
	if (TestObject::event_count == 0xFFFFFFFFFFFFFFFF)
		TestObject::event_count = 0;//if we are going below that means we are taking things off and threads getting notified before they should be
}

bool TestObject::DataAvaiable()
{
	return TestObject::event_count > 0;
}

bool TestObject::StopProcessing()
{
	bool ret = workerThreadWrapper.StopProcessing(11000); 
	ret = workerThreadWrapperWriter.StopProcessing(10000);//this is wrong but im lazy to create another routine
	QueueWrapper::QueueManager::Instance()->DeleteQueue<std::string>(&queueName, true);
	return ret;
}

void TestObject::processFunc(bool * running, bool * stopped, void * usrPtr)
{
	TestObject* testObject = (TestObject *)usrPtr;
	std::thread::id threadId = std::this_thread::get_id();
	QueueWrapper::QueueManager* pQueueManager = QueueWrapper::QueueManager::Instance();
	size_t queueSize = 1;//default to 1
	pQueueManager->RegisterThreadToQueue<std::string>(&testObject->queueName, threadId);
	int cc = QueueWrapper::QUEUE_SUCCESS;
	
	while (*running == true || queueSize > 0)
	{
		try
		{
			std::string str =  pQueueManager->GetDataFromQueue<std::string>(&testObject->queueName, 10, &cc);

			if (cc == QueueWrapper::QUEUE_SUCCESS)
			{
				//cout << str << "\n";
			}
			else
				cout << "Error taking data off queue " << testObject->queueName << "with error: " << cc << "\n";

			pQueueManager->GetQueuesize<std::string>(&testObject->queueName, &queueSize);
		}
		catch (exception e)
		{
			cout << e.what() << "\n";
			break;
		}
		
	}
	pQueueManager->RemoveThreadFromQueue<std::string>(&testObject->queueName, threadId);
	*stopped = true;

}

void TestObject::processFuncWrite(bool * running, bool * stopped, void * usrPtr)
{
	TestObject* testObject = (TestObject *)usrPtr;
	std::hash<std::thread::id> hashThread;
	unsigned long long  i = hashThread.operator()(std::this_thread::get_id());
	char string_buffer[100] = { 0 };
	std::string str;
	QueueWrapper::QueueManager* pQueueManager = QueueWrapper::QueueManager::Instance();
	int cc = QueueWrapper::QUEUE_SUCCESS;
	
	while (*running == true)
	{
		
		sprintf(string_buffer, "%lld", i);
		str = string_buffer;
		
		cc = pQueueManager->PutDataOnQueue<std::string>(&testObject->queueName,str);

		if(cc != QueueWrapper::QUEUE_SUCCESS)
			cout << "Error putting data on queue " << testObject->queueName << "with error: " << cc << "\n";

		usleep(10000);

	}

	*stopped = true;

}

/* Notes: if you have the writers sleep for 10 milliseconds i was able to have 30 writers and 1 reader and there was no queue backlog
           Once i took away the sleep for the writers you needed to have the more amount of readers than writers in this case 10/30 
		   there was queue backlog back is flacuated went from 0 to 10k to 1k back to 0 it did not grow. In normal circumstances 
		   the writers would sleep at the bottom of the loop for something like 10 milliseconds */