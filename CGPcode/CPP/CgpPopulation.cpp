/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CGPgraph_cgp.cpp
 * Author: user
 * 
 * Created on April 3, 2020, 3:44 PM
 */

#include "CGP.h"


void CgpPopulation::createNodes(string name_param,unsigned int num_cgps_param, unsigned int PIs_param,unsigned int num_columns, unsigned int num_lines,mt19937& mt){
    this->name=name_param;
    int  column1=0,column2=0,line2=0, line1=0,random=0,limit=num_lines+1;
    bool invert1=true,invert2=true;
    this->PIs=PIs_param;
    this->num_columns=num_columns;
    this->num_lines=num_lines;
#if old_selection ==0
    num_cgps_param=1;
#endif
    this->num_cgps=num_cgps_param;
#if MULTI_FUNC == 1
    int tuple_size=7,xor_count=0;
#else
    int tuple_size=6;
#endif
    vector<short int> aux((tuple_size*num_columns*num_lines)+4);
    for(int a=0;a<num_cgps_param;a++)
    {
        this->the_CGPpopulation.push_back(aux);
        this->all_scores_cgp.push_back(0.0);
        this->ordered_indexes_cgp.push_back(0);
        this->all_func_ands_cgp.push_back(0);
        this->all_func_pis_cgp.push_back(0);
    }    
    for(int ith=0;ith<num_cgps_param;ith++)
    {
        int j=1,i=0;
        for(int pos=0;pos<(num_columns*num_lines*tuple_size);pos+=tuple_size)
        {
//            cout<<"pos:"<<pos<<endl;;;
//                cout<<"J:"<<j<<",";
            uniform_real_distribution<double> dist(0,100);
            uniform_real_distribution<double> dist_j(0,j+PIs_param-1);
            
            invert1=true; invert2=true;
            column1=dist_j(mt);
            if(column1>=PIs_param) //its an AND
                {column1-=PIs_param;  line1=0;}
            else 
                {line1=column1; column1=0;}
            random=dist(mt);    if(random>=50)     invert1=false;
            column2=dist_j(mt);   
            if(column2>=PIs_param) //its an AND
                {column2-=PIs_param;  line2=0;}
            else
                {line2=column2; column2=0;}
//                cout<<"rand:"<<column2<<",";
            random=dist(mt);    if(random>=50)      invert2=false;
            if(column1==j || column2==j)
            {   cout<<endl<<"WRONG!!!->";        cout<<column2<<"!="<<j<<"  ";cout<<column1<<"!="<<j<<"  "<<endl;}
#if DEBUG >=4
            cout<<"line1:"<<line1<<", column1:"<<column1<<", inv1:"<<invert1<<", line2:"<<line2<<", column2:"<<column2<<", inv2:"<<invert2<<endl;
#endif
            this->the_CGPpopulation[ith][pos]=line1;
            this->the_CGPpopulation[ith][pos+1]=column1;
            this->the_CGPpopulation[ith][pos+2]=invert1;
            this->the_CGPpopulation[ith][pos+3]=line2;
            this->the_CGPpopulation[ith][pos+4]=column2;
            this->the_CGPpopulation[ith][pos+5]=invert2;
#if MULTI_FUNC == 1
            int function=0;
            uniform_int_distribution<int> dist_func(1,3);
            this->the_CGPpopulation[ith][pos+6]=dist_func(mt);
            if(the_CGPpopulation[ith][pos+6]==3)
                xor_count++;
#endif
            i++;
            if(i==num_lines)
            {   j++;    i=0;}
//                cout<<"j:"<<j<<endl;
        }
        uniform_real_distribution<double> dist_col(1,num_columns); //avoid PI begin a PO (never col 0).
        uniform_real_distribution<double> dist_pol(0,2);
#if MULTI_FUNC == 1
        this->the_CGPpopulation[0][this->the_CGPpopulation[0].size()-4]=xor_count;
#endif
    this->the_CGPpopulation[0][this->the_CGPpopulation[0].size()-3]=0; //i
    this->the_CGPpopulation[0][this->the_CGPpopulation[0].size()-2]=dist_col(mt); //j
    this->the_CGPpopulation[0][this->the_CGPpopulation[0].size()-1]=dist_pol(mt); //inv
    }
//    this->printGraph();
}

