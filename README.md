# TFG-Cooperative-Coevolutionary-Algorithms
Final project for a double degree in Computer Science and Mathematics. Covers introductory and advanced topics on evolutionary algorithms and CCEAs.

Provided code implements an experiment comparing performance of an evolutionary algorithm against its cooperative coevolutionary adaptation on optimizing a Boltzmann machine. The weights of this network are predefined in a way that problem decomposition can be exploited. In order to replicate such experiments, the next steps must be followed:

  1. Compile the executables with the make command.
  2. Run the dependency_extraction exe to generate the weights and read the dependencies into a file.
  3. Run the code in the dependency_analysis Jupyter Notebook to group the variables into clusters and generate a .txt file that will be read by the CCEA algorithm in the first experiment.
  4. Now you can execute the CCEAvsEA program. CCEAvsEA_2 can be executed right after compilation, since it generates its own weights assuming a specific variable clustering as domain knowledge.
  5. Both, CCEAvsEA and CCEAvsEA_2 can be easily modified to read the previously generated weights (in case of CCEAvsEA_2, by following the indications on the comments) and repeat the experiment as many times as desired.
  6. Obtained results can be visualized and analyzed within the comparison_EAvsCCEA notebook.

The attached report is written in Spanish with an extended summary in English at the beginning. For any question on the topics of this project or CCEAs, issues are accepted.

We give special recognition to the libnpy library and its creator, as they have been useful in the implementation of this project.
