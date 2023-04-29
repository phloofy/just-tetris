set -e

echo "gdb build/$1.kernel"

OPT=-O0 make $1
time `make qemu`						\
     `make qemu_flags`						\
     -s -S							\
     -no-reboot							\
     -nographic							\
     -monitor none						\
     -drive file=build/$1.img,index=0,media=disk,format=raw	\
     -device isa-debug-exit,iobase=0xf4,iosize=0x04 || true
