#include <stdio.h>
#include <stdlib.h> // for exit
#include <unistd.h> // for open/close
#include <fcntl.h> // for O_RDWR
#include <sys/ioctl.h> // for ioctl
#include <sys/mman.h>
#include <linux/fb.h> // for fb_var_screeninfo, FBIOGET_VSCREENINFO
#include "bitmap.h"
#define FBDEV_FILE "/dev/fb0"
#define BIT_VALUE_24BIT 24
void usage(void)
   {
   printf("====================================================\n");
   printf("\nUsage: ./bitmap [FILE.bmp]\n");
   printf("====================================================\n");
}


// filename의 파일을 열어 순수 bitmap data은 *data 주소에 저장하고 , pDib은
// malloc 해제하기 위한 메모리 포이트이다.
void read_bmp(char *filename, char **pDib, char **data, int *cols, int *rows)
{
   BITMAPFILEHEADER bmpHeader;
   BITMAPINFOHEADER *bmpInfoHeader;
   unsigned int size;
   unsigned char magicNum[2];
   int nread;
   FILE *fp;
   fp = fopen(filename, "rb");
   if(fp == NULL) {
   printf("ERROR\n");
   return;
   }
   // identify bmp file
   magicNum[0] = fgetc(fp);
   magicNum[1] = fgetc(fp);
   printf("magicNum : %c%c\n", magicNum[0], magicNum[1]);
   if(magicNum[0] != 'B' && magicNum[1] != 'M') {
   printf("It's not a bmp file!\n");
   fclose(fp);
   return;
   }
   nread = fread(&bmpHeader.bfSize, 1, sizeof(BITMAPFILEHEADER), fp);
   size = bmpHeader.bfSize - sizeof(BITMAPFILEHEADER);
   *pDib = (unsigned char *)malloc(size); // DIB Header(Image Header)
   fread(*pDib, 1, size, fp);
   bmpInfoHeader = (BITMAPINFOHEADER *)*pDib;
   printf("nread : %d\n", nread);
   printf("size : %d\n", size);
   // check 24bit
   if(BIT_VALUE_24BIT != (bmpInfoHeader->biBitCount)) // 24bit pixel form만지원
   {
   printf("It supports only 24bit bmp!\n");
   fclose(fp);
   return;
   }
   *cols = bmpInfoHeader->biWidth;
   *rows = bmpInfoHeader->biHeight;
   *data = (char *)(*pDib + bmpHeader.bfOffBits - sizeof(bmpHeader) - 2);
   fclose(fp); // 순수 bitmap 데어터
}

void close_bmp(char **pDib)
{
   free(*pDib); // 메모리 해제
}


int main (int argc, char **argv)
{
   int i, j, k, t, z,l,m,h,q,w,e,p;
   int fbfd;
   int screen_width;
   int screen_height;
   int bits_per_pixel;
   int line_length;
   int coor_x, coor_y;
   int cols = 0, rows = 0;
   int mem_size;
   char *pData, *data;
   char r, g, b;
   unsigned long bmpdata[1280*800];
   unsigned long pixel;
   unsigned char *pfbmap;
   unsigned long *ptr;
   struct fb_var_screeninfo fbvar;
   struct fb_fix_screeninfo fbfix;
   
   
   printf("=================================\n");
   printf("Frame buffer Application - Bitmap\n");
   printf("=================================\n\n");
   
   
   
   if(argc != 2) {
   usage();
   return 0;
   }
   
   
   
   read_bmp(argv[1], &pData, &data, &cols, &rows);
   printf("Bitmap : cols = %d, rows = %d\n", cols, rows);
   
   
   for( j = 0; j < rows; j++)
   {
      k = j * cols * 3;
      t = (rows - 1 - j) * cols; // 가로 size가 작을 수도 있다.
      
      for(i = 0; i < cols; i++)
      {
         b = *(data + (k + i * 3));
         g = *(data + (k + i * 3 + 1));
         r = *(data + (k + i * 3 + 2));
         pixel = ((r<<16) | (g<<8) | b);
         
         bmpdata[t+i] = pixel; // save bitmap data bottom up
   
   
   
         
         ///////////////////////////////////test1 어디에 어떤 값을 써야해? 
         //printf("bmpdata[cols(i):%d,rows(j):%d] : %d\n",i,j,pixel); //i는 411까지,  cols임.  
      }
   }
     
   
   
   close_bmp(&pData); // 메모리 해제
   if( (fbfd = open(FBDEV_FILE, O_RDWR)) < 0) //
   {
   printf("%s: open error\n", FBDEV_FILE);
   exit(1);
   }
   
   
   if( ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar) )
   {
   printf("%s: ioctl error - FBIOGET_VSCREENINFO \n", FBDEV_FILE);
   exit(1);
   }
   
   
   if( ioctl(fbfd, FBIOGET_FSCREENINFO, &fbfix) ) // screen info를 얻어옴
   {
   printf("%s: ioctl error - FBIOGET_FSCREENINFO \n", FBDEV_FILE);
   exit(1);
   }
   
   
   if (fbvar.bits_per_pixel != 32)
   {
   fprintf(stderr, "bpp is not 32\n");
   exit(1);
   }
   
   
   screen_width = fbvar.xres;
   screen_height = fbvar.yres;
   bits_per_pixel = fbvar.bits_per_pixel;
   line_length = fbfix.line_length;
   mem_size = line_length * screen_height;
   printf("screen_width : %d\n", screen_width);
   printf("screen_height : %d\n", screen_height);
   printf("bits_per_pixel : %d\n", bits_per_pixel);
   printf("line_length : %d\n", line_length);
   pfbmap = (unsigned char *)
   mmap(0, mem_size, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);
   
   
   if ((unsigned)pfbmap == (unsigned)-1)
   {
   perror("fbdev mmap\n");
   exit(1);
   }


















   for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(5000*1000);
	   break;
   }
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   // fb 검은색으로 지움.
   for(coor_y = 0; coor_y < screen_height; coor_y++) 
   {
      ptr = (unsigned long *)pfbmap + (screen_width * coor_y);
      for(coor_x = 0; coor_x < screen_width; coor_x++)
         {
            *ptr++ = 0x000000;
         }
   }
   
  for (j = 415; j<575; j++){         
      for (i =15; i<45; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 1-1
   }
}
  for (j = 420; j<450; j++){         
      for (i =850; i<880; i++){    //////////////////////////////////////////////////////////////
      bmpdata[(j*1000)+i] =50000;     // 1-1
   }
}
 for (j = 380; j<410; j++){         
      for (i =850; i<880; i++){    //////////////////////////////////////////////////////////////
      bmpdata[(j*1000)+i] =50000;     // 1-1
   }
}
 for (j = 340; j<370; j++){         
      for (i =850; i<880; i++){    //////////////////////////////////////////////////////////////
      bmpdata[(j*1000)+i] =50000;     // 1-1
   }
}
   for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }

