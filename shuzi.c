#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

#define NUMX 2
#define NUMY 16

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *frameBuffer = 0;
short color16;


//打印fb驱动中fix结构信息，注：在fb驱动加载后，fix结构不可被修改。
void printFixedInfo ()
{
    printf ("Fixed screen info:\n"
        "\tid: %s\n"
        "\tsmem_start: 0x%lx\n"
        "\tsmem_len: %d\n"
        "\ttype: %d\n"
        "\ttype_aux: %d\n"
        "\tvisual: %d\n"
        "\txpanstep: %d\n"
        "\typanstep: %d\n"
        "\tywrapstep: %d\n"
        "\tline_length: %d\n"
        "\tmmio_start: 0x%lx\n"
        "\tmmio_len: %d\n"
        "\taccel: %d\n"
        "\n",
        finfo.id, finfo.smem_start, finfo.smem_len, finfo.type,
        finfo.type_aux, finfo.visual, finfo.xpanstep, finfo.ypanstep,
        finfo.ywrapstep, finfo.line_length, finfo.mmio_start,
        finfo.mmio_len, finfo.accel);
}

//打印fb驱动中var结构信息，注：fb驱动加载后，var结构可根据实际需要被重置
void printVariableInfo ()
{
    printf ("Variable screen info:\n"
        "\txres: %d\n"
        "\tyres: %d\n"
        "\txres_virtual: %d\n"
        "\tyres_virtual: %d\n"
        "\tyoffset: %d\n"
        "\txoffset: %d\n"
        "\tbits_per_pixel: %d\n"
        "\tgrayscale: %d\n"
        "\tred: offset: %2d, length: %2d, msb_right: %2d\n"
        "\tgreen: offset: %2d, length: %2d, msb_right: %2d\n"
        "\tblue: offset: %2d, length: %2d, msb_right: %2d\n"
        "\ttransp: offset: %2d, length: %2d, msb_right: %2d\n"
        "\tnonstd: %d\n"
        "\tactivate: %d\n"
        "\theight: %d\n"
        "\twidth: %d\n"
        "\taccel_flags: 0x%x\n"
        "\tpixclock: %d\n"
        "\tleft_margin: %d\n"
        "\tright_margin: %d\n"
        "\tupper_margin: %d\n"
        "\tlower_margin: %d\n"
        "\thsync_len: %d\n"
        "\tvsync_len: %d\n"
        "\tsync: %d\n"
        "\tvmode: %d\n"
        "\n",
        vinfo.xres, vinfo.yres, vinfo.xres_virtual, vinfo.yres_virtual,
        vinfo.xoffset, vinfo.yoffset, vinfo.bits_per_pixel,
        vinfo.grayscale, vinfo.red.offset, vinfo.red.length,
        vinfo.red.msb_right, vinfo.green.offset, vinfo.green.length,
        vinfo.green.msb_right, vinfo.blue.offset, vinfo.blue.length,
        vinfo.blue.msb_right, vinfo.transp.offset, vinfo.transp.length,
        vinfo.transp.msb_right, vinfo.nonstd, vinfo.activate,
        vinfo.height, vinfo.width, vinfo.accel_flags, vinfo.pixclock,
        vinfo.left_margin, vinfo.right_margin, vinfo.upper_margin,
        vinfo.lower_margin, vinfo.hsync_len, vinfo.vsync_len,
        vinfo.sync, vinfo.vmode);
}

//32位颜色转16位颜色
int rgb32_to_rgb16(int color)
{
    const int red = (color & 0xff0000) >> (16 + 3);
    const int green = (color & 0xff00) >> (8 + 2);
    const int blue = (color & 0xff) >> 3;
    color16 = blue | (green << 5) | (red << (5 + 6));
	return color16;
}

//数字0
void draw0 (int x0, int y0, int width, int height, int color)
{
    const int bytesPerPixel = 2;
    const int stride = finfo.line_length / bytesPerPixel;
    color16 = rgb32_to_rgb16(color);
	short rgb;
    short *dest = (short *) (frameBuffer) + (y0 + vinfo.yoffset) * stride + (x0 + vinfo.xoffset);

    int x, y;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            if(y == 0 || x == 0 || y == (height - 1) || x == (width - 1))
			{
				rgb = dest[x];
				printf("rgb = %x\n",rgb);
				dest[x] = color16;
				rgb = dest[x];
				printf("rgb = %x\n",rgb);
			}   
        }
        dest += stride;
    }
}

