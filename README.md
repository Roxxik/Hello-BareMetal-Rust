# Hello-BareMetal-Rust
A simple PoC Hello World in Rust for BareMetal OS

# How to build
1. get libcore
  - cp -rf path/to/rust/ .
2. get rlibc
  - git clone https://github.com/rust-lang/rlibc.git
3. make

to get the binary into your bmImage:
change IMAGEDIR in the Makefile to match the bin folder where the bmfs.image and the bmfs executable are

make install


I built this with

    $ rustc --version --verbose
    rustc 1.3.0-nightly (2671e8cee 2015-06-28)
    binary: rustc
    commit-hash: 2671e8cee08eb35013dc211286a6765c80b49c29
    commit-date: 2015-06-28
    host: x86_64-unknown-linux-gnu
    release: 1.3.0-nightly

on Arch Linux

    $ uname -a
    Linux arch 4.0.7-2-ARCH #1 SMP PREEMPT Tue Jun 30 07:50:21 UTC 2015 x86_64 GNU/Linux

But I think this can build on any Linux with a gnu toolchain and some rust nightly
If i miss some dependencies please tell me so i can include them for everyone else trying to play with rust on BareMetal


# TODO
- make sure the target.json is sane
- get to know rust inline assembly to avoid the intermediate C code

eventually sometimes
start a new repo which can run libstd on BareMetal
