#include "graphicFX.h"


// Rotate_Image 函数：https://codebus.cn/lostperson/rotate
// 
// IMAGE * pTo : 新图
// IMAGE * pFrom : 原图
// double rad : 旋转的弧度
void RotateImage(IMAGE* pTo, IMAGE* pFrom, double rad)
{
	IMAGE* pWorking = GetWorkingImage();

	int iWidth = pFrom->getwidth();
	int iHeight = pFrom->getheight();												// 获取原图长宽

	while (rad > 2 * PI)													// 化简弧度
		rad -= 2 * PI;

	double pad = rad;														// 处理弧度
	if (pad > PI / 2 && pad <= PI)
	{
		pad -= PI / 2;
		pad = PI / 2 - pad;
	}
	else if (pad > PI && pad <= PI / 2 * 3)
	{
		pad -= PI;
	}
	else if (pad > PI / 2 * 3 && pad <= PI * 2)
	{
		pad -= PI / 2 * 3;
		pad = PI / 2 - pad;
	}

	int	tWidth = int(iWidth * cos(pad) + iHeight * sin(pad));
	int	tHeight = int(iHeight * cos(pad) + iWidth * sin(pad));				// 计算新图大小

	int iMinX = -(iWidth / 2), iMinY = -(iHeight / 2);
	int iMaxX = iMinX + iWidth, iMaxY = iMinY + iHeight;					// 计算原图最小（大）坐标

	int tMinX = -(tWidth / 2), tMinY = -(tHeight / 2);
	int tMaxX = tMinX + tWidth, tMaxY = tMinY + tHeight;					// 计算新图最小（大）坐标

	SetWorkingImage(pFrom);
	setorigin(-iMinX, -iMinY);												// 设置图片中心为原点

	SetWorkingImage(NULL);
	pTo->Resize(tWidth, tHeight);											// 初始化新图

	DWORD* dst = GetImageBuffer(pTo);
	DWORD* src = GetImageBuffer(pFrom);										// 获取新图、原图的缓冲区

	for (int y1 = 0; y1 < tHeight; y1++)
	{
		for (int x1 = 0; x1 < tWidth; x1++)
			dst[x1] = 0x00000000;
		dst += tWidth;
	}

	for (int y1 = 0; y1 < tHeight; y1++)									// 初始化新图
		dst -= tWidth;

	for (int y1 = tMinY; y1 < tMaxY; y1++)
	{
		for (int x1 = tMinX; x1 < tMaxX; x1++)
		{
			int x = int(x1 * cos(rad) - y1 * sin(rad));
			int y = int(x1 * sin(rad) + y1 * cos(rad));						// 计算变换后坐标

			int sxy = (iHeight - (y - iMinY) - 1) * iWidth + (x - iMinX);
			int dxy = (tHeight - (y1 - tMinY) - 1) * tWidth + (x1 - tMinX);	// 计算坐标在缓冲区的位置

			if (x >= iMinX && x < iMaxX && y >= iMinY && y < iMaxY)			// 越界特判
				dst[dxy] = src[sxy];
		}
	}

	SetWorkingImage(pFrom);
	setorigin(0, 0);
	SetWorkingImage(pWorking);												// 还原原图坐标
}


// 半透明贴图函数：https://codebus.cn/yangw/transparent-putimage
// 参数：
//		dstimg：目标 IMAGE（NULL 表示默认窗体）
//		x, y:	目标贴图位置
//		srcimg: 源 IMAGE 对象指针
void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg)
{
	// 变量初始化
	DWORD* dst = GetImageBuffer(dstimg);
	DWORD* src = GetImageBuffer(srcimg);
	int src_width = srcimg->getwidth();
	int src_height = srcimg->getheight();
	int dst_width = (dstimg == NULL ? getwidth() : dstimg->getwidth());
	int dst_height = (dstimg == NULL ? getheight() : dstimg->getheight());

	// 计算贴图的实际长宽
	int iwidth = (x + src_width > dst_width) ? dst_width - x : src_width;		// 处理超出右边界
	int iheight = (y + src_height > dst_height) ? dst_height - y : src_height;	// 处理超出下边界
	if (x < 0) { src += -x;				iwidth -= -x;	x = 0; }				// 处理超出左边界
	if (y < 0) { src += src_width * -y;	iheight -= -y;	y = 0; }				// 处理超出上边界

	// 修正贴图起始位置
	dst += dst_width * y + x;

	// 实现透明贴图
	for (int iy = 0; iy < iheight; iy++)
	{
		for (int ix = 0; ix < iwidth; ix++)
		{
			int sa = ((src[ix] & 0xff000000) >> 24);
			int sr = ((src[ix] & 0xff0000) >> 16);	// 源值已经乘过了透明系数
			int sg = ((src[ix] & 0xff00) >> 8);		// 源值已经乘过了透明系数
			int sb = src[ix] & 0xff;				// 源值已经乘过了透明系数
			int dr = ((dst[ix] & 0xff0000) >> 16);
			int dg = ((dst[ix] & 0xff00) >> 8);
			int db = dst[ix] & 0xff;

			dst[ix] = ((sr + dr * (255 - sa) / 255) << 16)
				| ((sg + dg * (255 - sa) / 255) << 8)
				| (sb + db * (255 - sa) / 255);
		}
		dst += dst_width;
		src += src_width;
	}
}


