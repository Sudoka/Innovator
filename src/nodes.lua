local Utils = require "Utils"

local function IndexBuffer(data)
   return Buffer {
      type = "UNSIGNED_INT",
      target = "ELEMENT_ARRAY",
      values = data.values
   }
end

local function VertexBuffer(data)
   return Buffer {
      type = "FLOAT",
      target = "ARRAY",
      values = data.values
   }
end

local function VertexAttribute3f(data)
   return VertexAttribute {
      size = 3,
      divisor = data.divisor,
      location = data.location
   }
end

local function VertexShader(data)
   return ShaderObject {
      type = "VERTEX_SHADER",
      source = data.source
   };
end

local function GeometryShader(data)
   return ShaderObject {
      type = "GEOMETRY_SHADER",
      source = data.source
   };
end

local function FragmentShader(data)
   return ShaderObject {
      type = "FRAGMENT_SHADER",
      source = data.source
   };
end

local function Sphere(data)
   local t = (1 + 5^0.5) / 2; -- golden ratio
   local indices = { {  1,  4, 0 }, {  4, 9, 0 }, { 4,  5, 9 }, { 8, 5, 4 }, {  1, 8, 4 },
                     {  1, 10, 8 }, { 10, 3, 8 }, { 8,  3, 5 }, { 3, 2, 5 }, {  3, 7, 2 },
                     {  3, 10, 7 }, { 10, 6, 7 }, { 6, 11, 7 }, { 6, 0,11 }, {  6, 1, 0 },
                     { 10,  1, 6 }, { 11, 0, 9 }, { 2, 11, 9 }, { 5, 2, 9 }, { 11, 2, 7 } };
   
   local vertices = { {-1, 0, t }, { 1, 0, t }, {-1, 0,-t }, { 1, 0,-t },
                      { 0, t, 1 }, { 0, t,-1 }, { 0,-t, 1 }, { 0,-t,-1 },
                      { t, 1, 0 }, {-t, 1, 0 }, { t,-1, 0 }, {-t,-1, 0 } };

   local lod = data.lod and data.lod or 1;
   Utils.normalizeArray(vertices);
   for l = 1, lod do
      Utils.subdivide(indices, vertices);
      Utils.normalizeArray(vertices);
   end

   return Separator {
      IndexBuffer { 
         values = Utils.flatten(indices) 
      },
      VertexBuffer { 
         values = Utils.flatten(vertices) 
      },
      VertexAttribute3f { 
         location = 0, 
      },
      VertexAttribute3f { 
         location = 1, 
      },
      BoundingBox { 
         min = { -1, -1, -1 }, 
         max = {  1,  1,  1 }
      },
      DrawElements { 
         mode = "TRIANGLES",
         type = "UNSIGNED_INT",
         count = #indices * 3
      }
   }
end

local function Box(data)
   return Separator {
      IndexBuffer {
         values = { 0, 1, 3, 3, 2, 0, 1, 5, 7, 7, 3, 1, 5, 4, 6, 6, 7, 5, 
                    4, 0, 2, 2, 6, 4, 2, 3, 7, 7, 6, 2, 1, 0, 4, 4, 5, 1 }
      },
      VertexBuffer {
         values = { -1, -1, -1, -1, -1,  1, -1,  1, -1, -1,  1,  1,
                     1, -1, -1,  1, -1,  1,  1,  1, -1,  1,  1,  1 } 
      },
      VertexAttribute3f {
         location = 0
      },
      BoundingBox { 
         min = { -1, -1, -1 }, 
         max = {  1,  1,  1 }
      },
      DrawElements { 
         mode = "TRIANGLES",
         type = "UNSIGNED_INT",
         count = 36
      }
   }
end

local function addMidPoint(i0, i1, v0, v1)
   local a = v0[i0];
   local b = v0[i1];
   v1[#v1 + 1] = a;
   v1[#v1 + 1] = Utils.avg(a, b);
end

local function subdivideDisk(vertices)
   local newvertices = {}
   for i = 1, #vertices - 1 do
      addMidPoint(i + 0, i + 1, vertices, newvertices);
   end
   addMidPoint(#vertices, 1, vertices, newvertices);
   return newvertices;
end

local function Cylinder(data)
   local disk = { { -1, -1, 0 }, { 1, -1, 0 }, { 1, 1, 0 }, {-1, 1, 0 } };
   Utils.normalizeArray(disk);

   for i = 1, data.lod do
      disk = subdivideDisk(disk);
      Utils.normalizeArray(disk);
   end

   local indices = {};
   local vertices = {};

   for i = 1, #disk do
      local v = disk[i];
      vertices[#vertices + 1] = { v[1], v[2],  1 };
      vertices[#vertices + 1] = { v[1], v[2], -1 };
   end

   indices[#indices + 1] = { #vertices - 2, #vertices - 1, 0 };
   indices[#indices + 1] = { 0, #vertices - 1, 1 };

   -- tube
   for i = 0, #vertices - 3, 2 do
      indices[#indices + 1] = { i + 0, i + 1, i + 2 };
      indices[#indices + 1] = { i + 2, i + 1, i + 3 };
   end

   -- top, bottom
   for i = 1, #vertices - 4, 2 do
      indices[#indices + 1] = { 0, i + 1, i + 3 };
      indices[#indices + 1] = { 1, i + 2, i + 4 };
   end

   indices = Utils.flatten(indices);
   vertices = Utils.flatten(vertices);

   return Separator {
      IndexBuffer {
         values = indices;
      },
      VertexBuffer {
         values = vertices;
      },
      VertexAttribute3f {
         location = 0
      },
      BoundingBox { 
         min = { -1, -1, -1 }, 
         max = {  1,  1,  1 }
      },
      DrawElements { 
         mode = "TRIANGLES",
         type = "UNSIGNED_INT",
         count = #indices
      }
   }
end

return {
   Box = Box,
   Sphere = Sphere,
   Program = Program,
   Cylinder = Cylinder,
   Separator = Separator,
   Transform = Transform,
   VertexShader = VertexShader,
   GeometryShader = GeometryShader,
   FragmentShader = FragmentShader,
}