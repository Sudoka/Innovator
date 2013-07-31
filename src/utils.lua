local Utils = {}

function Utils.flatten(list)
  if type(list) ~= "table" then return {list} end
  local flat_list = {}
  for _, elem in ipairs(list) do
    for _, val in ipairs(Utils.flatten(elem)) do
      flat_list[#flat_list + 1] = val
    end
  end
  return flat_list
end

function avg(a, b)
   return { (a[1] + b[1]) / 2, (a[2] + b[2]) / 2, (a[3] + b[3]) / 2 };
end

function normalize(vertex)
   local x = vertex[1];
   local y = vertex[2];
   local z = vertex[3];
   local l = math.sqrt(x * x + y * y + z * z);
   return { x / l, y / l, z / l };
end

function normalizeArray(vertices)
   for i = 1, #vertices do
      vertices[i] = normalize(vertices[i]);
   end
end

function Utils.subdivide(indices, vertices, lod)
   normalizeArray(vertices);
   for l = 1, lod do
      local num_faces = #indices;
      for face_index = 1, num_faces do
         local face = indices[face_index];

         local a = vertices[face[1] + 1];
         local b = vertices[face[2] + 1];
         local c = vertices[face[3] + 1];
         
         table.insert(vertices, avg(a, b));
         table.insert(vertices, avg(b, c));
         table.insert(vertices, avg(c, a));
         
         local i = #vertices - 3;
         local j = #vertices - 2;
         local k = #vertices - 1;
         
         table.insert(indices, { i, j, k });
         table.insert(indices, { face[1], i, k });
         table.insert(indices, { i, face[2], j });
         indices[face_index] = { k, j, face[3] };
      end
      normalizeArray(vertices);
   end
end

return Utils;