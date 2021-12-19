#pragma once
#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <vector>

#include <iostream>
#include <fstream>

using namespace std;

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
	vector<Point3D> points;
    vector<Point3D> normal;
    vector<PointUV> texture;
	Triangle(vector<Point3D> pnts, vector<Point3D> nrml, vector<PointUV> txtr)
	{
		points = pnts;
		normal = nrml;
        texture = txtr;
	}
};

struct Polyhedron
{
	vector<Point3D> vertices;
	vector<Point3D> normals;
	vector<Triangle> polygons;
    vector<PointUV> textures;
    vector<int> indexes;
};

vector<string> parse(string line, string delimiter)
{
    line += delimiter;//добавь delimiter в конце кажд строки
    vector<string> res;
    size_t pos = 0;
    std::string token;
    while ((pos = line.find(delimiter)) != std::string::npos) {
        token = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());
        res.push_back(token);
        //cout << token << endl;
    }
    return res;
}

void parseObjFile(Polyhedron & my_polyhedron, string path)
{
    std::string line;

    std::ifstream in(path); // окрываем файл для чтения
    if (in.is_open())
    {
        while (getline(in, line))
        {
            if (line.size() == 0 || line[0] == '#')
                continue;

            if (line[0] == 'v')
            {
                if (line[1] == ' ') // обрабатываем вершины
                {
                    line.erase(0, 2); // вырезали "v "

                    vector<string> res = parse(line, " ");

                    my_polyhedron.vertices.push_back(Point3D(stof(res[0]), stof(res[1]), stof(res[2])));
                }
                else if (line[1] == 'n') // нормали
                {
                    line.erase(0, 3); // вырезали "vn "
                    vector<string> res = parse(line, " ");

                    my_polyhedron.normals.push_back(Point3D(stof(res[0]), stof(res[1]), stof(res[2])));
                }
                else if (line[1] == 't') // текстуры
                {
                    line.erase(0, 3); // вырезали "vt "
                    vector<string> res = parse(line, " ");

                    my_polyhedron.textures.push_back(PointUV(stof(res[0]), stof(res[1])));
                }
            }
            else if (line[0] == 'f')
            {
                line.erase(0, 2); // вырезали "f "

                vector<string> res = parse(line, " ");
                vector<int> points_indeces;
                vector<int> points_textures;
                vector<int> normal_index;
                for (int i = 0; i < res.size(); i++)
                {
                    vector<string> tmp = parse(res[i], "/");
                    my_polyhedron.indexes.push_back(stoi(tmp[0]) - 1);
                    points_indeces.push_back(stoi(tmp[0]) - 1);
                    points_textures.push_back(stoi(tmp[1]) - 1);
                    normal_index.push_back(stoi(tmp[2]) - 1);
                }
                
                my_polyhedron.polygons.push_back(Triangle({ my_polyhedron.vertices[points_indeces[0]],
                    my_polyhedron.vertices[points_indeces[1]],
                    my_polyhedron.vertices[points_indeces[2]] },

                    { my_polyhedron.normals[normal_index[0]],
                    my_polyhedron.normals[normal_index[1]],
                    my_polyhedron.normals[normal_index[2]], },

                    { my_polyhedron.textures[points_textures[0]],
                    my_polyhedron.textures[points_textures[1]],
                    my_polyhedron.textures[points_textures[2]], }));

                
                /*
                std::string delimiter = " ";

                vector<float> point;

                size_t pos = 0;
                std::string token;
                vector<int> points_indeces;
                int normal_index = 0;

                while ((pos = line.find(delimiter)) != std::string::npos) {
                    token = line.substr(0, pos);
                    line.erase(0, pos + delimiter.length());
                    std::cout << token << std::endl;

                    std::string delimiter2 = "//";
                    size_t pos2 = token.find(delimiter2);
                    std::string token2 = token.substr(0, pos2);

                    int ind = std::stoi(token2);

                    points_indeces.push_back(ind);

                    token.erase(0, pos2 + delimiter2.length());

                    normal_index = std::stoi(token); // noraml
                }*/

                /*my_polyhedron.polygons.push_back(Triangle({ my_polyhedron.vertices[points_indeces[0] - 1],
                    my_polyhedron.vertices[points_indeces[1] - 1],
                    my_polyhedron.vertices[points_indeces[2] - 1] },
                    Point3D(my_polyhedron.normals[normal_index - 1])));*/
            }
        }
    }
    in.close();     // закрываем файл
}