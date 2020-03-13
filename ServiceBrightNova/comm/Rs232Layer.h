/*
 * Rs232Layer.h
 *
 *  Created on: 20/01/2014
 *      Author: Ernesto
 */

#ifndef RS485LAYER_H_
#define RS485LAYER_H_

typedef struct
{
	int cport_nr;
	int bdrate;
}Port;

#define PORT_TTYUSB0	0
#define PORT_TTYUSB1	1
#define PORT_TTYO0		2
#define PORT_TTYO1		3
#define PORT_TTYO2		4
#define PORT_TTYO3		5
#define PORT_TTYO4		6
#define PORT_TTYO5		7
#define PORT_TTYS0		8
#define PORT_TTYS1		9
#define PORT_TTYS2		10
#define PORT_TTYS3		11
#define PORT_TTYS4		12
#define PORT_TTYS5		13
#define PORT_TTYS6		14
#define PORT_TTYS7		15
#define PORT_TTYS8		16
#define PORT_TTYS9		17
#define PORT_TTYS10		18
#define PORT_TTYS11		19
#define PORT_TTYS12		20
#define PORT_TTYS13		21

int rs232OpenPort(Port * pPort);
int rs232SendPacket(Port * pPort,unsigned char * buff,int size);
int rs232Receive(Port * pPort,unsigned char * buff,int size);
int rs232ReceiveWithTimeout(Port * pPort,unsigned char * bufferRx,int size,int timeout);
void rs232ClosePort(Port * pPort);

#endif /* RS232LAYER_H_ */
