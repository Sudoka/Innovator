
root = Separator {
   children = {
      IndexBuffer {
         type = ivec3,
         values = { 0, 1, 3, 3, 2, 0,
                    1, 5, 7, 7, 3, 1,
                    5, 4, 6, 6, 7, 5,
                    4, 0, 2, 2, 6, 4,
                    2, 3, 7, 7, 6, 2,
                    1, 0, 4, 4, 5, 1 } 
      },

      VertexAttribute {
         index = 0,
         values = { 0, 0, 0, 0, 0, 1, 
                    0, 1, 0, 0, 1, 1,
                    1, 0, 0, 1, 0, 1,
                    1, 1, 0, 1, 1, 1 } 
      },

      VertexAttribute {
         index = 1,
         values = { 0, 0, 0, 0, 0, 1, 
                    0, 1, 0, 0, 1, 1,
                    1, 0, 0, 1, 0, 1,
                    1, 1, 0, 1, 1, 1 } 
      },

      VertexAttribute {
	 index = 2,
	 divisor = 1,
	 values = { 0, 0, 0, 1, 1, 1 }
      },

      Program {
         file = "drawprogram.lua"
      },

      Shape { 
	 attributes = { 0, 1 },
         mode = TRIANGLES 
      }

      Program {
	 file = "normalprogram.lua"
      }
      Shape {
	 attributes = { 0, 1 }
	 mode = POINTS
      }
   }
}