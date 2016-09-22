// Arrays, functions, constants and comments

void Extended()

   types  Vector = integer[10];
	  Matrix = Vector[5];
          AnotherMatrix = integer[10][5];

   vars   a, b: Matrix;

   functions
   
      integer sum1(m: Matrix)
      /* Matrix by value */
	 vars i, j, s: integer;
      {
	 i = 0;
	 s = 0;
	 while (i<10) {
	    j = 0;
	    while (j<5) {
	       s = s+m[i][j];
	       m[i][j] = -1;
	       j = j+1;
	    } // j
	    i = i+1;
	 } // i
	 return s;
      }

      integer sum2(var m: Matrix)
      /* Matrix by variable */
	 vars i, j, s: integer;
      {
	 i = 0;
	 s = 0;
	 while (i<10) {
	    j = 0;
	    while (j<5) {
	       s = s+m[i][j];
	       m[i][j] = -1;
	       j = j+1;
	    } // end loop j
	    i = i+1;
	 } // end loop i
	 return s;
      }

{
   i = 0;
   while (i<10) {
      j = 0;
      while (j<5) {
         a[i][j] = i*j+1;
         j = j+1;
      } 
      i = i+1;
   }
   
   b = a;            // save a
   write(sum1(a));  
   write(sum1(a));   // should repeat the first value
   write(sum2(a));   // should repeat the first value
   write(sum2(a));   // should print -50 -- check!
   write(sum2(b));   // 
   
}

   


      
      
      

   