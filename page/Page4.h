#ifndef ZLG_PAGE4_HH
#define ZLG_PAGE4_HH
#include "./Page.h"
#include "../setting/Setting.h"
#include "../api/lcd.h"
class Page4 : public Page
{
public:
    //���뵱ǰҳ��4
	virtual void enter(){
		index = 0;
		lcd::sendAddrValue(0x410,index == 0 ? 0 : 1); //�ж�ѡ��ť֮�����ҳ�棬0������߰�ť���������������棬1�����ұ߰�ť������������ݽ���
		lcd::sendAddrValue(0x414,index == 1 ? 0 : 1);
	}
	//�뿪��ǰҳ��4
	virtual void leave(){
		index = 0;
		lcd::sendAddrValue(0x410,index == 0 ? 0 : 1);//��¼����ҳ�棬0������߰�ť��1�����ұ߰�ť
		lcd::sendAddrValue(0x414,index == 1 ? 0 : 1);
	}
	virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
	{
			LOG_I("key pressed:%c/0x%x",cmd,cmd);
			switch(cmd){
				  case '<':	  //����ť�����CMD_Left
						index --;
					  if(index == 0xff)index = 1;
						lcd::sendAddrValue(0x410,index == 0 ? 0 : 1);
						lcd::sendAddrValue(0x414,index == 1 ? 0 : 1);
						break;
					case '>':	//����ť�����CMD_Right
						index ++;
					  if(index == 2)index = 0;
						lcd::sendAddrValue(0x410,index == 0 ? 0 : 1);
						lcd::sendAddrValue(0x414,index == 1 ? 0 : 1);
						break;
					case ext::CMD_Input:   //����ť�����CMD_Input
						lcd::jumpToPage(index == 0 ?  7 : 8); //��ת����Ӧ����
					  Setting::setConfigToothType( index == 0 ? Male : Female);	  //��¼��ĸ�ݿ�
					  return ext::None;
						break;
					case ext::CMD_FanHui: //������ذ�ť���ص�ҳ��3
							lcd::jumpToPage(3);
							return ext::None;
			}
			return cmd;
	}
private:
		char index;
};
#endif