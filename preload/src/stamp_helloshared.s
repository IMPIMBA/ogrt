    .section ".note.ogrt.info", "a"
    .p2align 2
    .long 1f - 0f           # name size (not including padding)
    .long 3f - 2f           # desc size (not including padding)
    .long 0x4F475254        # type
0:  .asciz "OG"     # name
1:  .p2align 2
2:  .asciz "hello from the shared stamp"        # desc
3:  .p2align 2

