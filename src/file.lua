
function flatten(list)
  if type(list) ~= "table" then return {list} end
  local flat_list = {}
  for _, elem in ipairs(list) do
    for _, val in ipairs(flatten(elem)) do
      flat_list[#flat_list + 1] = val
    end
  end
  return flat_list
end

function avg(a, b)
   return { (a[1] + b[1]) / 2, (a[2] + b[2]) / 2, (a[3] + b[3]) / 2 };
end

function subdivide(indices, vertices, lod)
   for l = 1, lod do
      local num_faces = #indices;
      for face_index = 1, num_faces do
         local face = indices[face_index];

         local a = vertices[face[1] + 1];
         local b = vertices[face[2] + 1];
         local c = vertices[face[3] + 1];
         
         table.insert(vertices, avg(a, b));
         table.insert(vertices, avg(b, c));
         table.insert(vertices, avg(c, a));
         
         local i = #vertices - 3;
         local j = #vertices - 2;
         local k = #vertices - 1;
         
         table.insert(indices, { i, j, k });
         table.insert(indices, { face[1], i, k });
         table.insert(indices, { i, face[2], j });
         indices[face_index] = { k, j, face[3] };
      end
   end
end

function IndexBuffer(data)
   return Buffer {
      type = "UNSIGNED_INT",
      target = "ELEMENT_ARRAY",
      values = data.values
   }
end

function VertexBuffer(data)
   return Buffer {
      type = "FLOAT",
      target = "ARRAY",
      values = data.values
   }
end

function VertexAttribute3f(data)
   return VertexAttribute {
      size = 3,
      divisor = data.divisor,
      location = data.location,
      VertexBuffer {
         values = data.values
      }
   }
end

local vertex = [[
#version 330
layout(location = 0) in vec3 Vertex;

uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);
uniform mat4 ProjectionMatrix = mat4(1.0);

out vec3 Position;

void main() 
{
  mat4 MV = ViewMatrix * ModelMatrix;
  Position = vec3(MV * vec4(Vertex, 1.0));
  gl_Position = ProjectionMatrix * vec4(Position, 1.0);
}
]]

local fragment = [[
#version 330
layout(location = 0) out vec4 FragColor;

in vec3 Position;

void main()
{
  vec3 dx = dFdx(Position);
  vec3 dy = dFdy(Position);
  vec3 n = normalize(cross(dx, dy));
  FragColor = vec4(n.zzz, 1.0);
}
]]

function Box(data)
   return Separator {
      Program {
         VertexShader { source = vertex },
         FragmentShader { source = fragment },
      },
      IndexBuffer {
         values = { 0, 1, 3,  3, 2, 0,
                    1, 5, 7,  7, 3, 1,
                    5, 4, 6,  6, 7, 5,
                    4, 0, 2,  2, 6, 4,
                    2, 3, 7,  7, 6, 2,
                    1, 0, 4,  4, 5, 1 }
      },
      VertexAttribute {
         location = 0,
         VertexBuffer {
            values = { -1, -1, -1, -1, -1,  1, 
                       -1,  1, -1, -1,  1,  1,
                        1, -1, -1,  1, -1,  1,
                        1,  1, -1,  1,  1,  1 } 
         }
      },
      BoundingBox { min = { -1, -1, -1 }, max = { 1, 1, 1 } },
      DrawElements { mode = "TRIANGLES" }
   }
end

function Sphere(data)
   local t = (1 + 5^0.5) / 2; -- golden ratio
   return Separator {
      Program {
         VertexShader { source = vertex },
         FragmentShader { source = fragment },
      },
      IndexBuffer {
         values = { 1,  4, 0,  4, 9, 0, 4, 5,  9, 8, 5,  4,  1,  8, 4,
                    1, 10, 8, 10, 3, 8, 8, 3,  5, 3, 2,  5,  3,  7, 2,
                    3, 10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11,  6,  1, 0,
                    10, 1, 6, 11, 0, 9, 2, 11, 9, 5, 2,  9, 11, 2, 7 }
      },
      -- vertices
      VertexAttribute {
         size = 3,
         location = 0,
         VertexBuffer { 
            values = { -1,  0,  t,  1,  0,  t, -1,  0, -t,  1,  0, -t,
                        0,  t,  1,  0,  t, -1,  0, -t,  1,  0, -t, -1,
                        t,  1,  0, -t,  1,  0,  t, -1,  0, -t, -1,  0 }
         }
      },
      BoundingBox { min = { -1, -1, -1 }, max = { 1, 1, 1 } },
      DrawElements { mode = "TRIANGLES" }
   }
end