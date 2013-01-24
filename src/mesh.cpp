#include <mesh.h>
#include <nodes.h>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

using namespace std;
using namespace glm;

Mesh::Mesh(InstancedTriangleSet * shape)
  : self(shape)
{
}

Mesh::~Mesh()
{

}

void
Mesh::inflate()
{
  vector<vec3> vertices = self->vertices;
  self->vertices.clear();

  for (size_t i = 0; i < self->indices.size(); i++) {
    for (size_t j = 0; j < 3; j++) {
      self->vertices.push_back(vertices[self->indices[i][j]]);
      self->indices[i][j] = self->vertices.size() - 1;
    }
  }
}

static int find(const vector<vec3> & vertices, vec3 & vertex)
{
  for (size_t i = 0; i < vertices.size(); i++) {
    if (length(vertices[i] - vertex) < 0.01) {
      return i;
    }
  }
  return -1;
}

void 
Mesh::deflate()
{
  vector<vec3> vertices = self->vertices;
  self->vertices.clear();

  for (size_t i = 0; i < self->indices.size(); i++) {
    for (int j = 0; j < 3; j++) {
      vec3 vertex = vertices[self->indices[i][j]];
      int ix = find(self->vertices, vertex);
      if (ix < 0) {
        self->indices[i][j] = self->vertices.size();
        self->vertices.push_back(vertex);
      } else {
        self->indices[i][j] = ix;
      }
    }
  }
}

void
Mesh::generatePerFaceNormals()
{
  this->inflate();
  self->normals.resize(self->vertices.size());

  for (size_t i = 0; i < self->indices.size(); i++) {
    vec3 facenormal = this->getFaceNormal(self->indices[i]);
    self->normals[self->indices[i][0]] = facenormal;
    self->normals[self->indices[i][1]] = facenormal;
    self->normals[self->indices[i][2]] = facenormal;
  }  
}

void
Mesh::generatePerVertexNormals()
{
  this->deflate();
  self->normals.resize(self->vertices.size());

  fill(self->normals.begin(), self->normals.end(), vec3(0));

  for (size_t i = 0; i < self->vertices.size(); i++) {
    for (size_t j = 0; j < self->indices.size(); j++) {
      if (any(equal(ivec3(i), self->indices[j]))) {
        self->normals[i] += this->getFaceNormal(self->indices[j]);
      }
    }
    self->normals[i] = normalize(self->normals[i]);
  }
}

vec3
Mesh::getFaceNormal(const ivec3 & face)
{
  vec3 p0 = self->vertices[face[0]];
  vec3 p1 = self->vertices[face[1]];
  vec3 p2 = self->vertices[face[2]];
  
  vec3 v0 = p1 - p0;
  vec3 v1 = p2 - p0;
  
  return normalize(cross(v0, v1));
}

void
Mesh::subdivide()
{
  int num_faces = self->indices.size();
  for (int faceindex = 0; faceindex < num_faces; faceindex++) {
    ivec3 face = self->indices[faceindex];
    
    vec3 a = self->vertices[face[0]];
    vec3 b = self->vertices[face[1]];
    vec3 c = self->vertices[face[2]];
    
    self->vertices.push_back(mix(a, b, 0.5));
    self->vertices.push_back(mix(b, c, 0.5));
    self->vertices.push_back(mix(c, a, 0.5));
    
    int i = self->vertices.size() - 3;
    int j = self->vertices.size() - 2;
    int k = self->vertices.size() - 1;
    
    self->indices.push_back(ivec3(i, j, k));
    self->indices.push_back(ivec3(face[0], i, k));
    self->indices.push_back(ivec3(i, face[1], j));
    self->indices[faceindex] = ivec3(k, j, face[2]);
    }
}
