#include <libusb-1.0/libusb.h>

#define ALIENWARE_VENDID   0x187c // Dell Alienware
#define ALIENWARE_PRODID   0x0522 // M11XR3

#define OK                 0
#define LIBUSB_INIT_ERR   -1
#define LIBUSB_OPEN_ERR   -2
#define LIBUSB_SIZE_ERR   -3
#define LIBUSB_WRITE_ERR  -4
#define LIBUSB_READ_ERR   -5

#define SEND_REQUEST_TYPE  0x21
#define SEND_REQUEST       0x09
#define SEND_VALUE         0x202
#define SEND_INDEX         0x00
#define SEND_DATA_SIZE     9

#define READ_REQUEST_TYPE  0xa1
#define READ_REQUEST       0x01
#define READ_VALUE         0x101
#define READ_INDEX         0x0
#define READ_DATA_SIZE     8


#define AW_RESET \
{ \
    unsigned char data[9]={0x02,0x07,0x04,0x00,0x00,0x00,0x00,0x00,0x00}; \
    retval=usbwrite(usbhandle,data,9); \
}


int InitDevice(libusb_context** usbcontext, libusb_device_handle** usbhandle, unsigned short idVendor, unsigned short idProduct)
{
    if( 0 == libusb_init(usbcontext) ) {
        libusb_set_debug(*usbcontext, 3);
        *usbhandle = libusb_open_device_with_vid_pid(*usbcontext, idVendor, idProduct);
        if( usbhandle )
            fprintf(stdout, "device opened\n");
        else
            return LIBUSB_OPEN_ERR;
    } else {
        return LIBUSB_INIT_ERR;
    }
    return OK;
}

void usbattach(libusb_device_handle *usbhandle)
{
    libusb_attach_kernel_driver(usbhandle, 0);
}

void usbdetach(libusb_device_handle *usbhandle)
{
    if (libusb_kernel_driver_active(usbhandle, 0))
        libusb_detach_kernel_driver(usbhandle, 0);
}

int usbwrite(libusb_device_handle *usbhandle, unsigned char *data, unsigned short len)
{
    int retval;
    if( len != SEND_DATA_SIZE )
        return LIBUSB_SIZE_ERR;

    fprintf(stderr,"write > ");
    for( int i = 0; i < SEND_DATA_SIZE; i++ )
        fprintf(stderr, "%02x ", 0xff & ((unsigned int)data[i]));
    fprintf(stderr, "\n");

    retval = libusb_control_transfer(usbhandle,
                    SEND_REQUEST_TYPE,
                    SEND_REQUEST,
                    SEND_VALUE,
                    SEND_INDEX,
                    data,
                    SEND_DATA_SIZE,
                    0);
    if( retval != SEND_DATA_SIZE )
        return LIBUSB_WRITE_ERR;

    usleep(8000);

    return OK;
}

int usbread(libusb_device_handle *usbhandle, char *data, unsigned int len)
{
    unsigned char buf[READ_DATA_SIZE];
    int readbytes;

    if( len != READ_DATA_SIZE )
        return LIBUSB_SIZE_ERR;

    readbytes = libusb_control_transfer(usbhandle,
                READ_REQUEST_TYPE,
                READ_REQUEST,
                READ_VALUE,
                READ_INDEX,
                buf,
                READ_DATA_SIZE,
                0);
    if (readbytes != READ_DATA_SIZE)
        return LIBUSB_READ_ERR;
    fprintf(stdout,"read < ");

    for( int i = 0; i < READ_DATA_SIZE; i++ ) {
        data[i] = buf[i];
        fprintf(stdout, "%02x ", 0xff & ((unsigned int)data[i]));
    }
    fprintf(stdout, "\n");

    return OK;
}

void afx_kbd(int r, int g, int b)
{
    char rply[8];

    unsigned char keys[6][9]={
        {0x02,0x07,0x04,0x00,0x00,0x00,0x00,0x00,0x00}, // RST
        {0x02,0x03,0x00,0x00,0x00,0x01,0xff,0xf0,0x00}, // keyboard
        {0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // END
        {0x02,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // PGE
        {0x02,0x07,0x04,0x00,0x00,0x00,0x00,0x00,0x00}, // RST
        {0x02,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00}  // CHK
    };
    libusb_context *usbcontext;
    libusb_device_handle *usbhandle;
    int retval;
    int ready;
    int i;

    retval = InitDevice(&usbcontext, &usbhandle, ALIENWARE_VENDID, ALIENWARE_PRODID);
    usbdetach(usbhandle);
    // AW_RESET;
    if( retval == OK )
        usbread(usbhandle, rply, 8);

    // keys[1][6] = (r << 4) & 0xf0;
    // keys[1][6]|= g & 0x0f;
    // keys[1][7] = (b << 4) & 0xf0;

    r *= 16;
    g *= 16;
    b *= 16;
    printf("Changing AlienFX color to rgb(%d, %d, %d)\n", r, g, b);
    printf("Changing AlienFX color to HEX(%x, %x, %x)\n", r, g, b);
    keys[1][6] = (r & 0xf0) | ((g >> 4) & 0x0F);
    keys[1][7] = b & 0xf0;

    // r /= 16;
    // g /= 16;
    // b /= 16;
    // keys[1][6] = (r << 4) | g;
    // keys[1][7] = b << 4;

    // #55cc55 becomes #cc55cc
    // #00bbee becomes #00eebb
    // #aaffff becomes #ffaaff
    // #ffee44 becomes #ee44ff

    for( i = 0; i < 5; i++ ) {
      if( retval == OK ) // XXX?
        usbwrite(usbhandle, keys[i], 9);
    }

    // Mutex_lock
   ready = 0;
   while( !ready ) {
     usbwrite(usbhandle, keys[5], 9); // CHK
     usbread(usbhandle, rply, 8);
     if( rply[0] == 0x11 ) ready = 1;
   }

   libusb_close(usbhandle);
   libusb_exit(usbcontext);

/*
    unsigned char chk[]={0x02,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        for (r=15;r>=0;r--)
        {
            keys[1][6]=(r<<4);
            keys[1][6]|=g;
            keys[1][7]=(b<<4);
            for (i=0;i<5;i++)
            {
                if (retval==OK)
                {
                    usbwrite(usbhandle,keys[i],9);
                }
            }
            ready=0;
            while (!ready)
            {
                usbwrite(usbhandle,chk,9);
                usbread(usbhandle,rply,8);
                if (rply[0]==0x11) ready=1;
            }
        }
*/
}

