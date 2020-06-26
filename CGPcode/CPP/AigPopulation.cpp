/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <thread>

#include "AIG.h"

void AigPopulation::CGPpopToAIGpop(CgpPopulation* cgp_popul, unsigned int i, unsigned int o, unsigned int a,bool internal_call) {
    string namex;
//    cout<<"cgp_popul->getPopulationSize():"<<cgp_popul->getPopulationSize()<<endl;
//    this->mean_p1_cgps=cgp_popul->getMeanP1Cgps();
    for(int i=0;i<cgp_popul->getPopulationSize();i++)
    {
        this->aig_population.push_back(AIGraph());
        this->all_scores.push_back(0);
        this->num_functional_ands.push_back(0);
        this->num_functional_PIs.push_back(0);
//        this->ordered_indexes.push_back(0);
    }

    for(int ith=0;ith<cgp_popul->getPopulationSize();ith++)
    {
//        cout<<"ITH:"<<ith<<endl;
        namex=cgp_popul->getName();
//        namex="CGP"; namex=namex+to_string(ith);
//        AIGraph my_aig;
//        my_aig.CGPtoAIG(cgp_popul->getVector(ith),cgp_popul->getNumLines(),i,o,a,PO_id,PO_polarity,name);
        this->aig_population[ith].CGPtoAIG(cgp_popul->getVector(ith),cgp_popul->getNumLines(),i,o,a,namex);
//        this->
        //        this->aig_population[ith].copyP1Cgp(cgp_popul->getIndividualP1Cgp());;
    }
    if(!internal_call)
        this->all_scores=cgp_popul->getScores();
    if(!internal_call)
        this->ordered_indexes=cgp_popul->getOrderedIndexes();
    if(!internal_call)
        this->num_functional_ands=cgp_popul->getSizes();
    if(!internal_call)
    {this->num_functional_PIs=cgp_popul->getFuncPis();}//cout<<"SETING PIS SIZE INTERNAL CALL:"<<num_functional_PIs[0]<<endl;}
//    cout<<"all scores size:"<<all_scores.size()<<endl;
//    cout<<"ordered indexes size:"<<this->ordered_indexes.size()<<endl;
////    cout<<"Finished to create AIG population!!"<<endl;
}


//void AigPopulation::evaluateScorseAbcCommLine(string PLA_file,int num_to_evaluate){
//#if write_times >=2
//        ofstream function_times("function_times.csv",ios::app);
//        auto begin = std::chrono::high_resolution_clock::now();
//#endif
////    cout<<"PLA_FILE:"<<PLA_file<<endl;
//    int ANDs_size=0; float score=0; string line;
//    ifstream input_file;
//    
//#if write_learning >= 2
//    ofstream ABC_output;
//    ABC_output.open("ABC_output.csv",ios::app); ABC_output<<endl;
//    ABC_output<<"evaluation method called,name:"<<PLA_file<<", number to be avaliate:"<<num_to_evaluate<<endl;
//#endif
////    cout<<"AIG TO EVALUATE SIZE:"<<aig_population.size()<<endl;
//    if(num_to_evaluate==0)
//        num_to_evaluate=aig_population.size();
//    for(int ith=0;ith<num_to_evaluate;ith++)
//    {
////        this->aig_population[ith].writeAIG("AIGs/",this->aig_population[0]);
//    abcCallML(this->aig_population[ith].getName(),PLA_file,"AIGs/");
//        //reading ABC output
//        input_file.close();
//        input_file.open("log.txt");
//        
//        while(getline(input_file,line))
//        {
//            if(line.find("and = ")!=string::npos)
//            {
//                line.erase(0,line.find("and = ")+9);
//                line.erase(line.find("lev"),line.back());
//                ANDs_size=atoi(line.c_str());
//                all_sizes[ith]=ANDs_size;
//            }
//            if(line.find("Correct =")!=string::npos)
//            {
//                line.erase(0,line.find_first_of("(")+2);
//                line.erase(line.find_first_of("%")-1,line.size());
//                score=atof(line.c_str());
//                all_scores[ith]=score;
//            }
//        }
//    }
//    vector<int> indexes(all_scores.size());
//    int x=0; iota(indexes.begin(),indexes.end(),x++);
//    sort(indexes.begin(),indexes.end(),[&](int i, int j)
//    {
//        if(all_scores[i]==all_scores[j] && all_scores[i]>0 && all_scores[j]>0)
//            return all_sizes[i]>all_sizes[j];
//        else
//            return all_scores[i]>all_scores[j];
//    });
//    ordered_indexes=indexes;
//#if write_times >=2
//        auto finish = std::chrono::high_resolution_clock::now();
//        std::chrono::duration<double> elapsed = finish - begin;    function_times<<elapsed.count()<<"abc,";    function_times.close();
//#endif
//#if write_learning == 2
//    for(int ith=0;ith<all_scores.size();ith++)
//        ABC_output<<all_sizes[indexes[ith]]<<","<<all_scores[indexes[ith]]<<endl;
//    ABC_output.close();
//#endif
//}

