#include <iostream>
#include <armadillo>

using namespace arma;
using namespace std;

int main(){

    mat var;
    var << 1 << 2 << 3 << endr 
        << 4 << 5 << 6 << endr 
        << 7 << 8 << 9 << endr ;
    
    cout<<"Test Armadillo C++"<<endl;

    var.print("a : ")

    return 0;
} 
