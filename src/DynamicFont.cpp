#include "DynamicFont.h"

void Font::InitTextRendering(WindowInfo windowInfo) {
    printf("\nInitializing Text Rendering...\n");
    Matrix::buildOrthoProjectionMatrix(&orthoMat, 
        0, windowInfo.width, 
        windowInfo.height, 0, .01, 100);
    textShader.smartLoad("text.vert", "text.frag");
    textShader.use();
    textShader.setInt("fontTex", 0);
    textShader.setMat4("projection", &orthoMat);

    if (textRectVAO == -1) {
        //If this is the first font being initialized, create the text VAO
        printf("Initializing text rendering VAO\n");
        GLuint vbo;

        float quadVertices[] = {//vec2 position, vec2 uv
            0, 1,
            0, 0,
            1, 0,

            0, 1,
            1, 0,
            1, 1
        };

        glGenVertexArrays(1, &textRectVAO);
        glBindVertexArray(textRectVAO);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
        //glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
}

DynamicFont* Font::newDynamicFont(char* filename, float fontSize, int res) {
    //printf("Loading font: \"%s\"\n", filename);
    DynamicFont* newFont = (DynamicFont*)malloc(sizeof(DynamicFont));
    unsigned char *temp_bitmap = (unsigned char*)malloc(res*res*sizeof(unsigned char));

    FILE* fid = fopen(filename, "rb");
    if (fid == NULL) {
        printf("Failed to open font file [%s]\n", filename);
        exit(1);
    }
    fread(newFont->ttf_buffer, 1, 1<<20, fid);
    stbtt_BakeFontBitmap(newFont->ttf_buffer,0, fontSize, temp_bitmap,res,res, 32,96, newFont->cdata); // no guarantee this fits!
    // can free ttf_buffer at this point
    glGenTextures(1, &newFont->ftex);
    glBindTexture(GL_TEXTURE_2D, newFont->ftex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, res,res, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
    // can free temp_bitmap at this point
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    //Free bitmap data
    free(temp_bitmap);

    newFont->bitmapRes = res;
    newFont->fontSize = fontSize;
    return newFont;
}

void Font::printFontData(DynamicFont* font) {
    for (int i = 0; i < 5; i++) {
        printf("x0: %2hu x1: %2hu y0: %2hu y1: %2hu xAdv: %7.3f xoff %6.3f yoff %7.3f\n", 
            font->cdata[i].x0, font->cdata[i].x1, font->cdata[i].y0, font->cdata[i].y1,
            font->cdata[i].xadvance, font->cdata[i].xoff, font->cdata[i].yoff);
    }
}

float Font::getLength(DynamicFont* font, char* text) {
    float length = 0;
    while (*text && *text != '\n') {
        if (*text >= 32 && *text < 128) {
            length += font->cdata[*text - 32].xadvance;;
        }
        ++text;
    }
    return length;
}

void Font::getTextOffset(float* hOffset, float *vOffset, TextAlignment align, float textLength, float textHeight) {
    switch(align) {
        case ALIGN_TOP_LEFT: {
            *hOffset = 0;
            *vOffset = textHeight;
        } break;
        case ALIGN_MID_LEFT: {
            *hOffset = 0;
            *vOffset = textHeight/2;
        } break;
        case ALIGN_BOT_LEFT: {
            *hOffset = 0;
            *vOffset = 0;
        } break;
        case ALIGN_TOP_MID: {
            *hOffset = -textLength/2;
            *vOffset = textHeight;
        } break;
        case ALIGN_MID_MID: {
            *hOffset = -textLength/2;
            *vOffset = textHeight/2;
        } break;
        case ALIGN_BOT_MID: {
            *hOffset = -textLength/2;
            *vOffset = 0;
        } break;
        case ALIGN_TOP_RIGHT: {
            *hOffset = -textLength;
            *vOffset = textHeight;
        } break;
        case ALIGN_MID_RIGHT: {
            *hOffset = -textLength/2;
            *vOffset = textHeight/2;
        } break;
        case ALIGN_BOT_RIGHT: {
            *hOffset = -textLength/2;
            *vOffset = 0;
        } break;
        default: {
            *hOffset = 0;
            *vOffset = 0;
        } break;
    }
}

void Font::drawText(DynamicFont* font, float startX, float startY, quat color, char *text, TextAlignment align) {
    float x = startX;
    float y = startY;

    float hOff, vOff;
    getTextOffset(&hOff, &vOff, align, getLength(font, text), font->fontSize);

    // assume orthographic projection with units = screen pixels, origin at top left
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font->ftex);
    glBindVertexArray(textRectVAO);
    glCullFace(GL_FRONT);
    glDisable(GL_DEPTH_TEST);
    textShader.use();

    // @Implement a color struct (vec4?)
    textShader.setVec4("textColor", color.x, color.y, color.z, color.w);

    while (*text) {
        if (*text == '\n') {
            //Increase y by one line,
            //reset x to start
            x = startX;
            y += font->fontSize;
        }
        if (*text >= 32 && *text < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(font->cdata, font->bitmapRes, font->bitmapRes, *text-32, &x,&y,&q,1);//1=opengl & d3d10+,0=d3d9
            
            if (font->p) {
                printf("Draw quad at: (%f, %f) (%f, %f)\n",
                    q.x0, q.y0, q.x1, q.y1);
                printf("UV coords:    (%f, %f) (%f, %f)\n\n",
                    q.s0, q.t0, q.s1, q.t1);
                font->p = false;
            }

            float scaleX = q.x1 - q.x0;
            float scaleY = q.y1 - q.y0;
            float transX = q.x0;
            float transY = q.y0;
            textShader.setVec4("transform", scaleX, scaleY, transX+hOff, transY+vOff);

            scaleX = q.s1 - q.s0;
            scaleY = q.t1 - q.t0;
            transX = q.s0;
            transY = q.t0;
            textShader.setVec4("transformUV", scaleX, scaleY, transX, transY);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        ++text;
    }
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glBindVertexArray(0);
}