#pragma once

#include <easyx.h>

class StylerFont {
public:
	StylerFont();
	StylerFont(LOGFONT const& _right); /* 来自 LOGFONT */
	StylerFont(StylerFont const& _right); /* 复制 */
	StylerFont(wchar_t const* fontFamily, LONG fontHeight, BYTE lfQuality = ANTIALIASED_QUALITY, LONG lfWeight = FW_NORMAL); /* 赋些值 */

	~StylerFont();
private:
	LOGFONT NativeFont;

	bool ConstantFontColor; /* 是否拥有自己的颜色 */
	COLORREF FontColor; /* 如果 ConstantFontColor 为 true, settextstyle() 的时候会顺便设置颜色 */

	void InitNativeFont(); /* 初始化 NativeFont 的值 */
public:
	LOGFONT const* getLogFont() const; /* 获取 LOGFONT const 指针 */
	LOGFONT& setValue(); /* 可修改 LOGFONT 的值 */
	void setLogFont(LOGFONT const& fontToBePassed); /* 复制 LOGFONT */

	operator LOGFONT const& (); /* 允许强制转换：LOGFONT&(StylerFont 对象) */
	operator LOGFONT const* (); /* 允许强制转换：LOGFONT*(StylerFont 对象) */

	friend void settextstyle(StylerFont const& fontToBeSet); /* 重载 settextstyle 以实现单独颜色功能 */
	friend void gettextstyle(StylerFont& fontToBeSet); /* 重载 gettextstyle 以便使用 */

	StylerFont& operator=(LOGFONT const& _right); /* 复制 */
	bool operator==(StylerFont const& _right); /* 对比 */

	/* 修改某单一值 */
	void setFontFamily(wchar_t const* fontFamily); /* 字体 */
	void setFontHeight(LONG fontHeight); /* 大小 */
	void setFontQuality(BYTE lfQuality); /* 输出质量 */
	void setFontWeight(LONG lfWeight); /* 粗细 */

	void fontColor(bool constantFontColor, COLORREF fontColor = 0x00000000); /* constantFontColor 为 true 才可以设置 fontColor */
};