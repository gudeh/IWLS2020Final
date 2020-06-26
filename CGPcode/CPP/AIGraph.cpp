/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   circuit.cpp
 * Author: augusto
 * 
 * Created on 23 de Março de 2018, 16:59
 */
 
#include <bits/c++config.h>
#include <bitset>

#include "AIG.h"

AIGraph::AIGraph(){
    this->constant1.setId(1);
//    this->constant1.setSignal(1);
    this->constant1.setBitVector(UINT_MAX);
    this->constant0.setId(0);
//    this->constant0.setSignal(0);
    this->constant0.setBitVector(0);
}

AIGraph::~AIGraph(){
}

AIGraph::AIGraph(string name) {
    this->name=name;
    this->constant1.setId(1);
//    this->constant1.setSignal(1);
    this->constant1.setBitVector(UINT_MAX);
    this->constant0.setId(0);
//    this->constant0.setSignal(0);
    this->constant0.setBitVector(0);
}


void AIGraph::setName(string param){
    this->name=param;
}

string AIGraph::getName(){
    return this->name;
}

//int AIGraph::getDepth(){
//    return this->graph_depth;
//}

input* AIGraph::pushPI(unsigned int index,input input_obj){
    input_obj.setUnvisited();
    this->all_inputs[index]=input_obj;
    return &this->all_inputs[index];
}

void AIGraph::pushPO(unsigned int index, output output_obj){
    this->all_outputs[index]=output_obj;
//    this->all_outputs.insert(pair<unsigned int,output>(index,output_obj));
}

AND* AIGraph::pushAnd(unsigned int index, AND AND_obj){
    this->all_ANDS[index]=AND_obj;
    return &this->all_ANDS[index];
}

void AIGraph::printCircuit() {
    cout<<"Circuit name: "<<this->name<<endl;
    cout<<"MILOA:"<<this->all_ANDS.size()-1+this->all_inputs.size()-1<<","<<this->all_inputs.size()-1<<",0,"<<this->all_outputs.size()<<","<<this->all_ANDS.size()-1<<endl;
 
    cout<<"Inputs: ";
    for(int i=1;i<all_inputs.size();i++)
         cout<<all_inputs[i].getId()<<",";
    cout<<endl;
    
    cout<<"Outputs: ";
    for(int i=0;i<all_outputs.size();i++)
    { if(all_outputs[i].getId()>1){
        cout<<all_outputs[i].getInput()->getId()+((int)all_outputs[i].getInputPolarity())<<",";
    }}
        cout<<endl;
    
    cout<<"ANDs: ";
    for(int i=1;i<all_ANDS.size();i++)
    {
        if(all_ANDS[i].getId()>1){
        if(all_ANDS[i].getInputs().size()==2)
            cout<<all_ANDS[i].getId()<<"("<<all_ANDS[i].getInputs()[0]->getId()+((int)all_ANDS[i].getInputPolarities()[0]);
            cout<<","<<all_ANDS[i].getInputs()[1]->getId()+((int)all_ANDS[i].getInputPolarities()[1])<<"),";
        }
    }
    cout<<endl;
}


void AIGraph::clearCircuit(){
    this->all_ANDS.clear();
    this->all_inputs.clear();
    this->all_outputs.clear();
    this->name.clear();
    this->current_score=0;
    this->name="";
//    this->p1_cgp=0;
    this->functional_and_count=0;
    this->all_depths.clear();
//    this->graph_depth=0;
    this->all_depths.clear();
    this->extra_ands.clear();
}

void AIGraph::writeAIG(string destination, string aig_name){
    ofstream write;
//    if(destination.empty())
//        destination="./";
    int M=all_inputs.size()+all_ANDS.size()-2;
    write.open((destination+aig_name).c_str(),ios::binary|ios::out|ios::trunc);
#if COUT ==1 
    if(write.is_open())
        cout<<"write aig file opened:"<<(destination+aig_name)<<endl;
    else
        cout<<"ERROR WRITE AIG FILE NOT OPENED!!"<<endl;
#endif
    write<<"aig "<<M<<" "<<all_inputs.size()-1<<" 0 "<<all_outputs.size()<<" "<<all_ANDS.size()-1<<endl;
    
    for(int i=0;i<all_outputs.size();i++)    
        write<<all_outputs[i].getId()+((int)all_outputs[i].getInputPolarity())<<endl;

    
//    cout<<"last guy:"; all_ANDS[all_ANDS.size()-1].printNode();
//    cout<<".back():"; all_ANDS.back().printNode();

    //deltas 
    int counter=0;
    unsigned int first;
    for(int i=1;i<all_ANDS.size();i++)
     {
         if(all_ANDS.back().getId()!=all_ANDS[i].getId() && (all_ANDS[i].getId()>=all_ANDS[i+1].getId()))
             cout<<"all_ANDS[i].getId()>=all_ANDS[i+1].getId():TRUE!! "<<all_ANDS[i].getId()<<"<"<<all_ANDS[i+1].getId()<<endl;


         if(all_ANDS[i].getId()>1)
         {
             if(all_ANDS[i].getId()<all_ANDS[i].getInputs()[0]->getId())
             {  cout<<"THIS IS WRONG:"<<all_ANDS[i].getId()<<"<"<<all_ANDS[i].getInputs()[0]->getId()<<endl;    all_ANDS[i].printNode();}
            if(all_ANDS[i].getId()<all_ANDS[i].getInputs()[1]->getId())
            {  cout<<"THIS IS WRONG:"<<all_ANDS[i].getId()<<"<"<<all_ANDS[i].getInputs()[1]->getId()<<endl;    all_ANDS[i].printNode();}
             counter++;
             encodeToFile(write,(all_ANDS[i].getId())-(all_ANDS[i].getInputs()[0]->getId()+(all_ANDS[i].getInputPolarities()[0])));
             first=(all_ANDS[i].getInputs()[0]->getId()+(all_ANDS[i].getInputPolarities()[0]));
             encodeToFile(write,(first)-(all_ANDS[i].getInputs()[1]->getId()+all_ANDS[i].getInputPolarities()[1]));
         }
     }
//     cout<<"counter:"<<counter<<", all_ANDS.size()-1:"<<all_ANDS.size()-1<<endl;
//     cout<<endl;
}

