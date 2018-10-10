
#include <Windows.h>
#include <stdlib.h>

typedef struct {
	int x;				//	对象的左上角位置（相对图象）
	int y;
	int beExist;		//	存在标记，0...不存在，1...存在
	int nAngle;			//	旋转角度  垂直向下是0度，右边是正角度，左边是负角度，只是对平面捺印指纹有用
	int nWidth;			//	对象宽度
	int nHeight;		//	对象长度
	int nObjectType;	//	对象类型，对这个参数必须事先赋值，输入参数中表示输入的数据的类型，输出参数中表示希望返回的对象类型
} REGIONRECT;

//判断平面指纹捺印区域和方向，如果捺印方向无法判断，
//函数返回值,    1....		判断成功，在pDestObj结构中返回采集框的左上角位置和指纹方向（垂直向下为0度，角度左负右正）
//				-1....		捺印方向无法判断时候，根据捺印区域的中心在pDestObj中返回采集框的左上角位置
//				KERROR_XXX..其他错误
//调用函数之前，需要在pDestObj中给采集框宽度和高度赋值
//pDestObj中指位信息nObjectType 赋值21...30, -1表示指位未知
extern int PlainImage_Splite(unsigned char *pImage, int nWidth, int nHeight, REGIONRECT *pDestObj);

//判断指掌纹捺印区域信息
//函数返回值：	>=0  区域的大小
//				< 0  错误信息 KERROR_***
extern int FPImage_AreaInfo(unsigned char *pImage,	//	输入的图像指针
						 int nWidth,				//	图像的宽度
						 int nHeight,				//	图像的高度
						 REGIONRECT *pDestObj);		//	返回的图像外接矩形信息，左上角位置和矩形的宽度高度