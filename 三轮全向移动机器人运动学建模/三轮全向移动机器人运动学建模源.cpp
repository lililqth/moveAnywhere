#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include"CnComm.h"
#define L 2.5
#define SIG_MOTOR_INDEX	   1
typedef unsigned char UCHAR;
UCHAR m_pSendBuf[17];
CnComm m_CnComm; 
void m_Split2Bytes(UCHAR *inTarg, short inSrc)
{
	if (inTarg == NULL)
	{
		return;
	}
	
	static unsigned short temp;
	memcpy(&temp,&inSrc,sizeof(short));
	inTarg[1] = (UCHAR)temp&0x00ff;
	temp >>= 8;
	inTarg[0] = (UCHAR)temp&0x00ff;
}

UCHAR m_CalSum(int length)
{
	int temp=0;
	for (int i=0;i<length;i++)
	temp+=m_pSendBuf[i];

	UCHAR ret;
	ret=(UCHAR)(temp & 0x000000ff);
	return ret;
}
void m_GenerateSigCmd(unsigned int inIndex, UCHAR inID,UCHAR inLen, UCHAR inMode, UCHAR inMethod, UCHAR *data)
{
	//55 AA 38 0A 08 70 1H 1L 2H 2L 3H 3L 4H 4L 5H 5L SUM	// 同时设置五电机速度
	//包头  数据  校验和
	
	m_pSendBuf[0]=(UCHAR)0x55;
	m_pSendBuf[1]=(UCHAR)0xaa;
	m_pSendBuf[2]=inID;
	m_pSendBuf[3]=inLen;
	m_pSendBuf[4]=inMode;
	m_pSendBuf[5]=inMethod;
	
	if (inLen>0 && data!=NULL)
	{
		memcpy(&m_pSendBuf[6],data,inLen);
	}
	m_pSendBuf[inLen+6]=m_CalSum(inLen+6);//添加校验和
	
	/*for(int i = 0; i < inLen+7; i ++)
		printf("%0x",m_pSendBuf[i]);
	system("pause");*/
	m_CnComm.Write(m_pSendBuf, inLen+7);
} 
void SetFiveMotorsSpeed(short* inSpeed){
	//55 AA 38 0A 08 70 1H 1L 2H 2L 3H 3L 4H 4L 5H 5L SUM	// 同时设置五电机速度
	UCHAR speedbuf[10];
	m_Split2Bytes(&speedbuf[0],inSpeed[0]);
	m_Split2Bytes(&speedbuf[2],inSpeed[1]);
	m_Split2Bytes(&speedbuf[4],inSpeed[2]);
	m_Split2Bytes(&speedbuf[6],inSpeed[3]);
	m_Split2Bytes(&speedbuf[8],inSpeed[4]);
	m_GenerateSigCmd(SIG_MOTOR_INDEX,0x38,0x0a,0x08,0x70,speedbuf);
}

void change(double a, double b, double c, double x){
	short speed[5] = {0};
	short v1 = 0, v2 = 0, v3 = 0, v4 = 0, v5 = 0;
	v1 = sin(60 - x) * a + cos(60 - x) * b + L * c;
	v2 = -sin(60 + x) * a + cos(60 + x) * b + L * c;
	v3 = sin(x) * a - cos(x) * b + L * c;
	speed[0] = v1;
	speed[1] = v2;
	speed[2] = v3;
	/*speed[0] = 500;
	speed[1] = -500;
	speed[2] = 0;*/
	SetFiveMotorsSpeed(speed);

}
int main(){
	double Vx, Vy, Vo;   //广义坐标系所需要的x轴上速度，y轴上速度，角速度o;
	double Angles; //机器人坐标系与世界坐标系之间的夹角；
	m_CnComm.Open(1);
	m_CnComm.SetState(2000000);
	/*if(m_CnComm.IsOpen())
	{
		printf("串口已成功打开");
		m_CnComm.SetWnd(this->m_hWnd);
		m_bSerialPortOpened=TRUE;
	}
	else
	{
		AfxMessageBox(_T("串口打开失败"));
		m_bSerialPortOpened=false;
	}*/

	scanf("%lf%lf%lf%lf", &Vx, &Vy, &Vo, &Angles);
	change(Vx, Vy, Vo, Angles);
	
}