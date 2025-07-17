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
