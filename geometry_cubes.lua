
local vertex = [[
#version 330
layout(location = 0) in vec3 Position;

void main() 
{
  gl_Position = vec4(Position, 1.0);
}
]]

local geometry = [[
#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 14) out;

out vec4 ViewPosition;

uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);
uniform mat4 ProjectionMatrix = mat4(1.0);

const vec3 cube_vertices[8] =  vec3[8](vec3(-1.0, -1.0, -1.0 ),
				       vec3(-1.0, -1.0,  1.0 ),
				       vec3(-1.0,  1.0, -1.0 ),
				       vec3(-1.0,  1.0,  1.0 ),
				       vec3( 1.0, -1.0, -1.0 ),
				       vec3( 1.0, -1.0,  1.0 ),
				       vec3( 1.0,  1.0, -1.0 ),
				       vec3( 1.0,  1.0,  1.0 ));
        
const int cube_indices[14] =  int[14](5, 1, 4, 0, 2, 1, 3, 
    				      5, 7, 4, 6, 2, 7, 3);

void main()
{
  mat4 MV = ViewMatrix * ModelMatrix;
  for (int i = 0; i < 14; i++) {
    vec3 vertex = cube_vertices[ cube_indices[i] ];
    ViewPosition = gl_in[0].gl_Position;
    ViewPosition.xyz += vertex;
    ViewPosition = MV * ViewPosition;
    gl_Position = ProjectionMatrix * ViewPosition;
    EmitVertex();
  }
  EndPrimitive();
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

root = Separator {
   VertexAttribute { 
      size = 3,
      location = 0, 
      VertexBuffer {
         values = 
            (function () 
                instances = {}
                for i = 1, 3000000 do
                   instances[i] = math.random() * 300
                end
                return instances
             end)()
     }
   },
   BoundingBox { 
      min = { 0, 0, 0 },
      max = { 300, 300, 300 }
   },
   Program {
      VertexShader   { source = vertex },
      GeometryShader { source = geometry },
      FragmentShader { source = fragment }
   },
   DrawArrays { mode = "POINTS" }
}