void AIGraph::encodeToFile(ofstream& file, unsigned x){
        unsigned char ch;
//        cout<<x<<",";
        while (x & ~0x7f)
        {
            ch = (x & 0x7f) | 0x80;
            file<<ch;
            x >>= 7;
        }
        ch = x;
        file<<ch;
}

unsigned char AIGraph::getnoneofch (ifstream& file,int index){
		
        int ch;
        char c;
        file.get(c);
        ch=(int)c;
        if (ch != '\0')
            return ch;
        else
        {
            cout<<"AND:"<<index<<" ch:"<<ch<<" c:"<<c<<endl;
            fprintf (stderr, "*** decode: unexpected EOF\n\n");
            return ch;
        }
    }

unsigned AIGraph::decode (ifstream& file,int index){
        unsigned x = 0, i = 0;
        unsigned char ch;
        while ((ch = getnoneofch (file,index)) & 0x80)
        {
            x |= (ch & 0x7f) << (7 * i++);
//            cout<<"x:"<<x<<"|";
        }
//        cout<<"X:"<<(x | (ch << (7 * i)))<<"|";
        return x | (ch << (7 * i));
    }

void AIGraph::readAIG(ifstream& file, string param_name, mt19937& mt){
    string line;
    string type;
    unsigned int M,I,L,O,A;//,num_extra_nodes=1;
    unsigned int lhs,rhs0,rhs1, delta1,delta2;
#if DEBUG >= 2
    ofstream debs("debug");
#endif
    if(param_name.find("/")!=string::npos)
        param_name.erase(0,param_name.find_last_of('/')+1);
    if(param_name.find(".")!=string::npos)
        param_name.erase(param_name.find_last_of('.'),param_name.size());
    this->setName(param_name);
    
     
    //reading the first line in the file
    file.seekg(file.beg);
    getline(file,line);
    //line has the index information
    type=wordSelector(line,1);
    M=stoi(wordSelector(line,2));
    I=stoi(wordSelector(line,3));
    L=stoi(wordSelector(line,4));
    O=stoi(wordSelector(line,5));
    A=stoi(wordSelector(line,6));
//    A+=(A*num_extra);
    bool polarity0,polarity1,lhs_polarity;
#if COUT == 1
    cout<<endl<<"READING AIG CIRCUIT:"<<name<<endl;
    cout<<"MILOA:"<<M<<","<<I<<","<<L<<","<<O<<","<<A<<endl;
#endif
    for(int y=0;y<=I;y++)
        this->all_inputs.push_back(input());
    for(int y=0;y<O;y++)
        this->all_outputs.push_back(output());
//    for(int y=0;y<=A;y++)
    for(int y=0;y<=A+(A*num_extra_nodes);y++)
        this->all_ANDS.push_back(AND(0));
    
    for(int ii=0;ii<O;ii++)
        getline(file,line); 
#if COUT == 1
    cout<<"Reading Inputs, ";
#endif
    //////////////////////INPUTS//////////////////////
    for(int i=1;i<=I;i++)
    {
        input input_obj(i*2);
        input_obj.setUnvisited();
        this->pushPI(i,input_obj);
#if DEBUG >= 4
        cout<<"pushing input "<<i*2<<endl;
#endif
    }
#if COUT == 1
    cout<<"Reading ANDs"<<endl;
#endif
    /////////////ANDS/////////////////////////
    int and_index=I+L;
    bool polar=false;
    AND* AND_ptr;
    for(int l=0;l<A;l++)
    {
        //cout<<"L:"<<l<<endl;
//        this->printCircuit();
        and_index++;
//        cout<<"CREATING AND "<<and_index*2<<"-----------------------------------"<<endl;
//        AND AND_obj(and_index*2);
//        AND_ptr=this->pushAnd(and_index-all_inputs.size()+1,AND_obj);
////        AND_ptr=this->pushAnd(l+1,AND_obj);
//        delta1=decode(file,and_index*2);
//        rhs0=and_index*2-delta1; 
//        delta2=decode(file,and_index*2);
//        rhs1=rhs0-delta2;
//        if(rhs0 % 2!=0)
//            {
//                polar=true;
//                rhs0--;
//            }
//            else
//                polar=false;
//            AND_ptr->pushInput(this->findAny(rhs0),polar);
//            if(rhs1 % 2!=0)
//            {
//                polar=true;
//                rhs1--;
//            }
//            else
//                polar=false;
//            AND_ptr->pushInput(this->findAny(rhs1),polar);
        
        
        uniform_int_distribution<int> dist(2,(((and_index-num_extra_nodes-1)*2)+(num_extra_nodes+(l*num_extra_nodes))*2));
        int random=0;
//        cout<<"andIndex:"<<and_index<<"->dist max:"<<dist.max()<<endl;
        for(int x=num_extra_nodes;x>0;x--)
        { 
            AND AND_extra(((and_index-x)*2)+(num_extra_nodes+(l*num_extra_nodes))*2);
//            cout<<"extra:"<<AND_extra.getId()<<", iterator:"<<AND_extra.getId()/2-all_inputs.size()+1<<endl;
            AND_ptr=this->pushAnd((AND_extra.getId()/2)-all_inputs.size()+1,AND_extra);
            random=dist(mt); //cout<<"random1:"<<random; 
            AND_ptr->pushInput(this->findAny(random),false);
            random=dist(mt); //cout<<"random2:"<<random<<endl; 
            AND_ptr->pushInput(this->findAny(random),false);
//            AND_ptr->printNode();
        }
        AND AND_obj((and_index*2)+(num_extra_nodes+(l*num_extra_nodes))*2);
        AND_ptr=this->pushAnd((AND_obj.getId()/2)-all_inputs.size()+1,AND_obj);
//        cout<<"-->real("<<and_index*2<<"):"<<AND_obj.getId()<<",iterator:"<<(AND_obj.getId()/2)-all_inputs.size()+1<<endl;
//        cout<<"and_index*2:"<<and_index*2<<", new id:"<<((and_index*2)+(num_extra_nodes+(l*num_extra_nodes))*2)<<endl;

        delta1=decode(file,and_index*2);
        rhs0=and_index*2-delta1; //cout<<"old rhs0:"<<rhs0;
        delta2=decode(file,and_index*2);
        rhs1=rhs0-delta2; //cout<<",old rhs1:"<<rhs1<<endl;
        if(rhs0 % 2!=0)
            {
                polar=true;
                rhs0--;
            }
        else
            polar=false;
        if(rhs0>(all_inputs.size()-1)*2)
            rhs0+=((num_extra_nodes+(((rhs0/2)-all_inputs.size())*num_extra_nodes))*2); //cout<<"rhs0+="<<((num_extra_nodes+((l-1)*num_extra_nodes))*2)<<",new rhs0:"<<rhs0<<endl;
//        cout<<rhs0<<","<<((all_inputs.size()-1)*2)<<endl;
        AND_ptr->pushInput(this->findAny(rhs0),polar);
            
        
        if(rhs1 % 2!=0)
            {
                polar=true;
                rhs1--;
            }
        else
            polar=false;
        if(rhs1>(all_inputs.size()-1)*2)
            rhs1+=((num_extra_nodes+(((rhs1/2)-all_inputs.size())*num_extra_nodes))*2); // cout<<",new rhs1:"<<rhs1<<endl;
        AND_ptr->pushInput(this->findAny(rhs1),polar);
#if DEBUG >=2
        debs<<"AND:"<<and_index*2<<". ";
        debs<<"delta1:"<<delta1<<" delta2:"<<delta2<<". ";
        debs<<"rhs0:"<<rhs0<<" rhs1:"<<rhs1<<endl;
#endif
            
            

            
#if IGNORE_OUTPUTS == 0
//        cout<<"rhs1:"<<rhs1<<" ";
//        this->findAny(rhs1)->printNode();
//            cout<<"rhs0:"<<rhs0<<" ";
//            this->findAny(rhs0)->printNode();
            cout<<"ADDING FANOUTS"<<endl;
    if(rhs0>1)
    {
         findAny(rhs0)->printNode();F
       this->findAny(rhs0)->pushOutput(AND_ptr);
    }
    if(rhs1>1)
    {
        findAny(rhs1)->printNode();
       this->findAny(rhs1)->pushOutput(AND_ptr); 
    }
//        cout<<"rhs1:"<<rhs1<<" ";
//        this->findAny(rhs1)->printNode();
//            cout<<"rhs0:"<<rhs0<<" ";
//            this->findAny(rhs0)->printNode();
#endif
//            cout<<"AND CREATED:"<<"-----------------------------------"; AND_ptr->printNode();
    }
#if DEBUG >= 2
    debs.close();
#endif
#if COUT == 1
    cout<<"Reading Outputs"<<endl;
#endif
        ////////////////////OUTPUTS///////////////////
    //jumping the header, right to outputs list
    file.seekg(file.beg);
    getline(file,line);
    
    for(int f=0;f<O;f++){
        getline(file,line);
       lhs=stoi(wordSelector(line,1));
       if(lhs>1)
       {
            if(lhs % 2 != 0)
            {
                lhs=lhs-1;
                polarity0=true;
            }
            else
                polarity0=false;
       }
       else
           cout<<"CONSTANT BEING INSTANTIATED AS PO!"<<endl;
//       cout<<"-->old PO:"<<lhs;
       lhs+=(num_extra_nodes+(((lhs/2)-all_inputs.size())*num_extra_nodes))*2;
//       cout<<"-->new PO:"<<lhs<<endl;
        output output_obj(lhs);
        if(lhs>1)
            output_obj.pushInput(findAny(lhs),polarity0);
        this->pushPO(f,output_obj);
#if DEBUG >= 4
        cout<<"pushing output "<<lhs<<" Polarity:"<<polarity0<<endl;
#endif  
    }

//    cout<<this->all_inputs.size()<<endl;
//    this->printCircuit();
}

