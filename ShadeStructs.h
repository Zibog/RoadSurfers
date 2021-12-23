#pragma once
#include <gl/glew.h>

struct ShaderInformation {
    // Переменные с индентификаторами ID
    // ID шейдерной программы
    GLuint shaderProgram;
    // ID атрибута вершин
    GLint attribVertex;
    // ID атрибута текстурных координат
    GLint attribTexture;
    // ID атрибута нормали
    GLint attribNormal;
    // ID юниформа текстуры
    GLint unifTexture;
    // ID юниформа сдвига
    GLint unifShift;

    GLint unifBusRotate;
    GLint unifBusScale;
    GLint unifBusShift;

    // Light position
    GLuint Unif_lightPos;

    // View vector
    GLuint Unif_eyePos;
    // unif lightOn
    GLuint unif_lightOn;
};

struct GameObject {
    // Количество вершин в буферах
    GLfloat buffers_size;
    // ID буфера вершин
    GLuint vertexVBO;
    // ID буфера текстурных координат
    GLuint textureVBO;
    // ID буфера нормалей
    GLuint normalVBO;
    // ID текстуры
    GLuint textureHandle;

    // Величина сдвига
    GLfloat shift[2];
    GLfloat rotateObj[3] = { 0.0f, 0.0f, 0.0f };
    GLfloat scaleObj[3] = { 1.0f, 1.0f, 1.0f };
    GLfloat shiftObj[3] = { 0.0f, 0.0f, 0.0f };

    GLfloat lightPos[3] = { 0.0f, 1.0f, 0.0f };
    // View vector
    GLfloat eyePos[3] = { 1.0f,1.0f,-10.0f };
    GLboolean lightOn = true;
};

struct Vertex
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};
