#include <iostream>
#include <iterator>
#include <vector>
#include <sstream>
#include <omp.h>
#include "BMOptimizer.h"
#include "libnpy/npy.hpp"

void read_variables_from_file(const char path[], std::vector<std::vector<int> >& variables){

    std::ifstream file(path);
    if (file.is_open()){
        std::string line;   
        int i=variables.size();     
        while (getline(file, line)){
            variables.push_back(std::vector<int>());
            std::istringstream ss(line);
            int var;
            while (ss >> var)
                variables[i].push_back(var);
            ++i;
        }   
    }
        
}

int main(){

    const int NITS = 250;
    const int POPSIZE = 100;

    int i = 1;

    //#pragma omp parallel for num_threads(5)
    //for(int i=1; i<11; ++i){

        std::vector<std::vector<int> > variables;
        std::vector<std::vector<float> > weights;
        
        std::vector<float> obj_evolution_EA, obj_evolution_CCEA;

        //We read the weights generated in the dependency extraction phase
        std::vector<unsigned long> shape;
        bool fortran_order;
        std::vector<float> data;
        npy::LoadArrayFromNumpy("weights.npy", shape, fortran_order, data);
        for(int i=0; i<shape[0]; ++i){
            weights.push_back(std::vector<float>());
            for(int j=0; j<shape[1]; ++j){
                weights[i].push_back( data[i*shape[1]+j] );
            }
        }

        std::cout<<"EA:"<<std::endl;
        
        variables.push_back(std::vector<int>());
        for(int i=0; i<weights.size(); ++i){
            variables[0].push_back(i);
        }
        
        BMOptimizer bm_opt(weights, POPSIZE, 3, 0.01, variables);
        
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

        //We read the variables clusters found in the 
        // dependency analysis phase
        shape.clear();

        variables.clear();
        read_variables_from_file("clustered_variables.txt", variables);

        BMOptimizer bm_ccopt(weights, POPSIZE, 3, 0.01, variables);

        for(int i=0; i<NITS/variables.size(); ++i){
            bm_ccopt.iterate();
            float max_energy = bm_ccopt.get_max_energy();
            obj_evolution_CCEA.push_back(max_energy);
            std::cout<<max_energy<<std::endl;
        }

        std::cout<<bm_ccopt.get_function_evals()<<std::endl;
    
        const long unsigned int shape1 [] = {NITS};
        //const long unsigned int shape2 [] = {NITS/variables.size()};
        long unsigned int shape2 [1];
        shape2[0] = obj_evolution_CCEA.size();
        
        npy::SaveArrayAsNumpy("obj_evolution_EA_rep"+std::to_string(i)+".npy", 
                              false, 1, shape1, obj_evolution_EA);
        npy::SaveArrayAsNumpy("obj_evolution_CCEA_rep"+std::to_string(i)+".npy", 
                              false, 1, shape2, obj_evolution_CCEA);
    

    //}


    return 0;
}