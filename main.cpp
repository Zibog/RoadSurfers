﻿// Вращающийся треугольник с текстурой (можно вращать стрелочками)

#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "Polyhedron.h"


// В C и C++ есть оператор #, который позволяет превращать параметры макроса в строку
#define TO_STRING(x) #x


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

    // Велечина сдвига
    GLfloat shift[2];
    GLfloat rotateBus[3] = { 0.0f, 0.0f, 0.0f };
    GLfloat scaleBus[3] = { 1.0f, 1.0f, 1.0f };
    GLfloat shiftBus[3] = { 0.0f, 0.0f, 0.0f };

    GLfloat lightPos[3] = { 0.0f, 1.0f, 0.0f };
    // View vector
    GLfloat eyePos[3] = { 1.0f,1.0f,-10.0f };
    GLboolean lightOn = true;
};

float offset[3] = { 0.02, -2.74, 0.9 };
float rotateGlob[3] = { -1.57, 0.0, 3.14 };
GLboolean lightOnGlobal = true;

sf::Texture textureData;
vector<sf::Texture> textureDataVector;
std::vector <GameObject> gameObjects;
ShaderInformation shaderInformation;
// Массив VBO что бы их потом удалить
std::vector <GLuint> VBOArray;

int numberOfSquares = 6;
Polyhedron polyhedron;

// Вершина
struct Vertex
{
    GLfloat x;
    GLfloat y;
    GLfloat z; //= 0.0f;
};

const char* VertexShaderSource = TO_STRING(
    \#version 330 core\n

    uniform struct Transform {
    mat4 viewProjection;
} transform;

uniform vec2 shift;
uniform vec3 unifBusRotate;
uniform vec3 unifBusScale;
uniform vec3 unifBusShift;
uniform vec3 lightPos;
uniform vec3 eyePos;
uniform bool unif_lightOn;

in vec3 vertCoord;
in vec3 texureCoord;
in vec3 normalCoord;

out vec2 tCoord;
out vec3 vnormal;
out vec3 lightp;
out vec3 vnew;

void main() {
    float x_angle = unifBusRotate.x;
    float y_angle = unifBusRotate.y;
    float z_angle = unifBusRotate.z;

    mat3 aff = mat3(
        1, 0, 0,
        0, cos(x_angle), -sin(x_angle),
        0, sin(x_angle), cos(x_angle)
    ) * mat3(
        cos(y_angle), 0, sin(y_angle),
        0, 1, 0,
        -sin(y_angle), 0, cos(y_angle)
    ) * mat3(
        cos(z_angle), -sin(z_angle), 0,
        sin(z_angle), cos(z_angle), 0,
        0, 0, 1
    );

    vec3 position = vertCoord * aff;

    vec3 newNormale = mat3(transpose(inverse(aff))) * normalCoord;
    vec3 lposition = lightPos;

    vec3 temp = lposition;
    vec3 temp1 = eyePos;

    vnormal = newNormale;
    lightp = temp - position;
    vnew = temp1 - position;
    
    position = vertCoord * mat3(
        1, 0, 0,
        0, cos(x_angle), -sin(x_angle),
        0, sin(x_angle), cos(x_angle)
    ) * mat3(
        cos(y_angle), 0, sin(y_angle),
        0, 1, 0,
        -sin(y_angle), 0, cos(y_angle)
    ) * mat3(
        cos(z_angle), -sin(z_angle), 0,
        sin(z_angle), cos(z_angle), 0,
        0, 0, 1
    );

    tCoord = vec2(texureCoord.x, texureCoord.y);
    vec4 position2 = vec4(position + vec3(shift, 1.0), 1.0) * mat4(
        unifBusScale.x, 0, 0, 0,
        0, unifBusScale.y, 0, 0,
        0, 0, unifBusScale.z, 0,
        0, 0, 0, 1
    ) * mat4(
        1, 0, 0, unifBusShift.x,
        0, 1, 0, unifBusShift.y,
        0, 0, 1, unifBusShift.z,
        0, 0, 0, 1
    );;
    // TODO: remove lightPos and eyePos;
    gl_Position = vec4(normalCoord, 1.0f);
    if (unif_lightOn) { gl_Position = vec4(normalCoord, 1.0f); }
    gl_Position = transform.viewProjection * position2;
}
);

