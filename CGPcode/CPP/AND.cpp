/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "AIG.h"


AND::AND(){}

AND::~AND(){}

vector<nodeAig*> AND::getInputs(){
    vector<nodeAig*> ret(2);
    ret[0]=this->inputs[0]->fixLSB();
    ret[1]=this->inputs[1]->fixLSB();
    return ret;
} 



void AND::pushInput(nodeAig* param, bool param_polarity){
    if(this->inputs.size()>2)
        cout<<"ERROR, trying to add more then 2 inputs on AND node: "<<this->id<<endl;
    else if (this->inputs.size()==0)
    {
        nodeAig dummy_node(-1);
        this->inputs.push_back(&dummy_node);
        this->inputs.push_back(&dummy_node);
        //polarity == TRUE for INVERTED and FALSE for REGULAR
        if(param_polarity)
            param=((nodeAig*)(((uintptr_t)param) ^ 01));
        this->inputs[0]=param;
    }
    else
    {
        if(param_polarity)
            param=((nodeAig*)(((uintptr_t)param) ^ 01));
        if(inputs[0]->fixLSB()->getId()<=param->fixLSB()->getId())
            inputs[1]=param;
        else
        {
            inputs[1]=inputs[0];
            inputs[0]=param;
        }
    }
}

int AND::computeDepthInToOut(){
//    int depth;
    
//    cout<<"visiting AND:"<<this->id<<endl;
    if(this->signal==-1)
    {
        int depth1,depth2;
//        depth1=((node*)(inputs[0]/2))->computeDepthInToOut();
//        depth2=((node*)(inputs[1]/2))->computeDepthInToOut();

        depth1=this->getInputs()[0]->fixLSB()->computeDepthInToOut();
        depth2=this->getInputs()[1]->fixLSB()->computeDepthInToOut();
        int v;         
        unsigned int r;  
        v=depth1-depth2;
        int const mask = v >> sizeof(int) * CHAR_BIT - 1;

        r = (v + mask) ^ mask;
        signal=(((depth1+depth2)+r)/2) +1 ;
        return signal;
    }
    else 
        return signal;
}


//void AND::computeDepthOutToIn(int previous_depth){
//    cout<<this->id<<endl;
//    if (this->signal > previous_depth+1 || this->signal==-1)
//    {
//        signal=previous_depth+1;
//        if(this->getInputs()[0]->getSignal() > signal+1 || this->getInputs()[0]->getSignal() ==-1)
//            this->getInputs()[0]->computeDepthOutToIn(this->signal);
//        if(this->getInputs()[1]->getSignal() > signal+1 || this->getInputs()[1]->getSignal() ==-1)
//            this->getInputs()[1]->computeDepthOutToIn(this->signal);
//    }    
//}


//unsigned int AND::enumerateDFS(unsigned int index){
//    this->id=index*2;
//    unsigned int new_index;
//    index++;
//    new_index=this->getInputs()[0]->enumerateDFS(index);
//    new_index=this->getInputs()[1]->enumerateDFS(new_index);
//    return new_index;
//}


//unsigned int AND::enumerateBFS(unsigned int index){
//    this->id=index*2;
//    unsigned int new_index;
//    index++;
//    new_index=this->getInputs()[0]->enumerateDFS(index);
//    new_index=this->getInputs()[1]->enumerateDFS(new_index);
//    return new_index;
//}

void AND::writeNode(ofstream& write){
    int depth;
//    ofstream write;
//    write.open("log.txt",ios::app);
//    vector<bool> polarities;
    
    write<<"AND: "<<this->id<<" .Inputs("<<this->getInputs().size()<<"):";
    
    write<<this->getInputs()[0]->getId()+this->getInputPolarities()[0];
    write<<",";
    
    write<<this->getInputs()[1]->getId()+this->getInputPolarities()[1];
#if IGNORE_OUTPUTS ==0
    write<<" Outputs: ";
    for(int i=0;i<this->outputs.size();i++)
        write<<this->outputs[i]->getId()<<",";
#endif
    write<<endl;
}

