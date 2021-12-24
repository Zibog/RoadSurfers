#pragma once
#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <vector>

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

    // View std::vector
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
    // View std::vector
    GLfloat eyePos[3] = { 1.0f,1.0f,-10.0f };
    GLboolean lightOn = true;
};

struct Vertex
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

struct Point3D
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat w;

    Point3D(GLfloat X, GLfloat Y, GLfloat Z)
    {
        x = X;
        y = Y;
        z = Z;
        w = 1.0;
    }

    Point3D()
    {
        x = 0.0;
        y = 0.0;
        z = 0.0;
        w = 1.0;
    }
};

struct PointUV
{
    GLfloat u;
    GLfloat v;

    PointUV(GLfloat U, GLfloat V)
    {
        u = U;
        v = V;
    }

    PointUV()
    {
        u = 0.0;
        v = 0.0;
    }
};

struct Triangle
{
    std::vector<Point3D> points;
    std::vector<Point3D> normal;
    std::vector<PointUV> texture;
    Triangle(std::vector<Point3D> pnts, std::vector<Point3D> nrml, std::vector<PointUV> txtr)
    {
        points = pnts;
        normal = nrml;
        texture = txtr;
    }
};