void AigPopulation::evaluateScoresMyImplement(binaryPLA* PLA,int num_to_evaluate,mt19937& mt){
    
#if write_times >=2
        ofstream function_times("function_times.csv",ios::app);
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        
#if write_learning >= 2
    ofstream ABC_output;
    ABC_output.open("ABC_output.csv",ios::app); ABC_output<<endl;
    ABC_output<<"evaluation method called,name:"<<PLA->getName()<<", number to be avaliate:"<<num_to_evaluate<<endl;
#endif
    int ith;
//    cout<<"pop size:"<<this->aig_population.size()<<endl;
//    this->aig_population[0].printCircuit();
//    cout<<"funcSize size:"<<this->all_functional_sizes.size()<<endl;
    if(num_to_evaluate==0)
        num_to_evaluate=aig_population.size();
//    PLA->printPLA();
#pragma omp parallel//for //private(ith),shared(PLA)
    {
#pragma omp for
        for(ith=0;ith<num_to_evaluate;ith++)
        {
//            cout<<"-------------------------------------------------------------------------------------"<<endl;
//            cout<<"ITH:"<<ith<<endl;
#if checkAllNodesAsPO == 0
            this->aig_population[ith].propagateSignalsBatchPLA(PLA); 
#else
            this->aig_population[ith].propagateAllAnds(PLA);
#endif
            all_scores[ith]=aig_population[ith].getCurrentScore();
//            cout<<"check:"<<all_scores[ith]<<",";
            num_functional_ands[ith]=aig_population[ith].getRealSize();            
            num_functional_PIs[ith]=aig_population[ith].getRealPIsize();
//            cout<<ith<<",func PI size:"<<num_functional_PIs[ith]<<endl;
//            cout<<"check2:"<<all_functional_sizes[ith]<<endl;
        }
    }
//        for(int a=0;a<all_scores.size();a++)
//            cout<<all_scores[a]<<","; cout<<endl;
        uniform_real_distribution<float> dist(0,1);
    vector<int> ordering_indexes(all_scores.size());
    float scoreI,scoreJ,uni_random;
        int x=0; iota(ordering_indexes.begin(),ordering_indexes.end(),x++);
        sort(ordering_indexes.begin(),ordering_indexes.end(),[&](int i, int j)
        {
    #if Sannealing == 0
            if(all_scores[i]==all_scores[j] && all_scores[i]>0 && all_scores[j]>0)
//            {cout<<"returningANDs:"<<num_functional_ands[i]<<">"<<num_functional_ands[j]<<":"<<(num_functional_ands[i]<num_functional_ands[j])<<endl;
                return num_functional_ands[i]>num_functional_ands[j]; //}
            else
//                {cout<<"returningScore:"<<all_scores[i]<<">"<<all_scores[j]<<":"<<(all_scores[i]>all_scores[j])<<endl;
                 return all_scores[i]>all_scores[j];//}
    #else
        return (all_scores[i]*num_functional_ands[i]>all_scores[j]*num_functional_ands[j]);
    //            scoreI=(all_sizes[i]*all_scores[i]);
    //            scoreJ=(all_sizes[j]*all_scores[j]);
    //            if(scoreI>=scoreJ)
    //                return scoreI>scoreJ;
    //            else 
    //            {
    //                uni_random=dist(mt);
    //                cout<<"-delta:"<<(-(abs(scoreI-scoreJ)));
    //                cout<<",rand="<<uni_random<<",temp:"<<temperature<<",exp:"<<exp(-(abs(scoreI-scoreJ))/temperature)<<endl;
    //                if(exp(-abs(scoreI-scoreJ)/temperature)<uni_random)
    //                    {cout<<"!!!!!!!!!!!!BAD ACCEPTED!!!!!!!!!!!!!"<<endl; return scoreI<scoreJ;}
    //            }
    #endif
        });
    ordered_indexes=ordering_indexes;
#if Sannealing == 1
    uni_random=dist(mt);
    cout<<"random uniform:"<<uni_random<<"temperature/init:"<<(temperature/temp_init)<<endl;
    if(uni_random<temperature/temp_init)
    {
        uniform_int_distribution<int> dist5(0,ordered_indexes.size()-1);
        ordered_indexes[0]=ordering_indexes[dist5(mt)];
        cout<<"CHANGING FIRST!!"<<endl;
    }
    temperature-=(temperature*0.001);
    
#endif
#if write_times >=2
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - begin;    function_times<<elapsed.count()<<"my,";    function_times.close();
#endif
#if write_learning >= 2
    for(int ith=0;ith<all_scores.size();ith++)
//        ABC_output<<all_sizes[indexes[ith]]<<","<<all_scores[indexes[ith]]<<endl;;
        ABC_output<<ith<<":"<<all_scores[ordering_indexes[ith]]<<endl;
    ABC_output.close();
#endif
}


