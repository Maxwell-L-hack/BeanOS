all: run
kernel.bin: Kernel/kernelentry.o Kernel/kernel.o
	i386-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary
kernelentry.o: Kernel/kernelentry.asm
	nasm $< -f elf -o $@
kernel.o: Kernel/kernel.c
	i386-elf-gcc -ffreestanding -c $< -o $@
kernel.dis: Kernel/kernel.bin
	ndiasm -b 32 $< > $@
boot.bin: boot/32main.asm
	nasm $< -f bin -o $@
os.bin: boot/boot.bin Kernel/kernel.bin
	cat $^ > $@
run: OS/os.bin	
	qemu-system-i386 -fda $<