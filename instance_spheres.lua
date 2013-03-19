
local vertex = [[
#version 330
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 offset;

uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);
uniform mat4 ProjectionMatrix = mat4(1.0);

out vec4 ViewPosition;

void main() 
{
  mat4 MV = ViewMatrix * ModelMatrix;
  ViewPosition = MV * vec4(Position + offset, 1.0);
  gl_Position = ProjectionMatrix * ViewPosition;
}
]]

local fragment = [[
#version 330
layout(location = 0) out vec4 FragColor;

in vec4 ViewPosition;

void main()
{
  vec3 dx = dFdx(ViewPosition.xyz);
  vec3 dy = dFdy(ViewPosition.xyz);
  vec3 normal = normalize(cross(dx, dy));
  FragColor = vec4(normal.zzz, 1.0);
}
]]

local t = (1 + 5^0.5) / 2; -- golden ratio
local bbox = 10;

root = Separator {
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
   VertexAttribute { 
      size = 3, 
      divisor = 1,
      location = 1, 
      VertexBuffer {
         values = 
            (function () 
                instances = {}
                for i = 1, 30 do
                   instances[i] = math.random() * bbox;
                end
                return instances;
             end)()
      }
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
