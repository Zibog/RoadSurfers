﻿#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <vector>

#include "ShadeStructs.h"
#include "Control.h"
#include "Helpers.h"
#include "Initialization.h"
#include "Parser.h"

using namespace std;

float offset[3] = { 0.02, -2.74, 0.9 };
float rotateGlob[3] = { 0.0, 0.0 ,0.0};
float lightPos[3] = { -13.0f, 0.0f, 0.0f };
GLboolean lightOnGlobal = true;

sf::Texture textureData;
vector<sf::Texture> textureDataVector;
std::vector <GameObject> gameObjects;
ShaderData shaderInformation;
// Массив VBO что бы их потом удалить
std::vector <GLuint> VBOArray;

int roadCount = 50;

float view_pos[]{ 0, -5, 2 };





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
                case (sf::Keyboard::Up):  incAxis(1); break;
                case (sf::Keyboard::Down):  decAxis(1); break;
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
            gameObjects[roadCount ].translation[0] = offset[0];
            gameObjects[roadCount ].translation[1] = offset[1];
            gameObjects[roadCount ].translation[2] = offset[2];
            gameObjects[roadCount ].rotation[0] = rotateGlob[0];
            gameObjects[roadCount ].rotation[1] = rotateGlob[1];
            gameObjects[roadCount ].rotation[2] = rotateGlob[2];

            object.lightOn = lightOnGlobal;
            Draw(object, i++);
        }


        tickCounter++;
        window.display();
    }

    Release();
    return 0;
}