// 图片模糊函数：@六月
// 参数：
//		output (IMAGE*):	输出图片（需大于等于原图片尺寸）
//		source (IMAGE*):	输入图片（不会作更改）
//		radius (int):		模糊半径
// 注意事项：
//	-	两个函数一起用才有效果
void LINEAR_BLUR_X(IMAGE* output, IMAGE* source, int radius, bool Alpha)
{
	DWORD* srcImageBuffer = GetImageBuffer(source);
	DWORD* outImageBuffer = GetImageBuffer(output);

	int imageWidth = source->getwidth(), imageHeight = source->getheight();

	int const totalSideCount = (radius * 2);

	double averageR = 0, averageG = 0, averageB = 0, averageA = 0;

	if (!Alpha)
	{
		for (int Y = 0; Y < imageHeight; ++Y)
		{

			// 先执行一次完全的
			DWORD* pos = srcImageBuffer + (Y * imageWidth);
			averageR = double((*(pos) & 0x00FF0000) >> 16) * radius;
			averageG = double((*(pos) & 0x0000FF00) >> 8) * radius;
			averageB = double((*(pos) & 0x000000FF)) * radius;

			for (int collectX = 0; collectX <= radius; ++collectX)
			{
				if (collectX < imageWidth)
				{
					averageR += double((*(pos + collectX) & 0x00FF0000) >> 16);
					averageG += double((*(pos + collectX) & 0x0000FF00) >> 8);
					averageB += double((*(pos + collectX) & 0x000000FF));
				}
				else
				{
					averageR += double((*(pos + imageWidth - 1) & 0x00FF0000) >> 16);
					averageG += double((*(pos + imageWidth - 1) & 0x0000FF00) >> 8);
					averageB += double((*(pos + imageWidth - 1) & 0x000000FF));
				}
			}

			averageR /= totalSideCount;
			averageG /= totalSideCount;
			averageB /= totalSideCount;

			*(outImageBuffer + (Y * imageWidth)) =
				((static_cast<DWORD>(averageR)) << 16) |
				((static_cast<DWORD>(averageG)) << 8) |
				((static_cast<DWORD>(averageB)));

			// 接下来只需要重复即可
			for (int X = 1; X < imageWidth; ++X)
			{
				DWORD* pos = srcImageBuffer + (Y * imageWidth);
				int toBeMinused = X - radius;
				int toBeAdded = X + radius - 1;

				// 减去最前面的
				if (toBeMinused < 0) {
					averageR -= double((*(pos) & 0x00FF0000) >> 16) / totalSideCount;
					averageG -= double((*(pos) & 0x0000FF00) >> 8) / totalSideCount;
					averageB -= double((*(pos) & 0x000000FF)) / totalSideCount;
				}
				else
				{
					averageR -= double((*(pos + toBeMinused) & 0x00FF0000) >> 16) / totalSideCount;
					averageG -= double((*(pos + toBeMinused) & 0x0000FF00) >> 8) / totalSideCount;
					averageB -= double((*(pos + toBeMinused) & 0x000000FF)) / totalSideCount;
				}

				// 加上最后面的
				if (toBeAdded >= imageWidth)
				{
					averageR += double((*(pos + imageWidth - 1) & 0x00FF0000) >> 16) / totalSideCount;
					averageG += double((*(pos + imageWidth - 1) & 0x0000FF00) >> 8) / totalSideCount;
					averageB += double((*(pos + imageWidth - 1) & 0x000000FF)) / totalSideCount;
				}
				else
				{
					averageR += double((*(pos + toBeAdded) & 0x00FF0000) >> 16) / totalSideCount;
					averageG += double((*(pos + toBeAdded) & 0x0000FF00) >> 8) / totalSideCount;
					averageB += double((*(pos + toBeAdded) & 0x000000FF)) / totalSideCount;
				}

				*(outImageBuffer + (Y * imageWidth) + X) =
					((static_cast<DWORD>(averageR)) << 16) |
					((static_cast<DWORD>(averageG)) << 8) |
					((static_cast<DWORD>(averageB)));
			}

			averageR = 0, averageG = 0, averageB = 0;
		}
	}
	else
	{
		for (int Y = 0; Y < imageHeight; ++Y)
		{

			// 先执行一次完全的
			DWORD* pos = srcImageBuffer + (Y * imageWidth);
			averageA = double((*(pos) & 0xFF000000) >> 24) * radius;
			averageR = double((*(pos) & 0x00FF0000) >> 16) * radius;
			averageG = double((*(pos) & 0x0000FF00) >> 8) * radius;
			averageB = double((*(pos) & 0x000000FF)) * radius;

			for (int collectX = 0; collectX <= radius; ++collectX)
			{
				if (collectX < imageWidth)
				{
					averageA += double((*(pos + collectX) & 0xFF000000) >> 24);
					averageR += double((*(pos + collectX) & 0x00FF0000) >> 16);
					averageG += double((*(pos + collectX) & 0x0000FF00) >> 8);
					averageB += double((*(pos + collectX) & 0x000000FF));
				}
				else
				{
					averageR += double((*(pos + imageWidth - 1) & 0xFF000000) >> 24);
					averageR += double((*(pos + imageWidth - 1) & 0x00FF0000) >> 16);
					averageG += double((*(pos + imageWidth - 1) & 0x0000FF00) >> 8);
					averageB += double((*(pos + imageWidth - 1) & 0x000000FF));
				}
			}

			averageA /= totalSideCount;
			averageR /= totalSideCount;
			averageG /= totalSideCount;
			averageB /= totalSideCount;

			*(outImageBuffer + (Y * imageWidth)) =
				((static_cast<DWORD>(averageA)) << 24) |
				((static_cast<DWORD>(averageR)) << 16) |
				((static_cast<DWORD>(averageG)) << 8) |
				((static_cast<DWORD>(averageB)));

			// 接下来只需要重复即可
			for (int X = 1; X < imageWidth; ++X)
			{
				DWORD* pos = srcImageBuffer + (Y * imageWidth);
				int toBeMinused = X - radius;
				int toBeAdded = X + radius - 1;

				// 减去最前面的
				if (toBeMinused < 0) {
					averageA -= double((*(pos) & 0xFF000000) >> 24) / totalSideCount;
					averageR -= double((*(pos) & 0x00FF0000) >> 16) / totalSideCount;
					averageG -= double((*(pos) & 0x0000FF00) >> 8) / totalSideCount;
					averageB -= double((*(pos) & 0x000000FF)) / totalSideCount;
				}
				else
				{
					averageA -= double((*(pos + toBeMinused) & 0xFF000000) >> 24) / totalSideCount;
					averageR -= double((*(pos + toBeMinused) & 0x00FF0000) >> 16) / totalSideCount;
					averageG -= double((*(pos + toBeMinused) & 0x0000FF00) >> 8) / totalSideCount;
					averageB -= double((*(pos + toBeMinused) & 0x000000FF)) / totalSideCount;
				}

				// 加上最后面的
				if (toBeAdded >= imageWidth)
				{
					averageA += double((*(pos + imageWidth - 1) & 0xFF000000) >> 24) / totalSideCount;
					averageR += double((*(pos + imageWidth - 1) & 0x00FF0000) >> 16) / totalSideCount;
					averageG += double((*(pos + imageWidth - 1) & 0x0000FF00) >> 8) / totalSideCount;
					averageB += double((*(pos + imageWidth - 1) & 0x000000FF)) / totalSideCount;
				}
				else
				{
					averageA += double((*(pos + toBeAdded) & 0xFF000000) >> 24) / totalSideCount;
					averageR += double((*(pos + toBeAdded) & 0x00FF0000) >> 16) / totalSideCount;
					averageG += double((*(pos + toBeAdded) & 0x0000FF00) >> 8) / totalSideCount;
					averageB += double((*(pos + toBeAdded) & 0x000000FF)) / totalSideCount;
				}

				*(outImageBuffer + (Y * imageWidth) + X) =
					((static_cast<DWORD>(averageA)) << 24) |
					((static_cast<DWORD>(averageR)) << 16) |
					((static_cast<DWORD>(averageG)) << 8) |
					((static_cast<DWORD>(averageB)));
			}

			averageA = 0, averageR = 0, averageG = 0, averageB = 0;
		}
	}
}

