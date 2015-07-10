#![allow(dead_code)]

#[lang="panic_fmt"]
#[no_mangle]
#[allow(private_no_mangle_fns)]
#[allow(unused_variables)]
fn rust_begin_unwind(args: ::core::fmt::Arguments, file: &str, line: usize) -> ! {
    loop {}
}

#[lang="stack_exhausted"]
#[no_mangle]
#[allow(private_no_mangle_fns)]
fn __morestack() -> ! {
    loop {}
}

#[lang="eh_personality"]
#[no_mangle]
#[allow(private_no_mangle_fns)]
fn rust_eh_personality() -> ! {
    loop{}
}

extern "C" {
    fn b_output_char(c: u8);
    fn b_output(cstr: *const u8);
    fn b_input(cstr: *const u8, n: u64) -> u64;
    fn b_input_key() -> u8;
    fn b_mem_allocate(n: u64) -> *mut u8;
    fn b_mem_release(mem: *mut u8, nbr: u64) -> u64;
    fn b_file_open(name: *const u8) -> u64;
    fn b_file_close(hdl: u64);
    fn b_file_read(hdl: u64, buf: *mut u8, count: u64) -> u64;
    pub fn malloc(size: u64) -> *mut u8;
    pub fn realloc(ptr: *mut u8, size: u64) -> *mut u8;
    pub fn free(ptr: *mut u8);
}


pub fn output_char(c: char) {
    unsafe{
        b_output_char(c as u8);
    }
}

pub fn output(s: &str) {
    unsafe{
        use core::str::StrExt;
        b_output(s.as_ptr());
    }
}

pub unsafe fn input(ptr: *mut u8, size: u64) -> u64 {
    b_input(ptr,size)
}

pub fn input_key() -> u8 {
    unsafe{
        let mut c = 0;
        while c == 0 {
            c = b_input_key();
        }
        c
    }
}
