
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
layout(triangle_strip, max_vertices = 4) out;

out vec2 QuadCoord;
out vec4 ViewPosition;

uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);
uniform mat4 ProjectionMatrix = mat4(1.0);

const vec2 view_quad[4] = vec2[4](vec2(-1.0, -1.0 ),
				  vec2( 1.0, -1.0 ),
				  vec2(-1.0,  1.0 ),
				  vec2( 1.0,  1.0 ));

void main()
{
  mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  ViewPosition = ModelViewMatrix * gl_in[0].gl_Position;
  for (int i = 0; i < 4; i++) {
    QuadCoord = view_quad[i];
    gl_Position = ModelViewMatrix * gl_in[0].gl_Position;
    gl_Position.xy += view_quad[i];
    gl_Position = ProjectionMatrix * gl_Position;
    EmitVertex();
  }
  EndPrimitive();
}
]]

local fragment = [[
#version 330
layout(location = 0) out vec4 FragColor;

uniform mat4 ProjectionMatrix = mat4(1.0);

in vec2 QuadCoord;
in vec4 ViewPosition;

void main()
{
  float x = QuadCoord.s;
  float y = QuadCoord.t;
  float z = 1.0 - x*x - y*y;
        
  if (z <= 0.0) discard;
  z = sqrt(z);
  vec3 n = vec3(x, y, z);

  FragColor = vec4(n.zzz, 1.0);

  vec4 pos = ViewPosition;
  pos.z += n.z;
  pos = ProjectionMatrix * pos;

  gl_FragDepth = (pos.z / pos.w + 1.0) / 2.0;
}
]]

local t = (1 + 5^0.5) / 2; -- golden ratio
local bbox = 1000;

root = Separator {
   VertexAttribute { 
      size = 3,
      location = 0, 
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
      GeometryShader { source = geometry },
      FragmentShader { source = fragment }
   },
   DrawArrays { mode = "POINTS" }
}
