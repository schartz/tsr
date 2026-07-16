#include "obj_loader.h"
#include "geometry_types.h"
#include <fstream>
#include <sstream>
#include <string>

ObjModel::ObjModel(const std::string filename) {

    std::ifstream in_file_stream;
    in_file_stream.open(filename, std::ifstream::in);
    if (in_file_stream.fail()) {
        std::cout << "Could not open specified model file" << std::endl;
        return;
    }

    std::string line;
    while (!in_file_stream.eof()) {
        std::getline(in_file_stream, line);
        std::istringstream single_line(line.c_str());
        char trash;

        if (!line.compare(0, 2, "v ")) {
            single_line >> trash;
            vec3 v;
            for (int i : {0, 1, 2})
                single_line >> v[i];
            this->verts.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            int face, tangent, normal, count = 0;
            single_line >> trash;
            while (single_line >> face >> trash >> tangent >> trash >> normal) {
                this->face_triangle_verts.push_back(--face);
                count++;
            }
            if (count != 3) {
                std::cerr << "Error: the obj file must be triangulated" << std::endl;
                return;
            }
        }
    }

    std::cerr << "# v# " << this->nfaces() << " f# " << this->nfaces() << std::endl;
}

int ObjModel::nverts() const { return verts.size(); }
int ObjModel::nfaces() const { return face_triangle_verts.size() / 3; }

vec3 ObjModel::vert(const int idx) const { return verts[idx]; }

vec3 ObjModel::vert(const int idx_face, const int nth_vert) const {
    return verts[face_triangle_verts[idx_face * 3 + nth_vert]];
}
