#include "TestObject.h"
#include "QueueWrapper.h"
int main()
{
	
	TestObject testObject;
	std::string queue_name = "test_queue";
	size_t queue_size = 0;
	int i,cc = 0;
	while (true)
	{
		usleep(10000);
		
		cc = QueueWrapper::QueueManager::Instance()->GetQueuesize<std::string>(&queue_name, &queue_size);
		if(cc == QueueWrapper::QUEUE_SUCCESS)
			printf("queue size is %d \n", queue_size);
		i++;

		if (i == 1000)
			break;
		
	}

	bool success_stop = testObject.StopProcessing();
	printf("queue size is %d after stop\n", testObject.GetQueueSize());
    return 0;
}