const char* FragShaderSource = TO_STRING(
    \#version 330 core\n

    uniform sampler2D textureData;
uniform bool unif_lightOn;
in vec2 tCoord;
in vec3 vnormal;
in vec3 lightp;
in vec3 vnew;
out vec4 color;

const vec4 diffColor = vec4(0.1f, 0.1f, 0.1f, 1.0f);
const vec4 specColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
const float specPower = 30.0f;

void main() {
    vec3 n2 = normalize(vnormal);
    vec3 l2 = normalize(lightp);
    vec3 v2 = normalize(vnew);
    vec3 r = reflect(-v2, n2);
    vec4 diff = diffColor * max(dot(n2, l2), 0.0f);
    vec4 spec = specColor * pow(max(dot(l2, r), 0.0f), specPower);

    color = texture(textureData, tCoord);
    if (!unif_lightOn) { color = color * 0.4f + (diff + spec); }
}
);


void Init();
void GameTick(int tick);
void Draw(GameObject gameObject, int i);
void Release();
void transform();
void InitBus();
void InitGrass();

void decAxis(int axis)//0x 1y 2z
{
    offset[axis] += 0.005;
    cout << "inc " << axis << endl;
    cout << offset[0] << " " << offset[1] << " " << offset[2] << endl;
}
void incAxis(int axis)//0x 1y 2z
{
    offset[axis] -= 0.005;
    cout << "dec " << axis << endl;
    cout << offset[0] << " " << offset[1] << " " << offset[2] << endl;
}
void decRotateAxis(int axis)//0x 1y 2z
{
    rotateGlob[axis] += 0.01;
    cout << rotateGlob[0] << " " << rotateGlob[1] << " " << rotateGlob[2] << endl;
}
void incRotateAxis(int axis)//0x 1y 2z
{
    rotateGlob[axis] -= 0.01;
    cout << rotateGlob[0] << " " << rotateGlob[1] << " " << rotateGlob[2] << endl;
}

int main() {
    parseObjFile(polyhedron, "model/bus2.obj");
    sf::Window window(sf::VideoMode(600, 600), "Subway Surf", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);

    window.setActive(true);

    glewInit();

    Init();

    // Счётчик кадров
    int tickCounter = 0;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
            }
            else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case (sf::Keyboard::W): incRotateAxis(0); break;
                case (sf::Keyboard::S): decRotateAxis(0); break;
                case (sf::Keyboard::A): incRotateAxis(1); break;
                case (sf::Keyboard::D): decRotateAxis(1); break;
                case (sf::Keyboard::Q): incRotateAxis(2); break;
                case (sf::Keyboard::E): decRotateAxis(2); break;
                case (sf::Keyboard::Left): decAxis(0); break;
                case (sf::Keyboard::Right):  incAxis(0); break;
                case (sf::Keyboard::Up):  incAxis(1); break;
                case (sf::Keyboard::Down):  decAxis(1); break;
                case (sf::Keyboard::PageUp):  decAxis(2); break;
                case (sf::Keyboard::PageDown):  incAxis(2); break;
                case (sf::Keyboard::L):  lightOnGlobal = !lightOnGlobal; break;
                default: break;
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GameTick(tickCounter);
        int i = 0;
        // Отрисовываем все объекты сцены
        for (GameObject& object : gameObjects)
        {
            //gameObjects[6].shiftBus[0] = offset[0];
            //gameObjects[6].shiftBus[1] = offset[1];
            //gameObjects[6].shiftBus[2] = offset[2];
            //gameObjects[6].rotateBus[0] = rotateGlob[0];
            //gameObjects[6].rotateBus[1] = rotateGlob[1];
            //gameObjects[6].rotateBus[2] = rotateGlob[2];

            gameObjects[7].shiftBus[0] = offset[0];
            gameObjects[7].shiftBus[1] = offset[1];
            gameObjects[7].shiftBus[2] = offset[2];
            gameObjects[7].rotateBus[0] = rotateGlob[0];
            gameObjects[7].rotateBus[1] = rotateGlob[1];
            gameObjects[7].rotateBus[2] = rotateGlob[2];
            object.lightOn = lightOnGlobal;
            Draw(object, i++);
            //Draw(gameObjects[6], 6);
        }


        tickCounter++;
        window.display();
    }

    Release();
    return 0;
}


