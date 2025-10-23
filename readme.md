# ☁️ Nephos (Nephele's Operating System)

Nephos️ is a fun little project, tinkering with x86 OS development, assembly and C++, as a fun learning opportunity.

# Dependencies

- An internet connection (required to download and build cross compiler).
- A linux machine of some sort (with common linux utilities available).
- `nasm`,
- `mtools`
- `qemu-system-x86_64` (for testing the OS) 

# Compilation

```sh
git clone --recursive https://github.com/fosspointer/nephos && cd nephos
./compilersetup.sh && make && qemu-system-x86_64 -fda build/boot.img
```