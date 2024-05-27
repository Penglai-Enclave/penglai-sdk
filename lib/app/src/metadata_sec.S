#define METADATA_SIZE 0x1000
.section ".note.penglaimeta", "", @note
    .p2align 2              /* section alignment */
    .long 1f - 0f           /* name size (not including padding) */
    .long 2f - 1f           /* desc size (not including padding) */
    .long 0x01              /* type */
0:  .asciz "penglai_metadata"   /* name */
1:  .fill METADATA_SIZE, 1, 0      /* desc - stand for metadata which is initialized with 0 */
2:  .p2align 2
