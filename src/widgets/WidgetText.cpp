#include "WidgetText.hpp"
#include <windows.h>
#include <iostream>

WidgetText::WidgetText() {
	text = "";
	font = "Arial";
	size = 12;
	r = 0;
	g = 0;
	b = 0;
	x = 0;
	y = 0;
	visible = true;
}

WidgetText::~WidgetText() {
}

void WidgetText::SetText(const std::string& text) {
	this->text = text;
}

void WidgetText::SetFont(const std::string& font) {
	this->font = font;
}

void WidgetText::SetSize(int size) {
	this->size = size;
}

void WidgetText::SetColor(int r, int g, int b) {
	this->r = r;
	this->g = g;
	this->b = b;
}

void WidgetText::SetPosition(int x, int y) {
	this->x = x;
	this->y = y;
}

void WidgetText::SetVisible(bool visible) {
	this->visible = visible;
}

void WidgetText::Draw(HDC hdc, RECT rect) {
	if (this->visible) {
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(this->r, this->g, this->b));

		HFONT hFont = CreateFont(this->size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, this->font.c_str());
		HGDIOBJ hOldFont = SelectObject(hdc, hFont);


		SelectObject(hdc, hFont);

		RECT textRect;
		textRect.left = this->x;
		textRect.top = this->y;
		textRect.right = rect.right;
		textRect.bottom = rect.bottom;

		DrawText(hdc, this->text.c_str(), -1, &textRect, 0);
		SelectObject(hdc, hOldFont);
		DeleteObject(hFont);
	}
}