//汉字杨
void draw_y (int x0, int y0, int color)
{
    const int bytesPerPixel = 2;
    const int stride = finfo.line_length / bytesPerPixel;
	int drawpoint[NUMY][NUMX] = {0x08,0x00,0xC8,0x1F,0x08,0x08,0x7F,0x04,0x08,0x02,0x08,0x01,0x9C,0x3F,0x2C,0x2A,0x2A,0x29,0x0A,0x25,0x89,0x24,0x48,0x22,0x08,0x21,0x88,0x20,0x08,0x28,0x08,0x10};
    color16 = rgb32_to_rgb16(color);
    short *dest = (short *) (frameBuffer) + (y0 + vinfo.yoffset) * stride + (x0 + vinfo.xoffset);

    int x,i,j,b,k;
	for (i = 0; i < NUMY; ++i)
	{
		for(j = 0; j < NUMX; ++j)
		{
			for (b = 1,k = 0; k < 7; ++k)
			{
				if ((drawpoint[i][j] & b))
				{
					x = k;
					if (j == 1)
						x = k + 8;
					dest[x] = color16;
				}
				b = 1 << (k + 1);
			}
		}
		dest += stride;
	}
}

//汉字贺
void draw_h (int x0, int y0, int color)
{
    const int bytesPerPixel = 2;
    const int stride = finfo.line_length / bytesPerPixel;
	int drawpoint[NUMY][NUMX] = {0x10,0x00,0xFE,0x3E,0x90,0x22,0x90,0x22,0x88,0x3E,0xA4,0x22,0x42,0x00,0xF0,0x0F,0x10,0x08,0x10,0x09,0x10,0x09,0x10,0x09,0x90,0x02,0x40,0x0C,0x30,0x38,0x0C,0x10};
    color16 = rgb32_to_rgb16(color);
    short *dest = (short *) (frameBuffer) + (y0 + vinfo.yoffset) * stride + (x0 + vinfo.xoffset);

    int x,i,j,b,k;
	for (i = 0; i < NUMY; ++i)
	{
		for(j = 0; j < NUMX; ++j)
		{
			for (b = 1,k = 0; k < 7; ++k)
			{
				if ((drawpoint[i][j] & b))
				{
					x = k;
					if (j == 1)
						x = k + 8;
					dest[x] = color16;
				}
				b = 1 << (k + 1);
			}
		}
		dest += stride;
	}
}

//数字1
void draw1 (int x0, int y0, int width, int height, int color)
{
    const int bytesPerPixel = 2;
    const int stride = finfo.line_length / bytesPerPixel;
    color16 = rgb32_to_rgb16(color);
    short *dest = (short *) (frameBuffer) + (y0 + vinfo.yoffset) * stride + (x0 + vinfo.xoffset);

    int x, y;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            if(x == 0)
                dest[x] = color16;
        }
        dest += stride;
    }
}

//数字2
void draw2 (int x0, int y0, int width, int height, int color)
{
    const int bytesPerPixel = 2;
    const int stride = finfo.line_length / bytesPerPixel;
    color16 = rgb32_to_rgb16(color);
    short *dest = (short *) (frameBuffer) + (y0 + vinfo.yoffset) * stride + (x0 + vinfo.xoffset);

    int x, y;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            if(y == 0 || y == (height - 1) || y == (height/2) || ((x ==0) && ((height/2) <= y) && ((height -1) >= y)) || ((x == (width -1)) && (0 <= y) && ((height/2) >= y)))
                dest[x] = color16;
        }
        dest += stride;
    }
}

//数字3
void draw3 (int x0, int y0, int width, int height, int color)
{
    const int bytesPerPixel = 2;
    const int stride = finfo.line_length / bytesPerPixel;
    color16 = rgb32_to_rgb16(color);
    short *dest = (short *) (frameBuffer) + (y0 + vinfo.yoffset) * stride + (x0 + vinfo.xoffset);

    int x, y;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            if(y == 0 || y == (height - 1) || y == (height/2) || x == (width - 1))
                dest[x] = color16;
        }
        dest += stride;
    }
}

//数字4
void draw4 (int x0, int y0, int width, int height, int color)
{
    const int bytesPerPixel = 2;
    const int stride = finfo.line_length / bytesPerPixel;
    color16 = rgb32_to_rgb16(color);
    short *dest = (short *) (frameBuffer) + (y0 + vinfo.yoffset) * stride + (x0 + vinfo.xoffset);

    int x, y;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            if(y == (height/2) || x == (width - 1) || ((x == 0) && (0 <= y) && ((height/2) >= y)))
                dest[x] = color16;
        }
        dest += stride;
    }
}
//数字5
void draw5 (int x0, int y0, int width, int height, int color)
{
    const int bytesPerPixel = 2;
    const int stride = finfo.line_length / bytesPerPixel;
    color16 = rgb32_to_rgb16(color);
    short *dest = (short *) (frameBuffer) + (y0 + vinfo.yoffset) * stride + (x0 + vinfo.xoffset);

    int x, y;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            if(y == 0 || y == (height - 1) || y == (height/2) || ((x ==0) && ((height/2) >= y) && (0 <= y)) || ((x == (width -1)) && ((height - 1) >= y) && ((height/2) <= y)))
                dest[x] = color16;
        }
        dest += stride;
    }
}