// Проверка ошибок OpenGL, если есть то вывод в консоль тип ошибки
void checkOpenGLerror() {
    GLenum errCode;
    // Коды ошибок можно смотреть тут
    // https://www.khronos.org/opengl/wiki/OpenGL_Error
    if ((errCode = glGetError()) != GL_NO_ERROR)
        std::cout << "OpenGl error!: " << errCode << std::endl;
}

// Функция печати лога шейдера
void ShaderLog(unsigned int shader)
{
    int infologLen = 0;
    int charsWritten = 0;
    char* infoLog;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
    if (infologLen > 1)
    {
        infoLog = new char[infologLen];
        if (infoLog == NULL)
        {
            std::cout << "ERROR: Could not allocate InfoLog buffer" << std::endl;
            exit(1);
        }
        glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
        std::cout << "InfoLog: " << infoLog << "\n\n\n";
        delete[] infoLog;
    }
}


void InitObjects()
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



    // sf::Texture::bind(&textureData);

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
                textureData.getNativeHandle(), {0, 0} });

        textureDataVector.push_back(textureData);
        //InitBus();
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
    

    /*const char* unif_name = "xpos";
    Unif_posx = glGetUniformLocation(Program, unif_name);
    if (Unif_posx == -1)
    {
        std::cout << "could not bind uniform " << unif_name << std::endl;
        return;
    }*/
    checkOpenGLerror();
}


//void InitTexture()
//{
//    //const char* filename = "image.jpg";
//    const char* filename = "road.jpg";
//    if (!textureData.loadFromFile(filename))
//    {
//        std::cout << "could not load texture" << std::endl;
//    }
//}

void Init() {
    InitShader();
    //InitTexture();
    InitObjects();
    InitBus();
    InitGrass();
    glEnable(GL_DEPTH_TEST);

}

