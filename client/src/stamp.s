    .section ".note.ogrt.info", "a"
    .p2align 2
    .long 1f - 0f           # name size (not including padding)
    .long 3f - 2f           # desc size (not including padding)
    .long 0x4F475254        # type
0:  .asciz "OGRT"     # name
1:  .p2align 2
2:  .byte  0x01 #version
    .asciz "AAAAAAAA-AAAA-AAAA-AAAA-AAAAAAAAAAAA"        # uuid
3:  .p2align 2

