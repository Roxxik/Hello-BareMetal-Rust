# Hello-BareMetal-Rust
A simple PoC Hello World in Rust for BareMetal OS

# How to build
1. get libcore
  - cp -rf path/to/rust/ .
2. get rlibc
  - git clone https://github.com/rust-lang/rlibc.git
3. build and install BareMetal
4. make

to get the binary into your bmImage:
change IMAGEDIR in the Makefile to match the bin folder where the bmfs.image and the bmfs executable are

make install


# TODO
- fix the memory leak in main
- make sure the target.json is sane
- do something usefull in start.asm (like providing argc and argv)
- factor the boilerplate out into libBareMetal.rs
- get to know rust inline assembly to avoid the intermediate C code
- provide a rust interface in libBareMetal.rs

eventually sometimes
start a new repo which can run libstd on BareMetal
