
// Fibonacci with labels and gotos

void Fibonacci()
  labels 
     wrt, out;
     
  vars
     k: int;
     
  functions
  
     void fibo(n: int; var g: int)
       vars h: int;
       {
	 if (n<0) {
	   goto wrt;
	 } else {
	   h = g;
	   fibo(n-1,h);
	   g = h;
	   fibo(n-2,g);
	 }
	 write(n,g);
       } // fibo
    
{
     k = 0;
     fibo(3,k);
     fibo(-1,k);
     goto out;
wrt: write(k);
out: ;

}

