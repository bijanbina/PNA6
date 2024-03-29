#ifndef __PNA_INTERFACE_H__
#define __PNA_INTERFACE_H__

#include <stdio.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <unistd.h>

#define PNA_INTERFACE_CONSOLE 0
#define PNA_INTERFACE_TCP 1
#define PORT_NUMBER 1253

void pna_printf(char *format, ...);
int pna_gets(char *buffer, int max_len);
void pna_write(unsigned char *data, int len);
size_t pna_read(unsigned char *data, int len, int escape_set);
int pna_is_escaped(unsigned char *data);
void pna_set_interface(int id);
void pna_init_interface(int id);
void pna_close_interface();

#endif //__PNA_INTERFACE_H__
