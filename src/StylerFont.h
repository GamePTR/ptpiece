#pragma once

#include <easyx.h>

class StylerFont {
public:
	StylerFont();
	StylerFont(LOGFONT const& _right); /* ���� LOGFONT */
	StylerFont(StylerFont const& _right); /* ���� */
	StylerFont(wchar_t const* fontFamily, LONG fontHeight, BYTE lfQuality = ANTIALIASED_QUALITY, LONG lfWeight = FW_NORMAL); /* ��Щֵ */

	~StylerFont();
private:
	LOGFONT NativeFont;

	bool ConstantFontColor; /* �Ƿ�ӵ���Լ�����ɫ */
	COLORREF FontColor; /* ��� ConstantFontColor Ϊ true, settextstyle() ��ʱ���˳��������ɫ */

	void InitNativeFont(); /* ��ʼ�� NativeFont ��ֵ */
public:
	LOGFONT const* getLogFont() const; /* ��ȡ LOGFONT const ָ�� */
	LOGFONT& setValue(); /* ���޸� LOGFONT ��ֵ */
	void setLogFont(LOGFONT const& fontToBePassed); /* ���� LOGFONT */

	operator LOGFONT const& (); /* ����ǿ��ת����LOGFONT&(StylerFont ����) */
	operator LOGFONT const* (); /* ����ǿ��ת����LOGFONT*(StylerFont ����) */

	friend void settextstyle(StylerFont const& fontToBeSet); /* ���� settextstyle ��ʵ�ֵ�����ɫ���� */
	friend void gettextstyle(StylerFont& fontToBeSet); /* ���� gettextstyle �Ա�ʹ�� */

	StylerFont& operator=(LOGFONT const& _right); /* ���� */
	bool operator==(StylerFont const& _right); /* �Ա� */

	/* �޸�ĳ��һֵ */
	void setFontFamily(wchar_t const* fontFamily); /* ���� */
	void setFontHeight(LONG fontHeight); /* ��С */
	void setFontQuality(BYTE lfQuality); /* ������� */
	void setFontWeight(LONG lfWeight); /* ��ϸ */

	void fontColor(bool constantFontColor, COLORREF fontColor = 0x00000000); /* constantFontColor Ϊ true �ſ������� fontColor */
};