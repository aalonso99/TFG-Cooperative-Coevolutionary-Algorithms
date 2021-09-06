#ifndef BM_OPTIMIZER_H
#define BM_OPTIMIZER_H

#include <vector>
#include <functional>
#include "CCEvolutionaryOptimizer.hpp"

class BMOptimizer{

    private:

        int n_units;
        int population_size;
        int n_competitors;      //number of competitors for K-tournament selection
        float mutation_rate;
        //static const int CLUST_SIZE = 100;
        //static const int CLUST_SIZE = 1000;
        int clust_size;

        std::vector<std::vector<float> > weights_mat;
        int n_populations;
        std::vector<std::vector<int> > population_variables;    //stores the variables assigned to each population

        //These are initialized in the constructor
        std::function<std::vector<int>()> unit_generator;
        std::function<float(const std::vector<int>&)> compute_energy;
        std::function<std::vector<int>(const std::vector<int>&, const std::vector<int>&, 
                      const std::vector<int>&)> crossover_operator;
        std::function<void(std::vector<int>&,const std::vector<int>& )> mutation_operator;

        //void save_float_matrix(char[] file, const std::vector<vector<float> >& matrix);

        CCEvolutionaryOptimizer<std::vector<int> > cceo;

    public:

        BMOptimizer(const int nUnits, const int clustSize, const int pop_size, const int n_comp,
                    const float mut_rate, const std::vector<std::vector<int> > &pop_variables);
        BMOptimizer(const std::vector<std::vector<float> >& matWeights, 
                    const int pop_size, const int n_comp, const float mut_rate,
                    const std::vector<std::vector<int> > &pop_variables);
        ~BMOptimizer();
        void init_operators();
        void init_unit_generator();
        void init_weights();
        void init_weights(const std::vector<std::vector<float> >& matWeights);

        std::vector<std::vector<float> > get_dependency_matrix();
        //void save_dependency_matrix(char[] file);
        void iterate();
        
        std::vector<std::vector<int> > get_population(int i);
        const std::vector<std::vector<float> >& get_weights();
        //void save_weights(char[] file);
        std::vector<int> combine_best_collaborators(std::vector<std::vector<int> > solutions);
        const float get_max_energy();
        int get_function_evals();

};

std::ostream & operator<<(std::ostream & os, std::vector<std::vector<float> > mat);

std::ostream & operator<<(std::ostream & os, std::vector<std::vector<int> > mat);

#endif