for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
    for (j = 415; j<575; j++){         
      for (i =15; i<45; i++){    
      bmpdata[(j*1000)+i] =0;     // 1-1
   }
}
for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j = 20; j<180; j++){         
      for (i =15; i<45; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 1-3
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
   for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j = 20; j<180; j++){         
      for (i =50; i<80; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 2-3
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
     for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j = 20; j<180; j++){         
      for (i =15; i<45; i++){    
      bmpdata[(j*1000)+i] =0;     // 1-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =85; i<115; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 3-3
   }
}
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
     for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
    for (j = 20; j<180; j++){         
      for (i =50; i<80; i++){    
      bmpdata[(j*1000)+i] =0;     // 2-3
   }
}
    for (j = 20; j<180; j++){       
      for (i = 120; i<150; i++){            //4-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
   for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =85; i<115; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 3-2
   }
}
    for (j = 20; j<180; j++){         
      for (i =85; i<115; i++){    
      bmpdata[(j*1000)+i] =0;     // 3-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 155; i<185; i++){            //5-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
      for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =120; i<150; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 4-2
   }
}
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
    for (j = 20; j<180; j++){         
      for (i =120; i<150; i++){    
      bmpdata[(j*1000)+i] =0;     // 4-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 190; i<220; i++){            //6-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
         for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =155; i<185; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 5-2
   }
}
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
    for (j = 20; j<180; j++){         
      for (i =155; i<185; i++){    
      bmpdata[(j*1000)+i] =0;     // 5-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 225; i<255; i++){            //7-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j = 20; j<180; j++){         
      for (i =15; i<45; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 1-3
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
         for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =190; i<220; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 6-2
   }
}
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
    for (j = 20; j<180; j++){         
      for (i =190; i<220; i++){    
      bmpdata[(j*1000)+i] =0;     // 6-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 260; i<290; i++){            //8-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j = 20; j<180; j++){         
      for (i =50; i<80; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 2-3
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =225; i<255; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 7-2
   }
}
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =225; i<255; i++){    
      bmpdata[(j*1000)+i] =0;     // 7-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 295; i<325; i++){            //9-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =15; i<45; i++){    
      bmpdata[(j*1000)+i] =0;     // 1-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =85; i<115; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 3-3
   }
}
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =260; i<290; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 8-2
   }
}
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =260; i<290; i++){    
      bmpdata[(j*1000)+i] =0;     // 8-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 330; i<360; i++){            //10-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =50; i<80; i++){    
      bmpdata[(j*1000)+i] =0;     // 2-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =120; i<150; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 4-3
   }
}
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =295; i<325; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 9-2
   }
}
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =295; i<325; i++){    
      bmpdata[(j*1000)+i] =0;     // 9-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 365; i<395; i++){            //11-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =85; i<115; i++){    
      bmpdata[(j*1000)+i] =0;     // 3-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =155; i<185; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 5-3
   }
}
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 15; i<45; i++){            //1-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){            //14-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =330; i<360; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 10-2
   }
}
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =330; i<360; i++){    
      bmpdata[(j*1000)+i] =0;     // 10-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 400; i<430; i++){            //12-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =120; i<150; i++){    
      bmpdata[(j*1000)+i] =0;     // 4-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =190; i<220; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 6-3
   }
}
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 15; i<45; i++){            //1-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){            //15-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =365; i<395; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 11-2
   }
}
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =365; i<395; i++){    
      bmpdata[(j*1000)+i] =0;     // 11-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 435; i<465; i++){            //13-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =155; i<185; i++){    
      bmpdata[(j*1000)+i] =0;     // 5-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =225; i<255; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 7-3
   }
}
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2 
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2 
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){            //14-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){            //16-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =400; i<430; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 12-2
   }
}
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =400; i<430; i++){    
      bmpdata[(j*1000)+i] =0;     // 12-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 470; i<500; i++){            //14-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =190; i<220; i++){    
      bmpdata[(j*1000)+i] =0;     // 6-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =260; i<290; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 8-3
   }
}
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
 for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){            //15-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){            //17-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =435; i<465; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 13-2
   }
}
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =435; i<465; i++){    
      bmpdata[(j*1000)+i] =0;     // 13-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 505; i<535; i++){            //15-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =225; i<255; i++){    
      bmpdata[(j*1000)+i] =0;     // 7-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =295; i<325; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 9-3
   }
}
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =15; i< 45; i++){          // 1-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){            //16-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){            //18-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =470; i<500; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 14-2
   }
}
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =470; i<500; i++){    
      bmpdata[(j*1000)+i] =0;     // 14-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 540; i<570; i++){            //16-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =260; i<290; i++){    
      bmpdata[(j*1000)+i] =0;     // 8-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =330; i<360; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 10-3
   }
}
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =50; i< 80; i++){          // 2-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){            //17-1
      bmpdata[(j*1000)+i] =0;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =505; i<535; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 15-2
   }
}
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =505; i<535; i++){    
      bmpdata[(j*1000)+i] =0;     // 15-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 575; i<605; i++){            //17-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =295; i<325; i++){    
      bmpdata[(j*1000)+i] =0;     // 9-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =365; i<395; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 11-3
   }
}
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =85; i< 115; i++){          // 3-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =15; i< 45; i++){          // 1-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){            //18-1
      bmpdata[(j*1000)+i] =0;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =540; i<570; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 16-2
   }
}
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =540; i<570; i++){    
      bmpdata[(j*1000)+i] =0;     // 16-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 610; i<640; i++){            //18-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =330; i<360; i++){    
      bmpdata[(j*1000)+i] =0;     // 10-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =400; i<430; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 12-3
   }
}
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =120; i< 150; i++){          // 4-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =50; i< 80; i++){          // 2-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
    for (j =  215; j<375; j++){         
      for (i =575; i<605; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 17-2
   }
}
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =575; i<605; i++){    
      bmpdata[(j*1000)+i] =0;     // 17-3
   }
}
   for (j = 20; j<180; j++){         
      for (i =365; i<395; i++){    
      bmpdata[(j*1000)+i] =0;     // 11-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =435; i<465; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 13-3
   }
}
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =155; i< 185; i++){          // 5-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =85; i< 115; i++){          // 3-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 15; i<45; i++){            //1-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
    for (j =  215; j<375; j++){         
      for (i =610; i<640; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 18-2
   }
}
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =610; i<640; i++){    
      bmpdata[(j*1000)+i] =0;     // 18-3
   }
}
   for (j = 20; j<180; j++){         
      for (i =400; i<430; i++){    
      bmpdata[(j*1000)+i] =0;     // 12-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =470; i<500; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 14-3
   }
}
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =190; i< 220; i++){          // 6-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =120; i< 150; i++){          // 4-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 15; i<45; i++){            //1-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){          // 16-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =435; i<475; i++){    
      bmpdata[(j*1000)+i] =0;     // 13-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =505; i<535; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 15-3
   }
}
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =225; i< 255; i++){          // 7-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =155; i< 185; i++){          // 5-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =15; i< 45; i++){          // 1-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =575; i< 605; i++){          // 17-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =470; i<500; i++){    
      bmpdata[(j*1000)+i] =0;     // 14-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =540; i<570; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 16-3
   }
}
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =260; i< 290; i++){          // 8-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =190; i< 220; i++){          // 6-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =50; i< 80; i++){          // 2-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =15; i< 45; i++){          // 1-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =610; i< 640; i++){          // 18-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =505; i<535; i++){    
      bmpdata[(j*1000)+i] =0;     // 15-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =575; i<605; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 17-3
   }
}
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =295; i< 325; i++){          // 9-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =225; i< 255; i++){          // 7-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =85; i< 115; i++){          // 3-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =50; i< 80; i++){          // 2-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 15; i<45; i++){            //1-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
   for (j = 20; j<180; j++){         
      for (i =540; i<570; i++){    
      bmpdata[(j*1000)+i] =0;     // 16-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =610; i<640; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 18-3
   }
}
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){          // 16-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){          // 16-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){            //14-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){            //14-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =330; i< 360; i++){          // 10-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =260; i< 290; i++){          // 8-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =120; i< 150; i++){          // 4-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =85; i< 115; i++){          // 3-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 15; i<45; i++){            //1-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 15; i<45; i++){            //1-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
   for (j = 20; j<180; j++){         
      for (i =575; i<605; i++){    
      bmpdata[(j*1000)+i] =0;     // 17-3
   }
}
for (j = 215; j<375; j++){         
      for (i =575; i< 605; i++){          // 17-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =575; i< 605; i++){          // 17-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){            //15-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){            //14-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){            //15-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =365; i< 395; i++){          // 11-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =295; i< 325; i++){          // 9-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =155; i< 185; i++){          // 5-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =120; i< 150; i++){          // 4-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 50; i<80; i++){            //2-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 15; i<45; i++){            //1-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 15; i<45; i++){            //1-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
   for (j = 20; j<180; j++){         
      for (i =610; i<640; i++){    
      bmpdata[(j*1000)+i] =0;     // 18-3
   }
}
for (j = 215; j<375; j++){         
      for (i =610; i< 640; i++){          // 18-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){          // 16-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =610; i< 640; i++){          // 18-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){            //16-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){            //15-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){            //16-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =400; i< 430; i++){          // 12-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =330; i< 360; i++){          // 10-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =190; i< 220; i++){          // 6-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =155; i< 185; i++){          // 5-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 85; i<115; i++){            //3-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 50; i<80; i++){            //2-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 15; i<45; i++){            //1-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =575; i< 605; i++){          // 17-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){            //17-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){            //16-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){            //17-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =435; i< 465; i++){          // 13-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =365; i< 395; i++){          // 11-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =225; i< 255; i++){          // 7-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =190; i< 220; i++){          // 6-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 120; i<150; i++){            //4-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 85; i<115; i++){            //3-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =610; i< 640; i++){          // 18-2
      bmpdata[(j*1000)+i] =0;    
   }
} 
            for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){            //18-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){            //17-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){            //18-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){          // 16-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =470; i< 500; i++){          // 14-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =400; i< 430; i++){          // 12-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =260; i< 290; i++){          // 8-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =225; i< 255; i++){          // 7-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 155; i<185; i++){            //5-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 120; i<150; i++){            //4-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){            //18-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){         
      for (i =575; i< 605; i++){          // 17-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =505; i< 535; i++){          // 15-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =435; i< 465; i++){          // 13-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =295; i< 325; i++){          // 9-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =260; i< 290; i++){          // 8-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 190; i<220; i++){            //6-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 155; i<185; i++){            //5-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =610; i< 640; i++){          // 18-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){          // 16-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =540; i< 570; i++){          // 16-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =470; i< 500; i++){          // 14-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =330; i< 360; i++){          // 10-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =295; i< 325; i++){          // 9-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 225; i<255; i++){            //7-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 190; i<220; i++){            //6-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =575; i< 605; i++){          // 17-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =575; i< 605; i++){          // 17-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =505; i< 535; i++){          // 15-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =365; i< 395; i++){          // 11-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =330; i< 360; i++){          // 10-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 260; i<290; i++){           //8-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 225; i<255; i++){            //7-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =610; i< 640; i++){          // 18-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =610; i< 640; i++){            // 18-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =540; i< 570; i++){          // 16-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){         // 16-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =470; i< 500; i++){          //14-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =400; i< 430; i++){          // 12-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =365; i< 395; i++){          // 11-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =330; i< 360; i++){            //10-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 295; i<325; i++){           //9-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 260; i<290; i++){            //8-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){           //8-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 20; j<180; j++){         
      for (i =575; i< 605; i++){          // 17-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
     for (i =575; i< 605; i++){       // 17-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =505; i< 535; i++){          //15-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){            //14-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =435; i< 465; i++){          // 13-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =400; i< 430; i++){          // 12-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){           // 12-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =365; i< 395; i++){            //11-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 330; i<360; i++){          //10-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 295; i<325; i++){            //9-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
     for (i = 295; i<325; i++){           //9-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 20; j<180; j++){         
      for (i =610; i< 640; i++){          // 18-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
     for (i =610; i< 640; i++){       // 18-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){          // 16-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
     for (i =540; i< 570; i++){         //16-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){            //15-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =470; i< 500; i++){          // 14-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =435; i< 465; i++){          // 13-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){            // 13-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =400; i< 430; i++){            //12-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 365; i<395; i++){           //11-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 330; i<360; i++){            //10-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
     for (i = 330; i<360; i++){          //10-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =575; i< 605; i++){          // 17-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
     for (i =575; i< 605; i++){       //17-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){            //16-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =505; i< 535; i++){          // 15-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =470; i< 500; i++){          // 14-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
       for (i =470; i< 500; i++){           // 14-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =435; i< 465; i++){             //13-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 400; i<430; i++){           //12-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 365; i<395; i++){            //11-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
     for (i = 365; i<395; i++){           //11-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =610; i< 640; i++){          // 18-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
     for (i =610; i< 640; i++){       //18-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){            //17-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =540; i< 570; i++){          // 16-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =505; i< 535; i++){          // 15-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
       for (i =505; i< 535; i++){           // 15-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =470; i< 500; i++){             //14-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 435; i<465; i++){           //13-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 400; i<430; i++){            //12-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
     for (i = 400; i<430; i++){           //12-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){            //18-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =575; i< 605; i++){          // 17-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =540; i< 570; i++){          // 16-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
       for (i =540; i< 570; i++){           // 16-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =505; i< 535; i++){             //15-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){            //14-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 470; i<500; i++){          //14-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 435; i<465; i++){            //13-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){          //13-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 20; j<180; j++){         
      for (i =610; i< 640; i++){          // 18-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =575; i< 605; i++){          // 17-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
       for (i =575; i< 605; i++){           // 17-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){          // 16-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =540; i< 570; i++){              //16-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){            //15-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
       for (i = 505; i<535; i++){           //15-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 470; i<500; i++){            //14-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){          //14-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 20; j<180; j++){         
      for (i =610; i< 640; i++){          // 18-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
       for (i =610; i< 640; i++){           // 18-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =575; i< 605; i++){          // 17-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =575; i< 605; i++){              //17-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){            //16-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
       for (i = 540; i<570; i++){           //16-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 505; i<535; i++){            //15-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){          //15-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){            //14-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =610; i< 640; i++){          // 18-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =610; i< 640; i++){              //18-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){            //17-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
       for (i = 575; i<605; i++){           //17-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 540; i<570; i++){            //16-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){          //16-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){            //15-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){            //18-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
       for (i = 610; i<640; i++){           //18-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 575; i<605; i++){            //17-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){          //17-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){            //16-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 215; j<375; j++){       
      for (i = 610; i<640; i++){            //18-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){          //18-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){            //17-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(400*1000);
	   break;
   }
for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){            //18-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(3000*1000);
	   break;
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // fb 검은색으로 지움.
   for(coor_y = 0; coor_y < screen_height; coor_y++) 
   {
      ptr = (unsigned long *)pfbmap + (screen_width * coor_y);
      for(coor_x = 0; coor_x < screen_width; coor_x++)
         {
            *ptr++ = 0x000000;
         }
   }
   
  for (j = 415; j<575; j++){         
      for (i =15; i<45; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 1-1
   }
}
  for (j = 300; j<330; j++){         
      for (i =850; i<880; i++){    //////////////////////////////////////////////////////////////
      bmpdata[(j*1000)+i] =50000;     // 1-1
   }
}
 for (j = 260; j<290; j++){         
      for (i =850; i<880; i++){    //////////////////////////////////////////////////////////////
      bmpdata[(j*1000)+i] =50000;     // 1-1
   }
}
   for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }

for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
    for (j = 415; j<575; j++){         
      for (i =15; i<45; i++){    
      bmpdata[(j*1000)+i] =0;     // 1-1
   }
}
for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j = 20; j<180; j++){         
      for (i =15; i<45; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 1-3
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
   for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j = 20; j<180; j++){         
      for (i =50; i<80; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 2-3
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
     for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j = 20; j<180; j++){         
      for (i =15; i<45; i++){    
      bmpdata[(j*1000)+i] =0;     // 1-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =85; i<115; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 3-3
   }
}
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
     for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
    for (j = 20; j<180; j++){         
      for (i =50; i<80; i++){    
      bmpdata[(j*1000)+i] =0;     // 2-3
   }
}
    for (j = 20; j<180; j++){       
      for (i = 120; i<150; i++){            //4-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
   for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =85; i<115; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 3-2
   }
}
    for (j = 20; j<180; j++){         
      for (i =85; i<115; i++){    
      bmpdata[(j*1000)+i] =0;     // 3-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 155; i<185; i++){            //5-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
      for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =120; i<150; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 4-2
   }
}
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
    for (j = 20; j<180; j++){         
      for (i =120; i<150; i++){    
      bmpdata[(j*1000)+i] =0;     // 4-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 190; i<220; i++){            //6-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
         for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =155; i<185; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 5-2
   }
}
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
    for (j = 20; j<180; j++){         
      for (i =155; i<185; i++){    
      bmpdata[(j*1000)+i] =0;     // 5-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 225; i<255; i++){            //7-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j = 20; j<180; j++){         
      for (i =15; i<45; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 1-3
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
         for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =190; i<220; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 6-2
   }
}
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
    for (j = 20; j<180; j++){         
      for (i =190; i<220; i++){    
      bmpdata[(j*1000)+i] =0;     // 6-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 260; i<290; i++){            //8-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j = 20; j<180; j++){         
      for (i =50; i<80; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 2-3
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =225; i<255; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 7-2
   }
}
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =225; i<255; i++){    
      bmpdata[(j*1000)+i] =0;     // 7-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 295; i<325; i++){            //9-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =15; i<45; i++){    
      bmpdata[(j*1000)+i] =0;     // 1-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =85; i<115; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 3-3
   }
}
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =260; i<290; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 8-2
   }
}
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =260; i<290; i++){    
      bmpdata[(j*1000)+i] =0;     // 8-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 330; i<360; i++){            //10-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =50; i<80; i++){    
      bmpdata[(j*1000)+i] =0;     // 2-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =120; i<150; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 4-3
   }
}
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =295; i<325; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 9-2
   }
}
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =295; i<325; i++){    
      bmpdata[(j*1000)+i] =0;     // 9-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 365; i<395; i++){            //11-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =85; i<115; i++){    
      bmpdata[(j*1000)+i] =0;     // 3-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =155; i<185; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 5-3
   }
}
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 15; i<45; i++){            //1-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){            //14-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =330; i<360; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 10-2
   }
}
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =330; i<360; i++){    
      bmpdata[(j*1000)+i] =0;     // 10-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 400; i<430; i++){            //12-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =120; i<150; i++){    
      bmpdata[(j*1000)+i] =0;     // 4-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =190; i<220; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 6-3
   }
}
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 15; i<45; i++){            //1-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){            //15-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =365; i<395; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 11-2
   }
}
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =365; i<395; i++){    
      bmpdata[(j*1000)+i] =0;     // 11-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 435; i<465; i++){            //13-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =155; i<185; i++){    
      bmpdata[(j*1000)+i] =0;     // 5-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =225; i<255; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 7-3
   }
}
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2 
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2 
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
  for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){            //14-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){            //16-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =400; i<430; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 12-2
   }
}
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =400; i<430; i++){    
      bmpdata[(j*1000)+i] =0;     // 12-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 470; i<500; i++){            //14-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =190; i<220; i++){    
      bmpdata[(j*1000)+i] =0;     // 6-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =260; i<290; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 8-3
   }
}
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
 for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){            //15-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){            //17-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =435; i<465; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 13-2
   }
}
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =435; i<465; i++){    
      bmpdata[(j*1000)+i] =0;     // 13-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 505; i<535; i++){            //15-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =225; i<255; i++){    
      bmpdata[(j*1000)+i] =0;     // 7-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =295; i<325; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 9-3
   }
}
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =15; i< 45; i++){          // 1-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){            //16-1
      bmpdata[(j*1000)+i] =0;    
   }
}
  for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){            //18-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =470; i<500; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 14-2
   }
}
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =470; i<500; i++){    
      bmpdata[(j*1000)+i] =0;     // 14-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 540; i<570; i++){            //16-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =260; i<290; i++){    
      bmpdata[(j*1000)+i] =0;     // 8-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =330; i<360; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 10-3
   }
}
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =50; i< 80; i++){          // 2-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){            //17-1
      bmpdata[(j*1000)+i] =0;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =505; i<535; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 15-2
   }
}
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =505; i<535; i++){    
      bmpdata[(j*1000)+i] =0;     // 15-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 575; i<605; i++){            //17-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =295; i<325; i++){    
      bmpdata[(j*1000)+i] =0;     // 9-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =365; i<395; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 11-3
   }
}
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =85; i< 115; i++){          // 3-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =15; i< 45; i++){          // 1-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){            //18-1
      bmpdata[(j*1000)+i] =0;    
   }
}
    for (j =  215; j<375; j++){         
      for (i =540; i<570; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 16-2
   }
}
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =540; i<570; i++){    
      bmpdata[(j*1000)+i] =0;     // 16-3
   }
}
  for (j = 20; j<180; j++){       
      for (i = 610; i<640; i++){            //18-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}
   for (j = 20; j<180; j++){         
      for (i =330; i<360; i++){    
      bmpdata[(j*1000)+i] =0;     // 10-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =400; i<430; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 12-3
   }
}
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =120; i< 150; i++){          // 4-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =50; i< 80; i++){          // 2-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
    for (j =  215; j<375; j++){         
      for (i =575; i<605; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 17-2
   }
}
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =575; i<605; i++){    
      bmpdata[(j*1000)+i] =0;     // 17-3
   }
}
   for (j = 20; j<180; j++){         
      for (i =365; i<395; i++){    
      bmpdata[(j*1000)+i] =0;     // 11-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =435; i<465; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 13-3
   }
}
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =155; i< 185; i++){          // 5-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =85; i< 115; i++){          // 3-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 15; i<45; i++){            //1-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
    for (j =  215; j<375; j++){         
      for (i =610; i<640; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 18-2
   }
}
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =610; i<640; i++){    
      bmpdata[(j*1000)+i] =0;     // 18-3
   }
}
   for (j = 20; j<180; j++){         
      for (i =400; i<430; i++){    
      bmpdata[(j*1000)+i] =0;     // 12-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =470; i<500; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 14-3
   }
}
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =190; i< 220; i++){          // 6-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =120; i< 150; i++){          // 4-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 15; i<45; i++){            //1-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){          // 16-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =435; i<475; i++){    
      bmpdata[(j*1000)+i] =0;     // 13-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =505; i<535; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 15-3
   }
}
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =225; i< 255; i++){          // 7-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =155; i< 185; i++){          // 5-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =15; i< 45; i++){          // 1-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =575; i< 605; i++){          // 17-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =470; i<500; i++){    
      bmpdata[(j*1000)+i] =0;     // 14-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =540; i<570; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 16-3
   }
}
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =260; i< 290; i++){          // 8-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =190; i< 220; i++){          // 6-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =50; i< 80; i++){          // 2-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =15; i< 45; i++){          // 1-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =610; i< 640; i++){          // 18-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
   for (j = 20; j<180; j++){         
      for (i =505; i<535; i++){    
      bmpdata[(j*1000)+i] =0;     // 15-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =575; i<605; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 17-3
   }
}
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =295; i< 325; i++){          // 9-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =225; i< 255; i++){          // 7-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =85; i< 115; i++){          // 3-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =50; i< 80; i++){          // 2-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =15; i< 45; i++){          // 1-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 15; i<45; i++){            //1-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
   for (j = 20; j<180; j++){         
      for (i =540; i<570; i++){    
      bmpdata[(j*1000)+i] =0;     // 16-3
   }
}
    for (j = 20; j<180; j++){         
      for (i =610; i<640; i++){    
      bmpdata[(j*1000)+i] =16776961;     // 18-3
   }
}
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){          // 16-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){          // 16-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){            //14-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){            //14-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =330; i< 360; i++){          // 10-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =260; i< 290; i++){          // 8-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =120; i< 150; i++){          // 4-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =85; i< 115; i++){          // 3-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =50; i< 80; i++){          // 2-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 15; i<45; i++){            //1-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 15; i<45; i++){            //1-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
   for (j = 20; j<180; j++){         
      for (i =575; i<605; i++){    
      bmpdata[(j*1000)+i] =0;     // 17-3
   }
}
for (j = 215; j<375; j++){         
      for (i =575; i< 605; i++){          // 17-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =575; i< 605; i++){          // 17-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){            //15-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){            //14-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){            //15-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =365; i< 395; i++){          // 11-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =295; i< 325; i++){          // 9-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =155; i< 185; i++){          // 5-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =120; i< 150; i++){          // 4-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =85; i< 115; i++){          // 3-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 50; i<80; i++){            //2-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 15; i<45; i++){            //1-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 15; i<45; i++){            //1-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
   for (j = 20; j<180; j++){         
      for (i =610; i<640; i++){    
      bmpdata[(j*1000)+i] =0;     // 18-3
   }
}
for (j = 215; j<375; j++){         
      for (i =610; i< 640; i++){          // 18-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){          // 16-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =610; i< 640; i++){          // 18-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){            //16-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){            //15-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){            //16-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =400; i< 430; i++){          // 12-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =330; i< 360; i++){          // 10-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =190; i< 220; i++){          // 6-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =155; i< 185; i++){          // 5-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =120; i< 150; i++){          // 4-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 85; i<115; i++){            //3-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 50; i<80; i++){            //2-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 15; i<45; i++){            //1-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =575; i< 605; i++){          // 17-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
            for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){            //17-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){            //16-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){            //17-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =435; i< 465; i++){          // 13-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =365; i< 395; i++){          // 11-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =225; i< 255; i++){          // 7-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =190; i< 220; i++){          // 6-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =155; i< 185; i++){          // 5-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 120; i<150; i++){            //4-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 85; i<115; i++){            //3-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 50; i<80; i++){            //2-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =610; i< 640; i++){          // 18-2
      bmpdata[(j*1000)+i] =0;    
   }
} 
            for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){            //18-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
            for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){            //17-1
      bmpdata[(j*1000)+i] =0;    
   }
}
           for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){            //18-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){          // 16-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =470; i< 500; i++){          // 14-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =400; i< 430; i++){          // 12-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =260; i< 290; i++){          // 8-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =225; i< 255; i++){          // 7-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =190; i< 220; i++){          // 6-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 155; i<185; i++){            //5-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 120; i<150; i++){            //4-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 85; i<115; i++){            //3-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
            for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){            //18-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){         
      for (i =575; i< 605; i++){          // 17-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =505; i< 535; i++){          // 15-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =435; i< 465; i++){          // 13-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =295; i< 325; i++){          // 9-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =260; i< 290; i++){          // 8-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =225; i< 255; i++){          // 7-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 190; i<220; i++){            //6-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 155; i<185; i++){            //5-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 120; i<150; i++){            //4-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =610; i< 640; i++){          // 18-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){          // 16-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =540; i< 570; i++){          // 16-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =470; i< 500; i++){          // 14-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =330; i< 360; i++){          // 10-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =295; i< 325; i++){          // 9-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =260; i< 290; i++){          // 8-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 225; i<255; i++){            //7-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 190; i<220; i++){            //6-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 155; i<185; i++){            //5-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =575; i< 605; i++){          // 17-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =575; i< 605; i++){          // 17-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =505; i< 535; i++){          // 15-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =365; i< 395; i++){          // 11-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =330; i< 360; i++){          // 10-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =295; i< 325; i++){          // 9-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 260; i<290; i++){           //8-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 225; i<255; i++){            //7-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 190; i<220; i++){            //6-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =610; i< 640; i++){          // 18-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =610; i< 640; i++){            // 18-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =540; i< 570; i++){          // 16-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){         // 16-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =470; i< 500; i++){          //14-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =400; i< 430; i++){          // 12-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =365; i< 395; i++){          // 11-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =330; i< 360; i++){          // 10-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =330; i< 360; i++){            //10-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 295; i<325; i++){           //9-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 260; i<290; i++){            //8-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){           //8-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 225; i<255; i++){            //7-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 20; j<180; j++){         
      for (i =575; i< 605; i++){          // 17-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
     for (i =575; i< 605; i++){       // 17-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =505; i< 535; i++){          //15-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){            //14-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =435; i< 465; i++){          // 13-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =400; i< 430; i++){          // 12-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){           // 12-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =365; i< 395; i++){          // 11-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =365; i< 395; i++){            //11-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 330; i<360; i++){          //10-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 295; i<325; i++){            //9-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
     for (i = 295; i<325; i++){           //9-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 260; i<290; i++){            //8-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 20; j<180; j++){         
      for (i =610; i< 640; i++){          // 18-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
     for (i =610; i< 640; i++){       // 18-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){          // 16-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
     for (i =540; i< 570; i++){         //16-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){            //15-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =470; i< 500; i++){          // 14-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =435; i< 465; i++){          // 13-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){            // 13-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =400; i< 430; i++){          // 12-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =400; i< 430; i++){            //12-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 365; i<395; i++){           //11-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 330; i<360; i++){            //10-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
     for (i = 330; i<360; i++){          //10-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 295; i<325; i++){            //9-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =575; i< 605; i++){          // 17-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
     for (i =575; i< 605; i++){       //17-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){            //16-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =505; i< 535; i++){          // 15-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =470; i< 500; i++){          // 14-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
       for (i =470; i< 500; i++){           // 14-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =435; i< 465; i++){          // 13-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =435; i< 465; i++){             //13-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 400; i<430; i++){           //12-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 365; i<395; i++){            //11-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
     for (i = 365; i<395; i++){           //11-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 330; i<360; i++){            //10-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =610; i< 640; i++){          // 18-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
     for (i =610; i< 640; i++){       //18-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){            //17-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =540; i< 570; i++){          // 16-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =505; i< 535; i++){          // 15-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
       for (i =505; i< 535; i++){           // 15-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =470; i< 500; i++){          // 14-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =470; i< 500; i++){             //14-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 435; i<465; i++){           //13-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 400; i<430; i++){            //12-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
     for (i = 400; i<430; i++){           //12-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 365; i<395; i++){            //11-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){            //18-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 20; j<180; j++){         
      for (i =575; i< 605; i++){          // 17-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =540; i< 570; i++){          // 16-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
       for (i =540; i< 570; i++){           // 16-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =505; i< 535; i++){          // 15-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =505; i< 535; i++){             //15-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){            //14-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 470; i<500; i++){          //14-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 435; i<465; i++){            //13-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){          //13-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 400; i<430; i++){            //12-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 20; j<180; j++){         
      for (i =610; i< 640; i++){          // 18-3
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 20; j<180; j++){         
      for (i =575; i< 605; i++){          // 17-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
       for (i =575; i< 605; i++){           // 17-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =540; i< 570; i++){          // 16-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =540; i< 570; i++){              //16-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){            //15-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
       for (i = 505; i<535; i++){           //15-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 470; i<500; i++){            //14-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){          //14-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 435; i<465; i++){            //13-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 20; j<180; j++){         
      for (i =610; i< 640; i++){          // 18-3
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 215; j<375; j++){         
       for (i =610; i< 640; i++){           // 18-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}  
for (j = 215; j<375; j++){         
      for (i =575; i< 605; i++){          // 17-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =575; i< 605; i++){              //17-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){            //16-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
       for (i = 540; i<570; i++){           //16-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 505; i<535; i++){            //15-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){          //15-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 470; i<500; i++){            //14-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 215; j<375; j++){         
      for (i =610; i< 640; i++){          // 18-2
      bmpdata[(j*1000)+i] =0;    
   }
}  
for (j = 415; j<575; j++){       
      for (i =610; i< 640; i++){              //18-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){            //17-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
       for (i = 575; i<605; i++){           //17-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 540; i<570; i++){            //16-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){          //16-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 505; i<535; i++){            //15-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){            //18-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 215; j<375; j++){       
       for (i = 610; i<640; i++){           //18-2
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 215; j<375; j++){       
      for (i = 575; i<605; i++){            //17-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){          //17-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 540; i<570; i++){            //16-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 215; j<375; j++){       
      for (i = 610; i<640; i++){            //18-2
      bmpdata[(j*1000)+i] =0;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){          //18-1
      bmpdata[(j*1000)+i] =16776961;    
   }
}
for (j = 415; j<575; j++){       
      for (i = 575; i<605; i++){            //17-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(200*1000);;
	   break;
   }
for (j = 415; j<575; j++){       
      for (i = 610; i<640; i++){            //18-1
      bmpdata[(j*1000)+i] =0;    
   }
}
for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
   while(1) {
	   usleep(3000*1000);
	   break;
   }
   

   // direction for image generating : (0,0)-> (1,0)-> (2,0)-> ...-> (x , y)
   for(coor_y = 0; coor_y < rows; coor_y++) {
   ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
   for (coor_x = 0; coor_x < cols; coor_x++) {
   *ptr++ = bmpdata[coor_x + coor_y*cols];
   }
   }
  
   munmap( pfbmap, mem_size);
   close( fbfd);
   return 0;
}
