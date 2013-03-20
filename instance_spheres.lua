
local vertex = [[
#version 330
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 InstancePosition;
layout(location = 2) in vec3 InstanceColor;

uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);
uniform mat4 ProjectionMatrix = mat4(1.0);

out vec4 ViewPosition;
out vec3 Color;

void main() 
{
  Color = InstanceColor;
  mat4 MV = ViewMatrix * ModelMatrix;
  ViewPosition = MV * vec4(normalize(Position) + InstancePosition, 1.0);
  gl_Position = ProjectionMatrix * ViewPosition;
}
]]

local fragment = [[
#version 330
layout(location = 0) out vec4 FragColor;

in vec3 Color;
in vec4 ViewPosition;

void main()
{
  vec3 dx = dFdx(ViewPosition.xyz);
  vec3 dy = dFdy(ViewPosition.xyz);
  vec3 normal = normalize(cross(dx, dy));
  FragColor = vec4(normal.zzz * Color, 1.0);
}
]]

local bbox = 2;
local num_instances = 1;

--[[
local t = (1 + 5^0.5) / 2; -- golden ratio
local indices = { 1,  4, 0,  4, 9, 0, 4, 5,  9, 8, 5,  4,  1,  8, 4,
                  1, 10, 8, 10, 3, 8, 8, 3,  5, 3, 2,  5,  3,  7, 2,
                  3, 10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11,  6,  1, 0,
                  10, 1, 6, 11, 0, 9, 2, 11, 9, 5, 2,  9, 11, 2, 7 };

local vertices = { -1,  0,  t,  1,  0,  t, -1,  0, -t,  1,  0, -t,
                    0,  t,  1,  0,  t, -1,  0, -t,  1,  0, -t, -1,
                    t,  1,  0, -t,  1,  0,  t, -1,  0, -t, -1,  0 };
   
--]]

local t = 1 / math.sqrt(2);
local indices = { 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1, 5, 2, 1, 5, 3, 2, 5, 4, 3, 5, 1, 4 };
local vertices = { 0, 0, 1, -t,-t, 0, t,-t, 0, t, t, 0, -t, t, 0, 0, 0,-1 };

function subdivide(lod)
   for l = 1, lod do
      local num_indices = #indices / 3;
      for face_index = 0, num_indices - 1, 1 do
         local i0 = indices[face_index * 3 + 1];
         local i1 = indices[face_index * 3 + 2];
         local i2 = indices[face_index * 3 + 3];
         
         local a = { vertices[i0 * 3 + 1], vertices[i0 * 3 + 2], vertices[i0 * 3 + 3] };
         local b = { vertices[i1 * 3 + 1], vertices[i1 * 3 + 2], vertices[i1 * 3 + 3] };
         local c = { vertices[i2 * 3 + 1], vertices[i2 * 3 + 2], vertices[i2 * 3 + 3] };
         
         table.insert(vertices, (a[1] + b[1]) / 2);
         table.insert(vertices, (a[2] + b[2]) / 2);
         table.insert(vertices, (a[3] + b[3]) / 2);
         
         table.insert(vertices, (b[1] + c[1]) / 2);
         table.insert(vertices, (b[2] + c[2]) / 2);
         table.insert(vertices, (b[3] + c[3]) / 2);
         
         table.insert(vertices, (c[1] + a[1]) / 2);
         table.insert(vertices, (c[2] + a[2]) / 2);
         table.insert(vertices, (c[3] + a[3]) / 2);
         
         local i = (#vertices / 3) - 3;
         local j = (#vertices / 3) - 2;
         local k = (#vertices / 3) - 1;
         
         table.insert(indices, i);
         table.insert(indices, j);
         table.insert(indices, k);
         
         table.insert(indices, i0);
         table.insert(indices, i);
         table.insert(indices, k);
         
         table.insert(indices, i);
         table.insert(indices, i1);
         table.insert(indices, j);
         
         indices[face_index * 3 + 1] = k;
         indices[face_index * 3 + 2] = j;
         indices[face_index * 3 + 3] = i2;
      end
   end
end

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

subdivide(3);

root = Separator {
   IndexBuffer {
      values = indices;
   },
   -- vertices
   VertexAttribute3f {
      location = 0,
      values = vertices;
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
