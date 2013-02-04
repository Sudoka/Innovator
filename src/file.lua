
function Shape(data)
   return Innovator_Shape.new()
end

function Program(data)
   return Innovator_Program.new()
end

function VertexAttribute(data)
   attrib = Innovator_VertexAttribute.new()
   Innovator_VertexAttribute.setVec3(attrib, data.values)
   return attrib;
end

function IndexBuffer(data)
   buffer = Innovator_IndexBuffer.new()
   Innovator_IndexBuffer.setiVec3(buffer, data.values)
   return buffer;
end

function Separator(data)
   group = Innovator_Separator.new()
   for _, child in pairs(data.children) do
      Innovator_Separator.addChild(group, child)
   end
   return group
end