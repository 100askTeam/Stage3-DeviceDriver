#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "font.h"

#define OLED_IOC_INIT 			123
#define OLED_IOC_SET_POS 		124


//为0 表示命令，为1表示数据
#define OLED_CMD 	0
#define OLED_DATA 	1

static int fd_spidev;
static int dc_pin_num;


void OLED_DIsp_Set_Pos(int x, int y);

void oled_write_datas(const unsigned char *buf, int len)
{
	write(fd_spidev, buf, len);
}

		  			 		  						  					  				 	   		  	  	 	  		  			 		  						  					  				 	   		  	  	 	  

/**********************************************************************
	 * 函数名称： OLED_DIsp_Clear
	 * 功能描述： 整个屏幕显示数据清0
	 * 输入参数：无
	 * 输出参数： 无
	 * 返 回 值： 
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/15		 V1.0	  芯晓		  创建
 ***********************************************************************/
void OLED_DIsp_Clear(void)  
{
    unsigned char x, y;
	char buf[128];

	memset(buf, 0, 128);
	
    for (y = 0; y < 8; y++)
    {
        OLED_DIsp_Set_Pos(0, y);
        oled_write_datas(buf, 128);
    }
}

/**********************************************************************
	 * 函数名称： OLED_DIsp_All
	 * 功能描述： 整个屏幕显示全部点亮，可以用于检查坏点
	 * 输入参数：无
	 * 输出参数：无 
	 * 返 回 值：
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/15		 V1.0	  芯晓		  创建
 ***********************************************************************/
void OLED_DIsp_All(void)  
{
    unsigned char x, y;
	char buf[128];

	memset(buf, 0xff, 128);
	
    for (y = 0; y < 8; y++)
    {
        OLED_DIsp_Set_Pos(0, y);
        oled_write_datas(buf, 128);
    }



}

//坐标设置
/**********************************************************************
	 * 函数名称： OLED_DIsp_Set_Pos
	 * 功能描述：设置要显示的位置
	 * 输入参数：@ x ：要显示的column address
	 			@y :要显示的page address
	 * 输出参数： 无
	 * 返 回 值： 
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/15		 V1.0	  芯晓		  创建
 ***********************************************************************/
void OLED_DIsp_Set_Pos(int x, int y)
{ 	
	ioctl(fd_spidev, OLED_IOC_SET_POS, x  | (y << 8));
}   	      	   			 
/**********************************************************************
	  * 函数名称： OLED_DIsp_Char
	  * 功能描述：在某个位置显示字符 1-9
	  * 输入参数：@ x ：要显示的column address
		 			@y :要显示的page address
		 			@c ：要显示的字符的ascii码
	  * 输出参数： 无
	  * 返 回 值： 
	  * 修改日期		版本号	  修改人 	   修改内容
	  * -----------------------------------------------
	  * 2020/03/15		  V1.0	   芯晓		   创建
***********************************************************************/
void OLED_DIsp_Char(int x, int y, unsigned char c)
{
	int i = 0;
	/* 得到字模 */
	const unsigned char *dots = oled_asc2_8x16[c - ' '];

	/* 发给OLED */
	OLED_DIsp_Set_Pos(x, y);
	/* 发出8字节数据 */
	//for (i = 0; i < 8; i++)
	//	oled_write_cmd_data(dots[i], OLED_DATA);
	oled_write_datas(&dots[0], 8);

	OLED_DIsp_Set_Pos(x, y+1);
	/* 发出8字节数据 */
	//for (i = 0; i < 8; i++)
		//oled_write_cmd_data(dots[i+8], OLED_DATA);
	oled_write_datas(&dots[8], 8);
}


/**********************************************************************
	 * 函数名称： OLED_DIsp_String
	 * 功能描述： 在指定位置显示字符串
	 * 输入参数：@ x ：要显示的column address
		 			@y :要显示的page address
		 			@str ：要显示的字符串
	 * 输出参数： 无
	 * 返 回 值： 无
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/15		 V1.0	  芯晓		  创建
***********************************************************************/
void OLED_DIsp_String(int x, int y, char *str)
{
	unsigned char j=0;
	while (str[j])
	{		
		OLED_DIsp_Char(x, y, str[j]);//显示单个字符
		x += 8;
		if(x > 127)
		{
			x = 0;
			y += 2;
		}//移动显示位置
		j++;
	}
}
/**********************************************************************
	 * 函数名称： OLED_DIsp_CHinese
	 * 功能描述：在指定位置显示汉字
	 * 输入参数：@ x ：要显示的column address
		 			@y :要显示的page address
		 			@chr ：要显示的汉字，三个汉字“百问网”中选择一个
	 * 输出参数： 无
	 * 返 回 值： 无
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/15		 V1.0	  芯晓		  创建
 ***********************************************************************/

void OLED_DIsp_CHinese(unsigned char x,unsigned char y,unsigned char no)
{      			    
	unsigned char t,adder=0;
	OLED_DIsp_Set_Pos(x,y);	
    for(t=0;t<16;t++)
	{//显示上半截字符	
		oled_write_datas(&hz_1616[no][t*2], 1);
		adder+=1;
    }	
	OLED_DIsp_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
	{//显示下半截字符
		oled_write_datas(&hz_1616[no][t*2+1], 1);
		adder+=1;
    }					
}
/**********************************************************************
	 * 函数名称： OLED_DIsp_Test
	 * 功能描述： 整个屏幕显示测试
	 * 输入参数：无
	 * 输出参数： 无
	 * 返 回 值： 无
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/15		 V1.0	  芯晓		  创建
 ***********************************************************************/
void OLED_DIsp_Test(void)
{ 	
	int i;
	
	OLED_DIsp_String(0, 0, "wiki.100ask.net");
	OLED_DIsp_String(0, 2, "book.100ask.net");
	OLED_DIsp_String(0, 4, "bbs.100ask.net");
	
	for(i = 0; i < 3; i++)
	{   //显示汉字 百问网
		OLED_DIsp_CHinese(32+i*16, 6, i);
	}
} 

/* spi_oled /dev/100ask_oled */
int main(int argc, char **argv)
{	
	if (argc != 2)
	{
		printf("Usage: %s /dev/100ask_oled\n", argv[0]);
		return -1;
	}

	fd_spidev = open(argv[1], O_RDWR);
	if (fd_spidev < 0) {
		printf("open %s err\n", argv[1]);
		return -1;
	}


	ioctl(fd_spidev, OLED_IOC_INIT);

	OLED_DIsp_Clear();
	
	OLED_DIsp_Test();

	return 0;
}

