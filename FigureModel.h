#pragma once

#include <vector>
#include <string>
#include <fstream>
#include "ShaderData.h"



class figure_model
{
    struct vertex_data
    {
        int vertex_index;
        int texture_index;
        int normal_index;
    };

    std::vector<std::string> split(std::string s, std::string delimiter) {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector< std::string> res;

        while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    }

public:

    std::vector<Vertex> vertices;
    std::vector<Vertex> normals;
    std::vector<Vertex> texture;
    std::vector<std::vector<vertex_data>> faces;

    int count() const
    {
        return faces.size() * 3;

    }
    figure_model(const std::string& path)
    {
        std::ifstream file(path);
        std::string str;
        while (std::getline(file, str))
        {
            auto arr = split(str, " ");
            if (arr.size() < 3) continue;;
            if (arr[0] == "v")
            {
                vertices.push_back({ std::stof(arr[1])+1, std::stof(arr[2]), std::stof(arr[3]) });//!!!!!!! x+1 - хардкд!!!!!!!!!!!!
            }
            else if (arr[0] == "vt")
            {
                texture.push_back({ std::stof(arr[1]), std::stof(arr[2]),  0 });
            }
            else if (arr[0] == "vn")
            {
                normals.push_back({ std::stof(arr[1]), std::stof(arr[2]), std::stof(arr[3]) });
            }
            else if (arr[0] == "f")
            {
                auto v = std::vector<vertex_data>();
                for (int i = 1; i < 4; i++)
                {
                    auto s = split(arr[i], "/");
                    v.push_back({ std::stoi(s[0]) - 1, std::stoi(s[1]) - 1,std::stoi(s[2]) - 1 });
                }
                faces.push_back(v);

            }
        }
    }

    std::vector<Vertex> get_vertices() const
    {
        auto fsize = faces[0].size();
        auto res = std::vector<Vertex>(faces.size() * fsize);
        for (int i = 0; i < faces.size(); i++)
        {
            for (int j = 0; j < fsize; j++)
            {
                res[fsize * i + j] = vertices[faces[i][j].vertex_index];
            }
        }
        return res;
    }

    std::vector<Vertex> get_texture() const
    {
        auto fsize = faces[0].size();
        auto res = std::vector<Vertex>(faces.size() * fsize);
        for (int i = 0; i < faces.size(); i++)
        {
            for (int j = 0; j < fsize; j++)
            {
                res[fsize * i + j] = texture[faces[i][j].texture_index];
            }
        }
        return res;
    }

    std::vector<Vertex> get_normals() const
    {
        auto fsize = faces[0].size();
        auto res = std::vector<Vertex>(faces.size() * fsize);
        for (int i = 0; i < faces.size(); i++)
        {
            for (int j = 0; j < fsize; j++)
            {
                res[fsize * i + j] = normals[faces[i][j].normal_index];
            }
        }
        return res;
    }

    
};

