--[==[
parseSeparator({ 
   type = "Separator",
   children = { 
      { type = "InstancedTriangleSet",
        indices = { { 0, 1, 3 }, { 3, 2, 0 },
                    { 1, 5, 7 }, { 7, 3, 1 },
                    { 5, 4, 6 }, { 6, 7, 5 },
                    { 4, 0, 2 }, { 2, 6, 4 },
                    { 2, 3, 7 }, { 7, 6, 2 },
                    { 1, 0, 4 }, { 4, 5, 1 } },
        
        vertices= { {-1,-1,-1 }, {-1,-1, 1 }, 
                    {-1, 1,-1 }, {-1, 1, 1 }, 
                    { 1,-1,-1 }, { 1,-1, 1 }, 
                    { 1, 1,-1 }, { 1, 1, 1 } }
      }
   }
})
   --]==]

function parseNode(node)
   nodeptr = createNode(node.type)
   print("parseNode")
   for field, value in pairs(node) do
      print(field, value)
   end

end

function parseSeparator(sep)
   for index, child in pairs(sep.children) do
      if child.type == "Separator" then
         parseSeparator(child)
      else
         parseNode(child)
      end
   end
end