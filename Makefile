C_SOURCES = $(wildcard Kernel/*.c drivers/*.c CPU/*.c)
HEADERS = $(wildcard Kernel/*.h drivers/*.h CPU/*.h)
OBJ = ${C_SOURCES:.c=.o CPU/interrupt.o}
CC = i386-elf-gcc
GDB = i386-elf-gdb
CFLAGS = -g

os.bin: boot/boot.bin kernel.bin
	cat $^ > os.bin
kernel.bin: boot/kernelentry.o ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

kernel.elf: boot/kernelentry.o ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^

boot/boot.bin: boot/32main.asm
	nasm -I boot $< -f bin -o $@

boot/kernelentry.o: boot/kernelentry.asm
	nasm $< -f elf32 -o $@
CPU/interrupt.o: CPU/interrupt.asm
	nasm $< -f elf32 -o $@
run: os.bin	
	qemu-system-i386 -fda os.bin

debug: os.bin kernel.elf
	qemu-system-i386 -s -fda os.bin &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf32 -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os.bin *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o CPU/*.o