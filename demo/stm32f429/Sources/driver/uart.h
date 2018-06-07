#ifndef __UART_H
#define __UART_H

#define NOPARITY		0
#define ODDPARITY       1
#define EVENPARITY      2

void uart_init(int id, void* buff, int buff_size);
void uart_open(int id, int baudrate, int parity);
void uart_close(int id);
void uart_write(int id, void* buf, int len);
int  uart_read(int id, void* buf, int len);
int  uart_overflow(int id);
void uart_clear(int id);

#endif
