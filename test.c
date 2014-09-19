 /*************************************************************************
	> File Name: test.c
	> Author: he.yang
	> Mail: yanghe9012@126.com
	> Created Time: 2014年09月16日 星期二 15时18分32秒
  *************************************************************************/

#include <stdio.h>
#define NUMY 16
#define NUMX 2

//汉字杨
void draw_y ()
{
	int drawpoint[NUMY][NUMX] = {0x08,0x00,0xC8,0x1F,0x08,0x08,0x7F,0x04,0x08,0x02,0x08,0x01,0x9C,0x3F,0x2C,0x2A,0x2A,0x29,0x0A,0x25,0x89,0x24,0x48,0x22,0x08,0x21,0x88,0x20,0x08,0x28,0x08,0x10};
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
					//if (NUMX == 1)
					//	x = k + 8;
                    printf("*");
				}
                else printf(" ");
				b = 1 << (k + 1);
			}
		}
        printf("\n");
	}
}

int main()
{
   draw_y();
    return 0;
}
