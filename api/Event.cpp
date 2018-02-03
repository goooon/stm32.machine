#include "Event.h"
#include "Tracer.h"
#include "Queue.h"
struct Node{
	EventType type;
	u32 hex;
	u32 dec;
	void* ptr;
};
static StaticQueue<Node,512> eventQueue;
extern "C" bool PostEvent(EventType type,u32 hex,u32 dec,void* param)
{
	/*if(eventQueue.isFull()){
		LOG_E("PostEvent(0x%x,0x%x,%d,%p) is Full",type,hex,dec,param);
		return false;
	}*/
	Node n;
	n.dec = dec;
	n.hex = hex;
	n.type = type;
	n.ptr = param;
	__disable_irq();
	if(!eventQueue.inQueue(n)){
		__enable_irq();
		LOG_E("eventQueue.inQueue() Failed");
		return false;
	}
	__enable_irq();
	return true;
}

extern "C" bool ReadEvent(EventType* type,u32* hex,u32* dec,void** param)
{
	 Node n;
	__disable_irq();
	 if(!eventQueue.outQueue(n)){//queue is empty
		 __enable_irq();
		 //LOG_E("eventQueue.outQueue() Failed");
		 return false;
	 }
	 *type = n.type;
	 *hex = n.hex;
	 *dec = n.dec;
	 *param = n.ptr;
	 return true;
}