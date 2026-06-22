C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h)
OBJ = ${C_SOURCES:.c=.o}
CC = i386-elf-gcc
GDB = i386-elf-gdb
CFLAGS = -g

os.bin: boot/boot.bin Kernel/kernel.bin
	cat $^ > os.bin
kernel.bin: boot/kernelentry.o ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

kernel.elf: boot/kernelentry.o ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^

run: os.bin	
	qemu-x86 -fda os.bin

debug: os.bin kernel.elf
	qemu-x86 -s -fda os.bin &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f bin -o $@