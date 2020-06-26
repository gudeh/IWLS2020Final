/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   binaryPLA.cpp
 * Author: gudeh
 * 
 * Created on April 11, 2020, 5:58 PM
 */

#include <random>

#include "binaryPLA.h"
binaryPLA::binaryPLA() {
    
}
void binaryPLA::readPLA(string name_param, int size_batch) {
    ifstream input_file; string line;
#if COUT == 1
    cout<<"reading:"<<name_param<<endl;
#endif
    input_file.open(name_param);
    
    string aux_name=name_param;
    aux_name.erase(0,aux_name.find_last_of("/")+1);
    aux_name.erase(aux_name.find_first_of("."),aux_name.size());
    this->name=aux_name;
    
    if(input_file.is_open()==false)
        cout<<"input file not open!!!!!"<<endl;
    //TRAIN! header information from PLA
    getline(input_file,line); //.i
    line.erase(0,line.find_first_of(" "));
    PI_size=atoi(line.c_str());
    getline(input_file,line); //.o
    line.erase(0,line.find_first_of(" "));
    PO_size=atoi(line.c_str());

    getline(input_file,line); //.p
    line.erase(0,line.find_first_of(" "));
    num_combinations=atoi(line.c_str());

    if(size_batch==0)
        size_batch=num_combinations;
    this->batch_size=size_batch;
    this->right_answers_batch.resize(size_batch,false);
    this->batch_combinations.resize(size_batch);
    //initialization
    vector<bool> aux(PI_size);
    for(int i=0;i<this->num_combinations;i++)
    {
        this->right_answers.push_back(false);
        this->all_combinations.push_back(aux);
    }

    getline(input_file,line); //.type
    //storing each combination in structure
    int ith=0,count=0;
    while(getline(input_file,line))
    {
        if(line.find(".e")!=string::npos)
            break;
//        cout<<line;
//        cout<<"!"<<endl;
//        right_answers[ith]=(int)(line[line.size()-1]=='1');
        right_answers[ith]=(int)(line.at(line.find_first_of(" ")+1)=='1');
//        cout<<line[(line.size()-2)]<<endl;
//        cout<<right_answers[ith]<<endl;
        if(right_answers[ith])
            count++;
        line.erase(line.find_first_of(" "),line.size());
        for(int x=0;x<all_combinations[ith].size();x++)
            this->all_combinations[ith][x]=(line[x] == '1');
        ith++;
    }
    //calculating primary output of being 1
    this->pONE_data=(float)count/right_answers.size();
#if COUT == 1
    cout<<count<<endl;
    cout<<"PROBABILITY OF ONE PLA:"<<pONE_data<<endl;
#endif
}


int binaryPLA::getPiSize(){
    return this->PI_size;
}

int binaryPLA::getNumCombinations(){
    return this->num_combinations;
}

binaryPLA::binaryPLA(const binaryPLA& orig) {
}

binaryPLA::~binaryPLA() {
}

void binaryPLA::clear(){
    this->PI_size=0;
    this->PO_size=0;
    this->all_combinations.clear();
//    this->valid_combinations.clear();
    this->batch_combinations.clear();
    this->name="";
    this->num_combinations=0;
    this->right_answers_batch.clear();
    this->right_answers.clear();
//    this->right_answers_valid.clear();
}

void binaryPLA::writePLAdebug(){
    ofstream out_file(this->name+"_struct.txt");
    for(int ith=0;ith<all_combinations.size();ith++)
    {
        out_file<<ith<<":";
        for(int x=0;x<all_combinations[ith].size();x++)
            out_file<<this->all_combinations[ith][x];
        out_file<<" "<<this->right_answers[ith]<<endl;
    }
    out_file.close();
}

vector<bool>* binaryPLA::getIthCombination(int ith){
    return &this->all_combinations[ith];
}

vector<bool>* binaryPLA::getRightAnswers(){
    return &this->right_answers;
}

string binaryPLA::getName(){
    return this->name;
}

float binaryPLA::getP1Data(){
    return this->pONE_data;
}

void binaryPLA::setRandomBatch(mt19937& mt){
#if COUT ==1
    cout<<"Generating new random batch!! Batch size:"<<batch_size<<", Num combinations:"<<num_combinations<<endl;
#endif
    if(batch_size==this->num_combinations)
        {this->batch_combinations=this->all_combinations; this->right_answers_batch=this->right_answers; return;}
    vector<int> generated_numbers(this->batch_size);
    this->batch_usage_count=0;
    int random_num=0;
    
    for(int i=0;i<batch_size;i++)
    {
        uniform_real_distribution<double> dist_comb(0,num_combinations);
        random_num=dist_comb(mt);
//        cout<<random_num<<",";
//        if(generated_numbers.end()!=find(generated_numbers.begin(),generated_numbers.end(),random_num))
//            cout<<"equal random number:"<<random_num<<"=="<<*(find(generated_numbers.begin(),generated_numbers.end(),random_num))<<endl;
        while(generated_numbers.end()!=find(generated_numbers.begin(),generated_numbers.end(),random_num))
             random_num=dist_comb(mt);
        generated_numbers[i]=random_num;
        right_answers_batch[i]=right_answers[random_num];
        batch_combinations[i]=all_combinations[random_num];
    }
//    cout<<endl;
}

int binaryPLA::getBatchSize(){
    return this->batch_size;
}

void binaryPLA::setBatchSize(int param){
    this->batch_size=param;
    if(param==this->num_combinations)
        {this->batch_combinations=this->all_combinations; this->right_answers_batch=this->right_answers;}
}
vector<vector<bool> >* binaryPLA::getBatch(){
    return &this->batch_combinations;
}
vector<bool>* binaryPLA::getBatchAnswers(){
    return &this->right_answers_batch;
}

void binaryPLA::setFalseNewBatch(){
    this->new_batch=false;
}
void binaryPLA::setTrueNewBatch(){
    this->new_batch=true;
}

string binaryPLA::isNewBatch(){
    if(this->new_batch)
        return "TRUE";
    else 
        return "FALSE";
}

void binaryPLA::operator++(){
    this->batch_usage_count++;
}

int binaryPLA::getBatchCounter(){
    return this->batch_usage_count;
}

void binaryPLA::printPLA(){
    cout<<"PLA INFO---> Name:"<<this->name<<",#PIs:"<<this->PI_size<<",#POs:"<<this->PO_size<<",#Combinations"<<this->num_combinations;
    cout<<",batch size:"<<this->batch_size<<",p1Data:"<<this->pONE_data<<endl;
//    for(int i=0;i<this->batch_combinations.size();i++)
//    {
//        for(int j=0;j<batch_combinations[i].size();j++)
//            cout<<batch_combinations[i][j]; cout<<"->"<<right_answers[i]<<endl;
//    }
}

void binaryPLA::printBatch(){
    cout<<"------------BATCH INFO:"<<endl;
    for (int i=0;i<batch_combinations.size();i++)
    {
        for(int j=0;j<batch_combinations[i].size();j++)
        cout<<batch_combinations[i][j];
        cout<<"->"<<right_answers_batch[i]<<"  ";
        cout<<i<<endl;
//        cout<<generated_numbers[i]<<endl;
    }
}