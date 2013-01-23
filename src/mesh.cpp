#include <mesh.h>
#include <nodes.h>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

using namespace std;
using namespace glm;

class Mesh::MeshP {
public:
  MeshP() {
  }
  ~MeshP() {
  }
  vector<vec3> normals;
  vector<vec3> vertices;
  vector<vec4> texcoords;
  vector<ivec3> indices;
};

Mesh::Mesh(InstancedTriangleSet * shape)
  : self(new MeshP)
{
  self->indices = shape->indices;
  self->vertices = shape->vertices;
}

Mesh::~Mesh()
{

}

void
Mesh::inflate()
{

}

void 
Mesh::deflate()
{

}

void
Mesh::generateNormals()
{

}

void
Mesh::subdivide(int lod)
{
  for (int l = 0; l < lod; l++) {
    int num_faces = self->indices.size();
    for (int faceindex = 0; faceindex < num_faces; faceindex++) {
      ivec3 face = self->indices[faceindex];
      
      vec3 a = self->vertices[face[0]];
      vec3 b = self->vertices[face[1]];
      vec3 c = self->vertices[face[2]];
      
      self->vertices.push_back(a + b * 0.5f);
      self->vertices.push_back(b + c * 0.5f);
      self->vertices.push_back(c + a * 0.5f);
      
      int i = self->vertices.size() - 3;
      int j = self->vertices.size() - 2;
      int k = self->vertices.size() - 1;
      
      self->indices.push_back(ivec3(i, j, k));
      self->indices.push_back(ivec3(face[0], i, k));
      self->indices.push_back(ivec3(i, face[1], j));
      self->indices[faceindex] = ivec3(k, j, face[2]);
    }
  }
}
