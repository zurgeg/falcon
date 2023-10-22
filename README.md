# Falcon

Falcon reimplements boot0, boot1, boot2, and the System Menu in C.

This version is compatible with:
- boot0: *
- boot1: ?
- boot2: ?

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

