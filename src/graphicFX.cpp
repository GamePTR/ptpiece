#include "graphicFX.h"


// Rotate_Image ������https://codebus.cn/lostperson/rotate
// 
// IMAGE * pTo : ��ͼ
// IMAGE * pFrom : ԭͼ
// double rad : ��ת�Ļ���
void RotateImage(IMAGE* pTo, IMAGE* pFrom, double rad)
{
	IMAGE* pWorking = GetWorkingImage();

	int iWidth = pFrom->getwidth();
	int iHeight = pFrom->getheight();												// ��ȡԭͼ����

	while (rad > 2 * PI)													// ���򻡶�
		rad -= 2 * PI;

	double pad = rad;														// ������
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
	int	tHeight = int(iHeight * cos(pad) + iWidth * sin(pad));				// ������ͼ��С

	int iMinX = -(iWidth / 2), iMinY = -(iHeight / 2);
	int iMaxX = iMinX + iWidth, iMaxY = iMinY + iHeight;					// ����ԭͼ��С��������

	int tMinX = -(tWidth / 2), tMinY = -(tHeight / 2);
	int tMaxX = tMinX + tWidth, tMaxY = tMinY + tHeight;					// ������ͼ��С��������

	SetWorkingImage(pFrom);
	setorigin(-iMinX, -iMinY);												// ����ͼƬ����Ϊԭ��

	SetWorkingImage(NULL);
	pTo->Resize(tWidth, tHeight);											// ��ʼ����ͼ

	DWORD* dst = GetImageBuffer(pTo);
	DWORD* src = GetImageBuffer(pFrom);										// ��ȡ��ͼ��ԭͼ�Ļ�����

	for (int y1 = 0; y1 < tHeight; y1++)
	{
		for (int x1 = 0; x1 < tWidth; x1++)
			dst[x1] = 0x00000000;
		dst += tWidth;
	}

	for (int y1 = 0; y1 < tHeight; y1++)									// ��ʼ����ͼ
		dst -= tWidth;

	for (int y1 = tMinY; y1 < tMaxY; y1++)
	{
		for (int x1 = tMinX; x1 < tMaxX; x1++)
		{
			int x = int(x1 * cos(rad) - y1 * sin(rad));
			int y = int(x1 * sin(rad) + y1 * cos(rad));						// ����任������

			int sxy = (iHeight - (y - iMinY) - 1) * iWidth + (x - iMinX);
			int dxy = (tHeight - (y1 - tMinY) - 1) * tWidth + (x1 - tMinX);	// ���������ڻ�������λ��

			if (x >= iMinX && x < iMaxX && y >= iMinY && y < iMaxY)			// Խ������
				dst[dxy] = src[sxy];
		}
	}

	SetWorkingImage(pFrom);
	setorigin(0, 0);
	SetWorkingImage(pWorking);												// ��ԭԭͼ����
}


// ��͸����ͼ������https://codebus.cn/yangw/transparent-putimage
// ������
//		dstimg��Ŀ�� IMAGE��NULL ��ʾĬ�ϴ��壩
//		x, y:	Ŀ����ͼλ��
//		srcimg: Դ IMAGE ����ָ��
void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg)
{
	// ������ʼ��
	DWORD* dst = GetImageBuffer(dstimg);
	DWORD* src = GetImageBuffer(srcimg);
	int src_width = srcimg->getwidth();
	int src_height = srcimg->getheight();
	int dst_width = (dstimg == NULL ? getwidth() : dstimg->getwidth());
	int dst_height = (dstimg == NULL ? getheight() : dstimg->getheight());

	// ������ͼ��ʵ�ʳ���
	int iwidth = (x + src_width > dst_width) ? dst_width - x : src_width;		// �������ұ߽�
	int iheight = (y + src_height > dst_height) ? dst_height - y : src_height;	// �������±߽�
	if (x < 0) { src += -x;				iwidth -= -x;	x = 0; }				// ��������߽�
	if (y < 0) { src += src_width * -y;	iheight -= -y;	y = 0; }				// �������ϱ߽�

	// ������ͼ��ʼλ��
	dst += dst_width * y + x;

	// ʵ��͸����ͼ
	for (int iy = 0; iy < iheight; iy++)
	{
		for (int ix = 0; ix < iwidth; ix++)
		{
			int sa = ((src[ix] & 0xff000000) >> 24);
			int sr = ((src[ix] & 0xff0000) >> 16);	// Դֵ�Ѿ��˹���͸��ϵ��
			int sg = ((src[ix] & 0xff00) >> 8);		// Դֵ�Ѿ��˹���͸��ϵ��
			int sb = src[ix] & 0xff;				// Դֵ�Ѿ��˹���͸��ϵ��
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


// ͼƬģ��������@����
// ������
//		output (IMAGE*):	���ͼƬ������ڵ���ԭͼƬ�ߴ磩
//		source (IMAGE*):	����ͼƬ�����������ģ�
//		radius (int):		ģ���뾶
// ע�����
//	-	��������һ���ò���Ч��
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

			// ��ִ��һ����ȫ��
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

			// ������ֻ��Ҫ�ظ�����
			for (int X = 1; X < imageWidth; ++X)
			{
				DWORD* pos = srcImageBuffer + (Y * imageWidth);
				int toBeMinused = X - radius;
				int toBeAdded = X + radius - 1;

				// ��ȥ��ǰ���
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

				// ����������
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

			// ��ִ��һ����ȫ��
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

			// ������ֻ��Ҫ�ظ�����
			for (int X = 1; X < imageWidth; ++X)
			{
				DWORD* pos = srcImageBuffer + (Y * imageWidth);
				int toBeMinused = X - radius;
				int toBeAdded = X + radius - 1;

				// ��ȥ��ǰ���
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

				// ����������
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
		// ��ִ��һ����ȫ��
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

		// ������ֻ��Ҫ�ظ�����
		for (int Y = 1; Y < imageHeight; ++Y)
		{
			DWORD* pos = srcImageBuffer + X;
			int toBeMinused = Y - radius;
			int toBeAdded = Y + radius - 1;

			// ��ȥ��ǰ���
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


			// ����������
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

// ͨ�� Alpha ����ͼ������Ӱ������ͼƬģ����������@����
void CreateShadowEffect(IMAGE* output, IMAGE* source, int radius) {

}