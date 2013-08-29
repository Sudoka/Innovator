
local Nodes = require "Nodes"

local shader = {
vertex = [[ 
#version 330
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

uniform Transform {
  mat4 ModelMatrix;
};

layout(std140) uniform Camera {
  mat4 ViewMatrix;
  mat4 ProjectionMatrix;
};

out vec3 Normal;

void main() 
{
   mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
   mat3 NormalMatrix = inverse(transpose(mat3(ModelViewMatrix)));
   Normal = normalize(NormalMatrix * VertexNormal);
   gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(VertexPosition, 1.0);
}
]]
,
fragment = [[
#version 330
layout(location = 0) out vec4 FragColor;

in vec3 Normal;

void main()
{
  FragColor = vec4(Normal.zzz, 1.0);
}
]]
}

SceneRoot = Nodes.Separator {
   Nodes.Program {
      Nodes.VertexShader { source = shader.vertex },
      Nodes.FragmentShader { source = shader.fragment }
   },
   Nodes.Transform { translation = { 2, 2, 2 } },
   Nodes.Box {},
   Nodes.Transform { translation = { 2, 2, 2 } },
   Nodes.Sphere { lod = 0 },
   Nodes.Transform { translation = { 2, 2, 2 } },
   Nodes.Cylinder { lod = 2 }
}
