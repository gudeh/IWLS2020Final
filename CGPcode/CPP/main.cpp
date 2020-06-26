/*
 * The main file controls the execution flow of the CGP.
 * The CGP  learning parameters may be modified here, such as the number of generations,
 * the mutation rate, or the batch size for example. 
 * 
 * With the smart_init option ON, the CGP looks for AIG files generated with the Scikit-learn
 * or the Espresso tool. Those would be possible starting points for the CGP to learn from.
 * If they have a relatively acceptable accuracy (55% or higher) the CGP will use it to start
 * the learning process. Otherwise the CGP starts with a random CGP initialization. 
 * Without the smart_init option the CGP runs with a single starting point option
 * in all exemplars to be run: SK, ESP or Random initialization.
 * 
 * Final results variables shall be written to the output file results.csv, with
 * the parameters utilized for the execution and the results obtained for each exemplar.
 * An AIG file is also written at the end, its name shall be composed with the same information
 * in the results.csv file.
 * 
 * Different number of exemplars shall be executed different times, it will depend
 * on the execution options selected and the type of CGP initialization. For example,
 * for a random initialization different sizes of changeEach generation will be used
 * and one set of generations will be executed for each changeEach value.
 * 
 * The more external loop selects the exemplar to be executed. Always reading the 
 * PLA input file and executing the CGP initialization. The learning process happens
 * with the internal loop, where the X generations are incremented.
 * 
 * The learning process involves the translation from the CGP structure to the AIG
 * structure repeatedly in each iteration. The mutations happens in the CGP data 
 * structure, while the evaluations with the PLA files happens with the AIG data 
 * structure. The evaluation process executes 64 (size of a long long int) 
 * PLA minterms each time. The signal propagation in the AIG format was already
 * implemented in another project, allowing for a faster coding development.
 * 
 * The CGP not only has different possible parameters, which may be changed in the
 * main file, but also different compilation options, which may be modified
 * at the AIG.h file.
 */

/* 
 * File:   main.cpp
 * Author: user
 *
 * Created on April 3, 2020, 1:41 PM
 */

#include "CGP.h"
#include "AIG.h"
#include "binaryPLA.h"
#include <algorithm>
#include <chrono>  // for high_resolution_clock

using namespace std;
void abcFromEspresso(string pla_name,string abc_name){
    ofstream script("script.scr");
    script<<"read_pla "<<pla_name<<".pla"<<endl<<"strash"<<endl<<"write "<<abc_name<<endl<<"quit";
    script.close();
    system("./abc -c 'source script.scr' >> log.txt ");
}

struct write_struct{
    int temp,num_ands,generation,originalPis,functionalPis;
    float best,mutation;
} generation_info;
vector<write_struct> record(write_size);
int write_offset=0;


