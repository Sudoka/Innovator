
function Shape(data)
   return Innovator_Shape.new()
end

function Transform(data)
   local transform = Innovator_Transform.new()
   Innovator_Transform.setScaleFactor(transform, data.scaleFactor)
   Innovator_Transform.setTranslation(transform, data.translation)
   return transform
end

function Program(data)
   local program = Innovator_Program.new()
   Innovator_Program.setFilename(program, data.fileName)
   return program
end

function VertexAttribute(data)
   local attrib = Innovator_VertexAttribute.new()
   Innovator_VertexAttribute.setVec3(attrib, data.values)
   return attrib;
end

function IndexBuffer(data)
   local buffer = Innovator_IndexBuffer.new()
   Innovator_IndexBuffer.setiVec3(buffer, data.values)
   return buffer;
end

function Separator(data)
   local group = Innovator_Separator.new()
   for _, child in pairs(data.children) do
      Innovator_Separator.addChild(group, child)
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
            index = 0,
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
            index = 0,
            values = { -1,  0,  t,  1,  0,  t, -1,  0, -t,  1,  0, -t,
                        0,  t,  1,  0,  t, -1,  0, -t,  1,  0, -t, -1,
                        t,  1,  0, -t,  1,  0,  t, -1,  0, -t, -1,  0 }
         },
         Shape {}
      }
   }
end