//数字6
void draw6 (int x0, int y0, int width, int height, int color)
{
    const int bytesPerPixel = 2;
    const int stride = finfo.line_length / bytesPerPixel;
    color16 = rgb32_to_rgb16(color);
    short *dest = (short *) (frameBuffer) + (y0 + vinfo.yoffset) * stride + (x0 + vinfo.xoffset);

    int x, y;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            if(y == 0 || y == (height - 1) || y == (height/2) || x == 0 ||((x == (width -1)) && ((height - 1) >= y) && ((height/2) <= y)))
                dest[x] = color16;
        }
        dest += stride;
    }
}

//数字7
void draw7 (int x0, int y0, int width, int height, int color)
{
    const int bytesPerPixel = 2;
    const int stride = finfo.line_length / bytesPerPixel;
    color16 = rgb32_to_rgb16(color);
    short *dest = (short *) (frameBuffer) + (y0 + vinfo.yoffset) * stride + (x0 + vinfo.xoffset);

    int x, y;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            if(x == (width - 1) || y == 0)
                dest[x] = color16;
        }
        dest += stride;
    }
}

//数字8
void draw8 (int x0, int y0, int width, int height, int color)
{
    const int bytesPerPixel = 2;
    const int stride = finfo.line_length / bytesPerPixel;
    color16 = rgb32_to_rgb16(color);
    short *dest = (short *) (frameBuffer) + (y0 + vinfo.yoffset) * stride + (x0 + vinfo.xoffset);

    int x, y;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            if(y == 0 || x == 0 || y == (height - 1) || x == (width - 1) || y == (height/2))
                dest[x] = color16;
        }
        dest += stride;
    }
}

//数字9
void draw9 (int x0, int y0, int width, int height, int color)
{
    const int bytesPerPixel = 2;
    const int stride = finfo.line_length / bytesPerPixel;
    color16 = rgb32_to_rgb16(color);
    short *dest = (short *) (frameBuffer) + (y0 + vinfo.yoffset) * stride + (x0 + vinfo.xoffset);

    int x, y;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            if(y == 0 || y == (height - 1) || x == (width - 1) || y == (height/2) || ((x ==0) && ((height/2) >= y) && (0 <= y)))
                dest[x] = color16;
        }
        dest += stride;
    }
}

int main (int argc, char **argv)
{
    const char *devfile = "/dev/fb0";
    long int screensize = 0;
    int fbFd = 0;
    if (argc < 2)
    {
        printf("framebuffer test\n");
        printf("Usage: %s devpath",argv[0]);
        printf("sample:%s /dev/fb0\n",argv[0]);
        exit(1);
    }
    printf("framebuffer test using device %s\n",argv[1]);
    /* Open the file for reading and writing */
    fbFd = open (devfile, O_RDWR);
    if (fbFd == -1)
    {
        perror ("Error: cannot open framebuffer device");
        exit (1);
    }


    //获取finfo信息并显示
    if (ioctl (fbFd, FBIOGET_FSCREENINFO, &finfo) == -1)
    {
        perror ("Error reading fixed information");
        exit (2);
    }
    printFixedInfo ();
    //获取vinfo信息并显示
    if (ioctl (fbFd, FBIOGET_VSCREENINFO, &vinfo) == -1)
    {
        perror ("Error reading variable information");
        exit (3);
    }
    printVariableInfo ();


    /* Figure out the size of the screen in bytes */
    screensize = finfo.smem_len;
    /* Map the device to memory */
    frameBuffer =
        (char *) mmap (0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,fbFd, 0);
    if (frameBuffer == MAP_FAILED)
    {
        perror ("Error: Failed to map framebuffer device to memory");
        exit (4);
    }

    printf ("Will draw 0-2 rectangles on the screen!\n");
	draw_y (400, 200, 0xff00ff00);
	draw_h (500, 200, 0xff00ff00);
    draw0 (20, 20, 20, 40, 0xff00ff00);
	sleep (2);
	draw1 (60, 20, 20, 40, 0xff00ff00);
    sleep (2);
	draw2 (80, 20, 20, 40, 0xff00ff00);
    sleep (2);
	draw3 (120, 20, 20, 40, 0xff00ff00);
    sleep (2);
	draw4 (160, 20, 20, 40, 0xff00ff00);
    sleep (2);
	draw5 (200, 20, 20, 40, 0xff00ff00);
    sleep (2);
	draw6 (240, 20, 20, 40, 0xff00ff00);
    sleep (2);
	draw7 (280, 20, 20, 40, 0xff00ff00);
    sleep (2);
	draw8 (320, 20, 20, 40, 0xff00ff00);
    sleep (2);
	draw9 (360, 20, 20, 40, 0xff00ff00);
    printf (" Done.\n");


    munmap (frameBuffer, screensize); //解除内存映射，与mmap对应


    close (fbFd);
    return 0;
}
