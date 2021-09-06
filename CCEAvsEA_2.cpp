#include <iostream>
#include <iterator>
#include <vector>
#include <omp.h>
#include "BMOptimizer.h"
#include "libnpy/npy.hpp"


int main(){

    const int NUNITS = 8000;
    const int NITS = 500;
    const int POPSIZE = 250;
    const int CLUST_SIZE = 1000;

    int i = 1;

    //#pragma omp parallel for num_threads(5)
    //for(int i=1; i<11; ++i){

        std::vector<std::vector<int> > variables;

        std::vector<float> obj_evolution_EA, obj_evolution_CCEA;

        std::cout<<"EA:"<<std::endl;
        
        variables.push_back(std::vector<int>());
        for(int i=0; i<NUNITS; ++i){
            variables[0].push_back(i);
        }
        
        //Creates new BM with generated weights
        BMOptimizer bm_opt(NUNITS, CLUST_SIZE, POPSIZE, 3, 0.1, variables);
        
        std::vector<float> unrolled_weights;
        for(auto row : bm_opt.get_weights()){
            unrolled_weights.insert(unrolled_weights.end(), row.begin(), row.end());
        } 
        const long unsigned int shape [] = {NUNITS,NUNITS};
        npy::SaveArrayAsNumpy("weights_2.npy", false, 2, shape, unrolled_weights);

        
        //Used to repeat the experiment without generating new weights
        ///////////////////////////////////

        // std::vector<std::vector<float> > weights;
        // std::vector<unsigned long> shape;
        // bool fortran_order;
        // std::vector<float> data;
        // npy::LoadArrayFromNumpy("weights_2.npy", shape, fortran_order, data);
        // for(int i=0; i<shape[0]; ++i){
        //    weights.push_back(std::vector<float>());
        //    for(int j=0; j<shape[1]; ++j){
        //        weights[i].push_back( data[i*shape[1]+j] );
        //    }
        // }

        // BMOptimizer bm_opt(weights, POPSIZE, 3, 0.01, variables);

        ///////////////////////////////////


        for(int i=0; i<NITS; ++i){
            bm_opt.iterate();
            float max_energy = bm_opt.get_max_energy();
            obj_evolution_EA.push_back(max_energy);
            std::cout<<max_energy<<std::endl;
        }

        std::cout<<bm_opt.get_function_evals()<<std::endl;


        std::cout<<std::endl;
        std::cout<<std::endl;
        std::cout<<"CCEA:"<<std::endl;

        variables = std::vector<std::vector<int> > ();
        for(int i=0; i<NUNITS/CLUST_SIZE; ++i){
            variables.push_back(std::vector<int>());
            for(int j=i*CLUST_SIZE; j<(i+1)*CLUST_SIZE; ++j){
                variables[i].push_back(j);
            }
        }
        
        BMOptimizer bm_ccopt(bm_opt.get_weights(), POPSIZE, 3, 0.01, variables);

        for(int i=0; i<NITS/(NUNITS/CLUST_SIZE); ++i){
            bm_ccopt.iterate();
            float max_energy = bm_ccopt.get_max_energy();
            obj_evolution_CCEA.push_back(max_energy);
            std::cout<<max_energy<<std::endl;
        }

        std::cout<<bm_ccopt.get_function_evals()<<std::endl;
        
        const long unsigned int shape1 [] = {NITS};
        long unsigned int shape2 [1];
        shape2[0] = obj_evolution_CCEA.size();
        npy::SaveArrayAsNumpy("obj_evolution_EA_2_rep"+std::to_string(i)+".npy", 
                              false, 1, shape1, obj_evolution_EA);
        npy::SaveArrayAsNumpy("obj_evolution_CCEA_2_rep"+std::to_string(i)+".npy", 
                              false, 1, shape2, obj_evolution_CCEA);

    //}
    
    return 0;
}