/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "AIG.h"

string wordSelector(string line, int word_index){
    int i=1;
    line.erase(0,line.find_first_not_of(" "));
    while(i<word_index)
    {
        line.erase(0,line.find_first_not_of(' '));
        line.erase(0,line.find_first_of(' ')+1);
        line.erase(0,line.find_first_not_of(' '));
        i++;
    }
    if(line.find(' ')!=string::npos)
        line.erase(line.find_first_of(' '),line.size());
//    cout<<line<<endl;
    
    return line;
}



bool getThisPtrPolarity(nodeAig* param){
    nodeAig* aux;
    
    aux=param->fixLSB();
//    cout<<aux->getId()<<"->"<<((int)((uintptr_t)param) & 01)<<endl;
    //returns TRUE for inverted and FALSE for regular
    return ((int)((uintptr_t)param) & 01);
}


int binToDec(vector<int> param){
    int N=param.size();
//    cout<<"N:"<<N<<endl;
    int result=0,aux=0;
    result=-param[N-1]*pow(2,N-1);
    
    for(int i=0;i<=N-2;i++)
        aux+=param[i]*pow(2,i);
//    cout<<"res:"<<result<<endl;
//    cout<<"aux:"<<aux<<endl;
    result=result+aux;
    
//    int soma=0,indice=0;
////    for(int e=param.size()-1;e>0;e--)
//     for(int e=0;e<param.size()-1;e++)
//           {            
//               soma+=pow(2,indice)*(param[e]);
//               indice++;
////               cout<<indice<<"*"<<param[e]<<"+";
//           }
////    if(param[0]==1)
//    if(param[param.size()-1]==1 && soma>0)
//        soma*=-1;
//    else if (soma==0 && param[param.size()-1]==1)
//        soma=1;
//        
//    if(soma==32767)
//        soma=-1;
    return result;
}

void abcCallML(string aig_name,string PLA_file,string source){
#if DEBUG >= 1
    cout<<"Calling ABC, read on:"<<aig_name<<",with &mltest on file:"<<PLA_file<<endl;
#endif
    ofstream script("script.scr"),log("log.txt");
    script<<"&r "<<(source+aig_name)<<".aig"<<endl<<"&ps"<<endl<<"&mltest "<<PLA_file<<endl<<"quit";
    script.close();
    system("./abc -c 'source script.scr' >> log.txt ");

}

void abcWrite(string aig_name,string abc_name){
    cout<<endl<<"ABC WRITE: ("<<aig_name<<") -> ("<<abc_name<<")"<<endl;
    ofstream script("script.scr");
    script<<"&r "<<aig_name<<".aig"<<endl<<"&ps"<<endl<<"&w "<<abc_name<<endl<<"quit";
    script.close();
    system("./abc -c 'source script.scr' >> log.txt ");
}