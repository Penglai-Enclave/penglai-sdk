## Document

### Configuration

#### brk_size and mmap_size

- The default values for brk_size and mmap_size are 4096 * 32 and 4096 * 64;
- If you need to increase the value of brk_size or mmap_size to support some large demos, you can add the corresponding parameter when you build sdk;
- The concrete build sdk command is like 'PENGLAI_SDK=$(pwd) CUSTOM_BRK_SIZE=4096\*256 CUSTOM_MMAP_SIZE=4096\*8192 make -j8'.
