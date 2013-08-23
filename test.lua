
local Nodes = require "Nodes"

local shader = {
vertex = [[ 
#version 330
layout(location = 0) in vec3 Position;

uniform Transform {
  mat4 ModelMatrix;
};

layout(std140) uniform Camera {
  mat4 ViewMatrix;
  mat4 ProjectionMatrix;
};

out vec4 ViewPosition;

void main() 
{
   ViewPosition = ViewMatrix * ModelMatrix * vec4(Position, 1.0);
   gl_Position = ProjectionMatrix * ViewPosition;
}
]]
,
fragment = [[
#version 330
layout(location = 0) out vec4 FragColor;

in vec4 ViewPosition;

void main()
{
  vec3 dx = dFdx(ViewPosition.xyz);
  vec3 dy = dFdy(ViewPosition.xyz);
  vec3 n = normalize(cross(dx, dy));
  FragColor = vec4(n.zzz, 1.0);
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
   Nodes.Sphere { lod = 3 }
}
