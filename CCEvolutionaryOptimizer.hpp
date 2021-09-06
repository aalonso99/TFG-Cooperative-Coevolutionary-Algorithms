#ifndef CCEVO_OPT_H
#define CCEVO_OPT_H

// Evolutionary Optimizer for a MAXIMIZATION problem

#include <vector>
#include <functional>
#include <limits>
#include "RandomGenerator.h"

template <typename T>
class CCEvolutionaryOptimizer{

    private:

        int population_size;
        int n_competitors;      //number of competitors for K-tournament selection
        float mutation_rate;
        int total_evaluations = 0;
        std::vector<T> best_sol;
        std::vector<float> best_sol_obj;
        int n_populations;
        std::vector<std::vector<int> > population_variables;    //stores the variables assigned to each population

        std::vector<std::vector<T> > populations;
        std::vector<std::vector<float> > pop_objective;
        std::vector<std::vector<T> > new_pop;     //auxiliary vector to generate the new population efficiently
        //std::vector<std::vector<float> > new_pop_obj;  

        std::function<float(T)> objective_function;
        std::function<T(T,T,std::vector<int>)> crossover_operator;
        std::function<void(T&,const std::vector<int>&)> mutation_operator;

        int obj_function_evals = 0;

    public:

        ~CCEvolutionaryOptimizer(){}

        void init(const int pop_size, const int n_comp, const float mut_rate, const std::vector<std::vector<int> > &pop_variables,
                  const std::function<float(T)> obj_function, const std::function<T(T,T,std::vector<int>)> cross_operator, 
                  const std::function<void(T&,const std::vector<int>&)> mut_operator, const std::function<T()> pop_generator){

            population_size = pop_size;
            //population_size = pop_size/pop_variables.size();
            n_competitors = n_comp;
            mutation_rate = mut_rate;
            n_populations = pop_variables.size();
            population_variables = pop_variables;
            best_sol.resize(n_populations);
            best_sol_obj = std::vector<float> (n_populations, -std::numeric_limits<float>::max());

            objective_function = obj_function;
            crossover_operator = cross_operator;
            mutation_operator = mut_operator;

            for(int i=0; i<n_populations; ++i){
                populations.push_back(std::vector<std::vector<int> >());
                pop_objective.push_back(std::vector<float>());
                new_pop.push_back(std::vector<std::vector<int> >());
                pop_objective[i].resize(pop_size);
                new_pop[i].resize(pop_size);
                //new_pop_obj.resize(pop_size);

                for(int j=0; j<pop_size; ++j){
                    populations[i].push_back(pop_generator());
                }
            }
            
        }

        void evaluate_populations(){

            for(int i=0; i<n_populations; ++i){
                for(int j=0; j<population_size; ++j){
                    float eval = objective_function(populations[i][j]);
                    obj_function_evals++;
                    pop_objective[i][j] = eval;
                    
                    if( eval > best_sol_obj[i] ){
                        best_sol[i] = populations[i][j];
                        best_sol_obj[i] = eval;
                    }
                }
            }
            
        }

        int tournament(RandomIntGenerator &rg, const int pop_index){

            int best = rg.next();
            for(int i=0; i<n_competitors-1; ++i){
                int comp = rg.next();
                if( pop_objective[pop_index][comp] > pop_objective[pop_index][best] ){
                    best = comp;
                }
            }

            return best;

        }

        void selection(){

            for(int i=0; i<n_populations; ++i){
                RandomIntGenerator rg(0,population_size-1);
                for(int j=0; j<population_size; ++j){
                    int winner = tournament(rg, i);
                    new_pop[i][j] = populations[i][winner];
                    //new_pop_obj[i][j] = pop_objective[i][winner];
                }

                populations[i].swap(new_pop[i]);
                //pop_objective[i].swap(new_pop_obj[i]);
            }

        }

        void crossover(){

            for(int i=0; i<n_populations; ++i){
                RandomIntGenerator rg(0,population_size-1);
                for(int j=0; j<population_size; ++j){
                    int parent1 = rg.next();
                    int parent2 = rg.next();
                    T child(crossover_operator(populations[i][parent1], populations[i][parent2], population_variables[i]));

                    new_pop[i][j] = child;
                }

                populations[i].swap(new_pop[i]);
            }

        }

        void mutate_populations(){

            for(int i=0; i<n_populations; ++i){
                RandomIntGenerator rg(0, population_size-1);
                for(int j=0; j<std::max(1,population_size/10); ++j){
                    int elem = rg.next();
                    mutation_operator(populations[i][elem], population_variables[i]);
                }
            }
                
        }

        std::vector<T> get_best_solutions(){

            return best_sol;

        }

        std::vector<T> get_population(const int i=0){

            return populations[i];

        }

        int get_function_evals(){

            return obj_function_evals;

        }

};

#endif