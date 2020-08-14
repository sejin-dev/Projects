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
 #include "button.h" 
 #include <pthread.h>
 #include <sys/mman.h>
#include <sys/msg.h>
 
 static  pthread_t  buttonTh_id; 
 
 static int fd = 0; 
 void *buttonThFunc(void*a); 
 static int msgID=0 ; 
 
 
 int buttonLibInit(void)
  {
      fd=open (BUTTON_DRIVER_NAME, O_RDONLY); 
      msgID = msgget (MESSAGE_ID, IPC_CREAT |0666); 
      pthread_create(&buttonTh_id, NULL, buttonThFunc, NULL); 
      }
      
 int buttonLibExit(void) 
  {
          pthread_exit(&buttonTh_id); 
  }

void *buttonThFunc(void*a)
 {
     BUTTON_MSG_T msgTx;
      msgTx.messageNum = 1; 
    
      struct input_event stEvent;
   
       while (1) 
{
          read(fd, &stEvent, sizeof (stEvent));
           if ( ( stEvent.type == EV_KEY) && ( stEvent.value == 0) ) 
           {
               msgTx.keyInput = stEvent.code;
               msgsnd(msgID, &msgTx, sizeof(int), 0);
             } 
      } 
}
