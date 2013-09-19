
local Nodes = require "Nodes"

local shader = {
vertex = [[ 
#version 330
layout(location = 0) in vec3 VertexPosition;

uniform Transform {
  mat4 ModelMatrix;
};

layout(std140) uniform Camera {
  mat4 ViewMatrix;
  mat4 ProjectionMatrix;
};

out vec4 position;

void main() 
{
   mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
   position = ModelViewMatrix * vec4(VertexPosition, 1.0);
   gl_Position = ProjectionMatrix * position;
}
]]
,
fragment = [[
#version 330
layout(location = 0) out vec4 FragColor;

in vec4 position;

void main()
{
  vec3 dx = dFdx(position.xyz);
  vec3 dy = dFdy(position.xyz);
  vec3 n = normalize(cross(dx, dy));
  FragColor = vec4(n.zzz, 1.0);
}
]]
}

local box = Shape {
   indices = { 0, 1, 3, 3, 2, 0, 1, 5, 7, 7, 3, 1, 5, 4, 6, 6, 7, 5, 
               4, 0, 2, 2, 6, 4, 2, 3, 7, 7, 6, 2, 1, 0, 4, 4, 5, 1 },
   vertices = { -1, -1, -1, -1, -1,  1, -1,  1, -1, -1,  1,  1,
                 1, -1, -1,  1, -1,  1,  1,  1, -1,  1,  1,  1 }
}


SceneRoot = Nodes.Separator {
   Nodes.Program {
      Nodes.VertexShader { source = shader.vertex },
      Nodes.FragmentShader { source = shader.fragment }
   },
   Separator {
      Transform { translation = { 0, 0, 0 } },
      box
   },
   Separator {
      Transform { translation = { 0, 0, 3 } },
      box
   },
   Separator {
      Transform { translation = { 0, 3, 0 } },
      box
   },
   Separator {
      Transform { translation = { 0, 3, 3 } },
      box
   },
   Separator {
      Transform { translation = { 3, 0, 0 } },
      box
   },
   Separator {
      Transform { translation = { 3, 0, 3 } },
      box
   },
   Separator {
      Transform { translation = { 3, 3, 0 } },
      box
   },
   Separator {
      Transform { translation = { 3, 3, 3 } },
      box
   }
}
