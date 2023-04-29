SECTIONS
{
 . = 0x7C00;
 .mbr16 : { *(.mbr16) }
 . = 0x8000;
 .entry16 : { *(.entry16) }
 .entry32 : { *(.entry32) }
 .text : { *(.text) }
 .rodata : { *(.rodata*) }
 .init_array :
 {
  /data/nvme0n1p5/phloofy/Documents/just-tetris/build/crti.o(.init_array)
  KEEP (*(SORT(EXCLUDE_FILE(/data/nvme0n1p5/phloofy/Documents/just-tetris/build/crti.o /data/nvme0n1p5/phloofy/Documents/just-tetris/build/crtn.o) .init_array)))
  KEEP (*(EXCLUDE_FILE(/data/nvme0n1p5/phloofy/Documents/just-tetris/build/crti.o /data/nvme0n1p5/phloofy/Documents/just-tetris/build/crtn.o) .init_array))
  /data/nvme0n1p5/phloofy/Documents/just-tetris/build/crtn.o(.init_array)
 }
 .fini_array :
 {
  /data/nvme0n1p5/phloofy/Documents/just-tetris/build/crti.o(.fini_array)
  KEEP (*(SORT(EXCLUDE_FILE(/data/nvme0n1p5/phloofy/Documents/just-tetris/build/crti.o /data/nvme0n1p5/phloofy/Documents/just-tetris/build/crtn.o) .fini_array)))
  KEEP (*(EXCLUDE_FILE(/data/nvme0n1p5/phloofy/Documents/just-tetris/build/crti.o /data/nvme0n1p5/phloofy/Documents/just-tetris/build/crtn.o) .fini_array))
  /data/nvme0n1p5/phloofy/Documents/just-tetris/build/crtn.o(.fini_array)
 }
 .data : { *(.data) }
 .bss : { *(.bss) }
}