//vector<unsigned long long int> AND::getPolaritiesIn64bits(){;
//    unsigned long long aux1,aux2;
//    aux1=(unsigned long long)((uintptr_t)inputs[0]) & 01;
//    aux2=(unsigned long long)((uintptr_t)inputs[1]) & 01;
//}

vector<int> AND::getInputPolarities(){
    vector<int> ret(2);
    //returns FALSE for regular and TRUE for inverted
    ret[0]=(int)((uintptr_t)inputs[0]) & 01;
    ret[1]=(int)((uintptr_t)inputs[1]) & 01;
//    cout<<this->getInputs()[0]->getId()<<":"<<ret[0]<<"|";
//    cout<<this->getInputs()[1]->getId()<<":"<<ret[1]<<endl;
//    ret[0]= inputs[0] & 0x1;
//    ret[1]= inputs[1] & 0x1;
    return ret;
}


void AND::printNode(){
    cout<<"AND:"<<this->id;//<<". Signal:"<<this->signal;
    cout<<". Inputs:";
    for(int a=0;a<this->inputs.size();a++)
        cout<<inputs[a]->fixLSB()->getId()+(int)getThisPtrPolarity(inputs[a])<<",";
    cout<<"signal:"<<this->signal;
#if IGNORE_OUTPUTS ==0
    cout<<". Outputs:";
    for(int a=0;a<this->outputs.size();a++)
        cout<<outputs[a]->fixLSB()->getId()+(int)getThisPtrPolarity(outputs[a])<<",";
#endif
    
    cout<<endl;
}

unsigned long long int AND::PropagSignalDFS(){
    
    unsigned long long int sig_rhs0,sig_rhs1;
    switch (this->signal){
        case -1:
        sig_rhs0=inputs[0]->fixLSB()->PropagSignalDFS();
        sig_rhs1=inputs[1]->fixLSB()->PropagSignalDFS();
#if DEBUG >= 3
        ofstream dump("dumpDFS.csv",ios::app);
        dump<<this->id<<"-->";    
        dump<<getInputs()[0]->getId()<<":"<<sig_rhs0<<",pol0:"<<(getInputPolarities()[0])<<","<<getInputs()[1]->getId()<<":"<<sig_rhs1<<",pol1:"<<(getInputPolarities()[1]);
        dump<<",pol0*U:"<<(getInputPolarities()[0]*ULLONG_MAX)<<",pol1*U:"<<(getInputPolarities()[1]*ULLONG_MAX);
        dump<<", XOR0:"<<(sig_rhs0^(getInputPolarities()[0]*ULLONG_MAX))<<", XOR1:"<<(sig_rhs1^(getInputPolarities()[1]*ULLONG_MAX));
        dump<<",my:"<<((sig_rhs0^(this->getInputPolarities()[0]*ULLONG_MAX))&(sig_rhs1^(this->getInputPolarities()[1]*ULLONG_MAX)))<<endl;
#endif
        if(getInputPolarities()[0])
            sig_rhs0=~sig_rhs0;
        if(getInputPolarities()[1])
            sig_rhs1=~sig_rhs1;
//        cout<<"sig_rhs1:"<<sig_rhs1<<endl;
#if MULTI_FUNC == 1
        if(this->isXOR)
            this->bit_vector= sig_rhs0 ^ sig_rhs1;
        else
            this->bit_vector= sig_rhs0 & sig_rhs1;
#else
        this->bit_vector= sig_rhs0 & sig_rhs1;
#endif
//        cout<<bit_vector<<endl;this->printNode();
//        bit_vector=(((inputs[0]->fixLSB()->runDFS())^(this->getInputPolarities()[0]*ULLONG_MAX))&((inputs[1]->fixLSB()->runDFS())^(this->getInputPolarities()[1]*ULLONG_MAX)));

#if DEBUG >= 3
//        dump<<"rhs0:"<<sig_rhs0<<endl;
//        dump<<"rhs1:"<<sig_rhs1<<endl;
        dump<<"this->getInputPolarities()[0]:"<<this->getInputPolarities()[0]<<endl;
        dump<<"this->getInputPolarities()[1]:"<<this->getInputPolarities()[1]<<endl;
        dump<<"bit1^(this->getInputPolarities()[0])"<<(sig_rhs0 & 1)<<endl;
        dump<<"bit2^(this->getInputPolarities()[1])"<<(sig_rhs1 & 1)<<endl;
        dump<<"result:"<<(bit_vector & 1)<<endl;
        bitset<64> x(bit_vector);
        dump<<"whole vector:"<<bit_vector<<","<<x<<endl<<endl;
#endif
        this->signal=1;
    }
//    cout<<"AND:"<<this->id<<endl;
//    cout<<"fanin0:"<<bitset<64>(sig_rhs0).to_string()<<endl;
//    cout<<"fanin1:"<<bitset<64>(sig_rhs1).to_string()<<endl;
//    cout<<"result:"<<bitset<64>(bit_vector).to_string()<<endl;
    return this->bit_vector;

}





