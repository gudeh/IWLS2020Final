/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
//
#include "AIG.h"

output::output(){}

output::~output(){}


//void output::setPolarity(bool param) {
//    this->polarity=param;
//}



void output::pushInput(nodeAig* param, bool param_polarity) {
    this->id=param->getId();
    //TRUE to invert FALSE to regular
    if(param_polarity)
            param=((nodeAig*)(((uintptr_t)param) ^ 01));
        input=param;
}

nodeAig* output::getInput(){
    return this->input->fixLSB();
}


int output::getInputPolarity(){
    return ((int)((uintptr_t)input) & 01);
}

void output::writeNode(ofstream& write){
    int depth;
//    ofstream write;
//    write.open("log.txt",ios::app);
    
    write<<"Output: "<<this->id+this->getInputPolarity()<<". Input:"<<this->getInput()->getId()<<endl;
}

int output::computeDepthInToOut(){
    int depth=0;
//    ofstream write;
//    cout<<"output starting search:"<<this->id<<endl;
    if(this->id>1)
        depth=this->getInput()->fixLSB()->computeDepthInToOut();

    return depth;
}

//void output::computeDepthOutToIn(int previous_signal){
//    this->signal=0;
//    this->input->fixLSB()->computeDepthOutToIn(-1);
//}


//unsigned int output::enumerateDFS(unsigned int param_index) {
//    this->id=param_index*2;
//    
//    return this->getInput()->enumerateDFS(param_index);
//}

//unsigned int output::enumerateBFS(unsigned int param_index) {
//    this->id=param_index*2;
//    
//    return this->getInput()->enumerateBFS(param_index);
//}
        

void output::printNode(){
    cout<<"PO:"<<this->id<<". Input:";
        cout<<this->input->fixLSB()->getId()+(int)getThisPtrPolarity(input)<<",";
    cout<<endl;
}
        
unsigned long long int output::PropagSignalDFS(){
//    cout<<"OUT "<<this->id<<"-";
    if(this->signal==-1)
        this->bit_vector=this->input->fixLSB()->PropagSignalDFS();
        
    return this->signal;
}


void output::clearInput(){
    nodeAig* null_node=NULL;
    this->input=null_node;
}