//void AigPopulation::gatherAndOrder(AigPopulation* previous_population){
//    int forward_index=previous_population->getScores()->size();
////    cout<<"forward_index:"<<forward_index<<endl;
//    for(int i=forward_index,j=0;i<this->all_scores.size();i++,j++)
//    {
//        this->all_scores[i]=previous_population->getScores()->at(previous_population->ordered_indexes.at(j));
//        this->all_sizes[i]=previous_population->getSizes()->at(previous_population->ordered_indexes.at(j));
//    }
//    vector<int> indexes(all_scores.size());
//    int x=0; iota(indexes.begin(),indexes.end(),x++);
//    sort(indexes.begin(),indexes.end(),[&](float i, float j){return all_scores[i]>all_scores[j];});
//    ordered_indexes=indexes;
//}
AIGraph* AigPopulation::getAigFromPopulation(int ith_aig){
    return &this->aig_population[ith_aig];
}

vector<float>* AigPopulation::getScores(){
    return &this->all_scores;
}
vector<int>* AigPopulation::getOrderedIndexes(){
    return &this->ordered_indexes;
}

vector<int>* AigPopulation::getFuncSizes(){
    return &this->num_functional_ands;
}

vector<int>* AigPopulation::getFuncPis(){
    return &this->num_functional_PIs;
}

//float AigPopulation::getMeanFromScores(){
//    return this->mean_from_scores;
//}
//float AigPopulation::getStandardDeviation(){
//    return this->standard_deviation;
//}


//void AigPopulation::setMeanFromScores(){
////    cout<<"scores size:"<<all_scores.size()<<endl;
//    float sum=0;
//    for(int i=0;i<this->all_scores.size();i++)
//        sum+=all_scores[i];
//    this->mean_from_scores=sum/all_scores.size();
//}