void  CgpPopulation::initializeWithDecisionTree(string exemplar_name,string path_to_aig,binaryPLA* PLA,AigPopulation* aig_pop,mt19937& mt){
    this->name=exemplar_name;
    AIGraph* aig_obj;
    aig_pop->readOneAig(path_to_aig,PLA->getName()+".aig",mt);
//    aig_pop->readOneAig("reduced_benchmarks_aig/",PLA->getName()+".train.aig",mt);
//    aig_pop->readOneAig("sop_aig_files/",PLA->getName()+".sop.aig");
//    aig_pop->readOneAig("original_train_aig/",PLA->getName()+".train.aig");
    aig_obj=aig_pop->getAigFromPopulation(0);
#if MULTI_FUNC == 1
    int tuple_size=7;
#else
    int tuple_size=6;
#endif
    int  column1=0,column2=0,line2=1, line1=1;
    bool invert1=true,invert2=true;
    num_columns=aig_obj->getAnds()->size()-1; num_lines=1;
    this->num_cgps=1;
    vector<short int> aux((tuple_size*num_columns*num_lines)+3);
    this->the_CGPpopulation.push_back(aux);
    this->all_scores_cgp.push_back(0.0);
    this->ordered_indexes_cgp.push_back(0);
    this->all_func_ands_cgp.push_back(aig_obj->getAnds()->size()-1);
    this->all_func_pis_cgp.push_back(0);
    this->PIs=PLA->getPiSize();
    
    for(int a=1,pos=0;a<aig_obj->getAnds()->size();a++,pos+=tuple_size)
    {
        line1=line2=0;
        if(((aig_obj->getAnds()->at(a).getInputs()[0]->getId()/2)-1)<aig_obj->getPIs()->size()-1)
        {
            line1=(aig_obj->getAnds()->at(a).getInputs()[0]->getId()/2)-1;
            column1=0;
        }
        else
            column1=aig_obj->getAnds()->at(a).getInputs()[0]->getId()/2-(aig_obj->getPIs()->size()-1);//aig_obj->getAnds()->at(a).getInputs()[0]->getDepth()+pos/6;
        if(((aig_obj->getAnds()->at(a).getInputs()[1]->getId()/2)-1)<aig_obj->getPIs()->size()-1)
        {
            line2=(aig_obj->getAnds()->at(a).getInputs()[1]->getId()/2)-1;
            column2=0;
        }
        else   
            column2=aig_obj->getAnds()->at(a).getInputs()[1]->getId()/2-(aig_obj->getPIs()->size()-1);//aig_obj->getAnds()->at(a).getInputs()[1]->getDepth()+pos/6;
        
        invert1=aig_obj->getAnds()->at(a).getInputPolarities()[0];
        invert2=aig_obj->getAnds()->at(a).getInputPolarities()[1];
//        aig_obj->getAnds()->at(a).printNode();
//            cout<<"pos:"<<pos/6<<",depth1:"<<aig_obj->getAnds()->at(a).getInputs()[0]->getDepth()<<"depth2:"<<aig_obj->getAnds()->at(a).getInputs()[1]->getDepth()<<",line1:"<<line1<<", column1:"<<column1<<", inv1:"<<invert1<<", line2:"<<line2<<", column2:"<<column2<<", inv2:"<<invert2<<endl;
        this->the_CGPpopulation[0][pos]=line1;
        this->the_CGPpopulation[0][pos+1]=column1;
        this->the_CGPpopulation[0][pos+2]=invert1;
        this->the_CGPpopulation[0][pos+3]=line2;
        this->the_CGPpopulation[0][pos+4]=column2;
        this->the_CGPpopulation[0][pos+5]=invert2;
#if MULTI_FUNC == 1
        this->the_CGPpopulation[0][pos+6]=1;
#endif
            //        cout<<(a*2)+(aig_obj->getPIs()->size()-1)*2<<"=="<<aig_obj->getAnds()->at(a).getId()<<endl;
    }
//    cout<<"PO:";
//    aig_obj->getPOs()->at(0).printNode();
//    aig_obj->getPOs()->at(0).getInput()->printNode();
    this->the_CGPpopulation[0][this->the_CGPpopulation[0].size()-3]=0; //i
    this->the_CGPpopulation[0][this->the_CGPpopulation[0].size()-2]=aig_obj->getPOs()->at(0).getInput()->getId()/2-(aig_obj->getPIs()->size()-1); //j
    this->the_CGPpopulation[0][this->the_CGPpopulation[0].size()-1]=aig_obj->getPOs()->at(0).getInputPolarity(); //inv
//    this->printGraph();
    
//    cout<<"ands size:"<<aig_obj->getAnds()->size()-1<<endl;

}

