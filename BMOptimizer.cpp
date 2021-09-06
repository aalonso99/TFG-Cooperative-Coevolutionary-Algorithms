#include <cassert>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <cmath>
#include <numeric>

#include "BMOptimizer.h"
#include "CCEvolutionaryOptimizer.hpp"
#include "RandomGenerator.h"

std::ostream & operator<<(std::ostream & os, std::vector<std::vector<float> > mat){

    for( auto row : mat){
        os<<"[ ";
        std::copy(row.begin(), row.end(), std::ostream_iterator<float>(os, " "));
        os<<"]"<<std::endl;
    }
    
    return os;

}

std::ostream & operator<<(std::ostream & os, std::vector<std::vector<int> > mat){

    for( auto row : mat){
        os<<"[ ";
        std::copy(row.begin(), row.end(), std::ostream_iterator<int>(os, " "));
        os<<"]"<<std::endl;
    }
    
    return os;

}

BMOptimizer::BMOptimizer(const int nUnits, const int clustSize, const int pop_size, const int n_comp,
                         const float mut_rate, const std::vector<std::vector<int> > &pop_variables){

    assert( nUnits > 0 );

    n_units = nUnits;
    population_size = pop_size;
    clust_size = clustSize;
    n_competitors = n_comp;      //number of competitors for K-tournament selection
    mutation_rate = mut_rate;
    population_variables = pop_variables;

    init_weights();
    init_unit_generator();
    init_operators();
    
    cceo.init(pop_size, n_comp, mut_rate, pop_variables,
            compute_energy, crossover_operator, 
            mutation_operator, unit_generator);

}

BMOptimizer::BMOptimizer(const std::vector<std::vector<float> >& matWeights,
                         const int pop_size, const int n_comp, const float mut_rate,
                         const std::vector<std::vector<int> > &pop_variables){

    assert( matWeights.size() > 0 );
    for( auto v : matWeights ){
        assert( v.size() > 0 );
    }

    n_units = matWeights.size();
    population_size = pop_size;
    n_competitors = n_comp;      //number of competitors for K-tournament selection
    mutation_rate = mut_rate;
    population_variables = pop_variables;

    //std::cout<<matWeights<<std::endl;
    init_weights(matWeights);
    //std::cout<<matWeights<<std::endl;
    init_unit_generator();
    init_operators();

    cceo.init(pop_size, n_comp, mut_rate, pop_variables,
            compute_energy, crossover_operator, 
            mutation_operator, unit_generator);

}

BMOptimizer::~BMOptimizer(){}

void BMOptimizer::init_operators(){

    compute_energy = [this](const std::vector<int>& units) -> float{

        float energy = 0.0f;
        for(int i=0; i<n_units; ++i){
            if(units[i]==1){
                for(int j=i+1; j<n_units; ++j){
                    energy += weights_mat[i][j]*units[j];
                }
            }
        }

        return energy;
    
    };

    crossover_operator = [](const std::vector<int>& units1, 
                            const std::vector<int>& units2, 
                            const std::vector<int>& indices) -> std::vector<int>{

        std::vector<int> child_units(units1);
        RandomIntGenerator rg(0,1);
        for(int i=0; i<indices.size(); ++i){
            child_units[indices[i]] = ( rg.next() ? units1[indices[i]] : units2[indices[i]] );
        }

        return child_units;

    };

    mutation_operator = [](std::vector<int>& units, 
                           const std::vector<int> indices) -> void{

        //We flip a bit with probability 1/n_units
        RandomIntGenerator rg(0, indices.size()-1);
        for(auto i : indices){
            if(!rg.next()){
                units[i] = 1-units[i];
            }
        }

    };

}

void BMOptimizer::init_unit_generator(){

    unit_generator = [this]() -> std::vector<int>{

        std::vector<int> units;

        RandomIntGenerator rg(0,1);
        for(int i=0; i<n_units; ++i){
            units.push_back( rg.next() );
        }

        return units;

    };

}

//Initializes a weight matrix with n_units/clust_size diagonal (clust_size X clust_size) blocks of 1s
// and all rest 0s, plus a random gaussian noise ~ N(0,0.1)
void BMOptimizer::init_weights(){

    //Allocates the matrix
    std::vector<std::vector<float> > matWeights;
    for(int i=0; i<n_units; ++i){
        matWeights.push_back(std::vector<float>(n_units, 0.0f));
    }

    //Fills the matrix
    for(int block=0; block<ceil(n_units/clust_size); ++block){
        for(int i=block*clust_size; i<(block+1)*clust_size && i<n_units; ++i){
            for(int j=i+1; j<(block+1)*clust_size && j<n_units; ++j){
                matWeights[j][i] = matWeights[i][j] = 1.0f;
            }
        }
    }

    //Adds noise
    RandomGaussianGenerator rg(0.0f, 0.1f);
    for(int i=0; i<n_units; ++i){
        for(int j=i+1; j<n_units; ++j){
            matWeights[j][i] = matWeights[i][j] += rg.next();
        }
    }

    init_weights(matWeights);

}

void BMOptimizer::init_weights(const std::vector<std::vector<float> >& matWeights){

    for(auto row : matWeights){
        weights_mat.push_back(row);
    }

}

std::vector<std::vector<float> > BMOptimizer::get_dependency_matrix(){

    std::vector<int> sample_unit = unit_generator();
    float sample_energy = compute_energy(sample_unit);

    std::vector<std::vector<float> > dependency_matrix(n_units, std::vector<float>(n_units));
    for(int i=0; i<n_units; ++i){
        for(int j=i+1; j<n_units; ++j){
            std::vector<int> perturbed_unit(sample_unit);
            perturbed_unit[i] = 1-perturbed_unit[i];
            float flip_i_energy = compute_energy(perturbed_unit);

            perturbed_unit[j] = 1-perturbed_unit[j];
            float flip_ij_energy = compute_energy(perturbed_unit);

            //We flip i again to leave only j flipped
            perturbed_unit[i] = 1-perturbed_unit[i];
            float flip_j_energy = compute_energy(perturbed_unit);

            dependency_matrix[j][i] = dependency_matrix[i][j] = std::abs(flip_i_energy + flip_j_energy - 
                                      flip_ij_energy - sample_energy);
        }
    }

    return dependency_matrix;

}

void BMOptimizer::iterate(){

    cceo.evaluate_populations();
    cceo.selection();
    cceo.crossover();
    cceo.mutate_populations();

}

const std::vector<std::vector<float> >& BMOptimizer::get_weights(){

    return weights_mat;

}

std::vector<std::vector<int> > BMOptimizer::get_population(const int i=0){

    auto copy = cceo.get_population(i);
    return copy;

}

std::vector<int> BMOptimizer::combine_best_collaborators(std::vector<std::vector<int> > solutions){

    std::vector<int> final_sol(n_units);
    for(int i=0; i<solutions.size(); ++i){
        for(auto var : population_variables[i]){
            final_sol[var] = solutions[i][var];
        }
    }

    return final_sol;

}

const float BMOptimizer::get_max_energy(){

    std::vector<std::vector<int> > best_solutions = cceo.get_best_solutions();
    std::vector<int> combined_solutions = combine_best_collaborators(best_solutions);
    return compute_energy(combined_solutions);

}

int BMOptimizer::get_function_evals(){

    return cceo.get_function_evals();

}