//void AigPopulation::setStandardDeviation(){
//    float std_dev=0.0;
////    cout<<all_scores.size()<<endl;
////    cout<<mean_from_scores<<endl;
//    for(int i=0;i<this->all_scores.size();i++)
//        std_dev+=pow((all_scores[i]-mean_from_scores),2);
////    cout<<std_dev<<endl;
////    cout<<sqrt((pow(std_dev,2))/all_scores.size())<<endl;
//    standard_deviation=sqrt(std_dev/all_scores.size());
//}

void AigPopulation::clearAigPopu(){
    this->aig_population.clear();
    this->all_scores.clear();
    this->num_functional_ands.clear();
    this->ordered_indexes.clear();
    this->num_functional_PIs.clear();
    
//    this->mean_p1_cgps=0;
}

void AigPopulation::writeAigs(){
    for(int a=0;a<this->aig_population.size();a++)
        aig_population[a].writeAIG("AIGs/",aig_population[a].getName()+".aig");
}

void AigPopulation::checkWithAbc(binaryPLA* PLA){
    for(int a=0;a<this->aig_population.size();a++)
        abcCallML(this->aig_population[a].getName(),PLA->getName(),"AIGs/");
}

tuple<float,float,float> AigPopulation::firstEvaluation(binaryPLA* PLA){
#if COUT == 1
    cout<<"--------------------------------------------------"<<endl;
    cout<<">>>>>>>>>>Processing FIRST evaluation!<<<<<<<<<<<<<"<<endl;
    cout<<"--------------------------------------------------"<<endl;
#endif
#if new_benchmarks == 1
    string pla_path="../../IWLS2020-benchmarks/";
#elif new_benchmarks == 2
    string pla_path="../../Benchmarks_2/";
#elif new_benchmarks == 3
    string pla_path="../../Benchmarks_3/";
#elif new_benchmarks == 4
    string pla_path="../../Benchmarks_4/";
#endif
    

    int aux=0; tuple<float,float,float> ret;
    binaryPLA train_pla;
    train_pla.readPLA(pla_path+PLA->getName()+".train.pla",0);
    train_pla.setBatchSize(train_pla.getNumCombinations());

    this->aig_population[0].propagateSignalsBatchPLA(&train_pla);
    all_scores[0]=aig_population[0].getCurrentScore();
    num_functional_ands[0]=aig_population[0].getRealSize();
    num_functional_PIs[0]=aig_population[0].getRealPIsize();
#if COUT == 1
    cout<<"-->TRAIN set accuracy:"; this->printScores();
//    cout<<"p1DataTrain:"<<train_pla.getP1Data()<<", p1Cgp:"<<(aig_population[0]).getP1Cgp()<<endl;
#endif
    get<0>(ret)=aig_population[0].getCurrentScore();
    
    binaryPLA valid_pla;
    valid_pla.readPLA(pla_path+PLA->getName()+".valid.pla",0);
    valid_pla.setBatchSize(valid_pla.getNumCombinations());

//    cout<<"reading validation:"<<pla_path+PLA->getName()+".valid.pla"<<endl;
        this->aig_population[0].propagateSignalsBatchPLA(&valid_pla);
        all_scores[0]=aig_population[0].getCurrentScore();
        num_functional_ands[0]=aig_population[0].getRealSize();
//        num_functional_PIs[0]=aig_population[0].getRealPIsize();
        
#if COUT == 1
    cout<<"-->VALIDATION set accuracy:"; this->printScores();
//    cout<<"p1DataValid:"<<valid_pla.getP1Data()<<", p1Cgp:"<<(aig_population[0]).getP1Cgp()<<endl;
#endif    
    get<1>(ret)=aig_population[0].getCurrentScore();
#if new_benchmarks != 1
    binaryPLA valid_pla2;
    valid_pla2.readPLA(pla_path+PLA->getName()+".valid_2.pla",0);
    valid_pla2.setBatchSize(valid_pla2.getBatch()->size());
        this->aig_population[0].propagateSignalsBatchPLA(&valid_pla2);
        all_scores[0]=aig_population[0].getCurrentScore();
        num_functional_ands[0]=aig_population[0].getRealSize();
//        num_functional_PIs[0]=aig_population[0].getRealPIsize();
#if COUT == 1
    cout<<"-->VALIDATION_2 set accuracy:"; this->printScores();
//    cout<<"p1DataValid2:"<<valid_pla2.getP1Data()<<", p1Cgp:"<<(aig_population[0]).getP1Cgp()<<endl;
#endif
    get<2>(ret)=aig_population[0].getCurrentScore();
#else
    get<2>(ret)=0;
#endif
    
    return ret;
}

