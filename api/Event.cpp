#include "Event.h"
#include "Tracer.h"
#include "Queue.h"
//申明结构体，用于传递中断参数
struct Node{
	EventType type;
	u32 hex;
	u32 dec;
	void* ptr;
};
//
static StaticQueue<Node,512> eventQueue;
/* 发送中断信息
	type：中断类型
	hex：32位整数
	dec：32位整数
	param：任意类型
	将所有传递进来的参数放进结构体里，把结构体放进队列里面，顺序执行
*/				
extern "C" bool PostEvent(EventType type,u32 hex,u32 dec,void* param)
{
	/*if(eventQueue.isFull()){
		LOG_E("PostEvent(0x%x,0x%x,%d,%p) is Full",type,hex,dec,param);
		return false;
	}*/
	//组装结构体
	Node n;
	n.dec = dec;
	n.hex = hex;
	n.type = type;
	n.ptr = param;
	//加锁
	__disable_irq();
	//结构体入队
	if(!eventQueue.inQueue(n)){
		__enable_irq();
		LOG_E("eventQueue.inQueue() Failed");
		return false;
	}
	//解锁
	__enable_irq();
	return true;
}
  /* 读取中断信息
	type：中断类型
	hex：32位整数
	param：任意数
	dec：32位整类型
    读取到消息队里的结构体，并且把结构体的参数赋值到传递进来的参数上
*/
extern "C" bool ReadEvent(EventType* type,u32* hex,u32* dec,void** param)
{
	 Node n;
	__disable_irq();
	//结构体出队
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