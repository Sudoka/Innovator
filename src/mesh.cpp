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
  vector<ivec3> indices = self->indices;
  vector<vec3> vertices = self->vertices;

  self->indices.clear();
  self->vertices.clear();

  for (int i = 0; i < indices.size(); i++) {
    ivec3 face = indices[i];
    self->vertices.push_back(vertices[face[0]]);
    self->vertices.push_back(vertices[face[1]]);
    self->vertices.push_back(vertices[face[2]]);

    self->indices.push_back(ivec3(self->vertices.size() - 3,
                                  self->vertices.size() - 2,
                                  self->vertices.size() - 1));
  }
}

void 
Mesh::deflate()
{

}

void
Mesh::generateNormals()
{
  this->inflate();
  self->normals.resize(self->vertices.size());

  for (int i = 0; i < self->indices.size(); i++) {
    ivec3 face = self->indices[i];
    vec3 p0 = self->vertices[face[0]];
    vec3 p1 = self->vertices[face[1]];
    vec3 p2 = self->vertices[face[2]];

    vec3 v0 = p1 - p0;    
    vec3 v1 = p2 - p0;
    vec3 n =  glm::normalize(glm::cross(v0, v1));

    self->normals[face[0]] = n;
    self->normals[face[1]] = n;
    self->normals[face[2]] = n;
  }  
}

void
Mesh::normalize()
{
	for (int i = 0; i < self->vertices.size(); i++) {
		self->vertices[i] = glm::normalize(self->vertices[i]);
	}
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
      
      self->vertices.push_back((a + b) * 0.5f);
      self->vertices.push_back((b + c) * 0.5f);
      self->vertices.push_back((c + a) * 0.5f);
      
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
