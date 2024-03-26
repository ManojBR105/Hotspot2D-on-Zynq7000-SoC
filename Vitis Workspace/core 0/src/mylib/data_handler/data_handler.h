#include <stdio.h>
#include <string.h>
#include "xil_printf.h"
#include "lwip/err.h"
#include "lwip/tcp.h"

#include "../constants.h"

#define ARR_SIZE (512*512)
#define PACK_SIZE 128


void handle_received_data(struct pbuf *p, struct tcp_pcb *tpcb, err_t err);
void send_success(struct pbuf *p, struct tcp_pcb *tpcb, err_t err);
void send_data(float* arr, int offset, struct pbuf *p, struct tcp_pcb *tpcb, err_t err);
void store_data(const char* recvd_data, int len, float* arr);
void store_data1(const char* recvd_data, int len, int* arr);
