#include <gl/glew.h>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include "ShadeStructs.h"
#include "Helpers.h"
#include "Polyhedron.h"
#include "Parser.h"

using namespace std;

extern GLboolean lightOnGlobal;
extern sf::Texture textureData;
extern vector<sf::Texture> textureDataVector;
extern std::vector <GameObject> gameObjects;
extern ShaderInformation shaderInformation;
// Массив VBO что бы их потом удалить
extern std::vector <GLuint> VBOArray;
extern int  numberOfSquares;
extern float rotateGlob[3];

extern const char* VertexShaderSource;
extern const char* FragShaderSource;


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
    for (int i = 0; i < numberOfSquares; ++i)
    {
        const char* filename = "model/road.png";
        textureData.loadFromFile(filename);
        gameObjects.push_back(GameObject{
                6,  // количество вершин в каждом буфере
                vertexVBO,
                textureVBO,
                normalVBO,
                textureData.getNativeHandle(), {0, 0}});

        textureDataVector.push_back(textureData);
    }

}

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

    shaderInformation.unifBusRotate = glGetUniformLocation(shaderInformation.shaderProgram, "unifBusRotate");
    if (shaderInformation.unifBusRotate == -1)
    {
        std::cout << "could not bind uniform bus angle" << std::endl;
        return;
    }

    shaderInformation.unifBusScale = glGetUniformLocation(shaderInformation.shaderProgram, "unifBusScale");
    if (shaderInformation.unifBusScale == -1)
    {
        std::cout << "could not bind uniform bus angle" << std::endl;
        return;
    }

    shaderInformation.unifBusShift = glGetUniformLocation(shaderInformation.shaderProgram, "unifBusShift");
    if (shaderInformation.unifBusShift == -1)
    {
        std::cout << "could not bind uniform bus angle" << std::endl;
        return;
    }

    const char* unif_name = "lightPos";
    shaderInformation.Unif_lightPos = glGetUniformLocation(shaderInformation.shaderProgram, unif_name);
    if (shaderInformation.Unif_lightPos == -1)
    {
        std::cout << "could not bind uniform " << unif_name << std::endl;
        return;
    }

    unif_name = "eyePos";
    shaderInformation.Unif_eyePos = glGetUniformLocation(shaderInformation.shaderProgram, unif_name);
    if (shaderInformation.Unif_eyePos == -1)
    {
        std::cout << "could not bind uniform " << unif_name << std::endl;
        return;
    }

    shaderInformation.unif_lightOn = glGetUniformLocation(shaderInformation.shaderProgram, "unif_lightOn");
    if (shaderInformation.unif_lightOn == -1)
    {
        std::cout << "could not bind unif_lightOn" << std::endl;
        return;
    }

    checkOpenGLerror();
}

int InitVBO(Polyhedron& pol, GLuint &vertexVBO, GLuint &textureVBO, GLuint &normalVBO)
{
    
    glGenBuffers(1, &vertexVBO);
    glGenBuffers(1, &textureVBO);
    glGenBuffers(1, &normalVBO);
    VBOArray.push_back(vertexVBO);
    VBOArray.push_back(textureVBO);
    VBOArray.push_back(normalVBO);

    vector<float> busCenter;
    busCenter.push_back(1.0);
    busCenter.push_back(0.0);
    busCenter.push_back(0.0);

    rotateGlob[0] = -1.66;
    rotateGlob[2] = 3.14;

    vector<vector<float>> vv;
    for (Triangle tr : pol.polygons)
    {
        for (int i = 0; i <= 2; i++)
        {
            vv.push_back({
                tr.points[i].x + busCenter[0],
                tr.points[i].y + busCenter[1],
                tr.points[i].z + busCenter[2] ,
                tr.texture[i].u,
                tr.texture[i].v,
                tr.normal[i].x,
                tr.normal[i].y,
                tr.normal[i].z });
        }
    }

    // структура: вершина(3) текстура(2) нормаль(3) цвет(3)
    int size = vv.size();
    Vertex* pointsCoord = new Vertex[size];
    for (int i = 0; i < size; i++)
    {
        pointsCoord[i] = { vv[i][0], vv[i][1], vv[i][2] };
    }

    Vertex* pointsTexture = new Vertex[size];
    for (int i = 0; i < size; i++)
    {
        pointsTexture[i] = { vv[i][3], vv[i][4] };
    }

    Vertex* pointsNormals = new Vertex[size];
    for (int i = 0; i < size; i++)
    {
        pointsNormals[i] = { vv[i][5], vv[i][6], vv[i][7] };
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, size * 3 * sizeof(GLfloat), pointsCoord, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glBufferData(GL_ARRAY_BUFFER, size * 3 * sizeof(GLfloat), pointsTexture, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, size * 3 * sizeof(GLfloat), pointsNormals, GL_STATIC_DRAW);

    checkOpenGLerror();
    return size;
}

void InitBus()
{
    Polyhedron pol;
    parseObjFile(pol, "model/bus2.obj");
    GLuint vertexVBO;
    GLuint textureVBO;
    GLuint normalVBO;
    int size = InitVBO(pol, vertexVBO, textureVBO, normalVBO);
    const char* filename = "model/bus2.png";
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
    Polyhedron pol;
    parseObjFile(pol, "model/grass.obj");
    GLuint vertexVBO;
    GLuint textureVBO;
    GLuint normalVBO;
    int size = InitVBO(pol, vertexVBO, textureVBO, normalVBO);
    const char* filename = "model/grass.png";
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
            {-1.6, -0.17f, 3.93},
            { 1.0f, 1.0f, 1.0f },
            { 0.02f, -19.74f, 18.9f }
            
        }
    ); 
}

void InitCone(int ind)
{
    Polyhedron pol;
    parseObjFile(pol, "model/cone.obj");
    GLuint vertexVBO;
    GLuint textureVBO;
    GLuint normalVBO;
    int size = InitVBO(pol, vertexVBO, textureVBO, normalVBO);
    const char* filename = "model/cone.png";
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

void Init() {
    InitShader();
    InitRoad();
    InitBus();
    InitGrass();
    for (int i = 0; i < 3; i++) {
        InitCone(i);
    }
    glEnable(GL_DEPTH_TEST);
}