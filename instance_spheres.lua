
local vertex = [[
#version 330
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 InstancePosition;

uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);
uniform mat4 ProjectionMatrix = mat4(1.0);

out vec4 ViewPosition;
out vec4 Normal;

void main() 
{
  ViewPosition = ViewMatrix * ModelMatrix * vec4(VertexPosition + InstancePosition, 1.0);
  Normal = ViewMatrix * ModelMatrix * vec4(VertexPosition, 0.0);
  gl_Position = ProjectionMatrix * ViewPosition;
}
]]

local fragment = [[
#version 330
layout(location = 0) out vec4 FragColor;

in vec4 ViewPosition;
in vec4 Normal;

void main()
{
  //vec3 dx = dFdx(ViewPosition.xyz);
  //vec3 dy = dFdy(ViewPosition.xyz);
  //vec3 n = normalize(cross(dx, dy));

  FragColor = vec4(normalize(Normal).zzz, 1.0);
}
]]

local SIZE = 1000;
local NUM_SPHERES = 10e5;

local InstancePositions = 
   (function() 
       positions = {};
       for i = 1, NUM_SPHERES * 3 do 
          positions[i] = math.random() * SIZE;
       end
       return positions;
    end)();

function SphereSet(data)
   return Separator {
   };
end

root = Separator {
   BoundingBox { 
      min = { 0, 0, 0 },
      max = { SIZE, SIZE, SIZE }
   },
   VertexAttribute3f {
      divisor = 1,
      location = 1,
      values = InstancePositions
   },
   Program {
      VertexShader   { source = vertex },
      FragmentShader { source = fragment }
   },
   Sphere {
      lod = 3
   }
}