#if IGNORE_OUTPUTS ==0
void AND::pushOutput(nodeAig* param){
    this->outputs.push_back(param);
}

vector<nodeAig*> AND::getOutputs(){
    return this->outputs;
}

void AND::clearOutputs() {
    this->outputs.clear();
}

//void AND::removeOutput(node* node_to_remove) {
void AND::removeOutput(unsigned int id_to_remove) {
    for(int i=0;i<outputs.size();i++)
    {
        if(outputs[i]->getId()==id_to_remove)
        {
//            cout<<"AND:"<<this->id<<" removing output:"<<outputs[i]->getId()<<endl;
            outputs.erase(outputs.begin()+i);
            break;
//            cout<<"after remove:";
//            this->printNode();
        }
    }
}

//void AND::recursiveRemoveOutput(unsigned int id_to_remove){
//    for(int i=0;i<outputs.size();i++)
//    {
//        if(outputs[i]->getId()==id_to_remove)
//        {
//            outputs.erase(outputs.begin()+i);
//            break;
//        }
//    }
//    
//    if(outputs.size()==0)
//    {
//        this->inputs[0]->fixLSB()->recursiveRemoveOutput(this->id);
//        this->inputs[1]->fixLSB()->recursiveRemoveOutput(this->id);
//    }
//}
#endif

//void AND::invertInputs() {
//        nodeAig* aux;
//    if(inputs[0]->fixLSB()->getId()<inputs[1]->fixLSB()->getId())
//    {
////		 cout<<"Inverting AND:"<<id<<" inputs:"<<inputs[0]->fixLSB()->getId()<<","<<inputs[1]->fixLSB()->getId()<<endl;
//        aux=inputs[0];
//        inputs[0]=inputs[1];
//        inputs[1]=aux;
////		 cout<<"Result:"<<id<<" inputs:"<<inputs[0]->fixLSB()->getId()<<","<<inputs[1]->fixLSB()->getId()<<endl;
//    }
//    else if (inputs[0]->fixLSB()->getId()==inputs[1]->fixLSB()->getId())
//    {
//        if(this->getInputPolarities()[0]==0 && this->getInputPolarities()[1]==1)
//        {
//                aux=inputs[0];
//                inputs[0]=inputs[1];
//                inputs[1]=aux;
//        }
//    }
//	
//}

#if MULTI_FUNC == 1
void AND::setAsXOR(bool param){
    this->isXOR=param;
}

bool AND::getIsXOR(){
    return isXOR;
}

void AND::clearInputs(){
    this->inputs.clear();
}
#endif