#pragma once

#include "stb_truetype.h"
#include "stb_image.h"

#include "glad/glad.h"
#include "stdio.h"

#include "Utils.h"
#include "ShaderProgram.h"

struct DynamicFont {
    unsigned char ttf_buffer[1<<20];//one megabyte?
    //unsigned char temp_bitmap[512*512];

    stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
    GLuint ftex;

    float fontSize;
    GLuint bitmapRes;

    bool p = true;
};

enum TextAlignment {
    ALIGN_TOP_LEFT = 0,
    ALIGN_MID_LEFT,
    ALIGN_BOT_LEFT,
    ALIGN_TOP_MID,
    ALIGN_MID_MID,
    ALIGN_BOT_MID,
    ALIGN_TOP_RIGHT,
    ALIGN_MID_RIGHT,
    ALIGN_BOT_RIGHT
};


namespace Font {
    //unsigned char temp_bitmap[512*512];
    static GLuint textRectVAO = -1; //static means you cant change these
    static ShaderProgram textShader;
    static mat4 orthoMat;

    void InitTextRendering(WindowInfo windowInfo);
    DynamicFont* newDynamicFont(char* filename, float fontSize, int res = 512);
    void printFontData(DynamicFont* font);
    float getLength(DynamicFont* font, char* text);
    void getTextOffset(float* hOffset, float *vOffset, TextAlignment align, float textLength, float textHeight);
    void drawText(DynamicFont* font, float startX, float startY, quat color, char *text, TextAlignment align = TextAlignment::ALIGN_TOP_LEFT);
}