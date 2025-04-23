#ifndef FONT_H
#define FONT_H
#include <stdint.h>
#include <iostream>
namespace font{
    uint32_t GetFileSize(FILE *fp);
    void GetFontImageData(const unsigned char *fontData, int bitmap_w, int bitmap_h, wchar_t word, unsigned char *out);
}
#endif