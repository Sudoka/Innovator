VertexShader = [[
#version 420
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

out vec3 Normal;
out vec3 Position;

void main() 
{
  Normal = VertexNormal;
  Position = VertexPosition;
}
]]

GeometryShader = [[
#version 420
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

FragmentShader = [[
#version 420
layout(location = 0) out vec4 FragColor;

void main()
{
  FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}
]]