float CgpPopulation::generateFathersOneToFive(AigPopulation* my_pop,double mutation_chance,double min_mut,binaryPLA* my_pla,int change_each,mt19937& mt){
    uniform_real_distribution<double> dist(0,100);
    uniform_int_distribution<int> dist_func(1,3);
    int  column1=0,column2=0,line2=0, line1=0,limit=num_lines+1, func=0;
    double random=0;
    bool invert1=true,invert2=true; float original_score=0;
    vector<vector<short int> > fatherhood_vector;
#if MULTI_FUNC == 1
    int tuple_size=7;
#else
    int tuple_size=6;
#endif
    vector<short int> aux((tuple_size*num_columns*num_lines)+4);
    //modifying batch
    if((my_pla->getBatchCounter()>=change_each) && my_pla->getBatchSize()<my_pla->getNumCombinations()){ //int new_batch_chance
//        cout<<"batch counter before change:"<<my_pla->getBatchCounter()<<endl;
//        cout<<"scores before new batch:"; my_pop->printScores();
        my_pla->setRandomBatch(mt);
        my_pla->setTrueNewBatch();
        my_pop->evaluateScoresMyImplement(my_pla,1,mt);
//        cout<<"batch counter after change:"<<my_pla->getBatchCounter()<<endl;
//        cout<<"scores with new batch:"; my_pop->printScores();
    }
    my_pla->operator ++();
    original_score=my_pop->getScores()->at(0);
//    cout<<"original score:"<<original_score<<",original scores size:"<<my_pop->getScores()->size()<<endl;
    for(int a=0;a<5;a++) 
        fatherhood_vector.push_back(aux);
    for(int i=0;i<5;i++)
        fatherhood_vector[i]=this->the_CGPpopulation[0];
//    cout<<"FIRST CGP COPIED FROM FATHER BEFORE MUTATION:": fatherhood_vector[0]
    //last CGP not modified, the father.
    
    for(int ith=0;ith<4;ith++)
    {
//        int col1NotZero=0,col2NotZero=0,total_inst=0;
//        cout<<"num_columns*num_lines*tuple_size:"<<num_columns*num_lines*tuple_size<<endl;
        int fanin_mut=0,pol_mut=0,func_mut=0;
        int j=0,i=0;
        for(int pos=0;pos<num_columns*num_lines*tuple_size;pos+=tuple_size)
        {
            line1=-1;line2=-1;column1=-1;column2=-1;
            //chance to mutate one fanin
            random=dist(mt);
//            cout<<"RANDOM:"<<random<<",MUT:"<<mutation_chance;
            uniform_real_distribution<double> dist_j(0,j+PIs-1);
//            cout<<"PIS size:"<<PIs<<",J:"<<j<<",J+PI:"<<j+PIs-1<<endl;
            if(random>=100-mutation_chance)
            {
//                cout<<"rand:"<<random<<">="<<100-mutation_chance<<", mut chance:"<<mutation_chance<<endl;
#if COUT == 1
                fanin_mut++;
#endif
                column1=dist_j(mt); 
//                cout<<"RANDOM->before:"<<line1<<","<<column1<<".";
                if(column1>=PIs) //its an AND
                    {column1-=PIs;  line1=0;}
                else //its a PI
                {line1=column1; column1=0;}
//                cout<<",after:"<<line1<<","<<column1<<endl;
//                line1=dist_limit(mt);
//                cout<<"LineChange->"<<fatherhood_vector[ith][pos]<<" to "<<line1<<endl;
//                cout<<"LineChange->"<<fatherhood_vector[ith][pos+1]<<" to "<<column1<<endl;
                fatherhood_vector[ith][pos]=line1;
                fatherhood_vector[ith][pos+1]=column1;
            }
            random=dist(mt);
            //chance to mutate first fanin polarity
            if(random>=100-mutation_chance)
            {
#if COUT == 1
                pol_mut++;
#endif
                invert1=!(bool)fatherhood_vector[ith][pos+2];      
                fatherhood_vector[ith][pos+2]=invert1;
            }
            //second fanin mutation
            random=dist(mt);
            if(random>=100-mutation_chance)
            {
#if COUT == 1
                fanin_mut++;
#endif
                column2=dist_j(mt);     
//                if(column2==0) limit=PIs+1; else limit=num_lines; uniform_real_distribution<double> dist_limit2(0,limit);  
//                line2=dist_limit2(mt);
                if(column2>=PIs) //its an AND
                    {column2-=PIs;  line2=0;} //TODO: matrix version wont work with this
                else //its a PI, column=0, line=generated value
                {line2=column2; column2=0;}
                fatherhood_vector[ith][pos+3]=line2;
                fatherhood_vector[ith][pos+4]=column2;
            }
            random=dist(mt);
            if(random>=100-mutation_chance)
            {
#if COUT == 1
                pol_mut++;
#endif
                invert2=!(bool)fatherhood_vector[ith][pos+5];       
                fatherhood_vector[ith][pos+5]=invert2;
            }
#if MULTI_FUNC == 1
            random=dist(mt);
            if(random>=100-mutation_chance)
            {
#if COUT == 1
                func_mut++;
#endif
                func=dist_func(mt);
                while(func==fatherhood_vector[ith][pos+6])
                    func=dist_func(mt);
//                cout<<"-----------------------Old func:"<<fatherhood_vector[ith][pos+6];
                fatherhood_vector[ith][pos+6]=func;
//                cout<<"->"<<fatherhood_vector[ith][pos+6]<<endl;
            }
#endif
            
//            if(column1>0)col1NotZero++; if(column2>0)col2NotZero++;
//            total_inst++;
            
//            cout<<"line1:"<<line1<<", column1:"<<column1<<", inv1:"<<invert1<<", line2:"<<line2<<", column2:"<<column2<<", inv2:"<<invert2<<endl;
            i++;
            if(i==num_lines)
            {   j++;    i=0;}
        }
#if mutable_PO == 1
        random=dist(mt);
//        cout<<"---RANDOM:"<<random<<endl;
        if(random>=100-mutation_chance)
        {
#if DEBUG >=1
            cout<<"---------------altering PO, from:("<<fatherhood_vector[ith][fatherhood_vector[ith].size()-3]<<","<<fatherhood_vector[ith][fatherhood_vector[ith].size()-2]<<"), to:(";
#endif
            uniform_real_distribution<double> dist_line(0,num_lines); //no restriction for the line.
            uniform_real_distribution<double> dist_col(1,num_columns); //avoid PI begin a PO (never col 0).
            fatherhood_vector[ith][fatherhood_vector[ith].size()-3]=dist_line(mt);
            fatherhood_vector[ith][fatherhood_vector[ith].size()-2]=dist_col(mt);
//            cout<<fatherhood_vector[ith][fatherhood_vector[ith].size()-3]<<","<<fatherhood_vector[ith][fatherhood_vector[ith].size()-2]<<")"<<endl;
        }
        random=dist(mt);
//        cout<<"---RANDOM:"<<random<<endl;
        if(random>=100-mutation_chance)
        {
#if DEBUG >=1
            cout<<"------------altering PO polarity, from:"<<fatherhood_vector[ith][fatherhood_vector[ith].size()-1]<<", to:";
#endif            
            invert1=!(bool)fatherhood_vector[ith][fatherhood_vector[ith].size()-1];
            fatherhood_vector[ith][fatherhood_vector[ith].size()-1]=invert1;
//            cout<<fatherhood_vector[ith][fatherhood_vector[ith].size()-1]<<endl;
        }
#endif
        
#if COUT == 1
        cout<<"CGP:"<<ith<<", mutations-> fanins:"<<fanin_mut<<", polarities:"<<pol_mut<<", functions:"<<func_mut<<endl;
//        cout<<"COL1 NOT ZERO (is AND):"<<(float)col1NotZero<<endl;///fanin_mut<<endl;
//        cout<<"COL2 NOT ZERO (is AND):"<<(float)col2NotZero<<endl;///fanin_mut<<endl;
#endif
    }
    
    //Instantiating CGP with matrix of tuples
//    for(int i=num_cgps;i<num_cgps+number_childs;i++)
//        fatherhood_vector[i]=this->the_CGPpopulation[my_pop->getOderedIndexes()->at(i-num_cgps)];
    CgpPopulation fatherhood_cgp(fatherhood_vector,this->PIs,this->num_columns,this->num_lines);
//    for(int x=0;x<5;x++)
//        fatherhood_cgp.printGraph();
    
    AigPopulation fatherhood_aig;
    fatherhood_aig.CGPpopToAIGpop(&fatherhood_cgp,PIs,1,num_columns*num_lines,true);
//    cout<<"fatherhood aig PI real size:"<<(fatherhood_aig.getAigFromPopulation(0)->getPIs()->size()-1)<<endl;

#if write_times >= 1
        ofstream function_times("function_times.csv",ios::app);
        auto begin = std::chrono::high_resolution_clock::now();
#endif
//    for(int a=0;a<fatherhood_aig.getScores()->size();a++)
//        {cout<<a<<":"; fatherhood_aig.getAigFromPopulation(a)->printCircuit(); cout<<endl;}
#if Sannealing == 1
        fatherhood_aig.setTemp(my_pop->getTemp());  cout<<"1"<<endl;
#endif
            fatherhood_aig.evaluateScoresMyImplement(my_pla,5,mt);
#if Sannealing == 1
        my_pop->setTemp(fatherhood_aig.getTemp());  cout<<"2"<<endl;
#endif 
#if COUT == 1
        cout<<"scores inside generateFathers:"; fatherhood_aig.printScores();
#endif

#if write_times >=1
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - begin;    function_times<<elapsed.count()<<"evaluate,";    function_times.close();
#endif
        this->all_scores_cgp[0]=fatherhood_aig.getScores()->at(fatherhood_aig.getOrderedIndexes()->at(0));
        this->ordered_indexes_cgp[0]=0;
        this->the_CGPpopulation[0]=fatherhood_vector[fatherhood_aig.getOrderedIndexes()->at(0)];  
        this->all_func_ands_cgp[0]=fatherhood_aig.getFuncSizes()->at(fatherhood_aig.getOrderedIndexes()->at(0));
        this->all_func_pis_cgp[0]=fatherhood_aig.getFuncPis()->at(fatherhood_aig.getOrderedIndexes()->at(0));
#if checkAllNodesAsPO == 1
        //updating CGP PO from AIG after evaluation
        int best_index=fatherhood_aig.getOrderedIndexes()->at(0);
        this->the_CGPpopulation[0][the_CGPpopulation[0].size()-3]=0;
        this->the_CGPpopulation[0][the_CGPpopulation[0].size()-2]=(fatherhood_aig.getAigFromPopulation(best_index)->getPOs()->at(0).getId()/2)-(fatherhood_aig.getAigFromPopulation(best_index)->getPIs()->size()-1);
        this->the_CGPpopulation[0][the_CGPpopulation[0].size()-1]=fatherhood_aig.getAigFromPopulation(best_index)->getPOs()->at(0).getInputPolarity();
//        cout<<"---------------PO CGP is:("<<the_CGPpopulation[0][the_CGPpopulation[0].size()-3]<<","<<the_CGPpopulation[0][the_CGPpopulation[0].size()-2]<<","<<the_CGPpopulation[0][the_CGPpopulation[0].size()-1]<<")"<<endl;
//        cout<<"---------------PO AIG is:"<<fatherhood_aig.getAigFromPopulation(fatherhood_aig.getOderedIndexes()->at(0))->getPOs()->at(0).getId()<<endl;
//        cout<<"---------------num PIs:"<<fatherhood_aig.getAigFromPopulation(fatherhood_aig.getOderedIndexes()->at(0))->getPIs()->size()-1<<endl;

#endif
//        cout<<"Best CGP after mutation:"; this->printGraph();


#if my_mutation == 1        
        if(this->all_scores_cgp[0]==original_score)
            {original_score_chosen++;  child_chosen=0;}
        else
            {child_chosen++; original_score_chosen=0;}
//        cout<<"child count:"<<child_chosen<<","<<"original score count:"<<original_score_chosen<<endl;

        if(original_score_chosen==100)
            {original_score_chosen=0;   mutation_chance=mutation_chance-(mutation_chance*0.01);}
        
        if(child_chosen==3)
            {child_chosen=0;    mutation_chance=mutation_chance+(mutation_chance*0.1);}
#else
//        if(original_score==0)
//            cout<<"-------------------ORIGINAL SCORE 0!!!!!"<<endl;
//        cout<<"original score:"<<original_score<<",original score from vec:"<<fatherhood_aig.getScores()->at(4)<<endl;
        if(fatherhood_aig.getScores()->size()>5)
            cout<<"SOMETHING WRONG! fatherood size is greater than 5.-->"<<fatherhood_aig.getScores()->size()<<endl;
//        fatherhood_aig.printScores();;
#if Sannealing == 1
        if(fatherhood_aig.getScores()->at(fatherhood_aig.getOrderedIndexes()->at(0))>=fatherhood_aig.getScores()->at(fatherhood_aig.getOrderedIndexes()->at(1)))
#endif
            //cout<<"ORIGINAL SCORE:"<<fatherhood_aig.getScores()->at(4)<<endl;
            //fatherhood_aig.printScores();
        for(int y=0;y<fatherhood_aig.getScores()->size()-1;y++)
        {
            if(fatherhood_aig.getScores()->at(y)>=fatherhood_aig.getScores()->at(4))
                {mutation_chance=mutation_chance*(1.4);}// cout<<"+";}
            else
                {mutation_chance=mutation_chance*(pow(1.4,(-0.25)));}// cout<<"-";}
        }//cout<<endl;
            
            
//        {
//            if(this->all_scores_cgp[0]==original_score)
//                {original_score_chosen++;  child_chosen=0;}
//            else
//                {child_chosen++; original_score_chosen=0;}
//            if(original_score_chosen==1)
//                {original_score_chosen=0;   mutation_chance=mutation_chance*(pow(1.4,(-0.25)));}
//
//            if(child_chosen==1)
//                {child_chosen=0;    mutation_chance=mutation_chance*(1.05);}
//        }
#endif
        
        if(mutation_chance<min_mut)
            mutation_chance=min_mut;
        return mutation_chance;
}

