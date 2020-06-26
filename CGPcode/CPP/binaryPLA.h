/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   binaryPLA.h
 * Author: gudeh
 *
 * Created on April 11, 2020, 5:58 PM
 */

#ifndef BINARYPLA_H
#define BINARYPLA_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <bitset>
#include <algorithm>
#include <random> //find
using namespace std;

class binaryPLA {
private:
    string name;
    int PI_size,PO_size,num_combinations;
//    vector<unsigned long long int> all_combinations;
    vector<vector <bool> > all_combinations;
//    vector<vector <bool> > valid_combinations;
    vector<bool> right_answers;
//    vector<bool> right_answers_valid;
    float pONE_data;
//    float pONE_data_valid;
    
    int batch_size;
    vector<vector<bool> > batch_combinations;
    vector<bool> right_answers_batch;
    bool new_batch;
    int batch_usage_count;
public:
    binaryPLA();
    binaryPLA(const binaryPLA& orig);
    virtual ~binaryPLA();
    void readPLA(string name,int batch_size);
    void setRandomBatch(mt19937& mt);
    void setTrueNewBatch();
    void setFalseNewBatch();
    void setBatchSize(int size);
    void operator++();
    
    string isNewBatch();
    int getPiSize();
    int getNumCombinations();
    string getName();
    vector<bool>* getIthCombination(int ith);
    vector<bool>* getRightAnswers();
    float getP1Data();
    int getBatchSize();
    vector<vector<bool> >* getBatch();
    vector<bool>* getBatchAnswers();
    int getBatchCounter();
    
    void writePLAdebug();
    void clear();
    void printPLA();
    void printBatch();
private:
};

#endif /* BINARYPLA_H */

