#ifndef LIBBAREMETAL_H
#define LIBBAREMETAL_H

void b_output(const char *str);

void b_output_chars(const char *str, unsigned long nbr);

void b_output_char(char c);

unsigned long b_input(unsigned char *str, unsigned long nbr);

unsigned char b_input_key(void);

void *b_mem_allocate(unsigned long nbr);

unsigned long b_mem_release(void *mem, unsigned long nbr);

#endif /* LIBBAREMETAL_H */
