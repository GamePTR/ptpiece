#include "StylerFont.h"

#include <iostream>

StylerFont::StylerFont() : ConstantFontColor(false), FontColor(0x000000f0) {
	this->InitNativeFont();
}

StylerFont::StylerFont(LOGFONT const& _right) : ConstantFontColor(false), FontColor(0x000000f0) {
	memcpy(&(this->NativeFont), &_right, sizeof(LOGFONT));
}

StylerFont::StylerFont(StylerFont const& _right) {
	this->setLogFont(_right.NativeFont);
}

StylerFont::StylerFont(wchar_t const* fontFamily, LONG fontHeight, BYTE lfQuality, LONG lfWeight)
	: ConstantFontColor(false), FontColor(0x000000f0) {
	this->InitNativeFont();
	wcscpy_s(NativeFont.lfFaceName, LF_FACESIZE, fontFamily);
	NativeFont.lfHeight = fontHeight;
	NativeFont.lfQuality = lfQuality;
	NativeFont.lfWeight = lfWeight;
}

StylerFont::~StylerFont() {

}

void StylerFont::InitNativeFont() {
	this->NativeFont = {
		20, 0, 0, 0,
		FW_NORMAL,
		false,
		false,
		false,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH
	};

	wcscpy_s(this->NativeFont.lfFaceName, LF_FACESIZE, L"Microsoft YaHei");

}

LOGFONT const* StylerFont::getLogFont() const {
	return &(this->NativeFont);
}

LOGFONT& StylerFont::setValue() {
	return this->NativeFont;
}

void StylerFont::setLogFont(LOGFONT const& fontToBePassed) {
	wchar_t* tmpFontFamilyString = new wchar_t[LF_FACESIZE];
	wcscpy_s(tmpFontFamilyString, LF_FACESIZE, fontToBePassed.lfFaceName);

	memcpy(&(this->NativeFont), &fontToBePassed, sizeof(LOGFONT));
	wcscpy_s(this->NativeFont.lfFaceName, LF_FACESIZE, tmpFontFamilyString);

	delete[] tmpFontFamilyString;
}

StylerFont::operator LOGFONT const& () {
	return this->NativeFont;
}

StylerFont::operator LOGFONT const* () {
	return &(this->NativeFont);
}

void settextstyle(StylerFont const& fontToBeSet) {
	settextstyle(&(fontToBeSet.NativeFont));
	if (fontToBeSet.ConstantFontColor) {
		settextcolor(fontToBeSet.FontColor);
	}
}

void gettextstyle(StylerFont& fontToBeSet) {
	gettextstyle(&(fontToBeSet.NativeFont));
	fontToBeSet.FontColor = gettextcolor();
}

StylerFont& StylerFont::operator=(LOGFONT const& _right) {
	this->setLogFont(_right);
	return *this;
}

bool StylerFont::operator==(StylerFont const& _right) {
	return !memcmp(&this->NativeFont, &_right, sizeof(LOGFONT));
}

void StylerFont::setFontFamily(wchar_t const* fontFamily) {
	wcscpy_s(this->NativeFont.lfFaceName, LF_FACESIZE, fontFamily);
}

void StylerFont::setFontHeight(LONG fontHeight) {
	this->NativeFont.lfHeight = fontHeight;
}

void StylerFont::setFontQuality(BYTE lfQuality) {
	this->NativeFont.lfQuality = lfQuality;
}

void StylerFont::setFontWeight(LONG lfWeight) {
	this->NativeFont.lfWeight = lfWeight;
}

void StylerFont::fontColor(bool constantFontColor, COLORREF fontColor) {
	this->ConstantFontColor = constantFontColor;
	this->FontColor = fontColor;
}