void AIGraph::CGPtoAIG(vector<short>* cgp_vector,unsigned int num_lines, unsigned int i, unsigned int o, unsigned int a, string name_param) {
    this->name=name_param;
    unsigned int M,I,L,O,A,PO_id,rhs0,rhs1,tuple_size; bool PO_polarity;
    M=i+a;    I=i;    L=0;    O=o;   A=a;
#if MULTI_FUNC == 1
    tuple_size=7;
    vector<bool> ANDs_polars(A+1,false);
#else    
    tuple_size=6;
#endif
//    cout<<"AIG name:"<<this->name<<endl;
//    cout<<"number of ANDs to be created:"<<a<<endl;
//    cout<<"Converting CGP to AIG!!"<<endl;
//    cout<<"MILOA:"<<M<<","<<I<<","<<L<<","<<O<<","<<A<<endl;
    for(int y=0;y<=I;y++)
        this->all_inputs.push_back(input());
    for(int y=0;y<O;y++)
        this->all_outputs.push_back(output());
    //TODO: DIFFERENT WITH XOR!!
    for(int y=0;y<=A;y++)
        this->all_ANDS.push_back(AND(0));
//    cout<<"CREATING AIG PRIMARY INPUTS!!"<<endl;
    for(int i=1;i<=I;i++)
    {
        input input_obj(i*2);
        this->pushPI(i,input_obj);
    }
//    cout<<"CREATING ANDs!!"<<endl;
    int and_index=I+L;
    bool polar0=false,polar1=false;
    AND* AND_ptr;
    for(int l=0;l<A*tuple_size;l+=tuple_size)
    {
//        cout<<l<<",";
        and_index++;
        AND AND_obj(and_index*2);
//        AND_ptr=this->pushAnd(and_index-all_inputs.size()+1,AND_obj);
//        cout<<"and index:"<<and_index<<",AND:"<<(and_index-all_inputs.size()+1)<<",all inpt:"<<all_inputs.size()<<","<<all_inputs.size()-1<<"=="<<I<<endl;
            AND_ptr=this->pushAnd(and_index-I,AND_obj);
        if(cgp_vector->at(l+1)>0) //AND
            rhs0=(((cgp_vector->at(l+1)-1)*num_lines)+cgp_vector->at(l)+1+I)*2;
        else  //PI
            rhs0=(cgp_vector->at(l)+1)*2;
        if(cgp_vector->at(l+4)>0)
            rhs1=(((cgp_vector->at(l+4)-1)*num_lines)+cgp_vector->at(l+3)+1+I)*2;
        else
            rhs1=(cgp_vector->at(l+3)+1)*2;
//        if(rhs0==2)
//        {cout<<"------------------------------------";
//            this->findAny(rhs0)->printNode();}
//        if(rhs1==2)
//        {cout<<"------------------------------------";this->findAny(rhs1)->printNode();}
        //        polar=cgp_vector->getVector()->at(l).getInv1();
        polar0=cgp_vector->at(l+2);
        polar1=cgp_vector->at(l+5);
#if MULTI_FUNC == 1
//        if(cgp_vector->at(l+6)==2)
//        {   polar0^=1;  polar1^=1;   
////            cout<<rhs0<<"?"<<I*2<<endl;
//            if(rhs0>I*2)
//            {
////                cout<<"rhs0-I:"<<(rhs0/2)-I<<",rhs0:"<<rhs0<<endl;
////                cout<<"ANDs_polars[(rhs0/2)-I]:"<<ANDs_polars[(rhs0/2)-I]<<endl;
//                polar0^=ANDs_polars[(rhs0/2)-I];
//            }
//            if(rhs1>I*2)
//            {
////                cout<<"rhs1-I:"<<(rhs1/2)-I<<",rhs1:"<<rhs1<<endl;
//                polar1^=ANDs_polars[(rhs1/2)-I];
////                cout<<"polar1^=ANDs_polars[(rhs1/2)-I]:"<<ANDs_polars[(rhs1/2)-I]<<endl;
//            }
//            ANDs_polars[and_index-I]=true;
//        }
//        else 
        if (cgp_vector->at(l+6)==3)
            AND_ptr->setAsXOR(true);
        else
            AND_ptr->setAsXOR(false);
#endif
//        cout<<"id:"<<AND_ptr->getId()<<", rhs0:"<<rhs0<<", rhs1:"<<rhs1<<endl;
        
        AND_ptr->pushInput(this->findAny(rhs0),polar0);
        AND_ptr->pushInput(this->findAny(rhs1),polar1);
//        cout<<"CREATING AND "<<and_index*2<<endl;
//        AND_ptr->printNode();
#if IGNORE_OUTPUTS == 0
//            cout<<"ADDING FANOUTS"<<endl;
    if(rhs0>1)
       this->findAny(rhs0)->pushOutput(AND_ptr);
    if(rhs1>1)
       this->findAny(rhs1)->pushOutput(AND_ptr); 
#endif
//            cout<<"AND CREATED:"; AND_ptr->printNode();
    }
#if MULTI_FUNC == 1
//    for(int m=1;m<ANDs_polars.size();m++)
//        cout<<m<<":"<<ANDs_polars[m]<<",";    cout<<endl;
#endif
//    cout<<"CREATING PRIMARY OUTPUTS!!"<<endl; 
    for(int f=0;f<O;f++){
//        cout<<"last and:"<<(this->all_ANDS.end()-1)->getId()<<endl;
        PO_id=((cgp_vector->at(cgp_vector->size()-3)+1)*(cgp_vector->at(cgp_vector->size()-2))+(this->all_inputs.size()-1))*2;
        
        output output_obj(PO_id);
        PO_polarity=cgp_vector->back();
//        cout<<"PO_id:"<<PO_id<<", polarity:"<<PO_polarity<<endl;
        output_obj.pushInput(findAny(PO_id),PO_polarity);
        this->pushPO(f,output_obj);
#if DEBUG >= 4
        cout<<"pushing output "<<lhs<<" Polarity:"<<polarity0<<endl;
#endif  
    }
//    this->printCircuit();    
//    for(it_and=all_ANDS.begin();it_and!=all_ANDS.end();it_and++)
//    for(int i=1;i<all_ANDS.size();i++)
//        all_ANDS[i].invertInputs();
}

