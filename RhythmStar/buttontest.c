#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h> // for open/close
#include <fcntl.h> // for O_RDWR
#include <sys/ioctl.h> // for ioctl
#include <sys/mman.h>
#include <stdint.h>
#include "button.h"
 #include <sys/ipc.h>
#include <sys/msg.h>



int main(int argc, char *argv[])
{
int fp;
int readSize,inputIndex;
struct input_event stEvent;
fp = buttonLibInit();
int   msgID = msgget (MESSAGE_ID, IPC_CREAT |0666); 
BUTTON_MSG_T msgRx;
while(1)
{
   msgrcv ( msgID, &msgRx, sizeof(int), 0, 0);
   switch(msgRx.keyInput)
   {
      case KEY_VOLUMEUP: printf("Volume up key):"); break;
      case KEY_HOME: printf("Home key):"); break;
      case KEY_SEARCH: printf("Search key):"); break;
      case KEY_BACK: printf("Back key):"); break;
      case KEY_MENU: printf("Menu key):"); break;
      case KEY_VOLUMEDOWN:printf("Volume down key):");break;
   }
printf("pressed\n");
}
buttonLibExit();
}
