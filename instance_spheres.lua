
local cull_vertex = [[
#version 330
layout(location = 0) in vec3 Position;

void main()
{
   gl_Position = vec4(Position, 1.0);
}
]]

local cull_geometry = [[
#version 330
layout(points) in;
layout(points, max_vertices = 1) out;
layout(stream = 0) out vec3 Position;

uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);
uniform mat4 ProjectionMatrix = mat4(1.0);

mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;

const vec4 planes[6] = vec4[6](vec4( 1.0, 0.0, 0.0, 1.0 ),
			       vec4(-1.0, 0.0, 0.0, 1.0 ),
                               vec4( 0.0, 1.0, 0.0, 1.0 ),
                               vec4( 0.0,-1.0, 0.0, 1.0 ),
                               vec4( 0.0, 0.0, 1.0, 1.0 ),
                               vec4( 0.0, 0.0,-1.0, 1.0 ));

bool CullTest(const in float radius)
{
  mat4 cliptoviewspace = transpose(ProjectionMatrix);
  vec4 p = ModelViewMatrix * gl_in[0].gl_Position;
          
  for (int i = 0; i < 6; i++) {
    vec4 plane = cliptoviewspace * planes[i];
    vec3 n = normalize(plane).xyz;
    if (dot(p.xyz, n) + radius < 0.0) return false;
  }
  return true;
}

void main()
{
   if (CullTest(1.0)) {
    Position = gl_in[0].gl_Position.xyz;
    EmitVertex();
    EndPrimitive();
  }
}
]]

local vertex = [[
#version 330
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 InstancePosition;

uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);
uniform mat4 ProjectionMatrix = mat4(1.0);

out vec4 ViewPosition;

void main() 
{
  ViewPosition = ViewMatrix * ModelMatrix * vec4(VertexPosition + InstancePosition, 1.0);
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
  vec3 n = normalize(cross(dx, dy));
  FragColor = vec4(n.zzz, 1.0);
}
]]

local InstancePositions = 
   (function() 
       positions = {};
       for i = 1, 300 do 
          positions[i] = math.random() * 20; 
       end
       return positions;
    end)();

root = Separator {

   VertexAttribute3f {
      divisor = 1,
      location = 1,
      buffer = FeedbackBuffer {
         count = #InstancePositions,
         scene = Separator {
            Program {
               VertexShader { source = cull_vertex; },
               GeometryShader { source = cull_geometry; },
               feedbackVarying = "Position"
            },
            VertexAttribute3f {
               location = 0,
               values = InstancePositions
            },
            DrawArrays { mode = "POINTS" }
         }
      }
   },
   BoundingBox { 
      min = { 0, 0, 0 },
      max = { 20, 20, 20 }
   },
   Program {
      VertexShader   { source = vertex },
      FragmentShader { source = fragment }
   },
   Sphere {
      lod = 2,
   }
}