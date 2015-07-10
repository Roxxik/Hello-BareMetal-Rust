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
    libBareMetal::output("Hello\n");
}
