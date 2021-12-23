#pragma once
#include <gl/glew.h>

struct ShaderInformation {
    // ���������� � ����������������� ID
    // ID ��������� ���������
    GLuint shaderProgram;
    // ID �������� ������
    GLint attribVertex;
    // ID �������� ���������� ���������
    GLint attribTexture;
    // ID �������� �������
    GLint attribNormal;
    // ID �������� ��������
    GLint unifTexture;
    // ID �������� ������
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
    // ���������� ������ � �������
    GLfloat buffers_size;
    // ID ������ ������
    GLuint vertexVBO;
    // ID ������ ���������� ���������
    GLuint textureVBO;
    // ID ������ ��������
    GLuint normalVBO;
    // ID ��������
    GLuint textureHandle;

    // �������� ������
    GLfloat shift[2];
    GLfloat rotateBus[3] = { 0.0f, 0.0f, 0.0f };
    GLfloat scaleBus[3] = { 1.0f, 1.0f, 1.0f };
    GLfloat shiftBus[3] = { 0.0f, 0.0f, 0.0f };

    GLfloat lightPos[3] = { 0.0f, 1.0f, 0.0f };
    // View vector
    GLfloat eyePos[3] = { 1.0f,1.0f,-10.0f };
    GLboolean lightOn = true;
};

struct Vertex
{
    GLfloat x;
    GLfloat y;
    GLfloat z; //= 0.0f;
};
