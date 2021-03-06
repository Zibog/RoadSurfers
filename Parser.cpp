#include "vector"
#include "string"
#include "Polyhedron.h"
using namespace std;

vector<string> parse(string line, string delimiter)
{
    line += delimiter;//?????? delimiter ? ????? ???? ??????
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

void parseObjFile(Polyhedron& my_polyhedron, string path)
{
    std::string line;

    std::ifstream in(path); // ???????? ???? ??? ??????
    if (in.is_open())
    {
        while (getline(in, line))
        {
            if (line.size() == 0 || line[0] == '#')
                continue;

            if (line[0] == 'v')
            {
                if (line[1] == ' ') // ???????????? ???????
                {
                    line.erase(0, 2); // ???????? "v "

                    vector<string> res = parse(line, " ");

                    my_polyhedron.vertices.push_back(Point3D(stof(res[0]), stof(res[1]), stof(res[2])));
                }
                else if (line[1] == 'n') // ???????
                {
                    line.erase(0, 3); // ???????? "vn "
                    vector<string> res = parse(line, " ");

                    my_polyhedron.normals.push_back(Point3D(stof(res[0]), stof(res[1]), stof(res[2])));
                }
                else if (line[1] == 't') // ????????
                {
                    line.erase(0, 3); // ???????? "vt "
                    vector<string> res = parse(line, " ");

                    my_polyhedron.textures.push_back(PointUV(stof(res[0]), stof(res[1])));
                }
            }
            else if (line[0] == 'f')
            {
                line.erase(0, 2); // ???????? "f "

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
            }
        }
    }
    in.close();     // ????????? ????
}