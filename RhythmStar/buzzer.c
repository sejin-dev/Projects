#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "buzzer.h"

char gBuzzerBaseSysDir[128]; 


int findBuzzerSysPath()
{
	 DIR * dir_info = opendir(BUZZER_BASE_SYS_PATH); 
	 int ifNotFound = 1; 
	 if (dir_info != NULL){
		  while (1){
			   struct dirent *dir_entry; 
			   dir_entry = readdir (dir_info); 
			   if (dir_entry == NULL)break; 
			   if (strncasecmp(BUZZER_FILENAME, dir_entry->d_name, strlen(BUZZER_FILENAME)) == 0){ 
				   ifNotFound = 0;
				   sprintf(gBuzzerBaseSysDir,"%s%s/",BUZZER_BASE_SYS_PATH,dir_entry->d_name); }
				    }
				     }
				      printf("find %s\n",gBuzzerBaseSysDir); 
				      return ifNotFound; 
				      }
				      
				      
	void buzzerEnable(int bEnable) 
	{
		
		 char path[200]; 
		 sprintf(path,"%s%s",gBuzzerBaseSysDir,BUZZER_ENABLE_NAME);
		  int fd=open(path,O_WRONLY);
		  if ( bEnable)  write(fd, &"1", 1); 
		  else write(fd, &"0", 1); 
		  close(fd);
		   }
void setFrequency(int frequency) 
{
	
	 char path[200];
	 sprintf(path,"%s%s",gBuzzerBaseSysDir,BUZZER_FREQUENCY_NAME);
	 int fd=open(path,O_WRONLY); 
	 dprintf(fd, "%d", frequency); 
	 close(fd);
	 } 
