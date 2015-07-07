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
change IMAGEDIR in the Makefile to match the bin folder where the bmfs.image and the bmfs executables are

make install
