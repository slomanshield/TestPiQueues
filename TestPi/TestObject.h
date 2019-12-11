#include <stdio.h>
#include <unistd.h>
#include <queue>

#include "ThreadWrapper.h"

typedef std::chrono::milliseconds Milli;

/* depnding on the application the queues would even be static in a global header file along with the mutexes and conditions and event counters 
    and the objects would use the routines to pass data along. So what you would need is something like a static object queue manager where each object would have its own event counters, queue in/out
	mutex and condition variable. Each object would register those variables (pointers) on a static list (in the queue manager) and objects would call routines like (put data on queue) where the data is 
	always a void pointer and the consumer knows how to cast the data. And they would call routines like get data and pass in the index of their data that was returned to them by the queue manager (so it can get the event counters and others etc)
	now who would delete the pointer? That is based on the application it could be that a group of writers have a pool of pointers and they get data off a socket store it in an internal structure 
	and send it to a processor.. processor processes it (the reader) then sends the pointer back where it is placed on the writer "free" list and the pointer gets reused*/

class TestObject
{
	public:
		TestObject();
		~TestObject();
		bool StopProcessing();
		int GetQueueSize();
	private:
		static threadProcess processFunc;
		static threadProcess processFuncWrite;
		ThreadWrapper<threadProcess> workerThreadWrapper;
		ThreadWrapper<threadProcess> workerThreadWrapperWriter;
		void IncrementEventCount();
		void DecrementEventCount();
		static bool DataAvaiable();
		std::queue<std::string> mainQueue;
		std::mutex m;
		std::condition_variable cv;
		static unsigned long long event_count;

		std::string queueName;

		/* making event count static is just a temporary fix in reality what is needed is a static mutex and a static std::list of event counts when an object is created 
		   it should make an entry in the list and store its index in the list on desctruion of the object it should delete that entry from the list (all under lock of the mutex obv)
		   so then when the event count is refrenced it should be based on the index in the list */
};