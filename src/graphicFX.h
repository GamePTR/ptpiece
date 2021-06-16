#pragma once

#include <easyx.h>
#include <cmath>

#include "StylerFont.h"

#define PI	3.141592653589793

// 保持透明度的旋转函数：https://codebus.cn/lostperson/rotate
// 参数：
//		IMAGE * pTo: 新图
//		IMAGE * pFrom: 原图
//		double rad: 旋转的弧度
void RotateImage(IMAGE* pTo, IMAGE* pFrom, double rad);


// 半透明贴图函数：https://codebus.cn/yangw/transparent-putimage
// 参数：
//		dstimg：目标 IMAGE（NULL 表示默认窗体）
//		x, y:	目标贴图位置
//		srcimg: 源 IMAGE 对象指针
void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg);


// 图片模糊函数：@六月
// 参数：
//		output (IMAGE*):	输出图片（需大于等于原图片尺寸）
//		source (IMAGE*):	输入图片（不会作更改）
//		radius (int):		模糊半径
// 注意事项：
//	-	两个函数一起用才有效果
void LINEAR_BLUR_X(IMAGE* output, IMAGE* source, int radius, bool Alpha = false);
void LINEAR_BLUR_Y(IMAGE* output, IMAGE* source, int radius, bool Alpha = false);


// 通过 Alpha 掩码图创建阴影（基于图片模糊函数）：@六月
void CreateShadowEffect(IMAGE* output, IMAGE* source, int radius);
