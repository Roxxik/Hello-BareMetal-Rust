# Hello-BareMetal-Rust
A simple PoC Hello World in Rust for BareMetal OS

It is possible (by liballoc) to (unsafely) allocate chunks of memory smaller than a whole page

Next thing to do is porting libstd/ rusts liballoc

# How to build
1. get libcore (same version as rustc)
  - cp -rf path/to/rust/src/libcore .
2. get rlibc
  - git clone https://github.com/rust-lang/rlibc.git
3. make

to get the binary into your bmImage:
change IMAGEDIR in the Makefile to match the bin folder where the bmfs.image and the bmfs executable are

make install


I think this can build on any Linux with a gnu toolchain and some rust nightly
If i miss some dependencies please tell me so i can include them for everyone else trying to play with rust on BareMetal


# TODO
- make sure the target.json is sane
- get to know rust inline assembly to avoid the intermediate C code

eventually sometimes
start a new repo which can run libstd on BareMetal
