// WidgetText.hpp
#include <string>
#include <windows.h>

class WidgetText {
public:
    WidgetText();
    ~WidgetText();
    void SetText(const std::string& text);
    void SetFont(const std::string& font);
    void SetSize(int size);
    void SetColor(int r, int g, int b);
    void SetPosition(int x, int y);
    void SetVisible(bool visible);
    void Draw(HDC hdc, RECT rect);

private:
    std::string text;
    std::string font;
    int size;
    int r, g, b;
    int x, y;
    bool visible;
};