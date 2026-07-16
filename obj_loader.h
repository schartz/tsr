#include "geometry_types.h"
#include <vector>
class ObjModel {
    std::vector<vec3> verts = {};              // array of indices
    std::vector<int> face_triangle_verts = {}; // per triangle index in the array `verts`

  public:
    ObjModel(const std::string filename);

    int nverts() const; // num verts in model
    int nfaces() const; // num faces in model

    vec3 vert(const int idx) const; // get a vert by it's index
    vec3 vert(const int idx_face,
              const int nth_vert) const; // get nth vert from a face
};
