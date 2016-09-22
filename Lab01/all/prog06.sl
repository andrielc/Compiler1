/* Towers of Hanoi */

void Hanoi() 
   vars
      n: integer;
      moves: integer;
   
   functions
      void hanoi(n: integer; origin, destination, aux: integer)
      {
	 if (n!=0) {
	    hanoi(n-1,origin,aux,destination);
	    write(origin,destination);
	    moves = moves+1;
	    hanoi(n-1,aux,destination,origin);
	 }
      }
   
{
   read(n);
   moves = 0;
   hanoi(n,1,2,3);
   write(moves);
}
