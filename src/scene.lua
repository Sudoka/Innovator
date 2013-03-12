
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

normal_vertex = [[
#version 330
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec3 offset;

out vec3 Normal;
out vec3 Position;

void main() 
{
  Normal = VertexNormal;
  Position = VertexPosition + offset;
}
]]

normal_geometry = [[
#version 330
layout(points) in;
layout(line_strip, max_vertices = 2) out;

uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);
uniform mat4 ProjectionMatrix = mat4(1.0);

in vec3 Normal[];
in vec3 Position[];

void main()
{
  mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

  gl_Position = MVP * vec4(Position[0], 1.0);
  EmitVertex();
  gl_Position = MVP * vec4(Position[0] + Normal[0], 1.0);
  EmitVertex();
  EndPrimitive();
}
]]

normal_fragment = [[
#version 330
layout(location = 0) out vec4 FragColor;

void main()
{
  FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}
]]

local t = (1 + 5^0.5) / 2; -- golden ratio

root = Separator {
   Transform {
      scaleFactor = { 1, 1, 1 }
   },
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
   VertexAttribute { location = 0 },
   VertexAttribute { location = 1 },

   VertexAttribute { 
      size = 3,
      divisor = 1,
      location = 2, 
      VertexBuffer {
         values = { 0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 3, 3, 3, 0, 0, 3, 0, 3, 3, 3, 0, 3, 3, 3 } 
      }
   },
   Program {
      VertexShader   { source = vertex },
      FragmentShader { source = fragment }
   },
   Shape {
      mode = "TRIANGLES"
   }
--   DrawElementsInstanced { mode = "TRIANGLES" },

--   Program {
--      VertexShader   { source = normal_vertex },
--      GeometryShader { source = normal_geometry },
--      FragmentShader { source = normal_fragment }
--   },

--   DrawArraysInstanced { mode = "POINTS" }
}
