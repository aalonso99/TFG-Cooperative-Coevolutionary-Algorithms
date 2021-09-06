// #include <iostream>
// #include <iterator>
#include <vector>
#include "BMOptimizer.h"
#include "libnpy/npy.hpp"


int main(){

    const int NUNITS = 800;
    const int CLUST_SIZE = 100;
    std::vector<std::vector<int> > variables;

    variables.push_back(std::vector<int>());
    for(int i=0; i<NUNITS; ++i){
        variables[0].push_back(i);
    }
    
    BMOptimizer bm_opt(NUNITS, CLUST_SIZE, 1, 3, 0.1, variables);
    
    //std::cout<<bm_opt.get_weights()<<std::endl;
    std::vector<float> unrolled_weights;
    for(auto row : bm_opt.get_weights()){
        unrolled_weights.insert(unrolled_weights.end(), row.begin(), row.end());
    } 
    const long unsigned int shape [] = {NUNITS,NUNITS};
    npy::SaveArrayAsNumpy("weights.npy", false, 2, shape, unrolled_weights);


    std::vector<float> unrolled_dm;
    for(auto row : bm_opt.get_dependency_matrix()){
        unrolled_dm.insert(unrolled_dm.end(), row.begin(), row.end());
    } 
    //const long unsigned int shape [] = {NUNITS,NUNITS};
    npy::SaveArrayAsNumpy("dependencies.npy", false, 2, shape, unrolled_dm);

    return 0;
}