void AIGraph::writeAAG(string destination, string aig_name){
    ofstream write;
    unsigned char charzin;
    int M=all_inputs.size()+all_ANDS.size()-2;
    write.open((destination+aig_name).c_str(),ios::binary|ios::out|ios::trunc);
//    write.open((name+".aag").c_str(),ios::out|ios::trunc);
    write<<"aag "<<M<<" "<<all_inputs.size()-1<<" 0 "<<all_outputs.size()<<" "<<all_ANDS.size()-1<<endl;
    
    for(int i=1;i<all_inputs.size();i++)    
        write<<all_inputs[i].getId()<<endl;
    
    for(int i=0;i<all_outputs.size();i++)    
        write<<all_outputs[i].getId()+((int)all_outputs[i].getInputPolarity())<<endl;

    //deltas
    for(int i=1;i<all_ANDS.size();i++)
    {
        unsigned int first;
        write<<(all_ANDS[i].getId())<<" ";
        write<<(all_ANDS[i].getInputs()[0]->getId()+all_ANDS[i].getInputPolarities()[0])<<" "<<(all_ANDS[i].getInputs()[1]->getId()+all_ANDS[i].getInputPolarities()[1])<<endl;
    }
}


void AIGraph::propagateSignalsBatchPLA(binaryPLA* PLA){  
    int count=0,count2=0,j,constant0=0,constant1=0;
    all_outputs[0].setBitVector(0);
    bitset<BITS_PACKAGE_SIZE> bits;
    unsigned long long int mask=1,aux;
    vector <int> counter_right_answers_ands(this->all_ANDS.size(),0);
    vector <int> constant_counter0(this->all_ANDS.size(),0);
    vector <int> constant_counter1(this->all_ANDS.size(),0);
    vector<AND>::iterator it_and;        vector<output>::iterator it_out;       vector<input>::iterator it_in;
    //PLA combinations must be mutiple of 64 (bits_package_size)
//    cout<<"pla batch true size:"<<PLA->getBatch()->size()<<endl;
    for(int i=0;i<PLA->getBatchSize();i+=BITS_PACKAGE_SIZE)
    {
//        cout<<"i:"<<i<<", count:"<<count<<endl;
        //initializing all ANDs with -1
        
        for(it_and=all_ANDS.begin();it_and!=all_ANDS.end();it_and++)
            it_and->setSignal(-1);
        for(it_out=all_outputs.begin();it_out!=all_outputs.end();it_out++)
            it_out->setSignal(-1);
        //setting signals on primary inputs
        it_in=all_inputs.begin()+1;
        int bitvec_count=0;
//        for(int x=0;x<PLA->getBatch()->at(0).size();x++)
//            cout<<PLA->getBatch()->at(0).at(x);cout<<" getBatch"<<endl;
//        cout<<"-----BatchSize():"<<PLA->getBatch()->size()<<", PI size:"<<PLA->getPiSize()<<endl;
        
        //set all Primary Inputs with chunks of 64 (long long int) combinations from the PLA
//        PLA->printPLA();
        while(bitvec_count<PLA->getPiSize())
       {
            bits.reset();
            for(int u=0;u<BITS_PACKAGE_SIZE;u++)
            {//cout<<u<<","; 
                bits.set(u,PLA->getBatch()->at(u+i).at(bitvec_count));} 
            //cout<<endl;
//            cout<<PLA->getBatch()->at(u+i).at(bitvec_count);} cout<<"-";
//            cout<<bits.to_string()<<endl;
//            cout<<bits[0];
            it_in->setBitVector(bits.to_ullong());
            bitvec_count++;
            it_in++;
        } //cout<<endl;
        if(it_in!=all_inputs.end())
            cout<<"WRONG!!!!"<<endl;
//        cout<<bits.to_string()<<"->";

        
//#if checkAllNodesAsPO == 0
        //propagating signals through recursive DFS
        for(it_out=all_outputs.begin();it_out!=all_outputs.end();it_out++)
                it_out->PropagSignalDFS();
        //inverting negatted POs.
        if(all_outputs[0].getInputPolarity())
            all_outputs[0].setBitVector(~all_outputs[0].getBitVector());
        //Counting right answers in current chunk.
        bits.reset();   
        for(int u=i;u<i+BITS_PACKAGE_SIZE;u++)
            bits.set(u-i,(bool)PLA->getBatchAnswers()->at(u));
//        cout<<bits[u];} cout<<"<-right answers"<<endl;
        aux=~(all_outputs[0].getBitVector() ^ bits.to_ullong());
        while(aux){   count+= aux & 1;    aux >>= 1;  }
        if(all_outputs[0].getBitVector()==0)            constant0++;
        if((~all_outputs[0].getBitVector())==0)            constant1++;
        aux=all_outputs[0].getBitVector();
        while(aux){      count2+= aux & 1;         aux >>= 1;     }
    }

    //Counting functional ANDs and PIs
    this->functional_and_count=0,this->functional_PI_count=0;
    for(it_and=this->all_ANDS.begin();it_and!=all_ANDS.end();it_and++)
    {
        if(it_and->getSignal()!=-1) 
            {functional_and_count=functional_and_count+1;}
    }
    for(it_in=this->all_inputs.begin()+1;it_in!=all_inputs.end();it_in++)
            {if(it_in->wasVisited())functional_PI_count++;}
    //    cout<<all_outputs[0].getBitVector()<<endl;
//    cout<<"count:"<<count<<endl;
    if((constant0!= PLA->getNumCombinations()/BITS_PACKAGE_SIZE)&&(constant1!=PLA->getNumCombinations()/BITS_PACKAGE_SIZE))
        this->current_score=(float)count/PLA->getBatchSize();
//        cout<<current_score<<endl;
    else 
        this->current_score=0;
//    p1_cgp=(((float)count2)/PLA->getBatchSize());
//    current_score=current_score*(1-2*abs(PLA->getProbabilityOneData()-p1_cgp));
//#endif
    
}

