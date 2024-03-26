
#include "data_handler.h"

int ETHERNET_DATA_READY_FLAG = 0;
static float* temp_rcvd =  (float*)TEMP_RECD;
static float* power_rcvd = (float*)POWER_RECD;

void handle_received_data(struct pbuf *p, struct tcp_pcb *tpcb, err_t err) {
	if (!strncmp((const char*) p->payload, "TEMP", 4))
	{
		const char* payload = p->payload;
		int len = (p->len) - 5;
		store_data(&payload[5], len, temp_rcvd);
		send_success(p, tpcb, err);
	}
	else if (!strncmp((const char*) p->payload, "POWR", 4))
	{
		const char* payload = p->payload;
		int len = (p->len) - 5;
		store_data(&payload[5], len, power_rcvd);
		send_success(p, tpcb, err);
	}
	else if (!strncmp((const char*) p->payload, "IDX0", 4))
	{
		const char* payload = p->payload;
		int len = (p->len) - 5;
		xil_printf("Start X\n\r");
		store_data1(&payload[5], len, (int*)START_ROW_IDX);
		send_success(p, tpcb, err);
	}
	else if (!strncmp((const char*) p->payload, "IDX1", 4))
	{
		const char* payload = p->payload;
		int len = (p->len) - 5;
		xil_printf("Start Y\n\r");
		store_data1(&payload[5], len, (int*)START_COL_IDX);
		send_success(p, tpcb, err);
	}
	else if (!strncmp((const char*) p->payload, "IDX2", 4))
	{
		const char* payload = p->payload;
		int len = (p->len) - 5;
		xil_printf("End X\n\r");
		store_data1(&payload[5], len, (int*)END_ROW_IDX);
		send_success(p, tpcb, err);
	}
	else if (!strncmp((const char*) p->payload, "IDX3", 4))
	{
		const char* payload = p->payload;
		int len = (p->len) - 5;
		xil_printf("End Y\n\r");
		store_data1(&payload[5], len, (int*)END_COL_IDX);
		send_success(p, tpcb, err);
	}
	else if (!strncmp((const char*) p->payload, "DONE", 4))
	{
		 ETHERNET_DATA_READY_FLAG = 1;
		xil_printf("Done Transferring\n\r");
		for(int i = 0; i < 5; i++){
			char buff[30];
			sprintf(buff, "%d\t%f\t%f\n\r", i, temp_rcvd[i], power_rcvd[i]);
			xil_printf("%s", buff);
		}
		for(int i = 511; i < 515; i++){
			char buff[30];
			sprintf(buff, "%d\t%f\t%f\n\r", i, temp_rcvd[i], power_rcvd[i]);
			xil_printf("%s", buff);
		}
		for(int i = ARR_SIZE-5; i < ARR_SIZE; i++){
			char buff[30];
			sprintf(buff, "%d\t%f\t%f\n\r", i, temp_rcvd[i], power_rcvd[i]);
			xil_printf("%s", buff);
		}
	}
	else if (!strncmp((const char*) p->payload, "SEND", 4)){
		int l = p->len - 5;
		char myStr[l+1];
		const char* recvd_data = (const char*) p->payload;
		strncpy(myStr,&recvd_data[5],l);
		myStr[l] = '\0';
		int offset = (atoi(myStr) - 1) * PACK_SIZE;
		send_data((float*)TEMP_SEND, offset, p, tpcb, err);
	}
}

void send_data(float* arr, int offset, struct pbuf *p, struct tcp_pcb *tpcb, err_t err) {
	xil_printf("Sending data with offset %d\r\n", offset);
	int l = 0;
	char* data = p->payload;
	for (int i = offset; i < offset + PACK_SIZE; i++) {
		char buff[15];
		sprintf(buff, "%f,",arr[i]);
		int myLen = strlen(buff);
		strncpy(&data[l], (const char*)buff, myLen);
		l += myLen;
	}
	p->len = (u16_t)l;
	if (tcp_sndbuf(tpcb) > p->len) {
		err = tcp_write(tpcb, p->payload, p->len, 1);
	} else
		xil_printf("no space in tcp_sndbuf\n\r");
}

void send_success(struct pbuf *p, struct tcp_pcb *tpcb, err_t err) {
	char buff[8] = "Success";
	p->payload = (void*)buff;
	p->len = (u16_t)8;
	if (tcp_sndbuf(tpcb) > p->len) {
		err = tcp_write(tpcb, p->payload, p->len, 1);
	} else
		xil_printf("no space in tcp_sndbuf\n\r");
}

void store_data(const char* recvd_data, int len, float* arr) {
	int count = 0;
	int prev = -1;
	int offset = 0;
	u8_t first = 1;
	for (int i = 0; i < len; i++) {
		if (count == ARR_SIZE)
			break;

		if (recvd_data[i] == ',') {
			int l = i - prev;
			char myStr[l];
			strncpy(myStr,&recvd_data[prev+1],l-1);
			myStr[l-1] = '\0';

			if (first) {
				offset = atoi(myStr)-1;
				count = offset * PACK_SIZE;
				//xil_printf("%d \t %s\n\r", count, myStr);
				first = 0;
			}
			else {
				float num = atof(myStr);
				//xil_printf("%d \t %s\n\r", count, myStr);
				arr[count++] = num;
			}
			prev = i;
		}
	}
}

void store_data1(const char* recvd_data, int len, int* arr) {
	int count = 0;
	int prev = -1;
	for (int i = 0; i < len; i++) {
		if (count == COMP_NO)
			break;

		if (recvd_data[i] == ',') {
			int l = i - prev;
			char myStr[l];
			strncpy(myStr,&recvd_data[prev+1],l-1);
			myStr[l-1] = '\0';

			float num = atoi(myStr);
			xil_printf("%d \t %s\n\r", count, myStr);
			arr[count++] = num;

			prev = i;
		}
	}
}
