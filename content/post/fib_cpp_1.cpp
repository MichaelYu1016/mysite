#include <Rcpp.h>
using namespace Rcpp;

// This is a simple example of exporting a C++ function to R. You can
// source this function into an R session using the Rcpp::sourceCpp 
// function (or via the Source button on the editor toolbar). Learn
// more about Rcpp at:
//
//   http://www.rcpp.org/
//   http://adv-r.had.co.nz/Rcpp.html
//   http://gallery.rcpp.org/
//

// [[Rcpp::export]]
int fib_cpp_1(int n)
{
  if(n==1||n==2) return 1;
  return fib_cpp_1(n-1)+fib_cpp_1(n-2);
}

/*** R
library(microbenchmark)
fib <- function(n){
  if(n==1|n==2) return(1)
  return(fib(n-1)+fib(n-2))
}
microbenchmark(fib_cpp_1(15),fib(15))
*/