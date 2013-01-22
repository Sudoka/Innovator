#version 420 core

//-- Vertex

layout(location = 1) in vec3 VertexPosition;

void main() 
{
  gl_Position = vec4(VertexPosition, 1.0);
}

//-- Geometry

layout(points) in;
layout(points, max_vertices = 1) out;
layout(stream = 0) out vec3 Position;
layout(binding = 0) uniform atomic_uint instancecount;

uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);
uniform mat4 ProjectionMatrix = mat4(1.0);

const vec4 frustum_planes[6] = vec4[6](vec4( 1.0, 0.0, 0.0, 1.0 ),
				       vec4(-1.0, 0.0, 0.0, 1.0 ),
				       vec4( 0.0, 1.0, 0.0, 1.0 ),
				       vec4( 0.0,-1.0, 0.0, 1.0 ),
				       vec4( 0.0, 0.0, 1.0, 1.0 ),
				       vec4( 0.0, 0.0,-1.0, 1.0 ));


mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
mat4 ClipToViewSpace = transpose(ProjectionMatrix);


bool CullTest(const in float radius)
{
  vec4 p = ModelViewMatrix * gl_in[0].gl_Position;
          
  for (int i = 0; i < 6; i++) {
    vec4 plane = ClipToViewSpace * frustum_planes[i];
    vec3 n = normalize(plane).xyz;
    if (dot(p.xyz, n) + radius < 0.0) return false;
  }
  return true;
}

void main()
{
  if (CullTest(length(vec3(1.0)))) {
    Position = gl_in[0].gl_Position.xyz;
    atomicCounterIncrement(instancecount);
    EmitStreamVertex(0);
  }
}
