
vertex = [[
#version 420
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;

uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);
uniform mat4 ProjectionMatrix = mat4(1.0);

out vec4 normal;

void main() 
{
  mat4 MV = ViewMatrix * ModelMatrix;
  gl_Position = ProjectionMatrix * MV * vec4(Position, 1.0);
  normal = MV * vec4(normalize(Normal), 0.0);
}
]]

fragment = [[
#version 420
layout(location = 0) out vec4 FragColor;

in vec4 normal;

void main()
{
  FragColor = vec4(normal.zzz, 1.0);
}
]]

root = Separator {
   Program {
      VertexShader { source = vertex },
      FragmentShader { source = fragment }
   },
   Sphere {
     radius = 1.0
   }
}