CgpPopulation::CgpPopulation() {
}

CgpPopulation::CgpPopulation(vector<vector<short int> > param, unsigned int PIs,unsigned int num_columns, unsigned int num_lines){
    this->the_CGPpopulation=param;
    this->PIs=PIs;
    this->num_cgps=the_CGPpopulation.size();
    this->num_columns=num_columns;
    this->num_lines=num_lines;
    this->original_score_chosen=0;
    this->child_chosen=0;
}

CgpPopulation::CgpPopulation(const CgpPopulation& orig) {
}

CgpPopulation::~CgpPopulation() {
}

vector<short int>* CgpPopulation::getVector(int ith_cgp) {
    return &this->the_CGPpopulation[ith_cgp];
}

void CgpPopulation::clearCgp(){
    this->PIs=0;
    this->num_cgps=0;
    this->num_columns=0;
    this->num_lines=0;
    this->the_CGPpopulation.clear();
    this->all_scores_cgp.clear();
    this->all_func_ands_cgp.clear();
    this->ordered_indexes_cgp.clear();
    this->original_score_chosen=0;
    this->child_chosen=0;
}

void CgpPopulation::printGraph(){
    cout<<"Number of PIs:"<<this->PIs<<", last PI id:"<<(this->PIs*2)<<endl;
    cout<<", num_cgps:"<<this->num_cgps<<", num_lins:"<<this->num_lines<<", columns:"<<this->num_columns<<endl;
    int column_counter=0,tuple_size=0;
#if MULTI_FUNC == 0
    tuple_size=6;
    cout<<"|ID(i1,j1,inv1)(i2,j2,inv2)|"<<endl;
#else
    tuple_size=7;
    cout<<"|ID(i1,j1,inv1)(i2,j2,inv2),FUNC|"<<endl;
#endif
    for(int ith=0;ith<the_CGPpopulation.size();ith++)
    {   
        for(int i=0;i<the_CGPpopulation[ith].size()-4;i+=tuple_size)
        {
            if((i)%(this->num_columns*tuple_size)==0 && i>1)
                cout<<endl;//<<"--i:"<<i<<"--";
//            cout<<i;
            cout<<((PIs+((i+tuple_size)/tuple_size))*2)<<"("<<the_CGPpopulation[ith][i]<<","<<the_CGPpopulation[ith][i+1]<<",";
            cout<<the_CGPpopulation[ith][i+2]<<")("<<the_CGPpopulation[ith][i+3]<<",";
            cout<<the_CGPpopulation[ith][i+4]<<","<<the_CGPpopulation[ith][i+5]<<")";
#if MULTI_FUNC == 1
            cout<<","<<the_CGPpopulation[ith][i+6];
#endif
            cout<<"|"<<endl;
        }
        cout<<"CGP primary output (i,j,pol)->("<<the_CGPpopulation[0][the_CGPpopulation[0].size()-3]<<","<<the_CGPpopulation[0][the_CGPpopulation[0].size()-2];
        cout<<","<<the_CGPpopulation[0][the_CGPpopulation[0].size()-1]<<")";
        cout<<"#XORs:"<<the_CGPpopulation[0][the_CGPpopulation[0].size()-4];
        cout<<endl<<endl;        
    }    
}

