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

