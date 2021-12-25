#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <vector>
#include "Helpers.h"
#include "Init.h"
#include "ShaderData.h"
#include "Control.h"

#define TO_STRING(x) #x

const char* VertexShaderSource = TO_STRING(
    \#version 330 core\n

    uniform struct Transform {
    mat4 viewProjection;
} transform;

uniform vec2 shift;
uniform vec3 rotation;
uniform vec3 scaling;
uniform vec3 translation;
uniform vec3 lightPos;
uniform vec3 eyePos;
uniform bool lightFlag;

in vec3 vertCoord;
in vec3 texureCoord;
in vec3 normalCoord;

out vec2 tCoord;
out vec3 viewNormal;
out vec3 lightDir;
out vec3 viewDir;

void main() {
    float x_angle = rotation.x;
    float y_angle = rotation.y;
    float z_angle = rotation.z;

    mat3 rotateMatrix = mat3(
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

    vec3 position = vertCoord * rotateMatrix;
    viewNormal = mat3(transpose(inverse(rotateMatrix))) * normalCoord;
    lightDir = lightPos - position;
    viewDir = eyePos - position;
    tCoord = vec2(texureCoord.x, texureCoord.y);

    mat4 scaleMatr = mat4(
        scaling.x, 0, 0, 0,
        0, scaling.y, 0, 0,
        0, 0, scaling.z, 0,
        0, 0, 0, 1
    );

    mat4 translateMatr = mat4(
        1, 0, 0, translation.x,
        0, 1, 0, translation.y,
        0, 0, 1, translation.z,
        0, 0, 0, 1
    );

    mat4 shiftMatr = mat4(
        1, 0, 0, shift.x,
        0, 1, 0, shift.y,
        0, 0, 1, 1,
        0, 0, 0, 1
    );

    vec4 position2 = vec4(position, 1.0) * shiftMatr * scaleMatr * translateMatr;
    // TODO: remove lightPos and eyePos;
    gl_Position = vec4(normalCoord, 1.0f);
    if (lightFlag) { gl_Position = vec4(normalCoord, 1.0f); }
    gl_Position = transform.viewProjection * position2;
}
);

const char* FragShaderSource = TO_STRING(
    \#version 330 core\n

    uniform sampler2D textureData;
uniform bool lightFlag;
in vec2 tCoord;
in vec3 viewNormal;
in vec3 lightDir;
in vec3 viewDir;
out vec4 color;


const float specPower = 30.0f;
const vec4 specColor = vec4(0.1f, 0.1f, 0.1f, 1.0f);
const vec4 diffColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);




void main() {
    vec3 nviewNormal = normalize(viewNormal);
    vec3 nlightDir = normalize(lightDir);

    vec3 nviewDir = normalize(viewDir);
    vec3 r = reflect(-nlightDir, nviewDir);
    vec4 diff = diffColor * max(dot(nviewNormal, nlightDir), 0.0f);
    vec4 spec = specColor * pow(max(dot(nlightDir, r), 0.0f), specPower);

    color = texture(textureData, tCoord) + (diff + spec);
    if (lightFlag) {
        color = texture(textureData, tCoord) + (diff + spec);
    }
    else {
        color = texture(textureData, tCoord) + (diff + spec) * 0;
    }
}
);



using namespace std;

float viewPosition[]{ 0, -5, 2 };

sf::Texture textureData;
vector<sf::Texture> textureDataVector;
std::vector <GameObject> gameObjects;
ShaderData shaderInformation;

std::vector <GLuint> VBOArray;

float lightPos[3] = { -13.0f, 0.0f, 0.0f };
float busShift[3] = { 0.02, -2.74, 0.9 };
float busRotate[3] = { 0.0, 0.0 ,0.0 };
GLboolean lightOnGlobal = true;



int roadCount = 50;



void GameTick(int tick) {
   

    for (int i = roadCount + 2; i < gameObjects.size(); ++i)
    {
        gameObjects[i].shift[1] -= 0.1;
        if (gameObjects[i].shift[1] < -10) {
            gameObjects[i].shift[1] += 40;
            int r = std::rand() % 3 - 1;
            gameObjects[i].shift[0] = r * 4.5f + 1.0f;
        }

    }

    for (int i = 0; i < roadCount; ++i)
    {
        gameObjects[i].shift[1] -=0.01;
        if (gameObjects[i].shift[1] < -10) {
            gameObjects[i].shift[1] += roadCount;
            
        }
    }
    gameObjects[roadCount+1].shift[1] -= 0.01;

    
}
glm::mat4 view_projection = glm::perspective(
    glm::radians(90.0f),
    4.0f / 3.0f,      
    0.1f,              
    100.0f             
) * glm::lookAt(
        glm::vec3(0.0f, -3.0f, 0.5f),
        glm::vec3(0.0f, 0.0f, 2.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
);


void Release() {
    glUseProgram(0);
    glDeleteProgram(shaderInformation.shaderProgram);
    glUseProgram(0);
    glDeleteProgram(shaderInformation.shaderProgram);
}




void Draw(GameObject gameObject, int index = 0) {
    glUseProgram(shaderInformation.shaderProgram);
    glUniform2fv(shaderInformation.unifShift, 1, gameObject.shift);
    glUniform3fv(shaderInformation.rotation, 1, gameObject.rotation);
    glUniform3fv(shaderInformation.scaling, 1, gameObject.scaling);
    glUniform3fv(shaderInformation.translation, 1, gameObject.translation);
    gameObject.lightPos[0] = lightPos[0];
    gameObject.lightPos[1] = lightPos[1];
    gameObject.lightPos[2] = lightPos[2];
    glUniform3fv(shaderInformation.lightPosition, 1, gameObject.lightPos);
    glUniform3fv(shaderInformation.eyePosition, 1, gameObject.eyePos);
    glUniform1i(shaderInformation.lightFlag, gameObject.lightOn);


    
    GLint s_proj = glGetUniformLocation(shaderInformation.shaderProgram, "transform.viewProjection");
    glUniformMatrix4fv(s_proj, 1, GL_FALSE, &view_projection[0][0]);


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

int main() {
    std::srand(std::time(nullptr));


    sf::Window window(sf::VideoMode(600, 600), "Subway Surf", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);

    window.setActive(true);

    glewInit();

    Init();

    // Счётчик кадров
    int tickCounter = 0;
    while (window.isOpen()) {
        makeMovement();
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
                case (sf::Keyboard::Left): 
                    moveLeft();
                    break;
                case (sf::Keyboard::Right):
                    moveRight();
                    break;
                case (sf::Keyboard::Up):  increaseShift(1); break;
                case (sf::Keyboard::Down):  decreseShift(1); break;
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
            gameObjects[roadCount].rotation[0] = busRotate[0];
            gameObjects[roadCount].rotation[1] = busRotate[1];
            gameObjects[roadCount].rotation[2] = busRotate[2];


            gameObjects[roadCount ].translation[0] = busShift[0];
            gameObjects[roadCount ].translation[1] = busShift[1];
            gameObjects[roadCount ].translation[2] = busShift[2];
           

            object.lightOn = lightOnGlobal;
            Draw(object, i++);
        }


        tickCounter++;
        window.display();
    }

    Release();
    return 0;
}