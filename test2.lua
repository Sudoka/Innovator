
local Nodes = require "Nodes"

local shader = {
vertex = [[ 
#version 330
layout(location = 0) in vec3 VertexPosition;

layout(std140) uniform Transform {
  mat4 iv_ViewMatrix;
  mat4 iv_ProjectionMatrix;
  mat4 ModelMatrix[8];
};

struct MaterialInfo {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
};

layout(std140) uniform Material {
  MaterialInfo Materials[8];
};

MaterialInfo iv_Material = Materials[gl_InstanceID];
mat4 iv_ModelViewMatrix = iv_ViewMatrix * ModelMatrix[gl_InstanceID];
vec4 iv_Vertex = vec4(VertexPosition, 1.0);

out vec4 position;
out vec3 color;

void main() 
{
   color = iv_Material.diffuse.rgb;
   position = iv_ModelViewMatrix * iv_Vertex;
   gl_Position = iv_ProjectionMatrix * position;
}
]]
,
fragment = [[
#version 330
layout(location = 0) out vec4 iv_FragColor;

in vec4 position;
in vec3 color;

void main()
{
  vec3 dx = dFdx(position.xyz);
  vec3 dy = dFdy(position.xyz);
  vec3 n = normalize(cross(dx, dy));
  iv_FragColor = vec4(color * n.zzz, 1.0);
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
      Material { diffuse = { 1, 1, 1 } },
      box
   },
   Separator {
      Transform { translation = { 0, 0, 3 } },
      Material { diffuse = { 1, 1, 0 } },
      box
   },
   Separator {
      Transform { translation = { 0, 3, 0 } },
      Material { diffuse = { 1, 0, 1 } },
      box
   },
   Separator {
      Transform { translation = { 0, 3, 3 } },
      Material { diffuse = { 1, 0, 0 } },
      box
   },
   Separator {
      Transform { translation = { 3, 0, 0 } },
      Material { diffuse = { 0, 1, 1 } },
      box
   },
   Separator {
      Transform { translation = { 3, 0, 3 } },
      Material { diffuse = { 0, 0, 1 } },
      box
   },
   Separator {
      Transform { translation = { 3, 3, 0 } }, 
      Material { diffuse = { 0, 1, 0 } },
     box
   },
   Separator {
      Transform { translation = { 3, 3, 3 } },
      Material { diffuse = { 0, 0, 0 } },
      box
   }
}
