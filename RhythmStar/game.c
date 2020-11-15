
 #include <fcntl.h>
 #include <unistd.h> 
 #include <linux/input.h>
 #include <stdio.h> 
 #include <stdlib.h> 
 #include <string.h>
 #include <sys/ioctl.h> 
 #include <ctype.h>
 #include <sys/ipc.h>
 #include <sys/types.h>
 #include <sys/stat.h> 
 #include <pthread.h>
 #include <sys/mman.h>
 #include "textlcd.h"
 #include "fnd.h"
 #include "led.h"
 #include "buzzer.h"
#include "button.h"
#include "colorLed.h"
 #include <sys/mman.h>
#include <sys/msg.h>


pthread_t tid[2];
 
 
const int musicScale[MAX_SCALE_STEP] =
{
262, /*do*/ 294,330,349,392,440,494, /* si */ 523
};

void* dosome(void *arg)
{
   
  int score=0;
  while(1){
      score = score+10;
      sleep(1);
      fndDisp(score , 0);   
  }
      
   
   }


void* dosome2(void *arg)
{
   
char *name1 = "schoolsong";
char *name2 = "solo";
char *singer1 = "dont know";
char *singer2 = "geny";
char keyboard_1;  


while(1)
{


   lcdtextwrite(name1,name1,1,2);
   lcdtextwrite(singer1,singer1,2,2); 

     sleep(5);
     
       lcdtextwrite(name2,name2,1,2);
      lcdtextwrite(singer2,singer2,2,2);      
sleep(5);

}

   }


int main(int argc , char **argv)
{
///////////////////////////////////////////////////////////////////////////////////////////////buzzer

  
  

   
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////colorled
   
pwmLedInit();
pwmSetPercent(0,0);
pwmSetPercent(0,1);
pwmSetPercent(0,2);
pwmInactiveAll();
   
   
  int err,arr;
  
  err = pthread_create(&(tid[1]), NULL, &dosome, NULL); 
  
  arr = pthread_create(&(tid[2]), NULL, &dosome2, NULL); 
   
///////////////////////////////////////////////////////// textlcd 출력


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////fnd출력
   
    int mode;
 
    int keyboard_2;  // 나중에 key 눌러서 타이밍 맞추어 게임 성공했을 떄.

    
   
     mode = MODE_STATIC_DIS;   // 내가 쓰는 값 출력.(option).
   
   
   //for(int i=0; i<6;i++)
   //{
   printf("bar matching sccess: press 6~9\n");
   scanf("%d",&keyboard_2);
   

   //fndOff();

   ///////////////////////////////////////////////////////////////////////////////////////////////////////////led
   
   int key_boardled;

char keyboard_1;     //키보드 입력. -> 나중에는 key 눌렀을 때 그 그값으로
      printf("startled\n");
      scanf("%x",&key_boardled);
      printf("write data :0x%X\n", key_boardled);

      ledLibInit();
      ledOnOff (key_boardled,1);
      //sleep(1);

       //ledLibExit();
       

       
       
       
   
       ////////////////////////////////////////////////////////////////////////////colorled
       
       
       
       
       /////////////////////////////////////////////////////////////////////////////button
 
 printf("start button\n");
 
 buttonLibInit();
int   msgID = msgget (MESSAGE_ID, IPC_CREAT |0666); 
BUTTON_MSG_T msgRx;

   msgrcv ( msgID, &msgRx, sizeof(int), 0, 0);
 


if(msgRx.keyInput==KEY_VOLUMEUP)
{
   ledOnOff (3,1);
}



pthread_join (tid[0], NULL); 
pthread_join (tid[1], NULL);
}











