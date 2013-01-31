
function BufferObject(data)
   return Innovator.newBufferObject(data.target, data.usage, data.type, data.values)
end

function VertexAttribute(data)
   return Innovator.newVertexAttribute(data.index, data.divisor)
end

function InstancedTriangleSet(data)
   return Innovator.newInstancedTriangleSet(data.indices, data.vertices)
end

function addChildren(group, children)
   for index, child in pairs(children) do
      Innovator.Group_addChild(group, child)
   end
   return group
end

function Group(data)
   return addChildren(Innovator.newGroup(), data.children)
end

function Separator(data)
   return addChildren(Innovator.newSeparator(), data.children)
end

function IndexBuffer(data)
   return BufferObject { 
      target = ELEMENT_ARRAY, 
      usage = STATIC_DRAW, 
      type = data.type,
      values = data.values 
   }
end

function VertexBuffer(data)
   return Group {
      children = {
         BufferObject { 
            target = ARRAY, 
            usage = STATIC_DRAW, 
            type = data.type, 
            value = data.values 
         },
         VertexAttribute { 
            data.index, 
            data.divisor 
         }
      }
   }
end