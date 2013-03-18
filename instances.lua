
vertex = [[
#version 330
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 offset;

uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);
uniform mat4 ProjectionMatrix = mat4(1.0);

out vec4 normal;

void main() 
{
  mat4 MV = ViewMatrix * ModelMatrix;
  gl_Position = ProjectionMatrix * MV * vec4(Position + offset, 1.0);
  normal = MV * vec4(normalize(Normal), 0.0);
}
]]

fragment = [[
#version 330
layout(location = 0) out vec4 FragColor;

in vec4 normal;

void main()
{
  FragColor = vec4(normal.zzz, 1.0);
}
]]

local t = (1 + 5^0.5) / 2; -- golden ratio

root = Separator {
   IndexBuffer {
      values = { 1,  4, 0,  4, 9, 0, 4, 5,  9, 8, 5,  4,  1, 8, 4,
                 1, 10, 8, 10, 3, 8, 8, 3,  5, 3, 2,  5,  3, 7, 2,
                 3, 10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11,  6, 1, 0,
                 10, 1, 6, 11, 0, 9, 2, 11, 9, 5, 2,  9, 11, 2, 7 }
   },
   VertexBuffer {
      values = { -1,  0,  t,  1,  0,  t, -1,  0, -t,  1,  0, -t,
                  0,  t,  1,  0,  t, -1,  0, -t,  1,  0, -t, -1,
                  t,  1,  0, -t,  1,  0,  t, -1,  0, -t, -1,  0 }
   },
   VertexAttribute { location = 0, size = 3 },
   VertexAttribute { location = 1, size = 3 },

   VertexBuffer {
      values = 
         (function () 
             instances = {}
             for i = 1, 3000000 do
                instances[i] = math.random() * 300
             end
             return instances
          end)()
   },

   VertexAttribute { location = 2, size = 3, divisor = 1 },

   BoundingBox { 
      min = { 0, 0, 0 },
      max = { 300, 300, 300 }
   },

   Program {
      VertexShader   { source = vertex },
      FragmentShader { source = fragment }
   },

   DrawElementsInstanced { mode = "TRIANGLES" },
}