tuple<float,float,float> AigPopulation::lastEvaluation(binaryPLA* PLA){
#if COUT == 1
    cout<<"//////////////////////////////////////////////////"<<endl;
    cout<<">>>>>>>>>>Processing LAST evaluation!<<<<<<<<<<<<<"<<endl;
    cout<<"//////////////////////////////////////////////////"<<endl;
#endif
#if new_benchmarks == 1
    string pla_path="../../IWLS2020-benchmarks/";
#elif new_benchmarks == 2
    string pla_path="../../Benchmarks_2/";
#elif new_benchmarks == 3
    string pla_path="../../Benchmarks_3/";
#elif new_benchmarks == 4
    string pla_path="../../Benchmarks_4/";
#endif

#if write_learning >= 2
    ofstream ABC_output;
    ABC_output.open("ABC_output.csv",ios::app); ABC_output<<endl;
    ABC_output<<"Last evaluation,name:"<<PLA->getName()<<endl;
#endif
    int aux=0; tuple<float,float,float> ret;
    //Evaluatinng with last generated batch
        this->aig_population[0].propagateSignalsBatchPLA(PLA);
        all_scores[0]=aig_population[0].getCurrentScore();
        num_functional_ands[0]=aig_population[0].getRealSize();
//        num_functional_PIs[0]=aig_population[0].getRealPIsize();
#if COUT == 1
    cout<<"-->Last batch accuracy:"; this->printScores();
    cout<<"batch size:"<<PLA->getBatchSize()<<endl;
//    cout<<"p1DataTrain:"<<PLA->getP1Data()<<", p1Cgp:"<<(aig_population[0]).getP1Cgp()<<endl;
#endif
    //Evaluating with TRAIN SET
        aux=PLA->getBatchSize();
        binaryPLA train_pla;
        train_pla.readPLA(pla_path+PLA->getName()+".train.pla",0);
        train_pla.setBatchSize(train_pla.getNumCombinations());
//        train_pla.printPLA();
        this->aig_population[0].propagateSignalsBatchPLA(&train_pla);
        all_scores[0]=aig_population[0].getCurrentScore();
        num_functional_ands[0]=aig_population[0].getRealSize();
//        num_functional_PIs[0]=aig_population[0].getRealPIsize();
#if COUT == 1
    cout<<"-->TRAIN set accuracy:"; this->printScores();
//    cout<<"p1DataTrain:"<<train_pla.getP1Data()<<", p1Cgp:"<<(aig_population[0]).getP1Cgp()<<endl;
#endif
    get<0>(ret)=aig_population[0].getCurrentScore();
    PLA->setBatchSize(aux);
    //Evaluating with VALIDATION SET
    binaryPLA valid_pla;
    valid_pla.readPLA(pla_path+PLA->getName()+".valid.pla",0);
    valid_pla.setBatchSize(valid_pla.getNumCombinations());
        this->aig_population[0].propagateSignalsBatchPLA(&valid_pla);
        all_scores[0]=aig_population[0].getCurrentScore();
        num_functional_ands[0]=aig_population[0].getRealSize();
//        num_functional_PIs[0]=aig_population[0].getRealPIsize();
#if COUT == 1
    cout<<"-->VALIDATION set accuracy:"; this->printScores();
//    cout<<"p1DataValid:"<<valid_pla.getP1Data()<<", p1Cgp:"<<(aig_population[0]).getP1Cgp()<<endl;
#endif
    get<1>(ret)=aig_population[0].getCurrentScore();
    
#if new_benchmarks != 1
    binaryPLA valid_pla2;
    valid_pla2.readPLA(pla_path+PLA->getName()+".valid_2.pla",0);
    valid_pla2.setBatchSize(valid_pla2.getBatch()->size());
        this->aig_population[0].propagateSignalsBatchPLA(&valid_pla2);
        all_scores[0]=aig_population[0].getCurrentScore();
        num_functional_ands[0]=aig_population[0].getRealSize();
//        num_functional_PIs[0]=aig_population[0].getRealPIsize();
#if COUT == 1
    cout<<"-->VALIDATION_2 set accuracy:"; this->printScores();
//    cout<<"p1DataValid2:"<<valid_pla2.getP1Data()<<", p1Cgp:"<<(aig_population[0]).getP1Cgp()<<endl;
#endif
    get<2>(ret)=aig_population[0].getCurrentScore();
#else
    get<2>(ret)=0;
#endif
    return ret;
#if write_learning >= 2
    for(int 0=0;0<all_scores.size();0++)
//        ABC_output<<all_sizes[indexes[0]]<<","<<all_scores[indexes[0]]<<endl;;
        ABC_output<<0<<":"<<all_scores[indexes[0]]<<endl;
    ABC_output.close();
#endif
}