int CgpPopulation::getNumCols() {
    return this->num_columns;
}

int CgpPopulation::getNumLines() {
    return this->num_lines;
}

int CgpPopulation::getPopulationSize(){
    return this->num_cgps;
//    return this->the_CGPpopulation.size();
}

void CgpPopulation::setVector(vector<vector<short int> > param){
    this->the_CGPpopulation=param;
}

vector<float> CgpPopulation::getScores(){
    return this->all_scores_cgp;
}

vector<int> CgpPopulation::getOrderedIndexes(){
    return this->ordered_indexes_cgp;
}

vector<int> CgpPopulation::getSizes(){
    return this->all_func_ands_cgp;
}

vector<int> CgpPopulation::getFuncPis(){
    return this->all_func_pis_cgp;
}

void CgpPopulation::setScores(vector<float>* param){
    this->all_scores_cgp=*param;
}

void CgpPopulation::setOrderedIndexes(vector<int>* param){
    this->ordered_indexes_cgp=*param;
}

void CgpPopulation::writeCgps(){
    ofstream cgp_out(this->name+".cgp.csv");
    for (int i=0;i<this->the_CGPpopulation[0].size();i++)
        cgp_out<<the_CGPpopulation[0][i]<<",";
}

string CgpPopulation::getName(){
    return name;
}