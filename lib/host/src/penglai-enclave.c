#include "penglai-enclave.h"

void PLenclave_init(struct PLenclave *PLenclave)
{
  PLenclave->elffile = NULL;
  PLenclave->eid = -1;
  PLenclave->fd = open(PENGLAI_ENCLAVE_DEV_PATH,O_RDWR);
  if(PLenclave->fd < 0)
  {
    fprintf(stderr,"LIB: cannot open enclave dev\n");
  }
}

void PLenclave_finalize(struct PLenclave *PLenclave)
{
  if(PLenclave->fd >= 0)
    close(PLenclave->fd);
}

int PLenclave_create(struct PLenclave* PLenclave, struct elf_args* u_elffile, struct enclave_args* u_param)
{
  int ret = 0;
  if(!u_elffile)
  {
    fprintf(stderr,"LIB: elffile is not existed\n");
    return -1;
  }

  PLenclave->elffile = u_elffile;

  PLenclave->user_param.elf_ptr = (unsigned long)u_elffile->ptr;
  PLenclave->user_param.elf_size = u_elffile->size;
  PLenclave->user_param.stack_size = u_param->stack_size;
  PLenclave->user_param.untrusted_mem_ptr = u_param->untrusted_mem_ptr;
  PLenclave->user_param.untrusted_mem_size = u_param->untrusted_mem_size;
  PLenclave->user_param.privilege = u_param->privilege;
  PLenclave->user_param.ocall_buf_size = 0;
  PLenclave->user_param.resume_type = 0;
  if(PLenclave->user_param.elf_ptr == 0 || PLenclave->user_param.elf_size <= 0)
  {
    fprintf(stderr, "LIB: ioctl create enclave: elf_ptr is NULL\n");
    return -1;
  }

  ret = ioctl(PLenclave->fd, PENGLAI_ENCLAVE_IOC_CREATE_ENCLAVE, &(PLenclave->user_param));
  if(ret < 0)
  {
    fprintf(stderr, "LIB: ioctl create enclave is failed\n");
    return -1;
  }

  PLenclave->eid = PLenclave->user_param.eid;
  return 0;
}

int PLenclave_run(struct PLenclave *PLenclave)
{
  int ret = 0;

  ret = ioctl(PLenclave->fd,PENGLAI_ENCLAVE_IOC_RUN_ENCLAVE, &(PLenclave->user_param));
  if(ret < 0)
  {
    fprintf(stderr, "LIB: ioctl run enclave is failed \n");
    return -1;
  }

  return ret;
}

int PLenclave_stop(struct PLenclave *PLenclave)
{
  int ret =0;
  fprintf(stderr, "[@%s] beign\n", __func__);
  ret = ioctl(PLenclave->fd, PENGLAI_ENCLAVE_IOC_STOP_ENCLAVE, &(PLenclave->user_param));
  if (ret < 0 )
  {
    fprintf(stderr, "LIB: ioctl stop enclave is failed with ret:%d\n", ret);
    return -1;
  }
  return 0;
}

int PLenclave_attest(struct PLenclave *PLenclave, uintptr_t nonce)
{
  int ret = 0;
  PLenclave->attest_param.eid = PLenclave->eid;
  PLenclave->attest_param.nonce = nonce;
  ret = ioctl(PLenclave->fd, PENGLAI_ENCLAVE_IOC_ATTEST_ENCLAVE, &(PLenclave->attest_param));
  if(ret < 0)
  {
    fprintf(stderr, "LIB: ioctl attest enclave is failed ret %d \n", ret);
    return -1;
  }

  return 0; 
}


int PLenclave_resume(struct PLenclave *PLenclave)
{
  int ret = 0;
  ret = ioctl(PLenclave->fd, PENGLAI_ENCLAVE_IOC_RESUME_ENCLAVE, &(PLenclave->user_param));
  if (ret < 0 )
  {
    fprintf(stderr, "LIB: ioctl resume enclave is failed \n");
    return -1;
  }
  return ret;
}

int PLenclave_destroy(struct PLenclave *PLenclave)
{
  int ret = 0;
  ret = ioctl(PLenclave->fd, PENGLAI_ENCLAVE_IOC_DESTROY_ENCLAVE, &(PLenclave->user_param));
  if (ret < 0 )
  {
    fprintf(stderr, "LIB: ioctl destory enclave is failed \n");
    return -1;
  }
  return 0;
}

int PLenclave_debug_print(struct PLenclave *PLenclave)
{
  int ret = 0;

  ret = ioctl(PLenclave->fd, PENGLAI_ENCLAVE_IOC_DEBUG_PRINT, &(PLenclave->user_param));
  if(ret < 0)
  {
    fprintf(stderr, "LIB: ioctl debug print is failed \n");
    return -1;
  }

  return 0;
}

