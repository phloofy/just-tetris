set -e

make $1
time `make qemu`						\
     `make qemu_flags`						\
     -no-reboot							\
     -monitor none						\
     -vga virtio						\
     -serial stdio						\
     -drive file=build/$1.img,index=0,media=disk,format=raw	\
     -device isa-debug-exit,iobase=0xf4,iosize=0x04 || true
