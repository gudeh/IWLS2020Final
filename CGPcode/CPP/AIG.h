/*
 * Pre-processor directives (#define) were used during the CGP development.
 * This directives enables for a better tracking of functionalities already 
 * implemented to be accounted for while new functionalities are under development.
 * They are used to set the many different combinations of execution options for
 * the CGP. 
 * 
 * This file also stores the declarations of classes involving the AIG structure.
 * With a main graph class(AIGraph), a nodeAig class that other types of nodes
 * inherit from, such as the input class (AIG primary input), output (AIG primary output),
 * and the AND for the internal AIG AND nodes. We also have an AigPopulation class
 * which stores multiple AIGs from the CGP population.
 * 
 * It is worth noticing that there is another set of classes for the CGP and another
 * for the PLA class with their header files.
 */

/* 
 * File:   AIG.h
 * Author: user
 *
 * Created on April 4, 2020, 12:13 PM
 */

#ifndef AIG_H
#define AIG_H

//With the exemplar set to 1, a vector is used in the main file with the exemplars 
//to be executed. Otherwise all the exemplars (0-99) are executed.
#define exemplar_set 0

//With the constant_seed option set to 1 one may repeat a same execution multiple
//times. Otherwise a random seed is used.
#define constant_seed 0

//The AIG primary input may mutate as any other node with this option set to 1.
#define mutable_PO 1

//By checking all nodes as a PO the CGP evaluates all its nodes as if they were 
//the circuit's PO. This process adds a lot of overhead, but sometimes finds
//better solutions as compared to if it wasn't set to 1.
#define checkAllNodesAsPO 0

//You may train with the validation set if wish to. This works only 
//if smart_init is 0.
#define train_with_validation_set 0

//The PLA benchmarks provided by the contest were handled and wiritten in 4 
//different versions. With configurations such as 50-50, 90-10 or 80-20.
#define new_benchmarks 4

//With the exmeplar_set set to 0, and this option set to 1, the CGP will run with
//execution parameters set as input. For example "./cgp 1 2 3" would execute the
//exemplars 0, 1 and 2.
#define argv_Ntimes 0

//With MULTI_FUNC set to 1 the CGP may include not only AND nodes but also
//XOR nodes. At the end of each exemplar run, the program applies a mapping, where
//one CGP node that is a XOR becomes 3 AND nodes with proper connections.
#define MULTI_FUNC 1

//The random initialization may run with different CGP sizes and different
//changeEach values. Executing more times when this is set to 1.
#define rand_loops 0

//Loop forever with the same configuration and set of exemplars.
#define LOOP 0

//with this option set to 0, only a minimal information is printed. Otherwise
//the learning process is printed, showing accuracies, sizes, mutation and other 
//values.
#define COUT 1

#define refresh_aig_folder 0

//This option only works if smart_init is set to 0. If decision_tree_init is 1
//the CGP will start all the exemplars with an previously generated AIG from the
//location at aigs_path. Otherwise it always starts with a random initialization.
#define decision_tree_init 1
#define aigs_path "../../Benchmarks_3_espresso_aig/"

//This option chooses the best evaluated AIG to start the CGP learning process.
//it looks for the AIGs in the paths set at expresso_path and sk_path.
#define smart_init 1
#define expresso_path "../../Benchmarks_3_espresso_aig/"
#define sk_path  "../../AIGS_BEST/"
#define generate_starting_aigs 0

//With a previous initialization, its CGP may be created with extra nodes for a 
//greater space exploration of solutions. 0 would be the original size, 1 creates
//a CGP two times greater, 2 a CGP three times greater and successively.
#define num_extra_nodes 1

//Output options, to print the execution information.
#define write_times 0
#define write_learning 0
#define writePIdebug 0
#define DEBUG 0

//this options are mostly unused and left behind.
#define temp_init 2000
#define Sannealing 0
#define old_selection 0
#define my_mutation 0

//This option saves storage space by not storing each AIG AND's fanouts pointers.
#define IGNORE_OUTPUTS 1

#define BITS_PACKAGE_SIZE 64
#define write_size 100

#include <vector>
#include <array>
#include <set>
#include <deque>
#include <stack>
#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <limits.h> //char_bit
#include <algorithm> //max
#include <stdint.h> //uintptr_t
#include <functional> //reference_wraper
#include <sys/time.h>
#include <math.h> //pow
#include <sys/resource.h> //getrusage
#include <string.h> //strlen, strncmp
#include <bitset>
#include <numeric> //iota
#include <chrono>
#include <omp.h>
//#include <unistd.h>
#include <thread>
#include <queue>
#include <functional> //reference  wrapper

#include "CGP.h"
#include "binaryPLA.h" //check bits on bit-parallel simulation



