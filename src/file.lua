
function Shape(data)
   return LuaShape.new()
end

function Transform(data)
   local transform = LuaTransform.new()
   LuaTransform.setScaleFactor(transform, data.scaleFactor)
   LuaTransform.setTranslation(transform, data.translation)
   return transform
end

function Program(data)
   local program = LuaProgram.new()
   for _, shader in pairs(data.shaders) do
      LuaProgram.addShader(program, shader)
   end
   return program
end

function VertexShader(data)
   local shader = LuaVertexShader.new()
   LuaVertexShader.setSource(shader, data.source)
   return shader;
end

function FragmentShader(data)
   local shader = LuaFragmentShader.new()
   LuaFragmentShader.setSource(shader, data.source)
   return shader
end

function VertexAttribute(data)
   local attrib = LuaVertexAttribute.new()
   LuaVertexAttribute.setVec3(attrib, data.values)
   LuaVertexAttribute.setIndex(attrib, data.index or 0)
   return attrib;
end

function IndexBuffer(data)
   local buffer = LuaIndexBuffer.new()
   LuaIndexBuffer.setiVec3(buffer, data.values)
   return buffer;
end

function Separator(data)
   local group = LuaSeparator.new()
   for _, child in pairs(data.children) do
      LuaSeparator.addChild(group, child)
   end
   return group
end

function Box(data)
   return Separator {
      children = {
         IndexBuffer {
            values = { 0, 1, 3,  3, 2, 0,
                       1, 5, 7,  7, 3, 1,
                       5, 4, 6,  6, 7, 5,
                       4, 0, 2,  2, 6, 4,
                       2, 3, 7,  7, 6, 2,
                       1, 0, 4,  4, 5, 1 }
         },
         VertexAttribute {
            values = { -1, -1, -1, -1, -1,  1, 
                       -1,  1, -1, -1,  1,  1,
                        1, -1, -1,  1, -1,  1,
                        1,  1, -1,  1,  1,  1 } 
         },
         Shape {}
      }
   }
end

function Sphere(data)
   local t = (1 + math.sqrt(5)) / 2; -- golden ratio
   return Separator {
      children = {
         IndexBuffer {
            values = { 1,  4, 0, 4,  9, 0, 4, 5,  9, 8, 5,  4, 1,  8, 4,
                       1, 10, 8, 10, 3, 8, 8, 3,  5, 3, 2,  5, 3,  7, 2,
                       3, 10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11, 6,  1, 0,
                       10, 1, 6, 11, 0, 9, 2, 11, 9, 5, 2,  9, 11, 2, 7 }
         },
         VertexAttribute {
            values = { -1,  0,  t,  1,  0,  t, -1,  0, -t,  1,  0, -t,
                        0,  t,  1,  0,  t, -1,  0, -t,  1,  0, -t, -1,
                        t,  1,  0, -t,  1,  0,  t, -1,  0, -t, -1,  0 }
         },
         Shape {}
      }
   }
end