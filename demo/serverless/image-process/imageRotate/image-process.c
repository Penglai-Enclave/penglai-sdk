/*
 * Programming introduction with the SOD Embedded Image Processing API.
 * Copyright (C) PixLab | Symisc Systems, https://sod.pixlab.io
 */
/*
* Compile this file together with the SOD embedded source code to generate
* the executable. For example:
*
*  gcc sod.c resize_image.c -lm -Ofast -march=native -Wall -std=c99 -o sod_img_proc
*  
* Under Microsoft Visual Studio (>= 2015), just drop `sod.c` and its accompanying
* header files on your source tree and you're done. If you have any trouble
* integrating SOD in your project, please submit a support request at:
* https://sod.pixlab.io/support.html
*/
/*
* This simple program is a quick introduction on how to embed and start
* experimenting with SOD without having to do a lot of tedious
* reading and configuration.
*
* Make sure you have the latest release of SOD from:
*  https://pixlab.io/downloads
* The SOD Embedded C/C++ documentation is available at:
*  https://sod.pixlab.io/api.html
*/
#include <stdio.h>
#include <eapp.h>
#include <print.h>
#include "sod.h"
#include <string.h>
void test_read_cycles(void)
{
	unsigned long cycles[10], pre_cycles, post_cycles;
	int i = 0;
	eapp_print("%s test begin\n", __func__);
	for (i=0; i<10; i++)
		cycles[i] = 0;
	for (i=0;i<20;i++){
		asm volatile(
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a0\n\t" //real-begin
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "rdcycle a1\n\t"
			     "mv %0,a0\n\t"
			     "mv %1,a1\n\t"
				:"=r"(pre_cycles), "=r"(post_cycles));
		if (i>=10)
			cycles[i-10]= (post_cycles-pre_cycles)/50;
	}
	for (i=0;i<10; i++)
		eapp_print("%s test rdcycle:  cylces[i]= %ld cycles\n", __func__, cycles[i]);
	return;
}
/*
* Resize an image (Minify) to half its original size.
*/
int sod_resize_main(unsigned long arg0, void* vaddr, unsigned long size)
{
	eapp_print("%s is running, 233\n", "Image-processing");

	test_read_cycles();

#if 0
	eapp_print("[@%s] test calloc begin\n", __func__);
	void* test_ptr = calloc(64*64*3, sizeof(float));
	eapp_print("[@%s] test calloc end, test_ptr:%p\n", __func__, test_ptr);
#endif

#if 0
	sod_img imgIn = sod_make_image(64,64,3);
#endif
	sod_img imgIn = *((sod_img*)vaddr);
	imgIn.data =  vaddr + sizeof(sod_img)*2;
	eapp_print("[@%s] recover imgIn from caller imgIn.w(%d), imgIn.h(%d), imgIn.c(%d)\n", __func__,
			imgIn.w, imgIn.h, imgIn.c);
	if (imgIn.data == 0) {
		/* Invalid path, unsupported format, memory failure, etc. */
		return 0;
	}
	/* Resize to half its original size  */
	int newWidth  = imgIn.w / 2;
	int newHeight = imgIn.h / 2;
	
	eapp_print("[@%s] begin resize img\n", __func__);
	sod_img rz = sod_resize_image(imgIn, newWidth, newHeight);
	eapp_print("[@%s] after resize img\n", __func__);
	eapp_print("[@%s] begin rotate img\n", __func__);
	sod_img rot = sod_rotate_image(rz, 180.0);
	eapp_print("[@%s] after rotate img\n", __func__);
	/* Save the result image to the specified path */
	//sod_img_save_as_png(rot, zOut);
	//eapp_print("[@%s] after save img\n", __func__);
	/* Cleanup */
	//sod_free_image(imgIn);
	sod_free_image(rz);
	sod_free_image(rot);
	eapp_print("[@%s] Done (End)\n", __func__);
	return 0;
}

int EAPP_ENTRY main(){
  unsigned long* args;
  EAPP_RESERVE_REG;
  unsigned long arg0 = args[10];
  void* vaddr = (void*)args[11];
  unsigned long size = args[12];
  eapp_print("server1 begin to run\n");


  sod_resize_main(arg0, vaddr, size);

  char server_name[20];
  strcpy(server_name, "test-server");
  unsigned long server_handle = acquire_enclave(server_name);
  struct call_enclave_arg_t call_arg;
  eapp_print("server_handle:0x%lx\n", server_handle);
  if(server_handle == -1UL)
  {
    call_arg.resp_val = -1;
    call_arg.req_vaddr = 0;
    call_arg.resp_vaddr = 0;
    SERVER_RETURN(&call_arg);
  }

  call_arg.req_arg = arg0;
  call_arg.req_vaddr = vaddr;
  call_arg.req_size = size;
  call_enclave(server_handle, &call_arg);
 // eapp_print("server1: read req_vaddr:0x%lx\n", sum);

#if 0
  if(call_arg.resp_vaddr)
  {
    int *nums = (int*)call_arg.resp_vaddr;
    size = call_arg.resp_size;
    sum = arg0;
    for(int i=0; i<size/sizeof(int); ++i)
    {
      sum += nums[i];
    }
    eapp_print("server1: read resp_vaddr:0x%lx\n", sum);
  }
#endif

  call_arg.resp_val = 3;
  eapp_print("server1 is exiting with retval:0x%lx\n", 3);
  SERVER_RETURN(&call_arg);
}