using namespace std;
//string learning_file_name;
class CgpPopulation;
class nodeAig{
protected:
    unsigned int id;
    short int signal;
    unsigned long long int bit_vector;
public:
    nodeAig();
    nodeAig(unsigned int);
    virtual ~nodeAig();
    
    //modifiers
    virtual void pushOutput(nodeAig* param){}
    virtual void pushInput(nodeAig* param,bool param_polarity){}
    void setId(unsigned int);
    void setDepth(short int);
    void setSignal(int);
    void setBitVector(unsigned long long int);
    
    //member access
    unsigned int getId();
    int getDepth();
    int getSignal();
    unsigned long long int getBitVector();
    nodeAig* fixLSB();    
    nodeAig* forceInvert();
    virtual vector<nodeAig*> getInputs(){}
    virtual vector<nodeAig*> getOutputs(){}
    virtual vector<int> getInputPolarities(){}
    
    //operations
    virtual int computeDepthInToOut(){}
    virtual unsigned long long int PropagSignalDFS(){}
    virtual void removeOutput(unsigned int){}
    virtual void clearOutputs(){}
    virtual void writeNode(ofstream&){}
    virtual void printNode(){}
};
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////

class input: public nodeAig {
    vector<nodeAig*> outputs;
    bool visited;
    int visit_count;
    
public:
    input();
    using nodeAig::nodeAig; //enable use of constructor from node
    using nodeAig::setDepth;;
    using nodeAig::setBitVector;
    using nodeAig::getDepth;
    using nodeAig::getId;
    using nodeAig::getSignal;
    using nodeAig::getBitVector;
    
    virtual ~input();
   
   //modifiers
   void pushOutput(nodeAig* param) override;
   
   //operations
   void setUnvisited();
   int computeDepthInToOut() override;;
   unsigned long long int PropagSignalDFS() override;
   void clearOutputs() override;
   void writeNode(ofstream&) override;
   void printNode() override;
   //member access
   vector<nodeAig*> getOutputs() override;
   bool wasVisited();
   int getVisitCount();
   
};
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////

class output : public nodeAig{
    nodeAig* input;
    
public:
    output();
    using nodeAig::nodeAig; //enable use of constructor from node
    virtual ~output();
    using nodeAig::setBitVector;
    using nodeAig::setDepth;
    using nodeAig::getDepth;
    using nodeAig::getSignal;
    using nodeAig::getBitVector;
    
    //modifiers
    void pushInput(nodeAig* param,bool param_polarity);
    void clearInput();
            
    //member access
    nodeAig* getInput();
    int getInputPolarity();
    
    //operations
    void writeNode(ofstream&) override;
    int computeDepthInToOut() override;
    void printNode() override;
    unsigned long long int PropagSignalDFS() override;
};
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////

class AND : public nodeAig{
    vector <nodeAig*> inputs;
#if MULTI_FUNC == 1
    bool isXOR;
#endif    
#if IGNORE_OUTPUTS == 0
    vector <nodeAig*> outputs;
#endif
    
public:
    AND();
    using nodeAig::nodeAig; 
    using nodeAig::setBitVector;
    using nodeAig::setDepth;
    using nodeAig::getDepth;
    using nodeAig::getSignal;
    using nodeAig::getBitVector;
    virtual ~AND();
    
    //member access
   vector<nodeAig*> getInputs() override; 
#if IGNORE_OUTPUTS == 0
       vector<nodeAig*> getOutputs() override;
#endif
   vector<int> getInputPolarities() override;
   
   //modifiers
   void pushInput(nodeAig* param,bool param_polarity) override;
   void invertInputs();
#if MULTI_FUNC == 1
   void setAsXOR(bool);
   bool getIsXOR();
   void clearInputs();
#endif
   
   //operations
   int computeDepthInToOut() override;
   unsigned long long int PropagSignalDFS() override;
   
#if IGNORE_OUTPUTS == 0
   void removeOutput(unsigned int id_to_remove) override;
   void clearOutputs() override;
   void pushOutput(nodeAig* param) override;
#endif
   
   void writeNode(ofstream&) override;
   void printNode() override;

};
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////

class XOR : public nodeAig{
    vector <nodeAig*> inputs;
#if IGNORE_OUTPUTS == 0
    vector <nodeAig*> outputs;
#endif
    
public:
    XOR();
    using nodeAig::nodeAig; 
    using nodeAig::setBitVector;
    using nodeAig::setDepth;
    using nodeAig::getDepth;
    using nodeAig::getSignal;
    using nodeAig::getBitVector;
    virtual ~XOR();
    
    //member access
   vector<nodeAig*> getInputs() override; 
#if IGNORE_OUTPUTS == 0
       vector<nodeAig*> getOutputs() override;
#endif
   vector<int> getInputPolarities() override;
   
   //modifiers
   void pushInput(nodeAig* param,bool param_polarity) override;
   void invertInputs();
   
