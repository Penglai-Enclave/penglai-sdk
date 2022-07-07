/*
 * Entry of Image Processing Serverless Application, for Penglai-Enclave
 * */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include "sod.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> 

//#define EAPP_DEBUG_PRINT
#define SHARED_MAGIC 0xCAFEBABE
#define UNIX_DOMAIN_SELF "/tmp/entry_function"
#define UNIX_DOMAIN_TARGET "/tmp/resize_function"

#ifdef EAPP_DEBUG_PRINT
#define debug_printf(...) printf(__VA_ARGS__)
#else
#define debug_printf(...) 
#endif
unsigned long readcycle(void){
	unsigned long ret;
	asm volatile(
		     "rdcycle a1\n\t"
		     "mv %0,a1\n\t"
	:"=r"(ret));
	return ret;
}

int main(){
  int* shm;
  unsigned long shm_size;

  debug_printf("[Serverless] Entry_enclave begin to run\n");
  /* Wait for invoked */
  socklen_t clt_addr_len;
  int listen_fd;
  int com_fd;
  int ret;
  int i;
  static char recv_buf[256];
  int len;
  struct sockaddr_un clt_addr;
  struct sockaddr_un srv_addr;
  listen_fd = socket(PF_UNIX, SOCK_STREAM, 0);
  if (listen_fd <0){
  	printf("Error@%s, socket error\n", __func__);
	exit(-1);
  }

  //set sever addr_param
  srv_addr.sun_family=AF_UNIX;
  strncpy(srv_addr.sun_path, UNIX_DOMAIN_SELF, sizeof(srv_addr.sun_path)-1);
  unlink(UNIX_DOMAIN_SELF);
  ret=bind(listen_fd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));
  if(ret==-1){
  	printf("Error@%s, bind error\n", __func__);
	exit(-1);
  }
  ret=listen(listen_fd,1);
  if(ret==-1){
  	printf("Error@%s, listen error\n", __func__);
	exit(-1);
  }
  len=sizeof(clt_addr);
  com_fd=accept(listen_fd,(struct sockaddr*)&clt_addr,&len);
  if(com_fd<0){
  	printf("Error@%s, accept error\n", __func__);
	exit(-1);
  }
  memset(recv_buf,0,256);
  int num=read(com_fd,recv_buf,sizeof(recv_buf));
  debug_printf("Entry: Message from client (%d)) :%s\n",num,recv_buf); 
  int img_size = atoi(recv_buf);

  //Now we ready to invoke the next function
  int connect_fd;
  char snd_buf[256];
  struct sockaddr_un srv_addr2;
  connect_fd = socket(PF_UNIX, SOCK_STREAM, 0);
  if (connect_fd<0){
  	printf("Error@%s, socket error\n", __func__);
	exit(-1);
  }
  srv_addr2.sun_family = AF_UNIX;
  strcpy(srv_addr2.sun_path, UNIX_DOMAIN_TARGET);
  memset(snd_buf, 0, 256);

  ret = connect(connect_fd, (struct sockaddr*)&srv_addr2, sizeof(srv_addr2));
  if (ret == -1){
  	printf("Error@%s, connect error\n", __func__);
	exit(-1);
  }
  sprintf(snd_buf, "%d\0", img_size);
  //strcpy(snd_buf, "Go! Ultraman!"); 
  //strcpy(snd_buf, itoa(img_size)); 
  write(connect_fd, snd_buf, sizeof(snd_buf));


  /*Now wait for reply*/
  read(connect_fd, recv_buf, sizeof(recv_buf));
  debug_printf("Entry: Reply from server (%d)) :%s\n",num,recv_buf); 

  close(connect_fd);


  /*Reply to client*/
  strcpy(snd_buf, "Go! Ultraman!"); 
  write(com_fd, snd_buf, sizeof(snd_buf));

  close(com_fd);
  close(listen_fd);
  unlink(UNIX_DOMAIN_SELF);

  return 0;
}
