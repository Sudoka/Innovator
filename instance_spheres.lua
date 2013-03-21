
local vertex = [[
#version 330
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 InstancePosition;
layout(location = 2) in vec3 InstanceColor;

uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);
uniform mat4 ProjectionMatrix = mat4(1.0);

out vec4 Normal;
out vec4 ViewPosition;
out vec3 Color;

void main() 
{
  Color = InstanceColor;
  mat4 MV = ViewMatrix * ModelMatrix;
  ViewPosition = MV * vec4(normalize(Position) + InstancePosition, 1.0);
  Normal = MV * vec4(normalize(Position), 0.0);
  gl_Position = ProjectionMatrix * ViewPosition;
}
]]

local fragment = [[
#version 330
layout(location = 0) out vec4 FragColor;

in vec3 Color;
in vec4 Normal;
in vec4 ViewPosition;

void main()
{
/*
  vec3 dx = dFdx(ViewPosition.xyz);
  vec3 dy = dFdy(ViewPosition.xyz);
  vec3 normal = normalize(cross(dx, dy));
*/
  vec3 normal = normalize(Normal.xyz);
  FragColor = vec4(normal.zzz * Color, 1.0);
}
]]

local bbox = 2;
local num_instances = 1;


local t = (1 + 5^0.5) / 2; -- golden ratio
local indices = { {1, 4, 0},  {4, 9, 0}, {4, 5, 9}, {8, 5, 4}, {1, 8, 4},
                  {1, 10, 8}, {10, 3, 8}, {8, 3, 5}, {3, 2, 5}, {3, 7, 2},
                  {3, 10, 7}, {10, 6, 7}, {6, 11, 7}, {6, 0, 11},  {6, 1, 0},
                  {10, 1, 6}, {11, 0, 9}, {2, 11, 9}, {5, 2, 9}, {11, 2, 7} };

local vertices = { {-1, 0, t}, {1, 0, t}, {-1, 0, -t}, {1, 0, -t},
                   {0, t, 1}, {0, t, -1}, {0, -t, 1}, {0, -t, -1},
                   {t, 1, 0}, {-t, 1, 0}, {t, -1, 0}, {-t, -1, 0} };
   
--[[

local t = 1 / math.sqrt(2);
local indices = { { 0, 1, 2 }, { 0, 2, 3 }, { 0, 3, 4 }, { 0, 4, 1 }, { 5, 2, 1 }, { 5, 3, 2 }, { 5, 4, 3 }, { 5, 1, 4 } };
local vertices = { { 0, 0, 1 }, { -t,-t, 0 }, { t,-t, 0 }, { t, t, 0 }, { -t, t, 0 }, { 0, 0,-1 } };
--]]

function instancePositions()
   instances = {}
   for i = 1, 3 * num_instances do
      instances[i] = math.random() * bbox;
   end
   return instances;
end

function instanceColors()
   instances = {}
   for i = 1, 3 * num_instances do
      instances[i] = math.random();
   end
   return instances;
end

subdivide(indices, vertices, 2);

root = Separator {
   IndexBuffer {
      values = flatten(indices);
   },
   -- vertices
   VertexAttribute3f {
      location = 0,
      values = flatten(vertices);
   },
   VertexAttribute3f { 
      divisor = 1,
      location = 1, 
      values = instancePositions();
   },
   VertexAttribute3f { 
      divisor = 1,
      location = 2, 
      values = instanceColors();
   },
 
   BoundingBox { 
      min = { 0, 0, 0 },
      max = { bbox, bbox, bbox }
   },

   Program {
      VertexShader   { source = vertex },
      FragmentShader { source = fragment }
   },

   DrawElementsInstanced { mode = "TRIANGLES" }
}
