
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
local bbox = 1000;

root = Separator {
   IndexBuffer {
      values = { 5, 1, 4, 0, 2, 1, 3, 5, 7, 4, 6, 2, 7, 3 }
   },

   VertexAttribute { 
      size = 3,
      location = 0, 
      VertexBuffer {
         values = { -1, -1, -1, -1, -1,  1, 
                    -1,  1, -1, -1,  1,  1,
                     1, -1, -1,  1, -1,  1,
                     1,  1, -1,  1,  1,  1 } 
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
                for i = 1, 30e6 do
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

   DrawElementsInstanced { mode = "TRIANGLE_STRIP" }
}
