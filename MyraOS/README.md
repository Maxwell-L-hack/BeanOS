# MyraOS

A x86 Unix-like OS made entirely from scratch.

![MyraOS Desktop](https://i.imgur.com/XRRSfOp.jpeg "MyraOS Desktop")

**Features**
- Protected mode (GDT/IDT, ISRs/IRQs)
- Paging and virtual memory
- Memory management
- Heap and dynamic memory
- User-mode (ring 3) and kernel mode (ring 0)
- Processes and scheduling
- Drivers (PIT, RTC, Keyboard, Mouse, Framebuffer, PATA)
- ext2 filesystem
- UI compositor with window widgets, labels, icons, buttons, and even a custom-made font
- ELF loader, which gives you the ability to run real apps

All these features let you run real games, just like Doom, giving the preloaded Doom port in MyraOS ready to be played!  
So, this isn't just a toy OS or a look-alike, it's a **real OS** that can run on **real devices**

## How to run it? 
1) Download the latest release from the release tab in GitHub
2) Download [QEMU](https://www.qemu.org/download/) - an open-source machine emulator and virtualizer

After you get the latest release, you can run this on your platform:
### macOS

**Normal**
```
qemu-system-i386 -cdrom MyraOS.iso -drive file=fs.img,format=raw,if=ide,index=0 -m 1024
```

**Fullscreen** (if you are like me and want it to look real)
```
qemu-system-i386 -cdrom MyraOS.iso -drive file=fs.img,format=raw,if=ide,index=0 -m 1024 -full-screen
```

### Linux

**Normal**
```
qemu-system-i386 -cdrom MyraOS.iso -drive file=fs.img,format=raw,if=ide,index=0 -m 1024
```

**Fullscreen**
```
qemu-system-i386 -cdrom MyraOS.iso -drive file=fs.img,format=raw,if=ide,index=0 -m 1024 -display gtk,zoom-to-fit=on -full-screen
```

### Windows

Here, Linux/macOS or even WSL are better; use it as a last resort:  
**Normal**
```
qemu-system-i386 -cdrom MyraOS.iso -drive file=fs.img,format=raw,if=ide,index=0 -m 1024
```

**Fullscreen**
```
qemu-system-i386 -cdrom MyraOS.iso -drive file=fs.img,format=raw,if=ide,index=0 -m 1024 -display gtk,zoom-to-fit=on -full-screen
```

## Feedback
I really hope you like it, as I spent a lot of time on it, and I'd really appreciate any feedback you have for me.  
If you have anything, from feature requests to feedback, or even if you want to talk, email me here: `dvirm.biton@gmail.com`.
