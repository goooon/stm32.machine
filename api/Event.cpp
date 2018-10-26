#include "Event.h"
#include "Tracer.h"
#include "Queue.h"
//�����ṹ�壬���ڴ����жϲ���
struct Node{
	EventType type;
	u32 hex;
	u32 dec;
	void* ptr;
};
//
static StaticQueue<Node,512> eventQueue;
/* �����ж���Ϣ
	type���ж�����
	hex��32λ����
	dec��32λ����
	param����������
	�����д��ݽ����Ĳ����Ž��ṹ����ѽṹ��Ž��������棬˳��ִ��
*/				
extern "C" bool PostEvent(EventType type,u32 hex,u32 dec,void* param)
{
	/*if(eventQueue.isFull()){
		LOG_E("PostEvent(0x%x,0x%x,%d,%p) is Full",type,hex,dec,param);
		return false;
	}*/
	//��װ�ṹ��
	Node n;
	n.dec = dec;
	n.hex = hex;
	n.type = type;
	n.ptr = param;
	//����
	__disable_irq();
	//�ṹ�����
	if(!eventQueue.inQueue(n)){
		__enable_irq();
		LOG_E("eventQueue.inQueue() Failed");
		return false;
	}
	//����
	__enable_irq();
	return true;
}
  /* ��ȡ�ж���Ϣ
	type���ж�����
	hex��32λ����
	param��������
	dec��32λ������
    ��ȡ����Ϣ����Ľṹ�壬���Ұѽṹ��Ĳ�����ֵ�����ݽ����Ĳ�����
*/
extern "C" bool ReadEvent(EventType* type,u32* hex,u32* dec,void** param)
{
	 Node n;
	__disable_irq();
	//�ṹ�����
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