int send1[] = {0x00,0x00,0x00,0x00,0x00,0xC9,0x04,0x00,0x00,0x00,0x00};//时间同步
int send2[] = {0x00,0x00,0x00,0x00,0x00,0xCA,0x04,0x00,0x00,0x00,0x00};//读设置项
int send3[] = {0x00,0x00,0x00,0x00,0x00,0xCB,0x1C,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//上传状态
int send4[] = {0x00,0x00,0x00,0x00,0x00,0x66,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//账户验证
int send5[] = {0x00,0x00,0x00,0x00,0x00,0xCC,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//预结算
int send6[] = {0x00,0x00,0x00,0x00,0x00,0xCD,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//结算
const unsigned char wake[24]={
  0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x03, 0xfd, 0xd4, 0x14, 0x01, 0x17, 0x00};//NFC唤醒
const unsigned char tag[11]={
  0x00, 0x00, 0xFF, 0x04, 0xFC, 0xD4, 0x4A, 0x01, 0x00, 0xE1, 0x00};//获取UID
const unsigned char std_ACK[25] = {
  0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x0C, \
0xF4, 0xD5, 0x4B, 0x01, 0x01, 0x00, 0x04, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x4b, 0x00};//获取UID后返回
unsigned char old_id[5];
unsigned char receive_ACK[25];//接收缓存
int UID[25];
#include "Arduino.h"
#define print1Byte(args) Serial1.write(args)
#define print1lnByte(args)  Serial1.write(args),Serial1.println()


void UART_Send_Byte(unsigned char command_data)
{//send byte to PC
  Serial.print(command_data,HEX);
  Serial.print(" ");
} 
void UART1_Send_Byte(unsigned char command_data)
{//send byte to device
  print1Byte(command_data);
  Serial1.flush();// complete the transmission of outgoing serial data 
} 
void read_ACK(unsigned char temp)
{//read ACK into reveive_ACK[]
  unsigned char i;
  for(i=0;i<temp;i++) {
    receive_ACK[i]= Serial1.read();
  }
}
void wake_card(void)//唤醒
{//send wake[] to device
  unsigned char i;
  for(i=0;i<24;i++) //send command
    UART1_Send_Byte(wake[i]);
}
void send_tag(void)//UID获取
{//send tag[] to device
  unsigned char i;
  for(i=0;i<11;i++) //send command
    UART1_Send_Byte(tag[i]);
}
void copy_id (void) //复制UID
{//save old id
  int ai, oi;
  for (oi=0, ai=19; oi<5; oi++,ai++) 
  {
    old_id[oi] = receive_ACK[ai];
  }
}

char cmp_id (void) //测重
{//return true if find id is old
  int ai, oi;
  for (oi=0,ai=19; oi<5; oi++,ai++) 
  {
    if (old_id[oi] != receive_ACK[ai])
      return 0;
  }
  return 1;
}

int test_ACK (void) //信息比对
{// return true if receive_ACK accord with std_ACK
  int i;
  for (i=0; i<19; i++) {
    if (receive_ACK[i] != std_ACK[i])
      return 0;
  }
  return 1;
}

void copy_UID()//复制ID号到send4
{
  int y;
  for (y = 19;y <= 22;y++)
  {
    send4[y-12] = receive_ACK[y];
  }

}

void send_4()//发送send4数据
{
 
  int z;
  for (z = 0;z <= 14;z++)
  {
    Serial.print(send4[z],HEX);
    Serial.print(" ");
  }
  Serial.println();

}
void send_id (void) //打印UID
{//send id to PC
  int i;
  //Serial.print ("ID: ");
  for (i=19; i<= 22; i++) 
  {
    //Serial.print (receive_ACK[i], HEX);
   // Serial.print (receive_ACK[i], HEX);
   // delay(100);
    UID[i] = receive_ACK[i];
    Serial.print (UID[i],HEX);
    Serial.print (" ");
  }
  Serial.println ();
}

void NFC_int()
{
    wake_card();
     delay(100);
    read_ACK(15);
     delay(100);
}

void NFC_loop()
{   
    send_tag(); 
    read_ACK(25);
    delay(100);
    if (!cmp_id ()) 
    {
        if (test_ACK ()) 
        {
            //display (25);
            // send_ID();
            //send_id();
            copy_UID();
            send_4();
            delay(50);
        }
    }
  copy_id ();

}

