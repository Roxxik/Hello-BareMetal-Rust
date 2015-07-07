ARCH = x86_64

RUSTC = rustc
LD=ld
AS=nasm
CC=gcc

TARGET = target.json
LINKSCRIPT = app.ld

RUSTFLAGS = -g -O --cfg arch__$(ARCH) --target=$(TARGET) -Z no-landing-pads

LINKFLAGS = -T $(LINKSCRIPT) --gc-sections -z max-page-size=0x20000

ASFLAGS=-f elf64

CFLAGS = -g -O -c -m64 -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer -mno-red-zone

LIBCORESRC=libcore/lib.rs
LIBCORE=libcore.rlib

RLIBCSRC=rlibc/src/lib.rs
RLIBC=librlibc.rlib

MAINSRC=hello.rs
MAIN=hello.o

STARTSRC=start.asm
START=start.o

LIBBAREMETALSRC=libBareMetal.c
LIBBAREMETAL=libBareMetal.o

OBJS=start.o hello.o libBareMetal.o $(RLIBC) $(LIBCORE)

BIN=hellors.app

IMAGEDIR=../BareMetal/bin/
BMFS=$(IMAGEDIR)bmfs
IMAGE=$(IMAGEDIR)bmfs.image


.PHONY: all run debug clean install run

all: $(BIN)

install: $(BIN)
	$(BMFS) $(IMAGE) delete $(BIN)
	$(BMFS) $(IMAGE) create $(BIN) 2
	$(BMFS) $(IMAGE) write $(BIN)

clean:
	rm -f $(OBJS) $(BIN)

$(BIN): $(OBJS) $(LINKSCRIPT)
	$(LD) -o $@ $(LINKFLAGS) --start-group $(OBJS) --end-group

$(LIBCORE): $(LIBCORESRC) $(TARGET)
	$(RUSTC) $(RUSTFLAGS) -o $@ --crate-type=lib --emit=link $(LIBCORESRC)

$(RLIBC): $(RLIBCSRC) $(LIBCORE) $(TARGET)
	$(RUSTC) $(RUSTFLAGS) -o $@ --crate-type=lib --emit=link --extern core=$(LIBCORE) $(RLIBCSRC)

$(MAIN): $(MAINSRC) $(LIBCORE) $(TARGET)
	$(RUSTC) $(RUSTFLAGS) -o $@ --emit=obj --extern core=$(LIBCORE) $(MAINSRC)

$(LIBBAREMETAL): $(LIBBAREMETALSRC)
	$(CC) $(CFLAGS) -o $@ $(LIBBAREMETALSRC)

$(START): $(STARTSRC)
	$(AS) -o $@ $(ASFLAGS) $(STARTSRC)
