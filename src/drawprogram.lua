VertexShader = [[
#version 420
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec3 VertexOffset;

uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);
uniform mat4 ProjectionMatrix = mat4(1.0);

out vec4 Normal;

void main() 
{
  mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  mat4 MVP = ProjectionMatrix * ModelViewMatrix;
  Normal = normalize(ModelViewMatrix * vec4(VertexNormal, 0.0));
  gl_Position = MVP * vec4(VertexPosition + VertexOffset, 1.0);
}
]]

FragmentShader = [[
#version 420
layout(location = 0) out vec4 FragColor;

in vec4 Normal;

void main()
{
  FragColor = vec4(Normal.zzz, 1.0);
}
]]
