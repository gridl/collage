#include <Rcpp.h>
#include <RcppParallel.h>

using namespace Rcpp;

inline double square(double x){
  return x*x;
}

//' For each pixel of X, indicates which base image is the closest (in RGB space)
//'
//' @param X a data frame with columns R, G and B
//' @param B a data frame with columns R, G and B
//' @examples
//' n <- 200*200
//' X <- data.frame( R = runif(n),  G = runif(n),  B = runif(n) )
//' Y <- data.frame( R = runif(50), G = runif(50), B = runif(50) )
//' system.time( closest_rgb( X, Y) )
// [[Rcpp::export]]
IntegerVector closest_rgb( DataFrame X, DataFrame B ){

  int nx = X.nrows(), nb = B.nrows() ;
  NumericVector red_x = X["R"], green_x=X["G"], blue_x=X["B"] ;
  NumericVector red_b = B["R"], green_b=B["G"], blue_b=B["B"] ;

  IntegerVector out(nx) ;

  tbb::parallel_for(0, nx, 1, [&](int i){
    // grab rgb for the j'th base image
    double red=red_x[i], green=green_x[i], blue=blue_x[i] ;

    auto get_distance = [&](int j){
      return sqrt( square(red-red_b[j]) + square(green-green_b[j]) + square(blue-blue_b[j]) ) ;
    } ;

    // calculate distances
    double distance = get_distance(0);
    int best = 0;

    for(int j=1; j<nb; j++){
      double distance_j = get_distance(j) ;
      if( distance_j < distance){
        best = j ;
        distance = distance_j ;
      }
    }
    out[i] = best + 1; // R is 1-based

  }) ;

  return out;
}

