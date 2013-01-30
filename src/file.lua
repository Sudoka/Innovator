
function InstancedTriangleSet(data)
   local node = InstancedTriangleSet2.new()
   InstancedTriangleSet2.setIndices(node, data.indices)
   return node
end

function Separator(data)
   --[[
      local separator = Separator2.new();
   for index, child in pairs(data.children) do
      Separator2.addChild(separator, child)
      end
--]]
end