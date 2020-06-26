//This piece of code is used to generate random number for the exemplars to be executed.
//It generates 5 scripts to be run by ssh with other scripts which calls the scripts.
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: user
 *
 * Created on June 7, 2020, 10:45 PM
 */

#include <cstdlib>
#include <vector>
#include <fstream>
#include <numeric> 
#include <random>
#include <algorithm> 
#include <iostream>
#include <string>
#define WITH 0

#define emulab 1
#define zeus 0 
#define sagi 0
#define brunno 0
#define cerberus 0

#define remove90s 1
#define only90s 0

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    vector<int> exemplars;
    string command,line,path;
    ofstream script;
    random_device device;    
    mt19937 mt(device());
    vector<int> solved_list={9,22,23,24,25,26,27,28,29,30,42,43,45,46,47,48,49,50,51,56,58,62,64,65,66,70,71,72,75};
    for(int x=0;x<solved_list.size();x++)
        cout<<solved_list[x]<<","; cout<<endl<<endl;
    int input,index=0,single_exemplar=0;
    exemplars.resize(100);
#if only90s == 1
        vector<int> aux(10);
        int x=91; iota(aux.begin(),aux.end(),x++);
        auto old_count = aux.size();
        aux.resize(2 * old_count);
        std::copy_n(aux.begin(), old_count, aux.begin() + old_count);
        old_count = aux.size();
        aux.resize(2 * old_count);
        std::copy_n(aux.begin(), old_count, aux.begin() + old_count);
        exemplars=aux;
#else
        int x=1; iota(exemplars.begin(),exemplars.end(),x++);
#if remove90s == 1
        uniform_int_distribution<int> dist(1,90);
        for(int d=0;d<exemplars.size();d++)
        {
            if(exemplars[d]>=91)
                exemplars[d]=dist(mt);
        }
#endif
#endif
#if remove90s == 0
        uniform_int_distribution<int> dist(1,100);
#endif
        for(int d=0;d<exemplars.size();d++)
        {
            while(find(solved_list.begin(),solved_list.end(),exemplars[d])!=solved_list.end())
                exemplars[d]=dist(mt);
        }
        shuffle(exemplars.begin(),exemplars.end(),mt);

    for(int d=0;d<exemplars.size();d++)
        cout<<exemplars[d]<<","; cout<<endl;
#if emulab == 1
#if WITH == 1
            path="cd /users/gudeh/COMCheckAll/cgp";
#else
            path="cd /users/gudeh/SEMCheckAll/cgp";
#endif
#elif zeus ==1
            path="cd /home/augusto/all_folders/cgp";
#elif sagi == 1
            path="cd /home/aasberndt/all_folders/cgp";
#elif cerberus == 1
            path="cd /home/eclvc/aasberndt/all_folders/cgp";
#elif brunno == 1
#if WITH == 1
            path="cd /workareas/share/cgps/COMCheckAll/cgp";
#else
            path="cd /workareas/share/cgps/SEMCheckAll/cgp";
#endif
#endif
    for(input=1;input<=5;input++)
    {
        script.open("script"+to_string(input)+".sh");
//#if only90s == 0
        for(int d=1*(5*input)-4;d<(5*input)+1;d++)
//#else
////            int d=input;
//        for(int d=1*(2*input)-1;d<(2*input)+1;d++)
//#endif
        {
#if only90s == 0
            script<<path+to_string(d)<<endl<<"nohup ./cgp "+to_string(exemplars[index])+" "+to_string(exemplars[index+1])+" "+to_string(exemplars[index+2])+" "+to_string(exemplars[index+3])+" >/dev/null &"<<endl;
            index+=4;
#else
            script<<path+to_string(d)<<endl<<"nohup ./cgp "+to_string(exemplars[index])+" >/dev/null &"<<endl;
            index++;
#endif
        }
        script.close();
    }
//        for(int a=0;a<20;a++)
//            exemplars.erase(exemplars.begin());
//        ofstream outFile("tracker.txt");
//        for(int d=0;d<exemplars.size();d++)
//            outFile<<exemplars[d]<<endl;
    return 0;
}