void AIGraph::propagateAllAnds(binaryPLA* PLA){
      int count=0,count2=0,j,constant0=0,constant1=0;
    all_outputs[0].setBitVector(0);
    bitset<BITS_PACKAGE_SIZE> bits;
    unsigned long long int mask=1,aux;
    vector <int> counter_right_answers_ands(this->all_ANDS.size(),0);
    vector <int> constant_counter0(this->all_ANDS.size(),0);
    vector <int> constant_counter1(this->all_ANDS.size(),0);
    vector<AND>::iterator it_and;        vector<output>::iterator it_out;       vector<input>::iterator it_in;
    //PLA combinations must be mutiple of 64 (bits_package_size)
    for(int i=0;i<PLA->getBatchSize();i+=BITS_PACKAGE_SIZE)
    {
//        cout<<"i:"<<i<<", count:"<<count<<endl;
        //initializing all ANDs with -1
        
        for(it_and=all_ANDS.begin();it_and!=all_ANDS.end();it_and++)
            it_and->setSignal(-1);
        for(it_out=all_outputs.begin();it_out!=all_outputs.end();it_out++)
            it_out->setSignal(-1);
        
        //setting signals on primary inputs
        it_in=all_inputs.begin()+1;
        int bitvec_count=0;
//        for(int x=0;x<PLA->getBatch()->at(0).size();x++)
//            cout<<PLA->getBatch()->at(0).at(x);cout<<" getBatch"<<endl;
//        cout<<"-----BatchSize():"<<PLA->getBatch()->size()<<", PI size:"<<PLA->getPiSize()<<endl;
        
        //set all Primary Inputs with chunks of 64 (long long int) combinations from the PLA
        while(bitvec_count<PLA->getPiSize())
       {
            bits.reset();
            for(int u=0;u<BITS_PACKAGE_SIZE;u++)
                bits.set(u,PLA->getBatch()->at(u+i).at(bitvec_count));
//            cout<<PLA->getBatch()->at(u+i).at(bitvec_count);} cout<<"-";
//            cout<<bits.to_string()<<endl;
//            cout<<bits[0];
            it_in->setBitVector(bits.to_ullong());
            bitvec_count++;
            it_in++;
        } //cout<<endl;
        if(it_in!=all_inputs.end())
            cout<<"WRONG!!!!"<<endl;
//        cout<<bits.to_string()<<"->";

       
        //Counting right answers in current chunk.
        bits.reset();   
        for(int u=i;u<i+BITS_PACKAGE_SIZE;u++)
            bits.set(u-i,(bool)PLA->getBatchAnswers()->at(u));
        //with right chunk's right answers, propagate signals and count for all ANDs if they were a PO
        for(int and_index=1;and_index<all_ANDS.size();and_index++)
        {
//            aux=~(all_ANDS[and_index].getBitVector() ^ bits.to_ullong());
            aux=~(all_ANDS[and_index].PropagSignalDFS() ^ bits.to_ullong());
            while(aux)
            {   counter_right_answers_ands[and_index]+= aux & 1;    aux >>= 1;}
            
            //tracking constant nodes
            if(all_ANDS[and_index].getBitVector()==0)            
                constant_counter0[and_index]++;
            if((~all_ANDS[and_index].getBitVector())==0)            
                constant_counter1[and_index]++;
//            cout<<"and_index:"<<and_index<<"->"<<counter_right_answers_ands[and_index]<<"|";
            //TODO: p1Cgp each AND
//            aux=all_outputs[0].getBitVector();
//            while(aux){      count2+= aux & 1;         aux >>= 1;     }
        }
//#endif
    }
//#if checkAllNodesAsPO == 1
        vector<int> ordering_indexes(counter_right_answers_ands.size());
        float first=0,last=0;
        int x=0; iota(ordering_indexes.begin(),ordering_indexes.end(),x++);
        sort(ordering_indexes.begin()+1,ordering_indexes.end(),[&](int i, int j)
        {   return counter_right_answers_ands[i]>counter_right_answers_ands[j]; });
//        cout<<"All ands scores in order:";
//        for(int and_index=1;and_index<all_ANDS.size();and_index++)
//            cout<<all_ANDS[ordering_indexes[and_index]].getId()<<"->"<<counter_right_answers_ands[ordering_indexes[and_index]]<<"|"; cout<<endl;
        
        first=(float)counter_right_answers_ands[ordering_indexes.at(1)]/PLA->getBatchSize();
        last=1-(float)counter_right_answers_ands[ordering_indexes.back()]/PLA->getBatchSize();
//        cout<<"FIRST:"<<all_ANDS[ordering_indexes.at(1)].getId()<<","<<first<<", LAST:"<<all_ANDS[ordering_indexes.back()].getId()<<","<<last<<endl;
        if(last>first)
        {
            //checking if solution is constant.
            if((constant_counter0[ordering_indexes.back()]!= PLA->getNumCombinations()/BITS_PACKAGE_SIZE)&&(constant_counter1[ordering_indexes.back()]!=PLA->getNumCombinations()/BITS_PACKAGE_SIZE))
                this->current_score=first;
            else 
                this->current_score=0;
            current_score=last;
//            cout<<"setting AND:"<<all_ANDS[ordering_indexes.back()].getId()<<" as PO, NEGATED. ACC:"<<current_score<<endl;
            this->all_outputs[0].pushInput(&all_ANDS[ordering_indexes.back()],true);
        }
        else
        {
            //checking if solution is constant.
            if((constant_counter0[ordering_indexes.at(1)]!= PLA->getNumCombinations()/BITS_PACKAGE_SIZE)&&(constant_counter1[ordering_indexes.at(1)]!=PLA->getNumCombinations()/BITS_PACKAGE_SIZE))
                this->current_score=first;
            else 
                this->current_score=0;
//            cout<<"setting AND:"<<all_ANDS[ordering_indexes.at(1)].getId()<<" as PO, DIRECT. ACC:"<<current_score<<endl;
            this->all_outputs[0].pushInput(&all_ANDS[ordering_indexes.at(1)],false);
        }
        
    //Counting functional ANDs and PIs after new PO set
    for(it_and=all_ANDS.begin();it_and!=all_ANDS.end();it_and++)
        it_and->setSignal(-1);
    for(it_out=all_outputs.begin();it_out!=all_outputs.end();it_out++)
        it_out->setSignal(-1);
    for(it_in=all_inputs.begin();it_in!=all_inputs.end();it_in++)
        it_in->setUnvisited();
    this->all_outputs[0].PropagSignalDFS();
        
    this->functional_and_count=0,this->functional_PI_count=0;
    for(it_and=this->all_ANDS.begin();it_and!=all_ANDS.end();it_and++)
    {
        if(it_and->getSignal()!=-1) 
            {functional_and_count=functional_and_count+1;}
    }
    
    for(it_in=this->all_inputs.begin()+1;it_in!=all_inputs.end();it_in++)
    {
        if(it_in->wasVisited())
            functional_PI_count++; 
    }
#if writePIdebug ==1
    ofstream piDebug("piDebug.txt",ios::app);
    for(it_in=this->all_inputs.begin()+1;it_in!=all_inputs.end();it_in++)
            piDebug<<it_in->getVisitCount()<<",";
    piDebug<<endl;    piDebug.close();
#endif
        
}

