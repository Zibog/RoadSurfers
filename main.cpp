#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <vector>

#include "Polyhedron.h"
#include "ShadeStructs.h"
#include "Control.h"
#include "Helpers.h"
#include "Initialization.h"
#include "Parser.h"

float offset[3] = { 0.02, -2.74, 0.9 };
float rotateGlob[3] = { 0.0, 0.0 ,0.0};
float lightPos[3] = { 3.0f, 0.0f, 0.0f };
GLboolean lightOnGlobal = true;

sf::Texture textureData;
vector<sf::Texture> textureDataVector;
std::vector <GameObject> gameObjects;
ShaderInformation shaderInformation;
// Массив VBO что бы их потом удалить
std::vector <GLuint> VBOArray;

int numberOfSquares = 50;
Polyhedron polyhedron;
void GameTick(int tick);
void Draw(GameObject gameObject, int i);
void Release();
void transform();
float view_pos[]{ 0, -3, 2 };

int position = 0;
bool leftTurn = false;
bool leftAlign = false;
bool rightTurn = false;
bool rightAlign = false;



// Обработка шага игрового цикла
void GameTick(int tick) {
    int frequency = 100;
    for (int i = 0; i < numberOfSquares; ++i)
    {
        gameObjects[i].shift[1] = 10.0f - ((tick + (frequency * i)) % (numberOfSquares * frequency)) / (float)frequency;
    
    }
    for (int i = numberOfSquares+2; i < gameObjects.size(); ++i)
    {
        gameObjects[i].shift[1] -= 0.1;
        if (gameObjects[i].shift[1] < -10) {
            gameObjects[i].shift[1] += 40;
            int r = std::rand() % 3 - 1;
            gameObjects[i].shift[0] = r * 4.5f + 1.0f;
        }

    }
    
}
glm::mat4 view_projection = glm::perspective(
    glm::radians(90.0f), // Вертикальное поле зрения в радианах. Обычно между 90&deg; (очень широкое) и 30&deg; (узкое)
    4.0f / 3.0f,       // Отношение сторон. Зависит от размеров вашего окна. Заметьте, что 4/3 == 800/600 == 1280/960
    0.1f,              // Ближняя плоскость отсечения. Должна быть больше 0.
    100.0f             // Дальняя плоскость отсечения.
) * glm::lookAt(
        glm::vec3(0.0f, -3.0f, 0.5f),
        glm::vec3(0.0f, 0.0f, 2.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
);


void Draw(GameObject gameObject, int index = 0) {
    glUseProgram(shaderInformation.shaderProgram);

    glUniform2fv(shaderInformation.unifShift, 1, gameObject.shift);
    glUniform3fv(shaderInformation.unifBusRotate, 1, gameObject.rotateObj);
    glUniform3fv(shaderInformation.unifBusScale, 1, gameObject.scaleObj);
    glUniform3fv(shaderInformation.unifBusShift, 1, gameObject.shiftObj);

    gameObject.lightPos[0] = lightPos[0];
    gameObject.lightPos[1] = lightPos[1];
    gameObject.lightPos[2] = lightPos[2];

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


int main() {
    std::srand(std::time(nullptr));
    parseObjFile(polyhedron, "model/bus2.obj");
    sf::Window window(sf::VideoMode(600, 600), "Subway Surf", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);

    window.setActive(true);

    glewInit();

    Init();

    // Счётчик кадров
    int tickCounter = 0;
    while (window.isOpen()) {
        if (leftTurn)
        {
            if (position == 0) 
            {
                if (offset[0]<0.3) 
                {
                    turnLeft();
                }
                else 
                {
                    position = -1;
                    leftTurn = false;
                    leftAlign = true;
                }
            }
            else if (position == 1)
            {
                if (offset[0] < 0.0)
                {
                    turnLeft();
                }
                else
                {
                    position = 0;
                    leftTurn = false;
                    leftAlign = true;
                }
            } 
        }
        if (rightTurn)
        {
            if (position == 0)
            {
                if (offset[0] > -0.3)
                {
                    turnRight();
                }
                else
                {
                    position = 1;
                    rightTurn = false;
                    rightAlign = true;
                }
            }
            else if (position == -1)
            {
                if (offset[0] > 0.0)
                {
                    turnRight();
                }
                else
                {
                    position = 0;
                    rightTurn = false;
                    rightAlign = true;
                }
            }
        }
        if (leftAlign) 
        {
            if (rotateGlob[2] < 3.14) {
                decRotateAxis(2);
            }
            else 
            {
                leftAlign = false;
            }
        } else  if (rightAlign)
        {
            if (rotateGlob[2] >3.14) {
                incRotateAxis(2);
            }
            else
            {
                rightAlign = false;
            }
        }
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
                case (sf::Keyboard::Q): incRotateAxis(1); break;
                case (sf::Keyboard::E): decRotateAxis(1); break;
                case (sf::Keyboard::A): incRotateAxis(2); break;
                case (sf::Keyboard::D): decRotateAxis(2); break;
                case (sf::Keyboard::Left): 
                    if (position != -1) {
                        leftTurn = true;
                        rightTurn = false;
                    }
                    break;
                case (sf::Keyboard::Right):
                    if (position != 1) {
                        rightTurn = true;
                        leftTurn = false;
                    }
                    break;
                case (sf::Keyboard::Up):  incAxis(1); break;
                case (sf::Keyboard::Down):  decAxis(1); break;
                case (sf::Keyboard::Comma):  decAxis(2); break;
                case (sf::Keyboard::Period):  incAxis(2); break;
                case (sf::Keyboard::U): incLightPos(0); break;
                case (sf::Keyboard::J): decLightPos(0); break;
                case (sf::Keyboard::H): incLightPos(1); break;
                case (sf::Keyboard::K): decLightPos(1); break;
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
            // First <numberOfSquares> elms = road
            // [numberOfSquares] el = bus
            // [numberOfSquares + 1] = grass
            gameObjects[numberOfSquares ].shiftObj[0] = offset[0];
            gameObjects[numberOfSquares ].shiftObj[1] = offset[1];
            gameObjects[numberOfSquares ].shiftObj[2] = offset[2];
            gameObjects[numberOfSquares ].rotateObj[0] = rotateGlob[0];
            gameObjects[numberOfSquares ].rotateObj[1] = rotateGlob[1];
            gameObjects[numberOfSquares ].rotateObj[2] = rotateGlob[2];

            object.lightOn = lightOnGlobal;
            Draw(object, i++);
        }


        tickCounter++;
        window.display();
    }

    Release();
    return 0;
}