void InitBus()
{
    //GameObject.rotateBus[0] = 1;
    Polyhedron pol;
    parseObjFile(pol, "model/bus2.obj");
    GLuint vertexVBO;
    GLuint textureVBO;
    GLuint normalVBO;
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

    vector<vector<float>> vv;
    for (Triangle tr : pol.polygons)
    {
        for (int i = 0; i <= 2; i++)
        {
            // vv.push_back({ tr.points[i].x, tr.points[i].y, tr.points[i].z, tr.texture[i].u, tr.texture[i].v, tr.normal[i].x, tr.normal[i].y, tr.normal[i].z });
            vv.push_back({ tr.points[i].x + busCenter[0], tr.points[i].y + busCenter[1], tr.points[i].z + busCenter[2] , tr.texture[i].u, tr.texture[i].v, tr.normal[i].x, tr.normal[i].y, tr.normal[i].z });
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
    //textureData.loadFromFile("bus2.jpg");
    //sf::Texture::bind(&textureData);

    const char* filename = "model/bus2.png";
    textureData.loadFromFile(filename);
    textureDataVector.push_back(textureData);
    //sf::Texture::bind(&textureData);
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

    vector<vector<float>> vv;
    for (Triangle tr : pol.polygons)
    {
        for (int i = 0; i <= 2; i++)
        {
            // vv.push_back({ tr.points[i].x, tr.points[i].y, tr.points[i].z, tr.texture[i].u, tr.texture[i].v, tr.normal[i].x, tr.normal[i].y, tr.normal[i].z });
            vv.push_back({ tr.points[i].x + busCenter[0], tr.points[i].y + busCenter[1], tr.points[i].z + busCenter[2] , tr.texture[i].u, tr.texture[i].v, tr.normal[i].x, tr.normal[i].y, tr.normal[i].z });
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
    //textureData.loadFromFile("bus2.jpg");
    //sf::Texture::bind(&textureData);

    const char* filename = "model/grass.png";
    textureData.loadFromFile(filename);
    textureDataVector.push_back(textureData);
    //sf::Texture::bind(&textureData);
    // Добавляем три одинаковых объект, менять им расположение мы будем потом при обработке каждого кадра
    rotateGlob[0] = -1.66;
    rotateGlob[1] = 2.91f;
    rotateGlob[2] = 3.05;
    gameObjects.push_back(
        GameObject
        {
            (GLfloat)size,  // количество вершин в каждом буфере
            vertexVBO,
            textureVBO,
            normalVBO,
            textureData.getNativeHandle(),
            {1.0f, 1.0f},
            {-1.66, 2.91f, 3.05}
        }
    ); //192023
}

float view_pos[]{ 0, -3, 2 };
// Обработка шага игрового цикла
void GameTick(int tick) {
    int frequency = 100;
    for (int i = 0; i < numberOfSquares; ++i)
    {
        gameObjects[i].shift[1] = 1.5f - ((tick + (frequency * i)) % (numberOfSquares * frequency)) / (float)frequency;
    }
}
glm::mat4 view_projection = glm::perspective(
    glm::radians(90.0f), // Вертикальное поле зрения в радианах. Обычно между 90&deg; (очень широкое) и 30&deg; (узкое)
    4.0f / 3.0f,       // Отношение сторон. Зависит от размеров вашего окна. Заметьте, что 4/3 == 800/600 == 1280/960
    0.1f,              // Ближняя плоскость отсечения. Должна быть больше 0.
    100.0f             // Дальняя плоскость отсечения.
) * glm::lookAt(glm::vec3(0.0f, -3.0f, 0.5f),
    glm::vec3(0.0f, 0.0f, 2.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));


void Draw(GameObject gameObject, int index = 0) {


    glUseProgram(shaderInformation.shaderProgram);
    glUniform2fv(shaderInformation.unifShift, 1, gameObject.shift);
    glUniform3fv(shaderInformation.unifBusRotate, 1, gameObject.rotateBus);
    glUniform3fv(shaderInformation.unifBusScale, 1, gameObject.scaleBus);
    glUniform3fv(shaderInformation.unifBusShift, 1, gameObject.shiftBus);
    glUniform3fv(shaderInformation.Unif_lightPos, 1, gameObject.lightPos);
    glUniform3fv(shaderInformation.Unif_eyePos, 1, gameObject.eyePos);
    glUniform1i(shaderInformation.unif_lightOn, gameObject.lightOn);
    transform();
    glActiveTexture(GL_TEXTURE0);
    sf::Texture::bind(&textureDataVector[index]);
    glUniform1i(shaderInformation.unifTexture, 0);

    // Подключаем VBO
    glEnableVertexAttribArray(shaderInformation.attribVertex);
    glBindBuffer(GL_ARRAY_BUFFER, gameObject.vertexVBO);
    glVertexAttribPointer(shaderInformation.attribVertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(shaderInformation.attribTexture);
    glBindBuffer(GL_ARRAY_BUFFER, gameObject.textureVBO);
    glVertexAttribPointer(shaderInformation.attribTexture, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(shaderInformation.attribNormal);
    glBindBuffer(GL_ARRAY_BUFFER, gameObject.normalVBO);
    glVertexAttribPointer(shaderInformation.attribNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Передаем данные на видеокарту(рисуем)
    glDrawArrays(GL_TRIANGLES, 0, gameObject.buffers_size);

    // Отключаем массив атрибутов
    glDisableVertexAttribArray(shaderInformation.attribVertex);
    // Отключаем шейдерную программу
    glUseProgram(0);
    checkOpenGLerror();
}

void transform()
{

    GLint s_proj = glGetUniformLocation(shaderInformation.shaderProgram, "transform.viewProjection");
    glUniformMatrix4fv(s_proj, 1, GL_FALSE, &view_projection[0][0]);

}


void ReleaseShader() {
    glUseProgram(0);
    glDeleteProgram(shaderInformation.shaderProgram);
}

void ReleaseVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Чистим все выделенные VBO
    for (GLuint VBO : VBOArray)
        glDeleteBuffers(1, &VBO);
}

void Release() {
    ReleaseShader();
    ReleaseVBO();
}