float AIGraph::getCurrentScore(){
    return this->current_score;
}


nodeAig* AIGraph::findAny(unsigned int param) {
//    cout<<endl<<"looking for:"<<param<<endl;
//    cout<<"(this->all_inputs.size()+1)*2):"<<((this->all_inputs.size()+1)*2)<<endl;
    if(param<(this->all_inputs.size()*2))
    {
//        cout<<"got1:"<<all_inputs[param/2].getId()<<endl;
//        all_inputs[param/2].printNode();
        return &all_inputs[param/2];}
    else
    {
//         cout<<"got2:"<<all_ANDS[(param/2)-(all_inputs.size()-1)].getId()<<endl;
        return &all_ANDS[param/2-(all_inputs.size()-1)];}
}

//float AIGraph::getP1Cgp(){
//    return this->p1_cgp;
//}

int AIGraph::getRealSize(){
    return this->functional_and_count;
}

int AIGraph::getRealPIsize(){
    return this->functional_PI_count;
}

//void AIGraph::copyP1Cgp(float param){
//    this->p1_cgp=param;
//}

//void AIGraph::setDepthsInToOut(){
//    cout<<"Setting AIG depths, NODE TO -->PI<--"<<endl;
//    int retval,depth=0;
//    vector<output>::iterator it_out;
//    vector<AND>::iterator it_and;
//    vector<input>::iterator it_in;
//  
//    //Initializing depths
//    for(it_in=all_inputs.begin();it_in!=all_inputs.end();it_in++)
//        it_in->setDepth(-1);
//    for(it_and=all_ANDS.begin();it_and!=all_ANDS.end();it_and++)
//        it_and->setDepth(-1);
//    for(it_out=all_outputs.begin();it_out!=all_outputs.end();it_out++)
//        it_out->setDepth(-1);
//    for(it_out=all_outputs.begin();it_out!=all_outputs.end();it_out++)
//    {
//#if DEBUG >= 3
//    ofstream write;
//    write.open("log2.txt",ios::app);
//    write<<"OUTPUT BFS:"<<it_out->first<<endl;
//#endif
//        depth=it_out->computeDepthInToOut();
//        it_out->setDepth(depth);
//    }
//    int greater=-1;
//    for(it_out=all_outputs.begin();it_out!=all_outputs.end();it_out++)
//    {
//        if(it_out->getDepth()>greater)
//            greater=it_out->getDepth();
//    }
//    all_depths.clear();
//    this->graph_depth=greater;
////    this->all_depths.push_back(0);
////    for(it_in=all_inputs.begin();it_in!=all_inputs.end();it_in++)
////        all_depths.push_back(0);
//    for(it_and=all_ANDS.begin();it_and!=all_ANDS.end();it_and++)
//        {all_depths.push_back(it_and->getDepth());cout<<it_and->getDepth()<<",";} cout<<endl;
//        cout<<"ALL DEPTHS:"<<endl;
//     for(int y=1;y<all_depths.size();y++)
//        {cout<<all_depths[y]<<",";} cout<<endl;
////    
////    for(it_and=all_ANDS.begin();it_and!=all_ANDS.end();it_and++)
////        all_depths.push_back(0);
////    for(int y=0;y<all_ANDS.size();y++)
////        {all_depths[y](it_and->getDepth());cout<<it_and->getDepth()<<",";} cout<<endl;
//#if DEBUG >= 3
//    write.open("Depths.txt");
//    write<<this->name<<","<<greater<<endl;
//    for(it_and=all_ANDS.begin();it_and!=all_ANDS.end();it_and++)
//        write<<it_and->getId()<<":"<<it_and->getDepth()<<endl;
//    write.close();
//#endif
//    cout<<"setting depths done."<<endl;
//}

