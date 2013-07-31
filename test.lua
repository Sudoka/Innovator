
local Nodes = require "Nodes"

local vertex = [[
#version 330
layout(location = 0) in vec3 Position;
uniform mat4 ViewMatrix = mat4(1.0);
uniform mat4 ModelMatrix = mat4(1.0);
uniform mat4 ProjectionMatrix = mat4(1.0);

out vec4 ViewPosition;

void main() 
{
   ViewPosition = ViewMatrix * ModelMatrix * vec4(Position, 1.0);
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

SceneRoot = Separator {
   Program {
      Nodes.VertexShader { source = vertex },
      Nodes.FragmentShader { source = fragment },
   },
   Transform { translation = { -2, -2, -2 } },
   Nodes.Box {},
   Transform { translation = { 2, 2, 2 } },
   Nodes.Sphere { lod = 0 },
   Transform { translation = { 2, 2, 2 } },
   Nodes.Sphere { lod = 3 }
}

--[[
Window {
   size = { 640, 480 },
   framebuffer = fb,
   keyPressed = function() end,
   mouseMoved = function() end,
   resize = function() end,
   paint = function() end,
}
--]]