   //operations
   int computeDepthInToOut() override;
   unsigned long long int PropagSignalDFS() override;
   
#if IGNORE_OUTPUTS == 0
   void removeOutput(unsigned int id_to_remove) override;
   void clearOutputs() override;
   void pushOutput(nodeAig* param) override;
#endif
   
   void writeNode(ofstream&) override;
   void printNode() override;

};
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////

class AIGraph {
protected:
    vector<input> all_inputs;
    vector<output> all_outputs;
    vector<AND> all_ANDS;
    vector<AND> extra_ands;
    string name;
    float current_score;
//    float p1_cgp;
    int functional_and_count,functional_PI_count;
//    int graph_depth;
    //all_depths' index is the node ID/2 and the all_depths[ID/2] is the depth for this node.
    vector<unsigned int> all_depths;
    nodeAig constant1,constant0;
public:
    AIGraph();
    AIGraph(string name);
   virtual ~AIGraph();
    
    //modifiers
    void CGPtoAIG(vector<short> *cgp_vector,unsigned int num_lines,unsigned int I,unsigned int O, unsigned int A, string name);
    void propagateSignalsTrainPLA(binaryPLA* PLA_combinations);
    void propagateSignalsBatchPLA(binaryPLA* PLA);
    void propagateAllAnds(binaryPLA* PLA);
    void readAIG(ifstream& aig_file,string aig_name,mt19937& mt);
    void setName(string);
//    void copyP1Cgp(float param);
    void clearCircuit();
    input* pushPI(unsigned int index,input input_obj);
    void pushPO(unsigned int index,output output_obj);
    AND* pushAnd(unsigned int index,AND AND_obj);
    
    nodeAig* findAny(unsigned int);
    string getName();
    float getCurrentScore();
//    float getP1Cgp();
    int getRealSize();
    int getRealPIsize();
    int getDepth();
    vector<AND>* getAnds();
    vector<input>* getPIs();
    vector<output>* getPOs();
    vector<unsigned int>* getAllDepths();
    
    void printCircuit();
//    void setDepthsInToOut();
    void writeAIG(string destination, string aig_name);
    void writeAAG(string destination, string aig_name);
    //The Primary Inputs are expected to be configured already.
#if MULTI_FUNC == 1
    void mapXorToAnd(string destination, string aig_name);
#endif
    
private:
    void encodeToFile(ofstream& file, unsigned x);
    unsigned char getnoneofch (ifstream& file,int);
    unsigned decode (ifstream& file,int);

};
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
///////////////////////////////////////////////

class AigPopulation{
private:
    vector<AIGraph> aig_population;
    vector<float> all_scores;
    vector<int> num_functional_ands;
    vector<int> num_functional_PIs;
    vector<int> ordered_indexes;
#if Sannealing == 1
    float temperature;
#endif
public:
    AIGraph* getAigFromPopulation(int ith_aig);
    vector<float>* getScores();
    vector<int>* getOrderedIndexes();
    vector<int>* getFuncSizes();
    vector<int>* getFuncPis();
    float getMeanFromScores();
    float getStandardDeviation();
    float getTemp();
//    float getMeanP1Cgp();
    
    void setMeanFromScores();
    void setStandardDeviation();
    void setTemp(int temp);
    void setSingleAig(AIGraph param);
    void setSingleScore(float score);
    void setSingleFuncSize(int size);
    void setSingleFuncPis(int size);
//    void copyMeanP1Cgp(float param);
//    void setMeanP1Cgp();
    
    void CGPpopToAIGpop(CgpPopulation* cgp_popu,unsigned int I,unsigned int O, unsigned int A,bool internal_call);
    void evaluateScoresAbcCommLine(string PLA_file,int num_to_evaluate);
    void evaluateScoresMyImplement(binaryPLA* PLA,int num_to_evaluate,mt19937& mt);
    tuple<float,float,float> lastEvaluation(binaryPLA* PLA);
    tuple<float,float,float> firstEvaluation(binaryPLA* PLA);
    void gatherAndOrder(AigPopulation* previous_population);
    void readOneAig(string path, string aig_name,mt19937& mt);
    void writeAigs();
    void checkWithAbc(binaryPLA* PLA);
    void clearAigPopu();
    void printScores();
    void printAigSizes();
};


////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
///////////////////////////////////////////////
struct aux_struct{
    vector<nodeAig*> outputs;
    vector<bool> firsts_polarity;
    
};

//returns a word from a phrase, words are expected to be separeted by any number of blank spaces
string wordSelector(string line, int word_index);

//checks the pointer address passed as reference if it is inverted or not.
bool getThisPtrPolarity(nodeAig* param);

int binToDec(vector<int> param);

void abcCallML(string aig_name,string PLA_file,string source);

void abcWrite(string aig_name,string abc_name);
#endif /* AIG_H */

