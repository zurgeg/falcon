# WARNING: Falcon has not been tested to work on real hardware, and is not meant to
If you somehow build Falcon and get it running on real hardware, that is a lot of work you have put in in order to break your Wii beyond any and all repair
# Falcon

Falcon reimplements boot0, boot1, boot2, and the System Menu in C.

This version is compatible with:
- boot0: *
- boot1: ?
- boot2: ?

## Psuedo-decompilation
For "psuedo-decompilation", we "decompile" a stage by just taking its instructions and changing them into setting memory and registers, so:
```s
mov R1, 0x1
STR R1, #0xD800000
```
becomes
```c
*(registers + R1) = 0x1;
*(memory + 0xD800000) = *(registers + R1);
```

## Progress
Bold is the current stage
- [x] Psuedo-decompile part of boot0
- [ ] **Actually decompile that part of boot0**
- [] Finish psuedo-decompiling boot0
- [ ] Psuedo-decompile boot1
- [ ] Psuedo-decompile boot2
- [ ] Test against real HW behavior
- [ ] Actually decompile boot0
- [ ] Test against previous boot0 behavior
- [ ] Actually decompile boot1
- [ ] Test against previous boot1 behavior
- [ ] Actually decompile boot2
- [ ] Test against previous boot2 behavior
- [ ] Psuedo-decompile System Menu
- *Who knows what's next?*
## Why?

The main goal of Falcon is for me to practice decompiling C,
and using C in general. 
It also may help security researchers find vulnerabilities in boot0 and boot1
Thus allowing for a potential, say, unbrick for Wiis bricked by 4.2, or
even BootMii as boot1/boot0.

## About `.crap-automake-me-made-me-put`
You can rm and regen Automake's crap like so:
```sh
# delete automake's crap
rm $(cat .crap-automake-made-me-put)
# regen automake's crap
autoreconf --install
aclocal
automake
autoconf
```

## Extra notes
boot0 was compiled with GNU GCC 3.4.3 (see FFFF0505 in `dolsrc/arm/boot0.s`)

