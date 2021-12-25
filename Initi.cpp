#include <gl/glew.h>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include "ShaderData.h"
#include "Helpers.h"
#include "Parser.h"
#include "FigureModel.h"

using namespace std;

extern GLboolean lightOnGlobal;
extern sf::Texture textureData;
extern vector<sf::Texture> textureDataVector;
extern std::vector <GameObject> gameObjects;
extern ShaderData shaderInformation;
// Массив VBO что бы их потом удалить
extern std::vector <GLuint> VBOArray;
extern int  roadCount;
extern float busRotate[3];

extern const char* VertexShaderSource;
extern const char* FragShaderSource;




void InitShader() {
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &VertexShaderSource, NULL);
    glCompileShader(vShader);
    std::cout << "vertex shader \n";
    ShaderLog(vShader);

    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &FragShaderSource, NULL);
    glCompileShader(fShader);
    std::cout << "fragment shader \n";
    ShaderLog(fShader);

    shaderInformation.shaderProgram = glCreateProgram();
    glAttachShader(shaderInformation.shaderProgram, vShader);
    glAttachShader(shaderInformation.shaderProgram, fShader);

    glLinkProgram(shaderInformation.shaderProgram);
    int link_status;
    glGetProgramiv(shaderInformation.shaderProgram, GL_LINK_STATUS, &link_status);
    if (!link_status)
    {
        std::cout << "error attach shaders \n";
        return;
    }

    shaderInformation.attribVertex =
        glGetAttribLocation(shaderInformation.shaderProgram, "vertCoord");
    if (shaderInformation.attribVertex == -1)
    {
        std::cout << "could not bind attrib vertCoord" << std::endl;
        return;
    }

    shaderInformation.attribTexture =
        glGetAttribLocation(shaderInformation.shaderProgram, "texureCoord");
    if (shaderInformation.attribTexture == -1)
    {
        std::cout << "could not bind attrib texureCoord" << std::endl;
        return;
    }

    shaderInformation.attribNormal =
        glGetAttribLocation(shaderInformation.shaderProgram, "normalCoord");
    if (shaderInformation.attribNormal == -1)
    {
        std::cout << "could not bind attrib normalCoord" << std::endl;
        return;
    }

    shaderInformation.unifTexture =
        glGetUniformLocation(shaderInformation.shaderProgram, "textureData");
    if (shaderInformation.unifTexture == -1)
    {
        std::cout << "could not bind uniform textureData" << std::endl;
        return;
    }

    shaderInformation.unifShift = glGetUniformLocation(shaderInformation.shaderProgram, "shift");
    if (shaderInformation.unifShift == -1)
    {
        std::cout << "could not bind uniform angle" << std::endl;
        return;
    }

    shaderInformation.rotation = glGetUniformLocation(shaderInformation.shaderProgram, "rotation");
    if (shaderInformation.rotation == -1)
    {
        std::cout << "could not bind uniform bus angle" << std::endl;
        return;
    }

    shaderInformation.scaling = glGetUniformLocation(shaderInformation.shaderProgram, "scaling");
    if (shaderInformation.scaling == -1)
    {
        std::cout << "could not bind uniform bus angle" << std::endl;
        return;
    }

    shaderInformation.translation = glGetUniformLocation(shaderInformation.shaderProgram, "translation");
    if (shaderInformation.translation == -1)
    {
        std::cout << "could not bind uniform bus angle" << std::endl;
        return;
    }

    const char* unif_name = "lightPos";
    shaderInformation.lightPosition = glGetUniformLocation(shaderInformation.shaderProgram, unif_name);
    if (shaderInformation.lightPosition == -1)
    {
        std::cout << "could not bind uniform " << unif_name << std::endl;
        return;
    }

    unif_name = "eyePos";
    shaderInformation.eyePosition = glGetUniformLocation(shaderInformation.shaderProgram, unif_name);
    if (shaderInformation.eyePosition == -1)
    {
        std::cout << "could not bind uniform " << unif_name << std::endl;
        return;
    }

    shaderInformation.lightFlag = glGetUniformLocation(shaderInformation.shaderProgram, "lightFlag");
    if (shaderInformation.lightFlag == -1)
    {
        std::cout << "could not bind lightFlag" << std::endl;
        return;
    }

    checkOpenGLerror();
}

void InitRoad()
{
    GLuint vertexVBO;
    GLuint textureVBO;
    GLuint normalVBO;
    glGenBuffers(1, &vertexVBO);
    glGenBuffers(1, &textureVBO);
    glGenBuffers(1, &normalVBO);
    VBOArray.push_back(vertexVBO);
    VBOArray.push_back(textureVBO);
    VBOArray.push_back(normalVBO);

    // Объявляем вершины треугольника
    Vertex triangle[] = {
        { -0.5f, -0.5f, 0.0f },
        { +0.5f, -0.5f, 0.0f },
        { +0.5f, +0.5f, 0.0f },

        { +0.5f, +0.5f, 0.0f },
        { -0.5f, +0.5f, 0.0f },
        { -0.5f, -0.5f, 0.0f },
    };

    // Объявляем текстурные координаты
    Vertex texture[] = {
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        { 1.0f, 0.0f },

        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 1.0f  },
    };

    Vertex normal[] = {
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },

        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
    };

    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texture), texture, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normal), normal, GL_STATIC_DRAW);

    checkOpenGLerror();

    // Добавляем три одинаковых объект, менять им расположение мы будем потом при обработке каждого кадра
    for (int i = 0; i < roadCount; ++i)
    {
        const string filename = "model/road.png";
        textureData.loadFromFile(filename);
        gameObjects.push_back(GameObject{
                6,  // количество вершин в каждом буфере
                vertexVBO,
                textureVBO,
                normalVBO,
                textureData.getNativeHandle(), {0, -2.5f + 1.0f * i} });

        textureDataVector.push_back(textureData);
    }

}

