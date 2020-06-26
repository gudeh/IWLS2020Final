/*
 * This piece of code is used to select the best AIGs generated for the contest.
 * It is expected an input folder with any number of AIG files.
 * The AIGs names are expected to follow a pattern, starting with the exemplar name,
 * its accuracy and followed by all the parameters used for the CGP to learn the exemplar.
 * 
 * This code reads all the AIGs and selects one for each exemplar, the chosen one has
 * the highest accuracy. Afterwards it uses the ABC binary to double check the accuracy achieved
 * and apply a simplification done by ABC with its structural hashing with the read command.
 * At last this code separates 100 AIGs for submission.
 * 
 */

/* 
 * File:   main.cpp
 * Author: user
 *
 * Created on June 21, 2020, 7:51 PM
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

void abcCallML(string aig_name,string PLA_file,string source){
    ofstream script("script.scr"),log("log.txt");
    script<<"&r "<<(source+aig_name)<<endl<<"&ps"<<endl<<"&mltest "<<PLA_file<<endl<<"quit";
    script.close();
    system("./abc.exe -c 'source script.scr' >> log.txt ");

}

void abcWrite(string aig_name,string abc_name){
    cout<<endl<<"ABC WRITE: ("<<aig_name<<") -> ("<<abc_name<<")"<<endl;
    ofstream script("script2.scr");
    script<<"&r "<<aig_name<<endl<<"&ps"<<endl<<"&w "<<abc_name<<endl<<"quit";
    script.close();
    system("./abc.exe -c 'source script2.scr' >> log2.txt ");
}


struct aig_data{
    int exemplar,Size,ABCsize,batchSize,changeEach,seed,generations;
    int benchmarkVersion,multiFunc,checkAllNodes;
    float myACC,ABCacc,doubleCheckABCacc;
    string init_type,initial_file_name;
    bool original_aig=false;
};

int main(int argc, char** argv) {
    system("rm lists.txt");
    system("rm the100/*.aig");
    system("dir inputAIGs /b > list.txt");
    ifstream input_file("list.txt");
    string line,cut;
    float acc,sums=0;
    int exemp_counter=0;
    vector<map<float,aig_data> > all_exemplars(100);
    vector<int> solved_list;
    
    
    ///////////////////////READING AIG RESULTS AS FILES////////////////////////////////
    getline(input_file,line);
    while(getline(input_file,line))
    {
        aig_data instance;
        if(line.find("Orign")!=string::npos)
            instance.original_aig=true;
        instance.initial_file_name=line;
        /////////////NAME////////////////
        cut=line;
        cut.erase(0,cut.find("ex")+2);
        cut.erase(2,cut.size());
        instance.exemplar=atoi(cut.c_str()); 
        exemp_counter=instance.exemplar;
        /////////////ACCURACY////////////////
        cut=line;
        string::size_type a=cut.find("acc")+3;
        while(isdigit(cut[a]))
            a++;
        cut.erase(a,cut.size());
        cut.erase(0,cut.find("acc")+3);
        acc=(float)atoi(cut.c_str())/1000;
        /////////////SIZE////////////////
        cut=line;
        cut.erase(0,cut.find("cols")+4);
        a=0;
        while(isdigit(cut[a]))
            a++;
        cut.erase(a,cut.size());
        instance.Size=atoi(cut.c_str());
        cout<<cut<<endl;
        
        all_exemplars[exemp_counter].insert(pair<float,aig_data> (acc,instance));
    }
    
    
    //////////////////////PROCESSING RESULTS READ///////////////////////
    int solved=0,givenUp=0;
    ofstream output("BestCGP.csv");
    output<<"Name,BestACC,ABCacc,doubleCheckAbcACC,OrignDTisBest,mySize,ABCsize,,CompleteAIGname"<<endl;
    for(int i=0;i<all_exemplars.size();i++)
//    int i=21;
    {
        for(map<float,aig_data>::iterator it=all_exemplars[i].begin();it!=all_exemplars[i].end();it++)
            cout<<i<<"->"<<it->first<<endl;
        cout<<"GREATER:"<<i<<"->"<<all_exemplars[i].rbegin()->first<<endl;
        sums+=all_exemplars[i].rbegin()->first;
        
        //looking for AIGs to be ignored and gain time in future CGP executions.
        if(all_exemplars[i].rbegin()->first>=99.1)
        {    solved_list.push_back(i);  solved++;}
        if(all_exemplars[i].rbegin()->first<=55)
        {  solved_list.push_back(i); givenUp++;}
       

        /////////////////ABC CHECK///////////////////////
        if(i>=10)
            abcCallML(all_exemplars[i].rbegin()->second.initial_file_name,"../../Benchmarks_4/ex"+to_string(i)+".valid_2.pla","inputAIGs/");
        else
            abcCallML(all_exemplars[i].rbegin()->second.initial_file_name,"../../Benchmarks_4/ex0"+to_string(i)+".valid_2.pla","inputAIGs/");
        //reading ABC output
        int ANDs_size;  float score;
        input_file.close();
        input_file.open("log.txt");
        while(getline(input_file,line))
        {
            if(line.find("and = ")!=string::npos)
            {
                line.erase(0,line.find("and = ")+9);
                line.erase(line.find("lev"),line.back());
                ANDs_size=atoi(line.c_str());
                cout<<"abc_size:"<<ANDs_size<<", size:"<<all_exemplars[i].rbegin()->second.Size<<endl;
                all_exemplars[i].rbegin()->second.ABCsize=ANDs_size;            
            }
            if(line.find("Correct =")!=string::npos)
            {
                line.erase(0,line.find_first_of("(")+1);
                line.erase(line.find_first_of("%"),line.size());
                score=atof(line.c_str());
                cout<<"score:"<<score<<"=="<<all_exemplars[i].rbegin()->first<<endl;
                all_exemplars[i].rbegin()->second.ABCacc=score;
                    
            }
        }   input_file.close();
        if(i>=10)
            abcWrite("inputAIGs/"+all_exemplars[i].rbegin()->second.initial_file_name,"the100/ex"+to_string(all_exemplars[i].rbegin()->second.exemplar)+".aig");
        else
            abcWrite("inputAIGs/"+all_exemplars[i].rbegin()->second.initial_file_name,"the100/ex0"+to_string(all_exemplars[i].rbegin()->second.exemplar)+".aig");
        //double checking ABC accuracy
        if(i>=10)
            abcCallML(to_string(all_exemplars[i].rbegin()->second.exemplar)+".aig","../../Benchmarks_4/ex"+to_string(i)+".valid_2.pla","the100/ex");
        else
            abcCallML(to_string(all_exemplars[i].rbegin()->second.exemplar)+".aig","../../Benchmarks_4/ex0"+to_string(i)+".valid_2.pla","the100/ex0");
        input_file.open("log.txt");
        while(getline(input_file,line))
        {
//            if(line.find("and = ")!=string::npos)
//            {
//                line.erase(0,line.find("and = ")+9);
//                line.erase(line.find("lev"),line.back());
//                ANDs_size=atoi(line.c_str());
//                cout<<"abc_size:"<<ANDs_size<<", size:"<<all_exemplars[i].rbegin()->second.Size<<endl;
//                all_exemplars[i].rbegin()->second.ABCsize=ANDs_size;            
//            }
            if(line.find("Correct =")!=string::npos)
            {
                line.erase(0,line.find_first_of("(")+1);
                line.erase(line.find_first_of("%"),line.size());
                score=atof(line.c_str());
                cout<<"score:"<<score<<"=="<<all_exemplars[i].rbegin()->first<<endl;
                all_exemplars[i].rbegin()->second.doubleCheckABCacc=score;
            }
        }   input_file.close();
        
        /////////////////WRITING OUTPUT TABLE/////////////////////////
        output<<"ex"<<i<<","<<all_exemplars[i].rbegin()->first<<","<<all_exemplars[i].rbegin()->second.ABCacc<<",";
        output<<all_exemplars[i].rbegin()->second.doubleCheckABCacc<<",";
        if(all_exemplars[i].rbegin()->second.original_aig)
            output<<1;
        output<<","<<all_exemplars[i].rbegin()->second.Size<<","<<all_exemplars[i].rbegin()->second.ABCsize;
        output<<",,"<<all_exemplars[i].rbegin()->second.initial_file_name<<endl;
    }
    output<<"average,"<<sums/100<<endl;
    for(int b=0;b<solved_list.size();b++)
        cout<<solved_list[b]+1<<","; cout<<endl;
        cout<<"IGNORED SIZE:"<<solved_list.size()<<endl;
        cout<<"solved:"<<solved<<", given up:"<<givenUp<<endl;
    return 0;
}

