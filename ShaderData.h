#pragma once
#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


struct ShaderData {
    //ID шейдерной программы
    GLuint shaderProgram;

    GLint attribVertex;

    GLint attribTexture;

    GLint attribNormal;

    GLint unifTexture;

    GLint unifShift;

    GLint rotation;
    GLint scaling;
    GLint translation;

 
    GLuint lightPosition;


    GLuint eyePosition;

    GLuint lightFlag;
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

    GLfloat shift[2];
    GLfloat rotation[3] = { 0.0f, 0.0f, 0.0f };
    GLfloat scaling[3] = { 1.0f, 1.0f, 1.0f };
    GLfloat translation[3] = { 0.0f, 0.0f, 0.0f };

    GLfloat lightPos[3] = { 0.0f, 1.0f, 0.0f };
  
    GLfloat eyePos[3] = { 1.0f,1.0f,-10.0f };
    GLboolean lightOn = true;
};

struct Vertex
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