int main(int argc, char** argv) {
    string espresso_command,pla_name,aig_name;
#if refresh_aig_folder == 1
    system("rm -rf ../../AIGs/*.aig");
#endif
    system("rm -rf ../../AIGs/*.aag");
    system("rm -rf Learning/*.csv");
    
    //Executing the Scikit-learn and Espresso tools to generate starting point AIGs.
    //If the proper folders already have AIG files on it, this call is unnecessary. 
    //The git version already has some AIGs generated from both techniques.
#if generate_starting_aigs == 1
	system("sh run_all.sh");
    system("mv AIGS_BEST ../../");
    
    for(int f=0;f<100;f++)
    {
        if(f<10)
        {
            espresso_command = "espresso ../../Benchmarks_3/ex0"+to_string(f)+".train.pla > ../../Benchmarks_3_espresso_aig/ex0"+to_string(f)+".pla";
            pla_name="../../Benchmarks_3_espresso_aig/ex0"+to_string(f)+".pla";
            aig_name="../../Benchmarks_3_espresso_aig/ex0"+to_string(f)+".aig";
        }
        else
        {
            espresso_command = "espresso ../../Benchmarks_3/ex"+to_string(f)+".train.pla > ../../Benchmarks_3_espresso_aig/ex"+to_string(f)+".pla";
            pla_name="../../Benchmarks_3_espresso_aig/ex"+to_string(f)+".pla";
            aig_name="../../Benchmarks_3_espresso_aig/ex"+to_string(f)+".aig";
        }
            system(espresso_command.c_str());
            abcFromEspresso(pla_name,aig_name);
    }
#endif
    
    binaryPLA whole_pla;
    auto begin_all = std::chrono::high_resolution_clock::now();
    tuple<float,float,float> scores_pair;
    string pla_path,pla_train_exten,line,exemplar_name,init_type;

    int column_size,line_size,PIs,num_individuals,child_batch,change_each,init_pla_batch,pla_batch_size,complete_batch_size,num_generations;//,temp_init;
    num_individuals=5; //useless with (1+4)
    child_batch=num_individuals/2; //useless with (1+4)
    double min_mutation,mutation_chance,init_mut;
#if constant_seed ==0
    random_device dv;
    mt19937 mt(dv());
    int seed_num=0;
#endif

    //////////////////LEARNING PARAMETERS/////////////
    line_size=1;
    init_mut=2;
    mutation_chance=init_mut;
    min_mutation=0.01;
    int seed_num_init=10;
    vector<int> cols={500,5000}; 
//    vector<int> exemplars={99,98,97,96,95,94,93,92,91,90};
    vector<int> exemplars={06};
    num_generations=1000;
   //////////////////////////////////////////////////
#if new_benchmarks == 1
    complete_batch_size=6400;
    pla_path="../../IWLS2020-benchmarks/";
    pla_train_exten=".train.pla";
#elif new_benchmarks == 2
    complete_batch_size=5760;
    pla_path="../../Benchmarks_2/";
    pla_train_exten=".valid.pla";
#elif new_benchmarks == 3
    complete_batch_size=5120;
    pla_path="../../Benchmarks_3/";
    pla_train_exten=".valid.pla";
#elif new_benchmarks == 4
    pla_path="../../Benchmarks_4/";
    complete_batch_size=10240;
    pla_train_exten=".trainvalid.pla";
#endif

#if train_with_validation_set == 1
    pla_train_exten=".valid.pla";
#endif
    pla_batch_size=complete_batch_size;
    init_pla_batch=pla_batch_size;
    change_each=0;    
#if old_selection == 0
    num_individuals=1;
#endif
    ofstream piDebug("piDebug.txt");
    ofstream results("results.csv");
    ofstream learning_fitnes;
    std::chrono::duration<double> elapsed;
#if write_times >= 0
    ofstream function_times("function_times.csv");    function_times.close();    
    function_times.open("function_times.csv",ios::app);
#endif
    
    
    results<<"Initialization:"; if(smart_init==1)   results<<"EXP.SK.RAND, exp-path:"<<expresso_path<<",sk_path:"<<sk_path; else {if(decision_tree_init==1) results<<"DT init,path:"<<aigs_path<<",Extra nds:"<<(num_extra_nodes)+1<<"x"; else results<<" Random";} results<<endl;
     results<<"Train with validation:"<<train_with_validation_set<<", New PLA benchmakrs:"<<new_benchmarks<<endl;
    results<<"Generations,"<<num_generations<<endl<<"Selection,";
    if(old_selection==0)        results<<"(1+4)";
    else        results<<"old:("<<num_individuals<<"+"<<child_batch*3<<")";
    results<<", SAnnealing:"<<Sannealing; if(Sannealing==1) results<<", InitialTemp:"<<temp_init;results<<endl;
    results<<"Check all ANDs:"<<checkAllNodesAsPO<<",PO mutable:"<<mutable_PO<<",Line:"<<line_size<<endl;
    results<<"Columns:";
    if(decision_tree_init==0){    for(int y=0;y<cols.size();y++)
        results<<cols[y]<<"|";  } else results<<"Initializing with AIG sizes.";
    results<<endl;
    results<<"Exemplars:";
    if(exemplar_set==1){
    for(int y=0;y<exemplars.size();y++)
        results<<exemplars[y]<<"|"; results<<endl;} else results<<"All 100 exemplars!"<<endl;
    results<<"mRate initial:"<<init_mut<<"%,mRate minimal:"<<min_mutation<<"%"<<endl;
    results<<"Batch size,"<<pla_batch_size<<endl<<"Change each,"<<change_each<<endl;
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
//    cout<<"Generations,"<<num_generations<<endl<<"Selection,";
//    if(old_selection==0)        cout<<"(1+5)";
//    else        cout<<"old:("<<num_individuals<<"+"<<child_batch*3<<")";
//    cout<<", SAnnealing,"<<Sannealing<<endl;
//    cout<<"PO mutable,"<<mutable_PO<<endl<<"Line,"<<line_size<<endl<<"Columns,";
//    for(int y=0;y<cols.size();y++)
//        cout<<cols[y]<<"|";  cout<<endl;
//    cout<<"Exemplars:";
//    for(int y=0;y<exemplars.size();y++)
//        cout<<exemplars[y]<<"|"; cout<<endl;
//    cout<<"mRate initial,"<<((float)init_mut/100)<<endl<<"mRate minimal,"<<min_mutation<<endl;
//    cout<<"Batch size,"<<pla_batch_size<<endl<<"Change each,"<<change_each<<", InitialTemp:"<<temp_init<<endl;
    

    
    //    results<<num_generations<<","<<num_individuals<<","<<line_size<<","<<column_size<<","<<child_batch<<","<<mutation_chance<<endl<<endl;
//    results<<"#generations,batchSize,changeEach,";
    results<<"Init,Seed,CGPSize,ChangeEach,Name,TrainAcc,ValidAcc,ValidAcc_2(Original),ANDsFunc,PIsOriginal,PIsFunc,Time"<<endl;
    CgpPopulation my_CgpPopulation;
    AigPopulation aig_popu_obj;
    
    exemplar_name="ex00";
    string learning_file_name="Learning/fitnes-ex00.csv";
    
#if exemplar_set == 1
    for(int s=0;s<exemplars.size();s++)
    {
        int counter=exemplars[s]; 
#else
#if argv_Ntimes == 0
    for(int counter=0;counter<=99;counter++) 
        {
#else
        int counter=0,g=1;
        vector<int> exemplars_argv(argc-1);
        for (g=1;g<argc;g++)
            exemplars_argv[g-1]=atoi(argv[g])-1;
        sort(exemplars_argv.begin(),exemplars_argv.end());
//        shuffle(exemplars_argv.begin(),exemplars_argv.end(),mt);
        cout<<"inputs:";
    for (g=1;g<argc;g++)
        cout<<exemplars_argv[g-1]<<","; cout<<endl;//=atoi(argv[g])-1;
#if LOOP == 1
    for(;;){
#endif
    for(int s=0;s<exemplars_argv.size();s++)
    {
        exemplar_name="ex00";
        int counter=exemplars_argv[s]; 
#endif
#endif
//        cout<<"BEFORE:"<<exemplar_name<<endl;
        if(counter<=9)
            {exemplar_name.replace(3,1,to_string(counter)); exemplar_name.replace(2,1,to_string(0));}
        else
            exemplar_name.replace(2,2,to_string(counter)); 
        cout<<"processing "<<exemplar_name<<endl;
#if write_learning >= 1
        if(counter<=9)
            learning_file_name.replace(19,1,to_string(counter));
        else
            learning_file_name.replace(18,2,to_string(counter));
//        learning_fitnes.open(learning_file_name);
//        learning_fitnes<<"Generation,Best,mRate,#ANDs"<<endl;
#endif
        whole_pla.clear();
        whole_pla.readPLA(pla_path+exemplar_name+pla_train_exten,complete_batch_size);
        PIs=whole_pla.getPiSize();
        int cgp_size=0;
#if decision_tree_init == 0
    for(int d=0;d<cols.size();d++)
    {
        column_size=cols[d];
#else
    {
#endif
        float original_valid_2=0;
        cgp_size=column_size*line_size;
#if constant_seed > 0 
    for(int seed_num=seed_num_init;seed_num<=constant_seed;seed_num+=1)
    { 
        if(learning_file_name.find("--")==string::npos)
        {learning_file_name.replace(learning_file_name.find("."),learning_file_name.size(),"--"+to_string(seed_num)+"-"+to_string(column_size)+".csv"); cout<<learning_file_name<<endl;}
        else
            learning_file_name.replace(learning_file_name.find("--"),learning_file_name.size(),"--"+to_string(seed_num)+"-"+to_string(column_size)+".csv");
#if write_learning >=1
        cout<<learning_file_name<<endl;
        learning_fitnes.close(); learning_fitnes.open(learning_file_name);
        learning_fitnes<<"Generation,Best,mRate,FuncAnds,FuncPIs,Temperature"<<endl;
#endif
        mt19937 mt(seed_num);
#endif
        write_offset=0;
        auto begin_generation = std::chrono::high_resolution_clock::now();
        
        whole_pla.setRandomBatch(mt);
        //Instantiate CGP population and their CGP nodes.
        my_CgpPopulation.clearCgp();
        aig_popu_obj.clearAigPopu();
#if smart_init == 0
#if decision_tree_init == 0
        my_CgpPopulation.createNodes(exemplar_name,num_individuals,PIs,column_size,line_size,mt); init_type="Rand";
        //Generating Population AIG from the CGP created.
        aig_popu_obj.CGPpopToAIGpop(&my_CgpPopulation,PIs,1,column_size*line_size,true);
#else
        pla_batch_size=complete_batch_size;
        init_pla_batch=pla_batch_size;
//        whole_pla.readPLA(pla_path+exemplar_name+pla_train_exten,pla_batch_size);
        ifstream aux_stream;
        aux_stream.open(aigs_path+exemplar_name+".aig");
        if(!aux_stream.is_open())
            {cout<<"SKIPPING EXEMPLAR "<<counter<<endl; continue;}    aux_stream.close();
        my_CgpPopulation.initializeWithDecisionTree(exemplar_name,aigs_path,&whole_pla,&aig_popu_obj,mt);
        string aux_str=aigs_path;
        if(aux_str.find("espresso")!=string::npos)
            init_type="EXP";        else           init_type="SK";
        cgp_size=aig_popu_obj.getAigFromPopulation(0)->getAnds()->size()-1;
        column_size=cgp_size;
#endif
        cout<<"cgp size:"<<cgp_size<<",colum size:"<<column_size<<",type:"<<init_type<<endl;
        aig_popu_obj.evaluateScoresMyImplement(&whole_pla,0,mt);
        cout<<"CREATED CGP SCORES:";   aig_popu_obj.printScores();
#else
        ifstream aux_stream; tuple<float,float,float> expresso_pair; get<0>(expresso_pair)=-1; get<1>(expresso_pair)=-1;
        AigPopulation expresso_aig; expresso_aig.clearAigPopu(); CgpPopulation expresso_cgp; expresso_cgp.clearCgp();
        aux_stream.open(expresso_path+exemplar_name+".aig");
        cout<<"READING INPUT EXEMPLAR FILES (SK and ESP)!!!!"<<endl;
        if(aux_stream.is_open())
        {
            aux_stream.close();
#if COUT == 1
            cout<<"----------Expresso AIG for "<<exemplar_name<<" found."<<endl;       
            cout<<"expresso path:"<<expresso_path<<endl;
            cout<<"exemplar name:"<<exemplar_name<<endl;
#endif
            expresso_cgp.initializeWithDecisionTree(exemplar_name,expresso_path,&whole_pla,&expresso_aig,mt);
            expresso_pair=expresso_aig.firstEvaluation(&whole_pla);
            if(get<1>(expresso_pair)<=0.55) 
                {get<0>(expresso_pair)=-1; get<1>(expresso_pair)=-1; get<2>(expresso_pair)=-1;}
        }
        tuple<float,float,float> sk_pair; get<0>(sk_pair)=-1; get<1>(sk_pair)=-1;
        AigPopulation sk_aig; sk_aig.clearAigPopu(); CgpPopulation sk_cgp; sk_cgp.clearCgp();
        aux_stream.open(sk_path+exemplar_name+".aig");
        if(aux_stream.is_open())
        {
            aux_stream.close();
#if COUT == 1
            cout<<"----------SK AIG for "<<exemplar_name<<" found."<<endl;       
#endif
            sk_cgp.initializeWithDecisionTree(exemplar_name,sk_path,&whole_pla,&sk_aig,mt);
            sk_pair=sk_aig.firstEvaluation(&whole_pla);
            if(get<1>(sk_pair)<=0.55) 
                {get<0>(sk_pair)=-1; get<1>(sk_pair)=-1;  get<2>(sk_pair)=-1;}        
        }
#if COUT == 1
        cout<<"Exemplar name:"<<exemplar_name<<endl;
        cout<<"Expresso scores (train,valid):"<<get<0>(expresso_pair)<<","<<get<1>(expresso_pair)<<","<<get<2>(expresso_pair)<<endl;
        cout<<"SK scores (train,valid):"<<get<0>(sk_pair)<<","<<get<1>(sk_pair)<<","<<get<2>(sk_pair)<<endl;
#endif
#if rand_loops == 1
        for(int d=0;d<cols.size();d++)
#else
        int d=0;
#endif
        {
#if rand_loops == 1
            for(int change=1000;change<=2000;change+=1000)
#else
            int change=1000;
#endif
            {
                if(get<1>(sk_pair)>0.55 || get<1>(expresso_pair)>0.55)
                {
#if new_benchmarks == 4
                    complete_batch_size=5120;
                    pla_train_exten=".valid.pla";
                    whole_pla.readPLA(pla_path+exemplar_name+pla_train_exten,pla_batch_size);
#endif
                    if(get<1>(sk_pair)>=get<1>(expresso_pair))
                        {my_CgpPopulation.initializeWithDecisionTree(exemplar_name,sk_path,&whole_pla,&aig_popu_obj,mt);  
                        init_type="SK"; original_valid_2=get<2>(sk_pair);}
                    else if(get<1>(sk_pair)<get<1>(expresso_pair))
                        {my_CgpPopulation.initializeWithDecisionTree(exemplar_name,expresso_path,&whole_pla,&aig_popu_obj,mt);
                        init_type="EXP"; original_valid_2=get<2>(expresso_pair);}
                pla_batch_size=complete_batch_size;     init_pla_batch=pla_batch_size;
                d=10;change=9999; change_each=0;
                whole_pla.setBatchSize(pla_batch_size);
                }
                else //random init!
                {
#if new_benchmarks == 4
                    complete_batch_size=10240;
                    pla_train_exten=".trainvalid.pla";
#endif
#if COUT == 1
                    cout<<"STARTING RANDOM INIT, d:"<<d<<", change:"<<change<<endl;
#endif
                    pla_batch_size=64*16;
                    pla_batch_size=complete_batch_size;
                    init_pla_batch=pla_batch_size;
                    column_size=cols[d];    change_each=change;
                    whole_pla.readPLA(pla_path+exemplar_name+pla_train_exten,pla_batch_size);
                    whole_pla.setRandomBatch(mt);
//                    whole_pla.printBatch();
                    my_CgpPopulation.clearCgp(); aig_popu_obj.clearAigPopu();
                    my_CgpPopulation.createNodes(exemplar_name,num_individuals,PIs,column_size,line_size,mt);
                    init_type="Rand";
                    aig_popu_obj.CGPpopToAIGpop(&my_CgpPopulation,PIs,1,column_size*line_size,true);
                }
        cgp_size=aig_popu_obj.getAigFromPopulation(0)->getAnds()->size()-1;
        aig_popu_obj.evaluateScoresMyImplement(&whole_pla,0,mt);
#if COUT == 1
        cout<<endl<<"Batch size from vec:"<<whole_pla.getBatch()->size()<<", batch size:"<<whole_pla.getBatchSize()<<endl;
        cout<<"SMART CREATED CGP SCORES "<<init_type<<","<<exemplar_name<<":";   aig_popu_obj.printScores();;
#endif
#endif
#if COUT == 1
//        cout<<"GENERATED CGP:"; my_CgpPopulation.printGraph();
//        cout<<"GENERATED AIG:"; aig_popu_obj.getAigFromPopulation(0)->printCircuit();
#endif
#if Sannealing ==1
        aig_popu_obj.setTemp(temp_init);
#endif
#if COUT ==1
        mutation_chance=init_mut;
        cout<<"Starting generations"<<endl;
#endif
        for(int X=0;X<num_generations;X++)
        {
#if write_times == 0
            auto begin_gen = std::chrono::high_resolution_clock::now();
#endif
#if COUT == 1
            cout<<"Generation:"<<X<<",mutation chance:"<<(float)mutation_chance<<"%"<<endl;
#endif
            if(aig_popu_obj.getScores()->at(aig_popu_obj.getOrderedIndexes()->at(0))==1)
            {
                cout<<"EXEMPLAR "<<exemplar_name<<" has trainning accuracy:"<<aig_popu_obj.getScores()->at(aig_popu_obj.getOrderedIndexes()->at(0))<<", SKIPPING!"<<endl;
                break;
            }
            //last 10% generations go with complete train set
            if(X>=num_generations*0.999)
            {
//                cout<<"Setting batch as complete train set! gen:"<<X<<">="<<num_generations*0.9<<endl;
                pla_batch_size=complete_batch_size;
                whole_pla.setBatchSize(pla_batch_size);
                whole_pla.setRandomBatch(mt);
            }
            
#if write_times == 1
            auto begin = std::chrono::high_resolution_clock::now();
#endif
          mutation_chance=my_CgpPopulation.generateFathersOneToFive(&aig_popu_obj,mutation_chance,min_mutation,&whole_pla,change_each,mt);
#if write_times == 1
            auto finish = std::chrono::high_resolution_clock::now();
            elapsed = finish - begin;
            function_times<<elapsed.count()<<"generateFathers,";
#endif      
#if write_times == 1
            begin = std::chrono::high_resolution_clock::now();
#endif
            aig_popu_obj.clearAigPopu();
            aig_popu_obj.CGPpopToAIGpop(&my_CgpPopulation,PIs,1,cgp_size,false);
#if write_times == 1
            finish = std::chrono::high_resolution_clock::now();
            elapsed = finish - begin;
            function_times<<elapsed.count()<<"cgpToAig"<<endl;
#endif
#if write_learning >=1
//            aig_popu_obj.setMeanFromScores(); aig_popu_obj.setStandardDeviation();// aig_popu_obj.setMeanP1Cgp();
            generation_info.generation=X;
//            cout<<">>>>>>AIGS SIZE:"<<aig_popu_obj.getScores()->size()<<endl;
            generation_info.best=aig_popu_obj.getScores()->at(0);
            generation_info.mutation=(float)mutation_chance/100;
            generation_info.num_ands=aig_popu_obj.getFuncSizes()->at(0); 
//            generation_info.num_ands=my_CgpPopulation.getSizes()[0]; 
            generation_info.functionalPis=aig_popu_obj.getFuncPis()->at(0);
            generation_info.temp=aig_popu_obj.getTemp();
            record[X-write_offset]=generation_info;
            if((X+1)%write_size==0 && X!=0)
            {
                cout<<"Writing outpts at generation "<<X<<endl;
                for(int l=0;l<record.size();l++)
                    learning_fitnes<<record[l].generation<<","<<record[l].best<<","<<record[l].mutation<<","<<record[l].num_ands<<","<<record[l].functionalPis<<","<<(float)record[l].temp/temp_init<<endl;
                write_offset+=write_size;
            }
#endif
#if COUT == 1
            cout<<"Best:"<<aig_popu_obj.getScores()->at(0)<<", #ANDs:"<<my_CgpPopulation.getSizes()[0]; 
#endif
#if write_times == 0
            auto finish_gen = std::chrono::high_resolution_clock::now();
            elapsed = finish_gen - begin_gen;
#if COUT == 1 
            cout<<", time to process gen:"<<elapsed.count()<<endl;
#endif
#endif
        }
        //last generation evaluation
        scores_pair=aig_popu_obj.lastEvaluation(&whole_pla);
#if COUT == 1
        cout<<"TRAIN SCORE:"<<get<0>(scores_pair)<<", VALID SCORE:"<<get<1>(scores_pair)<<", VALID_2 SCORE:"<<get<2>(scores_pair)<<endl;
#endif
#if MULTI_FUNC == 1
//        my_CgpPopulation.printGraph();
//        aig_popu_obj.getAigFromPopulation(0)->printCircuit();
        aig_popu_obj.getAigFromPopulation(0)->mapXorToAnd("../../AIGs/",exemplar_name+"acc"+to_string((int)(100000*get<2>(scores_pair)))+init_type+"cols"+to_string(cgp_size)+"batch"+to_string(init_pla_batch)+"ce"+to_string(change_each)+"seed"+to_string(seed_num)+"can"+to_string(checkAllNodesAsPO)+"gens"+to_string(num_generations)+"bench"+to_string(new_benchmarks)+"mf"+to_string(MULTI_FUNC)+".aig");
#else
        aig_popu_obj.getAigFromPopulation(0)->writeAIG("../../AIGs/",exemplar_name+"acc"+to_string((int)(100000*get<2>(scores_pair)))+init_type+"cols"+to_string(cgp_size)+"batch"+to_string(init_pla_batch)+"ce"+to_string(change_each)+"seed"+to_string(seed_num)+"can"+to_string(checkAllNodesAsPO)+"gens"+to_string(num_generations)+"bench"+to_string(new_benchmarks)+"mf"+to_string(MULTI_FUNC)+"Origin.aig");
#endif
        //        aig_popu_obj.getAigFromPopulation(0)->writeAAG("AIGs/",exemplar_name+"S"+to_string(seed_num)+"C"+to_string(column_size)+".aag");
        auto finish_generation = std::chrono::high_resolution_clock::now();
        elapsed=finish_generation-begin_generation;
        
       // results<<num_generations<<","<<pla_batch_size<<","<<change_each<<",";
        results<<init_type<<","<<seed_num<<","<<cgp_size<<","<<change_each<<","<<exemplar_name<<","<<get<0>(scores_pair)<<","<<get<1>(scores_pair)<<","<<get<2>(scores_pair)<<"("<<original_valid_2<<")";
        results<<","<<(aig_popu_obj.getFuncSizes()->at(aig_popu_obj.getOrderedIndexes()->at(0)))<<",";
        results<<aig_popu_obj.getAigFromPopulation(aig_popu_obj.getOrderedIndexes()->at(0))->getPIs()->size()-1<<",";
        results<<(aig_popu_obj.getFuncPis()->at(aig_popu_obj.getOrderedIndexes()->at(0)))<<",";
//        results<<aig_popu_obj.getAigFromPopulation(0)->getP1Cgp()<<",";
        results<<elapsed.count()<<endl;
        
        learning_fitnes<<record.size()<<","<<(aig_popu_obj.getScores()->at(aig_popu_obj.getOrderedIndexes()->at(0)))<<",";
        learning_fitnes<<record[record.size()].mutation<<","<<(aig_popu_obj.getFuncSizes()->at(aig_popu_obj.getOrderedIndexes()->at(0)))<<endl;
        learning_fitnes.close();
    }   
#if constant_seed > 0
    }    
#endif
#if smart_init ==1
    }}
#endif
        whole_pla.clear();
        aig_popu_obj.clearAigPopu();
        my_CgpPopulation.clearCgp();
    }
#if LOOP == 1
    }
#endif
    
    auto finish_all = std::chrono::high_resolution_clock::now();
    elapsed=finish_all-begin_all;
    cout<<"Time to process everything:"<<elapsed.count()<<endl; 
    results<<"Time to process everything:"<<elapsed.count()<<endl;
    
    
    return 0;
}

