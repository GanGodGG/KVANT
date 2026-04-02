#include "ObjectLoader.h"
#include <sstream>
#include <fstream>
#include <algorithm>

void ObjectLoader::LoadModel(string Path, Mesh& mesh)
{
    vector<glm::vec3> temp_positions;
    vector<glm::vec2> temp_textures;
    vector<glm::vec3> temp_normals;

    ifstream ObjectFile(PROJECT_DIR + Path);
    if (!ObjectFile.is_open()) {
        cerr << "Could not open file: " << Path << endl;
        return;
    }

    string line;
    while (getline(ObjectFile, line)) {
        stringstream sub(line);
        string prefix;
        sub >> prefix;

        if (prefix == "v") {
            glm::vec3 pos;
            sub >> pos.x >> pos.y >> pos.z;
            temp_positions.push_back(pos);
        }
        if (prefix == "vt") {
            glm::vec2 tex;
            sub >> tex.x >> tex.y;
            temp_textures.push_back(tex);
        }
        if (prefix == "vn") {
            glm::vec3 norm;
            sub >> norm.x >> norm.y >> norm.z;
            temp_normals.push_back(norm);
        }
        if (prefix == "f") {
            vector<Vertex> faceVertices;
            string vertexBlock;

            // 1. Read every vertex block in the line (handles 3, 4, or more)
            while (sub >> vertexBlock) {
                unsigned int v_idx = 0, t_idx = 0, n_idx = 0;

                // Robust parsing of v/vt/vn
                // Replace '/' with ' ' but handle '//' correctly
                size_t doubleSlash = vertexBlock.find("//");
                if (doubleSlash != string::npos) {
                    replace(vertexBlock.begin(), vertexBlock.end(), '/', ' ');
                    stringstream ss(vertexBlock);
                    ss >> v_idx >> n_idx; // Format: v//vn
                }
                else {
                    replace(vertexBlock.begin(), vertexBlock.end(), '/', ' ');
                    stringstream ss(vertexBlock);
                    ss >> v_idx >> t_idx >> n_idx; // Format: v/vt/vn or v/vt
                }

                // Create a temporary vertex
                Vertex v;
                v.position = temp_positions[v_idx - 1];
                v.color = glm::vec3(1.0f); // Default white
                v.texture = (t_idx > 0) ? temp_textures[t_idx - 1] : glm::vec2(0.0f);
                v.normal = (n_idx > 0) ? temp_normals[n_idx - 1] : glm::vec3(0.0f, 1.0f, 0.0f);

                faceVertices.push_back(v);
            }

            // 2. Triangulate the face (Triangle Fan)
            // If the face is a triangle: loop runs once (0, 1, 2)
            // If the face is a quad: loop runs twice (0, 1, 2) and (0, 2, 3)
            for (size_t i = 1; i < faceVertices.size() - 1; ++i) {
                mesh.vertices.push_back(faceVertices[0]);
                mesh.indices.push_back(mesh.indices.size());

                mesh.vertices.push_back(faceVertices[i]);
                mesh.indices.push_back(mesh.indices.size());

                mesh.vertices.push_back(faceVertices[i + 1]);
                mesh.indices.push_back(mesh.indices.size());
            }
        }
    }
    ObjectFile.close();
}