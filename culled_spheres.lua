
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
uniform vec3 lod = vec3(0.0);

const vec4 Frustum[6] = vec4[6](vec4( 1.0, 0.0, 0.0, 1.0 ),
                                vec4(-1.0, 0.0, 0.0, 1.0 ),
				vec4( 0.0, 1.0, 0.0, 1.0 ),
				vec4( 0.0,-1.0, 0.0, 1.0 ),
				vec4( 0.0, 0.0, 1.0, 1.0 ),
				vec4( 0.0, 0.0,-1.0, 1.0 ));

bool CullTest(in vec3 position, const in float radius)
{
  mat4 ClipToViewSpace = transpose(ProjectionMatrix);

  for (int i = 0; i < 6; i++) {
    vec3 normal = normalize(ClipToViewSpace * Frustum[i]).xyz;
    if (dot(position, normal) + radius < 0.0) return false;
  }
  return true;
}

void main()
{
  vec4 Vertex = gl_in[0].gl_Position;
  vec4 ViewPosition = ViewMatrix * ModelMatrix * Vertex;

  float distance = length(ViewPosition.xyz);
  if (distance < lod.x || distance > lod.y) return;

  if (!CullTest(ViewPosition.xyz, 1.0)) return;

  Position = Vertex.xyz;
  EmitVertex();
  EndPrimitive();
}
]]

local vertex = [[
#version 330
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 InstancePosition;

uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);
uniform mat4 ProjectionMatrix = mat4(1.0);

out vec4 Normal;

void main() 
{
  mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  mat4 ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
  vec3 Vertex = VertexPosition + InstancePosition;

  Normal = ModelViewMatrix * vec4(VertexPosition, 0.0);
  gl_Position = ModelViewProjectionMatrix * vec4(Vertex, 1.0);
}
]]

local fragment = [[
#version 330
layout(location = 0) out vec4 FragColor;
uniform vec3 Color;
in vec4 Normal;

void main()
{
  FragColor = vec4(Color * normalize(Normal).zzz, 1.0);
}
]]

local SIZE = 500;
local LOD_RANGE = SIZE / 5;
local NUM_SPHERES = 10e4;

local InstancePositions = 
   (function() 
       positions = {};
       for i = 1, NUM_SPHERES * 3 do 
          positions[i] = math.random() * SIZE;
       end
       return positions;
    end)();

function SceneBuffer(data)
   return FeedbackBuffer {
      count = data.count,
      scene = Separator {
         Uniform3f { 
            name = "lod", 
            value = data.lodrange 
         },
         Program {
            VertexShader { source = cull_vertex; },
            GeometryShader { source = cull_geometry; },
            feedbackVarying = data.varying,
         },
         VertexAttribute3f {
            location = 0,
            values = data.values
         },
         DrawArrays { mode = "POINTS" }
      }
   }
end

function ShapeSet(data)
   return Separator {
      VertexAttribute3f {
         divisor = 1,
         location = 1,
         buffer = SceneBuffer {
            varying = "Position",
            values = data.positions,
            count = #data.positions,
            lodrange = data.lodrange
         }
      },
      Uniform3f { name = "Color", value = data.color },
      Program {
         VertexShader   { source = vertex },
         FragmentShader { source = fragment }
      },
      data.shape
   };
end

SceneRoot = Separator {
   --SceneTexture {
      BoundingBox { 
         min = { 0, 0, 0 },
         max = { SIZE, SIZE, SIZE }
      },
      ShapeSet {
         color = { 1, 0, 0 },
         lodrange = { 0, LOD_RANGE, 0 },
         positions = InstancePositions,
         shape = InstancedSphere {
            lod = 3,
         }
      },
      ShapeSet {
         color = { 1, 0.5, 0 },
         lodrange = { 1 * LOD_RANGE, 2 * LOD_RANGE, 0 },
         positions = InstancePositions,
         shape = InstancedSphere {
            lod = 2,
         }
      },
      ShapeSet {
         color = { 1, 1, 0 },
         lodrange = { 2 * LOD_RANGE, 3 * LOD_RANGE, 0 },
         positions = InstancePositions,
         shape = InstancedSphere {
            lod = 1,
         }
      },
      ShapeSet {
         color = { 0, 1, 0 },
         lodrange = { 3 * LOD_RANGE, 10000, 0 },
         positions = InstancePositions,
         shape = InstancedSphere {
            lod = 0,
         }
      },
   --}
}

postprocessvertex = [[
         #version 330
         layout(location = 0) in vec2 Position;
         out vec2 TexCoord;
         void main() 
         {
           TexCoord = Position * 0.5 + 0.5;
           gl_Position = vec4(Position.x, Position.y, 0.5, 1.0);
         }]]

postprocessfragment = [[
         #version 330
         layout(location = 0) out vec4 FragColor;
         in vec2 TexCoord;
         uniform sampler2D FrameBuffer;
         void main() {
           FragColor = texture(FrameBuffer, TexCoord);
         }
]]

--[[
Framebuffer = FrameBuffer {
   TextureUnit { unit 0 },
   Texture2D { COLOR_TEXTURE },
   TextureUnit { unit 1 },
   Texture2D { DEPTH_TEXTURE }
}

PostProcess = Separator {
   Program {
      VertexShader { 
         source = postprocessvertex
      },
      FragmentShader { 
         source = postprocessfragment
      },
   },
   VertexAttribute3f {
      location = 0,
      values = { -1, -1, 0.5, -1, 1, 0.5, 1, -1, 0.5, 1, 1, 0.5 };
   },
   DrawArrays {
      mode = "TRIANGLE_STRIP"
   }
}
--]]