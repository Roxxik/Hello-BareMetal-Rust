#![feature(no_std)]
#![feature(lang_items)]
#![feature(core)]
#![feature(core_str_ext)]
#![feature(core_prelude)]
#![no_std]

#[macro_use]
extern crate core;

pub mod prelude {
    pub use core::prelude::*;
}

pub use prelude::*;

#[lang="panic_fmt"]
#[no_mangle]
#[allow(unused_variables)]
pub fn rust_begin_unwind(args: ::core::fmt::Arguments, file: &str, line: usize) -> ! {
    loop {}
}

#[lang="stack_exhausted"]
#[no_mangle]
pub fn __morestack() -> ! {
    loop {}
}

#[lang="eh_personality"]
#[no_mangle]
pub fn rust_eh_personality() -> ! {
    loop{}
}

extern{
    fn b_output_char(c: u8);
    fn b_output(cstr: *const u8);
    fn b_output_chars(cstr: *const u8, n: u64);
    fn b_input(cstr: *const u8, n: u64) -> u64;
    fn b_input_key() -> u8;
    fn b_mem_allocate(n: u64) -> *mut u8;
}

#[no_mangle]
#[allow(unused_variables)]
pub extern fn main(argc: isize, argv: *const *const u8) -> isize {
    unsafe{
        let mem = b_mem_allocate(1);
        if (mem.is_null()) {
            b_output("Failed to get a page\n\0".as_ptr());
        } else {
            b_output("Got a page\n\0".as_ptr());
            b_output("now type some stuff\n\0".as_ptr());
            let n = b_input(mem, 20);
            //b_output_char('\n' as u8);
            b_output("read chars:\n\0".as_ptr());
            b_output(mem);
            b_output("\n\0".as_ptr());
        }
    }
    0
}