void LINEAR_BLUR_Y(IMAGE* output, IMAGE* source, int radius, bool Alpha)
{
	DWORD* srcImageBuffer = GetImageBuffer(source);
	DWORD* outImageBuffer = GetImageBuffer(output);

	int imageWidth = source->getwidth(), imageHeight = source->getheight();

	int const totalSideCount = (radius * 2);

	double averageR = 0, averageG = 0, averageB = 0;

	for (int X = 0; X < imageWidth; ++X)
	{
		// 先执行一次完全的
		DWORD* pos = srcImageBuffer + X;
		for (int collectY = -radius; collectY < radius; ++collectY)
		{
			DWORD* pos = srcImageBuffer + X;
			averageR += double(
				(collectY >= 0 && collectY < imageHeight) ?
				(*(pos + collectY * imageWidth) & 0x00FF0000) >> 16 :
				(*(pos) & 0x00FF0000) >> 16
				);

			averageG += double(
				(collectY >= 0 && collectY < imageHeight) ?
				(*(pos + collectY * imageWidth) & 0x0000FF00) >> 8 :
				(*(pos) & 0x0000FF00) >> 8
				);

			averageB += double(
				(collectY >= 0 && collectY < imageHeight) ?
				(*(pos + collectY * imageWidth) & 0x000000FF) :
				(*(pos) & 0x000000FF)
				);
		}

		averageR /= totalSideCount;
		averageG /= totalSideCount;
		averageB /= totalSideCount;

		*(outImageBuffer + X) =
			((DWORD(averageR)) << 16) |
			((DWORD(averageG)) << 8) |
			((DWORD(averageB)));

		// 接下来只需要重复即可
		for (int Y = 1; Y < imageHeight; ++Y)
		{
			DWORD* pos = srcImageBuffer + X;
			int toBeMinused = Y - radius;
			int toBeAdded = Y + radius - 1;

			// 减去最前面的
			if (toBeMinused < 0) {
				averageR -= double((*(pos) & 0x00FF0000) >> 16) / totalSideCount;
				averageG -= double((*(pos) & 0x0000FF00) >> 8) / totalSideCount;
				averageB -= double((*(pos) & 0x000000FF)) / totalSideCount;
			}
			else
			{
				averageR -= double((*(pos + toBeMinused * imageWidth) & 0x00FF0000) >> 16) / totalSideCount;
				averageG -= double((*(pos + toBeMinused * imageWidth) & 0x0000FF00) >> 8) / totalSideCount;
				averageB -= double((*(pos + toBeMinused * imageWidth) & 0x000000FF)) / totalSideCount;
			}


			// 加上最后面的
			if (toBeAdded >= imageHeight)
			{
				averageR += double((*(pos + (imageHeight - 1) * imageWidth) & 0x00FF0000) >> 16) / totalSideCount;
				averageG += double((*(pos + (imageHeight - 1) * imageWidth) & 0x0000FF00) >> 8) / totalSideCount;
				averageB += double((*(pos + (imageHeight - 1) * imageWidth) & 0x000000FF)) / totalSideCount;
			}
			else
			{
				averageR += double((*(pos + toBeAdded * imageWidth) & 0x00FF0000) >> 16) / totalSideCount;
				averageG += double((*(pos + toBeAdded * imageWidth) & 0x0000FF00) >> 8) / totalSideCount;
				averageB += double((*(pos + toBeAdded * imageWidth) & 0x000000FF)) / totalSideCount;
			}

			*(outImageBuffer + (Y * imageWidth) + X) =
				((DWORD(averageR)) << 16) |
				((DWORD(averageG)) << 8) |
				((DWORD(averageB)));
		}
	}
}

// 通过 Alpha 掩码图创建阴影（基于图片模糊函数）：@六月
void CreateShadowEffect(IMAGE* output, IMAGE* source, int radius) {

}