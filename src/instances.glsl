#version 400 core

//-- Vertex

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexOffset;

uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);
uniform mat4 ProjectionMatrix = mat4(1.0);

void main() 
{
  mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
  gl_Position = MVP * vec4(VertexPosition + VertexOffset, 1.0);
}

//-- Fragment

layout(location = 0) out vec4 FragColor;

void main()
{
  FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}
