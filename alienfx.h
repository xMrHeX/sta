#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>
#define	ALIENWARE_VENDORID		0x187c
#define	ALIENWARE_PRODUCTID_M11XR3	0x0522

#define	OK			0
#define	LIBUSB_INIT_ERR		-1
#define	LIBUSB_OPEN_ERR		-2
#define	LIBUSB_SIZE_ERR		-3
#define	LIBUSB_WRITE_ERR	-4
#define	LIBUSB_READ_ERR		-5

#define SEND_REQUEST_TYPE 0x21
#define SEND_REQUEST      0x09
#define SEND_VALUE        0x202
#define SEND_INDEX        0x00
#define SEND_DATA_SIZE    9

#define	READ_REQUEST_TYPE	0xa1
#define	READ_REQUEST		0x01
#define	READ_VALUE		0x101
#define	READ_INDEX		0x0
#define	READ_DATA_SIZE		8

#define	MIN_SPEED		100
#define	MAX_SPEED		1000
#define	STEP_SPEED		100

int InitDevice(libusb_context** usbcontext,libusb_device_handle** usbhandle,unsigned short idVendor,unsigned short idProduct)
{
	if (0==libusb_init(usbcontext))
	{
		libusb_set_debug(*usbcontext,3);	
		if (*usbhandle=libusb_open_device_with_vid_pid(*usbcontext,idVendor,idProduct))
		{
			fprintf(stderr,"device opened\n");	
		} else {
			return LIBUSB_OPEN_ERR;
		}
	} else {
		return	LIBUSB_INIT_ERR;
	}
	return OK;
}
void usbdetach(libusb_device_handle* usbhandle)
{
	if (libusb_kernel_driver_active(usbhandle,0))
		libusb_detach_kernel_driver(usbhandle,0);

}
void usbattach(libusb_device_handle* usbhandle)
{
	libusb_attach_kernel_driver(usbhandle,0);
}
int usbsetdelay(libusb_device_handle* usbhandle,unsigned int delay)
{
	unsigned char data[9];
	unsigned int retval;
	int i;
	delay=(delay/STEP_SPEED)*STEP_SPEED;	// quantize to step multiple

	if (delay<MIN_SPEED) delay=MIN_SPEED;
	if (delay>MAX_SPEED) delay=MAX_SPEED;

	data[0]=0x02;
	data[1]=0x0e;	//COMMAND_SET_SPEED;
	data[2]=(delay>>8)&0xff;
	data[3]=(delay>>0)&0xff;
	data[4]=0;
	data[5]=0;
	data[6]=0;
	data[7]=0;
	data[8]=0;
	
	fprintf(stderr,"write> ");
	for (i=0;i<SEND_DATA_SIZE;i++) 
	{
		fprintf(stderr,"%02x ",0xff&((unsigned int)data[i]));
	}
	fprintf(stderr,"\n");
	retval=libusb_control_transfer(usbhandle,
					SEND_REQUEST_TYPE,
					SEND_REQUEST,
					SEND_VALUE,
					SEND_INDEX,
					data,
					SEND_DATA_SIZE,
					0);

	if (retval!=9)
		return LIBUSB_WRITE_ERR;

	return	OK;
}
int usbwrite(libusb_device_handle* usbhandle,unsigned char* data,unsigned short len)
{
	int retval;
	int i;
	if (len!=SEND_DATA_SIZE)
		return	LIBUSB_SIZE_ERR;

	fprintf(stderr,"write> ");
	for (i=0;i<SEND_DATA_SIZE;i++) 
	{
		fprintf(stderr,"%02x ",0xff&((unsigned int)data[i]));
	}
	fprintf(stderr,"\n");

	retval=libusb_control_transfer(usbhandle,
					SEND_REQUEST_TYPE,
					SEND_REQUEST,
					SEND_VALUE,
					SEND_INDEX,
					data,
					SEND_DATA_SIZE,
					0);
	if (retval!=SEND_DATA_SIZE)
		return	LIBUSB_WRITE_ERR;
	usleep(9000);	
	return OK;
}
int usbread(libusb_device_handle* usbhandle,char* data,unsigned int len)
{
	unsigned char buf[READ_DATA_SIZE];
	int readbytes;
	int i;

	if (len!=READ_DATA_SIZE)
		return	LIBUSB_SIZE_ERR;
	
	readbytes=libusb_control_transfer(usbhandle,
				READ_REQUEST_TYPE,
				READ_REQUEST,
				READ_VALUE,
				READ_INDEX,
				buf,
				READ_DATA_SIZE,
				0);
	if (readbytes!=READ_DATA_SIZE)
		return LIBUSB_READ_ERR;
	fprintf(stderr,"read>  ");
	for (i=0;i<READ_DATA_SIZE;i++) 
	{
		data[i]=buf[i];
		fprintf(stderr,"%02x ",0xff&((unsigned int)data[i]));
	}
	fprintf(stderr,"\n");
	return OK;
}
#define	AW_RESET \
{ \
	unsigned char data[9]={0x02,0x07,0x04,0x00,0x00,0x00,0x00,0x00,0x00}; \
	retval=usbwrite(usbhandle,data,9); \
}
#define	AW_END \
{ \
	unsigned char data[9]={0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00};	\
	retval=usbwrite(usbhandle,data,9); \
}