vector<AND>* AIGraph::getAnds(){
    return &all_ANDS;
}

vector<output>* AIGraph::getPOs(){
    return &all_outputs;
}

vector<input>* AIGraph::getPIs(){
    return &all_inputs;
}

vector<unsigned int>* AIGraph::getAllDepths(){
    return &this->all_depths;
}

#if MULTI_FUNC == 1
void AIGraph::mapXorToAnd(string destination, string aig_name){
    int num_xors=0,and_index=this->all_inputs.size()+all_ANDS.size()-2,extra_index=0;
    AND* AND_ptr0; AND* AND_ptr1;   
//    vector<AND> extra_ands;
    for(int i=1;i<this->all_ANDS.size();i++)
    {
        if(all_ANDS[i].getIsXOR())
            num_xors++;
    }
    extra_ands.resize((num_xors*2)+1);
//    cout<<"extra nodes size:"<<extra_ands.size()<<endl;
//    this->printCircuit();
    for(int i=1;i<this->all_ANDS.size();i++)
    {
        if(all_ANDS[i].getIsXOR())
        {
            
//            cout<<"AND, "<<all_ANDS[i].getId()<<" is a XOR."<<endl;
//            all_ANDS[i].printNode();
            vector<nodeAig*> aux_fanins;
            aux_fanins=all_ANDS[i].getInputs();
            
            extra_index++; and_index++;    AND AND_obj0(and_index*2);
            extra_ands[extra_index]=AND_obj0;
            AND_ptr0=&extra_ands[extra_index];
            AND_ptr0->pushInput(aux_fanins[0],(false^all_ANDS[i].getInputPolarities()[0]));
            AND_ptr0->pushInput(aux_fanins[1],(false^all_ANDS[i].getInputPolarities()[1]));
            
            extra_index++; and_index++;    AND AND_obj1(and_index*2);
            extra_ands[extra_index]=AND_obj1;
            AND_ptr1=&extra_ands[extra_index];
            AND_ptr1->pushInput(aux_fanins[0],(true^all_ANDS[i].getInputPolarities()[0]));
            AND_ptr1->pushInput(aux_fanins[1],(true^all_ANDS[i].getInputPolarities()[1]));
            
//            cout<<"and_index:"<<and_index<<", id:"<<and_index*2<<endl;
//            cout<<"-new0 "; AND_ptr0->printNode();
//            cout<<"-new1 "; AND_ptr1->printNode();
            
            all_ANDS[i].clearInputs();
            all_ANDS[i].pushInput(AND_ptr0,true);
            all_ANDS[i].pushInput(AND_ptr1,true);
//            all_ANDS[i].printNode();
//            cout<<endl;
        }
    }
//    cout<<"====renumbering!!"<<endl;
    //RENUMBERING!!!!
//    queue<nodeAig*> aux_queue; 
    stack<nodeAig*> my_stack,aux_stack;
//    set<nodeAig*> duplic_check;
    nodeAig* current;
    
    for(int i=1;i<this->all_ANDS.size();i++)
        all_ANDS[i].setSignal(-1);
    for(int i=1;i<this->extra_ands.size();i++)
        extra_ands[i].setSignal(-1);
    for(int i=1;i<this->all_inputs.size();i++)
        all_inputs[i].setSignal(1);
    int new_id=all_inputs.size()*2;
    my_stack.push(all_outputs[0].getInput()); 
//    cout<<"===PO:";
//    all_outputs[0].getInput()->printNode();
//    aux_stack.push(all_outputs[0].getInput());   
//    aux_queue.push(all_outputs[0].getInput());
//    duplic_check.insert(all_outputs[0].getInput());
    while (!my_stack.empty())
    {
        
            current=my_stack.top();
//            cout<<"current:";
//            current->printNode();
            if(current->getInputs()[0]->getSignal()!=-1 && current->getInputs()[1]->getSignal()!=-1)
            {
                current->setId(new_id);
                new_id+=2;
                current->setSignal(1);
                aux_stack.push(current);
                my_stack.pop();
            }
            else if (current->getInputs()[0]->getSignal()==-1)
                my_stack.push(current->getInputs()[0]);
            else if (current->getInputs()[1]->getSignal()==-1)
                my_stack.push(current->getInputs()[1]);
    }

//    cout<<endl<<endl;
//    this->printCircuit
//    int new_id=(all_inputs.size()+aux_queue.size()-1)*2;
//    cout<<"new id:"<<new_id<<endl;
    vector<nodeAig*> my_vec(aux_stack.size()+1);
    for(int u=1;u<my_vec.size();u++)
    {
//        cout<<aux_queue.front()->getId()<<",";
//        cout<<aux_stack.top()->getId()<<",";
//        aux_stack.top()->printNode();
        my_vec[u]=aux_stack.top();
        aux_stack.pop();
//        new_id-=2;
    }
//    cout<<endl<<"my_vec:";
//    for(int u=1;u<my_vec.size();u++)
//        my_vec[u]->printNode();//<<","; 
//    cout<<endl;
//    this->printCircuit();
    
    //WRITING OUTPUT FILE!
    ofstream write;
     int M=all_inputs.size()+my_vec.size()-2;
    write.open((destination+aig_name).c_str(),ios::binary|ios::out|ios::trunc);
//    write.open("../../AIGs/test.aig",ios::binary|ios::out|ios::trunc);
//    cout<<"aig "<<M<<" "<<all_inputs.size()-1<<" 0 "<<all_outputs.size()<<" "<<my_vec.size()-1<<endl;
    write<<"aig "<<M<<" "<<all_inputs.size()-1<<" 0 "<<all_outputs.size()<<" "<<my_vec.size()-1<<endl;
    
    for(int i=0;i<all_outputs.size();i++)    
        write<<all_outputs[i].getInput()->getId()+((int)all_outputs[i].getInputPolarity())<<endl;
    //deltas 
    int counter=0;
    unsigned int first;
    for(int i=my_vec.size()-1;i>=1;i--)
     {
//        my_vec[i]->printNode();
//         if(my_vec.back()->getId()!=my_vec[i]->getId() && (my_vec[i]->getId()>=my_vec[i+1]->getId()))
//             cout<<"my_vec[i]->getId()>=my_vec[i+1]->getId():TRUE!! "<<my_vec[i]->getId()<<"<"<<my_vec[i+1]->getId()<<endl;


         if(my_vec[i]->getId()>1)
         {
             if(my_vec[i]->getId()<my_vec[i]->getInputs()[0]->getId())
             {  cout<<"THIS IS WRONG:"<<my_vec[i]->getId()<<"<"<<my_vec[i]->getInputs()[0]->getId()<<endl;}   // my_vec[i]->printNode();}
            if(my_vec[i]->getId()<my_vec[i]->getInputs()[1]->getId())
            {  cout<<"THIS IS WRONG:"<<my_vec[i]->getId()<<"<"<<my_vec[i]->getInputs()[1]->getId()<<endl;}   // my_vec[i]->printNode();}
             counter++;
             encodeToFile(write,(my_vec[i]->getId())-(my_vec[i]->getInputs()[0]->getId()+(my_vec[i]->getInputPolarities()[0])));
             first=(my_vec[i]->getInputs()[0]->getId()+(my_vec[i]->getInputPolarities()[0]));
             encodeToFile(write,(first)-(my_vec[i]->getInputs()[1]->getId()+my_vec[i]->getInputPolarities()[1]));
         }
     }
}
#endif