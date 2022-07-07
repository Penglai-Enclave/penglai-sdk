#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "sod.h"
#include <errno.h>

//#define EAPP_DEBUG_PRINT
#define SHARED_MAGIC 0xCAFEBABE
#define UNIX_DOMAIN "/tmp/entry_function"

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

unsigned long long readtime(void){
	unsigned long long ret;
	asm volatile(
		     "rdtime a1\n\t"
		     "mv %0,a1\n\t"
	:"=r"(ret));
	return ret;
}

int main(int argc, char** argv)
{
  int img_size;

  unsigned long start_cycle, mid_cycle, end_cycle;
  unsigned long long start_time, end_time;

  debug_printf("Start init\n");

  start_cycle = readcycle();

  img_size = atoi(argv[1]);
  if (img_size<32 || img_size>4096){
	printf("[Image Processing Serverless] Error img size:%d\n", img_size);
	return -1;
  }

  unsigned long shm_size = img_size * img_size * 3 * sizeof(float) + 0x1000;  //0x1000 * 8 * 4;
  int shmid = shmget((key_t)SHARED_MAGIC, shm_size, 0666|IPC_CREAT);
  if (shmid==-1){
  	printf("Error@%s, shmget error:%s\n", __func__,strerror(errno));
	exit(-1);
  }
  debug_printf("After shmget");
  void * shm = shmat(shmid, (void*)0, 0);
  if (shmat==(void*)-1){
  	printf("Error@%s, shmat error\n", __func__);
	exit(-1);
  }
  debug_printf("After shmat");
  memset(shm, 0, shm_size);
  sod_img imgIn = sod_make_image_inplace(img_size,img_size, 3, shm);

  /* Ready to invoke the first serverless functions */
  int ret, num;
  int connect_fd;
  char snd_buf[256];
  char recv_buf[256];
  struct sockaddr_un srv_addr;
  connect_fd = socket(PF_UNIX, SOCK_STREAM, 0);
  if (connect_fd<0){
  	printf("Error@%s, socket error\n", __func__);
	exit(-1);
  }
  srv_addr.sun_family = AF_UNIX;
  strcpy(srv_addr.sun_path, UNIX_DOMAIN);
  memset(snd_buf, 0, 256);
  memset(recv_buf, 0, 256);
  /* Connect the first serverless function */
  mid_cycle = readcycle();
  start_time = readtime();
  ret = connect(connect_fd, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
  if (ret == -1){
  	printf("Error@%s, connect error\n", __func__);
	exit(-1);
  }
  sprintf(snd_buf, "%d\0", img_size);
  //strcpy(snd_buf, "Go! Ultraman!");
  //strcpy(snd_buf, itoa(img_size));
  write(connect_fd, snd_buf, sizeof(snd_buf));

  /*Now wait for reply*/
  num = read(connect_fd, recv_buf, sizeof(recv_buf));
  debug_printf("Host: Reply from server (%d)) :%s\n",num,recv_buf);

  close(connect_fd);
  end_cycle = readcycle();
  end_time = readtime();

  if (shmdt(shm)<0){
  	printf("Error@%s, shmdt error\n", __func__);
  }
  if (shmctl(shmid, IPC_RMID, 0)<0){
  	printf("Error@%s, shmctl RMID error\n", __func__);
  }

//  printf("[Serverless Test] Image Size: %dx%d (x3)\n\trun time:%ld cycles\n",
//		  img_size, img_size, end_cycle-mid_cycle);
  printf("[Serverless Test] Image Size: %dx%d (x3)\n\trun time:%llu cycles\n",
		  img_size, img_size, end_time - start_time);


  return 0;
}