int InitVBO(figure_model & fm, GLuint &vertexVBO, GLuint &textureVBO, GLuint &normalVBO)
{
    
    glGenBuffers(1, &vertexVBO);
    glGenBuffers(1, &textureVBO);
    glGenBuffers(1, &normalVBO);
    VBOArray.push_back(vertexVBO);
    VBOArray.push_back(textureVBO);
    VBOArray.push_back(normalVBO);
    busRotate[0] = -1.66;
    busRotate[2] = 3.14;
    size_t size = fm.count();

    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, size * 3 * sizeof(GLfloat), fm.get_vertices().data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glBufferData(GL_ARRAY_BUFFER, size * 3 * sizeof(GLfloat), fm.get_texture().data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, size * 3 * sizeof(GLfloat), fm.get_normals().data(), GL_STATIC_DRAW);

    checkOpenGLerror();
    return size;
}

void InitBus()
{
    const string objFilename = "model/bus2.obj";
    const string filename = "model/bus2.png";
    figure_model fm(objFilename);
    GLuint vertexVBO;
    GLuint textureVBO;
    GLuint normalVBO;
    int size = InitVBO(fm, vertexVBO, textureVBO, normalVBO);
    
    textureData.loadFromFile(filename);
    textureDataVector.push_back(textureData);
    // Добавляем три одинаковых объект, менять им расположение мы будем потом при обработке каждого кадра
    gameObjects.push_back(
        GameObject
        {
            (GLfloat)size,  // количество вершин в каждом буфере
            vertexVBO,
            textureVBO,
            normalVBO,
            textureData.getNativeHandle(),
            {0, 0},
            {-1.6f, 0.0f, 3.15f},
            {0.02f, 0.013f, 0.02f},
            {0.02f, -2.7f, 0.9f}
        }
    );
}

void InitGrass()
{
    const string objFilename = "model/grass.obj";
    const string filename = "model/grass.png";
    figure_model fm(objFilename);
    GLuint vertexVBO;
    GLuint textureVBO;
    GLuint normalVBO;
    int size = InitVBO(fm, vertexVBO, textureVBO, normalVBO);
    
    textureData.loadFromFile(filename);
    textureDataVector.push_back(textureData);
    // Добавляем три одинаковых объект, менять им расположение мы будем потом при обработке каждого кадра
    gameObjects.push_back(
        GameObject
        {
            (GLfloat)size,  // количество вершин в каждом буфере
            vertexVBO,
            textureVBO,
            normalVBO,
            textureData.getNativeHandle(),
            {1.0f, 1.0f},
            {-1.56, -0.15f, -0.17},
            { 1.0f, 1.0f, 1.0f },
            { 0.0f, 0.0f, 19.0f }
            
        }
    ); 
}

void InitCone(int ind)
{
    const string objFilename = "model/cone.obj";
    const string filename = "model/cone.png";
    figure_model fm(objFilename);
    GLuint vertexVBO;
    GLuint textureVBO;
    GLuint normalVBO;
    int size = InitVBO(fm, vertexVBO, textureVBO, normalVBO);
    textureData.loadFromFile(filename);
    textureDataVector.push_back(textureData);
    int r = std::rand() % 3 - 1;
    // Добавляем три одинаковых объект, менять им расположение мы будем потом при обработке каждого кадра
    gameObjects.push_back(
        GameObject
        {
            (GLfloat)size,  // количество вершин в каждом буфере
            vertexVBO,
            textureVBO,
            normalVBO,
            textureData.getNativeHandle(),
            {1.0f+r*4.5f, (ind+1)*10.0f},
            {-1.6, 0.0f, 3.0},//-0.005 -2.36499 0.9
            { 0.08f, 0.08f, 0.08f },
            { 0.0f, -2.365f, 0.9f }
        }
    );
}

void InitBox(int ind)
{
    const string objFilename = "model/box.obj";
    const string filename = "model/box.png";
    figure_model fm(objFilename);
    GLuint vertexVBO;
    GLuint textureVBO;
    GLuint normalVBO;
    int size = InitVBO(fm, vertexVBO, textureVBO, normalVBO);
    textureData.loadFromFile(filename);
    textureDataVector.push_back(textureData);
    int r = std::rand() % 3 - 1;
    // Добавляем три одинаковых объект, менять им расположение мы будем потом при обработке каждого кадра
    gameObjects.push_back(
        GameObject
        {
            (GLfloat)size,  // количество вершин в каждом буфере
            vertexVBO,
            textureVBO,
            normalVBO,
            textureData.getNativeHandle(),
            {1.0f + r * 4.5f, (ind + 1) * 10.0f+30.0f},
            {-1.6, 0.0f, 3.0},//-0.005 -2.36499 0.9
            { 0.08f, 0.08f, 0.08f },
            { 0.0f, -2.365f, 0.9f }
        }
    );
}


void Init() {
    InitShader();
    InitRoad();
    InitBus();
    InitGrass();
    for (int i = 0; i < 3; i++) {
        InitCone(i);
    }
    for (int i = 0; i < 3; i++) {
        InitBox(i);
    }
    glEnable(GL_DEPTH_TEST);
}