void AigPopulation::printScores(){
    cout<<"Ordered scores:";
    if(ordered_indexes.size()>0 && all_scores.size()>0)
    {
        for(int a=0;a<this->all_scores.size();a++)
            cout<<this->ordered_indexes[a]<<":"<<this->all_scores[this->ordered_indexes[a]]<<"("<<this->num_functional_ands[this->ordered_indexes[a]]<<","<<this->num_functional_PIs[ordered_indexes[a]]<<")"<<","; cout<<endl;
    }
}

void AigPopulation::printAigSizes(){
    cout<<"Ordered sizes:";
    for(int a=0;a<this->num_functional_ands.size();a++)
        cout<<this->ordered_indexes[a]<<":"<<this->num_functional_ands[this->ordered_indexes[a]]<<","; cout<<endl;
}

void AigPopulation::readOneAig(string path,string aig_name,mt19937& mt){
#if COUT ==1
    cout<<"Trying to read string AIG:"<<path+aig_name<<endl;
#endif
    this->aig_population.clear();
    this->aig_population.push_back(AIGraph());
    this->all_scores.push_back(0);
    this->num_functional_ands.push_back(0);
    this->num_functional_PIs.push_back(0);
    this->ordered_indexes.push_back(0);
    ifstream aig_file(path+aig_name,ios::binary);
    aig_population[0].clearCircuit();
    aig_population[0].readAIG(aig_file,aig_name,mt);
}

//void AigPopulation::setTemp(int temp){
//    this->temperature=temp;
//    cout<<"TEMP:"<<temperature<<endl;
//}

//float AigPopulation::getTemp(){
//    return this->temperature;
//}

//void AigPopulation::setMeanP1Cgp(){
//    float accum=0;
//    for(int a=0;a<this->aig_population.size();a++)
//    {
//        cout<<aig_population[a].getName()<<":"<<aig_population[a].getP1Cgp()<<",";
//        accum+=aig_population[a].getP1Cgp();
//    }
//    cout<<"accum"<<accum<<endl;
//    accum/=aig_population.size();
//    cout<<"accum"<<accum<<endl;
//    this->mean_p1_cgps=accum;
//}
//
//float AigPopulation::getMeanP1Cgp(){
//    return this->mean_p1_cgps;
//}
//
//void AigPopulation::copyMeanP1Cgp(float param){
//    this->mean_p1_cgps=param;
//}

void AigPopulation::setSingleAig(AIGraph param){
    this->aig_population[0]=param;
}

void AigPopulation::setSingleScore(float score){
    this->all_scores[0]=score;
}
void AigPopulation::setSingleFuncSize(int size){
    this->num_functional_ands[0]=size;
}