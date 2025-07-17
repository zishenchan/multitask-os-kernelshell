# multitask-os-kernelshell (x86 kernel)

> **32‑bit pre‑emptive OS** written in **C** and **NASM**, booting from a custom assembler loader and running user programs in protected mode.

## 🚀 Features
- FAT‑16 filesystem driver  
- Virtual memory & paging (4 KB pages)  
- Round‑robin task scheduler (100 Hz PIT)  
- ELF binary loader + userspace shell  
- PS/2 keyboard & basic IRQ handling  
- GDB remote debugging via QEMU monitor  

## 🖼️ Demo
![Kernel demo](docs/demo.gif)

## ⚡ Quick‑start
```bash
git clone https://github.com/<you>/baremetal32
cd baremetal32
./setup-toolchain.sh   # one‑time, builds i686‑elf‑gcc
make run               # boots in QEMU
```

## 🛠️ Tech
![C](https://img.shields.io/badge/C-99-blue.svg)
![NASM](https://img.shields.io/badge/ASM-NASM-critical.svg)
![QEMU](https://img.shields.io/badge/Emulator-QEMU-orange.svg)
![GitHub Actions](https://img.shields.io/badge/CI-GitHub_Actions-brightgreen.svg)
C 99 · NASM · GNU ld · i686‑elf GCC · QEMU · GDB · GitHub Actions  

### 📚 What I learned
- Built a paging MMU and virtual memory from scratch  
- Wrote a FAT16 driver & ELF program loader  
- Implemented pre‑emptive round‑robin scheduler at 100 Hz  
- Debugged disassembled x86 with GDB remote stub

### 🗺️ Roadmap
- [ ] Migrate to x86‑64 long mode  
- [ ] Add ext2 filesystem module  
- [ ] Integrate basic TCP/IP stack  

### 🙏 Credits
Based on “Multitasking OS From Scratch” by **[Instructor Name]**  
Inspired by Linux kernel patterns; thanks to NASM & QEMU communities.  

## ⚖️ License
Distributed under the MIT License.  
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)  
See `LICENSE` for details.  

