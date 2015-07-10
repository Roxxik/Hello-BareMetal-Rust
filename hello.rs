#![feature(no_std)]
#![feature(lang_items)]
#![feature(core)]
#![feature(core_str_ext)]
#![feature(core_prelude)]
#![no_std]

#![feature(start)]

#[macro_use]
extern crate core;

pub mod prelude {
    pub use core::prelude::*;
}

pub use prelude::*;

#[allow(non_snake_case)]
mod libBareMetal;

#[lang="start"]
#[no_mangle]
#[allow(private_no_mangle_fns)]
fn main() {
    libBareMetal::output("type something\n");
    unsafe{
        let buf = libBareMetal::malloc(21);
        let _ = libBareMetal::input(buf,20);
        *buf.offset(20) = 0;
        libBareMetal::output(core::str::from_utf8_unchecked(core::slice::from_raw_parts(buf,20)));
        libBareMetal::free(buf);
    }
}
