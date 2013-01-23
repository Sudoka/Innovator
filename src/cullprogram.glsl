#version 420 core

//-- Vertex

layout(location = 1) in vec3 VertexPosition;
out vec3 Position;

void main() 
{
  Position = VertexPosition;
}

//-- Geometry

layout(points) in;
layout(points, max_vertices = 1) out;
layout(stream = 0) out vec3 PositionOut;
layout(binding = 0) uniform atomic_uint instancecount;

in vec3 Position[];

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
  vec4 p = ModelViewMatrix * vec4(Position[0], 1.0);
          
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
    PositionOut = Position[0];
    atomicCounterIncrement(instancecount);
    EmitStreamVertex(0);
  }
}
