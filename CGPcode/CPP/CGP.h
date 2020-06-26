/*
 * This header file stores the CGP structure, which consists of a matrix to 
 * store the CGP nodes and their connections. The mutations happens in the CGP
 * scope, while the evaluation happens with the AIG data structure and a 
 * propper mapping from CGP to AIG structure is done.
 * 
 * The generateFathersOneToFive method is responsible for executing the mutations
 * during training with the configuration of (1+5), in other words 1 individual
 * mutates in order to generate 4 children, afterwards they are evaluated and
 * compared, the one that has the best accuracy is chosen as the father of the
 * next generation.
 * 
 * The main methods from this class are commented.
 */

/* 
 * File:   CGP.h
 * Author: user
 *
 * Created on April 3, 2020, 3:50 PM
 */

#ifndef CGP_H
#define CGP_H

#define write_times 0
//#define constant_seed 1

#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <time.h>
#include <chrono>
#include <random>

#include "AIG.h"

using namespace std;


class AigPopulation;
class binaryPLA;

class CgpPopulation {
private:
    string name;
    vector<vector<short int> > the_CGPpopulation;
    unsigned int num_lines,num_columns,PIs,num_cgps;
    vector<float> all_scores_cgp;
    vector<int> ordered_indexes_cgp;
    vector<int> all_func_ands_cgp;
    vector<int> all_func_pis_cgp;
    int original_score_chosen,child_chosen;
public:
    CgpPopulation();
    CgpPopulation(vector<vector<short int> > param, unsigned int PIs,unsigned int num_columns, unsigned int num_lines);
    CgpPopulation(const CgpPopulation& orig);
    virtual ~CgpPopulation();
    
    //initialization of the CGP with random connections of nodes.
    void createNodes(string name,unsigned int num_cgps_param, unsigned int PIs,unsigned int num_columns, unsigned int num_lines,mt19937& seed);
    //initialization with a precomputed AIG file, its size may be greater than the original AIG size.
    void initializeWithDecisionTree(string exemplar_name,string path_to_aig,binaryPLA* PLA,AigPopulation* aig_pop,mt19937& mt);
    //old learning process with (100-300) father-offspring configuration. The (1+5)
    //configuration proved to be better from previous works published.
    void generateFathersOldSelection(AigPopulation* my_pop,double number_childs,double mutation_chance,binaryPLA* my_pla,string PLA_file,int batch_threshold);
    //method responsible for mutation, evaluation of offspring and mutation rate update.
    float generateFathersOneToFive(AigPopulation* my_pop,double mutation_chance,double min_mut,binaryPLA* my_pla,int batch_threshold,mt19937& seed);
    void chooseChildren(AigPopulation* previous_population);

    void setVector(vector<vector<short int> > param);
    void setScores(vector<float>* param);
    void setOrderedIndexes(vector<int>* param);

    //retrieves the ith CGP individual
    vector<short int>* getVector(int ith_individual);
    vector<float> getScores();
    vector<int> getOrderedIndexes();
    vector<int> getSizes();
    vector<int> getFuncPis();
    int getNumLines();
    int getNumCols();
    int getPopulationSize();
    string getName();
    
    void writeCgps();
    void clearCgp();
    void printGraph();
    

};
#endif /* CGP_H */