#define	AW_PGE \
{ \
	unsigned char data[9]={0x02,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00};	\
	retval=usbwrite(usbhandle,data,9); \
}

#define	AW_CHK \
{ \
	unsigned char data[9]={0x02,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00};	\
	retval=usbwrite(usbhandle,data,9); \
}
/*
#define AW_SETCOLOR(zone,r,g,b) \
{	\
	unsigned char data[9]={0x02,0x03,0x05,0x00,0x00,0x01,0x0f,0xf0,0x00}; \
	data[3]=(zone>>16)&0xff;\
	data[4]=(zone>>8)&0xff;	\
	data[5]=(zone>>0)&0xff;	\
	data[6]=(r>>0)&0xf0;	\
	data[6]|=(g>>4)&0x0f;	\
	data[7]=(b>>0)&0xf0;		\
	retval=usbwrite(usbhandle,data,9); \
}
*/
#define AW_SETCOLOR(zone,r,g,b) \
{	\
	unsigned char data[9]={0x02,0x03,0x05,0x00,0x00,0x01,0x0f,0xf0,0x00}; \
	data[4]=(zone>>8)&0xff;	\
	data[5]=(zone>>0)&0xff;	\
	data[6]=(r<<4)&0xf0;	\
	data[6]|=(g&0xf);	\
	data[7]=(b<<4)&0xf0;		\
	retval=usbwrite(usbhandle,data,9); \
}
#define AW_SETCOLORS(zone,r1,g1,b1, r2,g2,b2) \
{	\
	unsigned char data[9]={0x02,0x03,0x05,0x00,0x00,0x01,0x0f,0xf0,0x00}; \
	data[4]=(zone>>8)&0xff;	\
	data[5]=(zone>>0)&0xff;	\
	data[6]=(r1<<4)&0xf0;	\
	data[6]|=(g1&0xf);	\
	data[7]=(b1<<4)&0xf0;		\
	data[7]|=(r1&0xf);		\
	data[8]=(g1<<4)&0xf0;	\
	data[8]=(b1&0xf);	\
	retval=usbwrite(usbhandle,data,9); \
}
#define	AW_WAIT	\
{	\
	unsigned char rply[8];	\
	int ready=0;	\
	while (!ready)	\
	{	\
		AW_CHK				\
		usbread(usbhandle,rply,8);	\
		if (rply[0]==0x11) ready=1;	\
	}	\
}	
#define	CHANGECOLOR(zone,r,g,b) \
{	\
	AW_SETCOLOR(zone,r,g,b); 	\
	AW_END;		\
	AW_PGE;	\
	AW_RESET;	\
	AW_WAIT;	\
}	
#define	ZONE_KEYBOARD		0x01
#define	ZONE_SPEAKER_LEFT	0x20
#define	ZONE_SPEAKER_RIGHT	0x40
#define	ZONE_SPEAKERS		0x60
#define	ZONE_LOGO		0x100

