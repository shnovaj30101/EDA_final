#include<iostream>
#include<fstream>
#include<cstring>
#include<vector>
#include<string>
#include<map>
#include"gate.h"
#include"cirMgr.h"
using namespace std;

struct fault {
   public:
   string from_id;
   string near_id;
   int sa;
   fault() {}
   fault(string id1,string id2,int s) { from_id=id1; near_id=id2; sa=s; }
   ~fault() {}
};////add

CirMgr *cirMgr=new CirMgr();
vector<vector<int> > testpattern;////add
vector<fault> faults;////add
vector<string> sim_input;//dictionary order ////add
vector<string> faults_name;////add

void read_cir(ifstream &file);
void read_pat(ifstream &file);////add
void read_fau(ifstream &file);////add
void print_testpattern();////add
void print_faults();////add
void print_sim(ofstream &file);

int main(int argc,char* argv[]) {
    ifstream cirfile(argv[1]);
    ifstream patfile(argv[3]);////add
    ifstream faufile(argv[2]);////add
    ofstream outfile(argv[4]);////revise
    read_cir(cirfile);
    read_pat(patfile);////add
    read_fau(faufile);////add
    //cirMgr->printGate();
    //cirMgr->print_buf();
    //cirMgr->print_dic_order();////add
    //print_testpattern();////add
    //print_faults();////add
    print_sim(outfile);
    delete cirMgr;
}

void read_cir(ifstream &file) {
    string line;
    int parse_flag=0;
    string input;
    string name;
    string id;
    string parse_gate;
    bool read_parse_gate=false;
    bool read_inputs=true;
    bool read_PI=true;
    bool read_PO=true;
    bool input_continue=false;
    bool output_continue=false;
    //bool read_name=false;
    //bool read_id=false;
    while (getline(file,line)) {
       if (input_continue || output_continue) {
          for ( ; parse_flag<line.size() ; parse_flag++) {
              if (line[parse_flag]==' ') continue;
              if (line[parse_flag]!=' ') { parse_flag--; break; }
          }
       }

       else {
          for ( ; parse_flag<line.size() ; parse_flag++) {
              if (!read_parse_gate && line[parse_flag]==' ') continue;
              if (read_parse_gate && line[parse_flag]==' ') break;
              read_parse_gate=true;
              string temp(1,line[parse_flag]);
              parse_gate.append(temp);
          }
          if (parse_gate!="input" && parse_gate!="output") { 
             for (parse_flag++ ; parse_flag<line.size() ; parse_flag++) {
                 if (line[parse_flag]=='(')  break;
                 string temp(1,line[parse_flag]);
                 name.append(temp);
             }
          } 
       }
       //cout<<parse_gate<<endl;
       if (parse_gate=="input" || input_continue) {
          input_continue=false;
          while (read_PI) {
                for (parse_flag++ ; parse_flag<line.size() ; parse_flag++) { 
                    if (line[parse_flag]==',') { break; }
                    if (line[parse_flag]==';') { read_PI=false; break; }
                    string temp(1,line[parse_flag]);
                    id.append(temp);
                }
                if (id!="VDD" && id!="CK" && id!="GND" && id.size()>0) {
                   cirMgr->addGate(id,id,"PI"); ////revise
                   cirMgr->add_piinput(id);
                }
                id.clear();
                if (parse_flag>=line.size()) { input_continue=true; break; }           
          }
       } 
       else if (parse_gate=="output" || output_continue) {
          output_continue=false;
          while (read_PO) {
                for (parse_flag++ ; parse_flag<line.size() ; parse_flag++) { 
                    if (line[parse_flag]==',') { break; }
                    if (line[parse_flag]==';') { read_PO=false; break; }
                    string temp(1,line[parse_flag]);
                    id.append(temp);
                }
                if (id.size()>0)
                   cirMgr->add_primary_Out(id,id); ////revise
                id.clear();      
                if (parse_flag>=line.size()) { output_continue=true; break; }        
          }
       }
       
       else if (parse_gate=="dff") {
             for (parse_flag++ ; parse_flag<line.size() ; parse_flag++) {
                 if (line[parse_flag]==',')  break;
             }
             for (parse_flag++ ; parse_flag<line.size() ; parse_flag++) {
                 if (line[parse_flag]==',')  break;
                 string temp(1,line[parse_flag]);
                 id.append(temp);
             }
             cirMgr->addGate(id,name,"PPI");
             cirMgr->add_piinput(id);
             id.clear();
             for (parse_flag++ ; parse_flag<line.size() ; parse_flag++) {
                 if (line[parse_flag]==')')  break;
                 string temp(1,line[parse_flag]);
                 id.append(temp);
             }
             cirMgr->add_primary_Out(id,name);
       }
       if (parse_gate=="not") {
             for (parse_flag++ ; parse_flag<line.size() ; parse_flag++) {
                 if (line[parse_flag]==',')  break;
                 string temp(1,line[parse_flag]);
                 id.append(temp);
             }
             cirMgr->addGate(id,name,parse_gate);
             for (parse_flag++ ; parse_flag<line.size() ; parse_flag++) {
                 if (line[parse_flag]==')')  break;
                 string temp(1,line[parse_flag]);
                 input.append(temp);
             }
             cirMgr->addinput(id,input);
       }
       if (parse_gate=="and") {
             for (parse_flag++ ; parse_flag<line.size() ; parse_flag++) {
                 if (line[parse_flag]==',')  break;
                 string temp(1,line[parse_flag]);
                 id.append(temp);
             }
             cirMgr->addGate(id,name,parse_gate);
             while (read_inputs) {
                for (parse_flag++ ; parse_flag<line.size() ; parse_flag++) {
                    if (line[parse_flag]==',') { break; }
                    if (line[parse_flag]==')') { read_inputs=false; break; }
                    string temp(1,line[parse_flag]);
                    input.append(temp);
                }
                cirMgr->addinput(id,input);
                input.clear();
             }
       }
       if (parse_gate=="nand") {
             for (parse_flag++ ; parse_flag<line.size() ; parse_flag++) {
                 if (line[parse_flag]==',')  break;
                 string temp(1,line[parse_flag]);
                 id.append(temp);
             }
             cirMgr->addGate(id,name,parse_gate);
             while (read_inputs) {
                for (parse_flag++ ; parse_flag<line.size() ; parse_flag++) {
                    if (line[parse_flag]==',') { break; }
                    if (line[parse_flag]==')') { read_inputs=false; break; }
                    string temp(1,line[parse_flag]);
                    input.append(temp);
                }
                cirMgr->addinput(id,input);
                input.clear();
             }
       }
       if (parse_gate=="or") {
             for (parse_flag++ ; parse_flag<line.size() ; parse_flag++) {
                 if (line[parse_flag]==',')  break;
                 string temp(1,line[parse_flag]);
                 id.append(temp);
             }
             cirMgr->addGate(id,name,parse_gate);
             while (read_inputs) {
                for (parse_flag++ ; parse_flag<line.size() ; parse_flag++) {
                    if (line[parse_flag]==',') { break; }
                    if (line[parse_flag]==')') { read_inputs=false; break; }
                    string temp(1,line[parse_flag]);
                    input.append(temp);
                }
                cirMgr->addinput(id,input);
                input.clear();
             }
       }
       if (parse_gate=="nor") {
             for (parse_flag++ ; parse_flag<line.size() ; parse_flag++) {
                 if (line[parse_flag]==',')  break;
                 string temp(1,line[parse_flag]);
                 id.append(temp);
             }
             cirMgr->addGate(id,name,parse_gate);
             while (read_inputs) {
                for (parse_flag++ ; parse_flag<line.size() ; parse_flag++) {
                    if (line[parse_flag]==',') { break; }
                    if (line[parse_flag]==')') { read_inputs=false; break; }
                    string temp(1,line[parse_flag]);
                    input.append(temp);
                }
                cirMgr->addinput(id,input);
                input.clear();
             }
       }
       if (parse_gate=="endmodule") {
            cirMgr -> connect_input();
             return;
       }
       parse_gate.clear();
       input.clear();
       name.clear();
       id.clear();
       parse_flag=0;
       read_parse_gate=false;
       read_inputs=true;
       read_PI=true;
       read_PO=true;
       //read_name=false;
       //read_id=false;
    }
    cirMgr -> connect_input();
}

void read_pat(ifstream &file) {
    string line;
    int parse_flag=0;
    int line_num=0;
    while (getline(file,line)) {
       if (line[0]!='{')  continue;
       //cout<<line<<endl;
       testpattern.push_back(vector<int>());
       //cout<<testpattern.size();
       for ( ; parse_flag<line.size() ; parse_flag++) {
           if (line[parse_flag]=='1') testpattern[line_num].push_back(1);
           else if (line[parse_flag]=='0') testpattern[line_num].push_back(0);
       }
       line_num++;
       parse_flag=0;
    }
}////add

void read_fau(ifstream &file) {
    string line;
    string id;
    string name;
    int sa;
    bool read_first=false; //if read first '_' read_first=true
    int first_pos; //first '_' pos
    int last_pos; // last '_' pos
    //int parse_flag=0;
    //int line_num=0;
    while (getline(file,line)) {
       if (line[0]=='#' || line[0]==' ') continue;
       faults_name.push_back(line);
       for (int i=0 ; i<line.size() ; i++) {
           if (line[i]=='_') {
              last_pos=i;
              if (!read_first) {
                 read_first=true;
                 first_pos=i;
              }
           }
       }
       id=line.substr(0,first_pos);
       name=line.substr(first_pos+1,last_pos-first_pos-1);
       if (line[line.size()-1]=='1') sa=1;
       else if (line[line.size()-1]=='0') sa=0;
       //cout<<id<<" "<<name<<" "<<sa<<endl;
       if (cirMgr->has_primary_output(id) && cirMgr->is_FF_name(name))
          faults.push_back(fault(id,id,sa));
       else faults.push_back(fault(id,cirMgr->Name_to_Id(name),sa));

       read_first=false;
    }
}////add

void print_testpattern() {
   for (int i=0 ; i<testpattern.size() ; i++) {
       cout<<"{  ";
       for (int j=0 ; j<testpattern[i].size() ; j++) {
           cout<<testpattern[i][j];
       }
       cout<<"  }\n";
   }
}////add

void print_faults() {
   for (int i=0 ; i<faults.size() ; i++) {
       cout<<faults[i].from_id<<" "<<faults[i].near_id<<" "<<faults[i].sa<<endl;
   }
}////add

void print_sim(ofstream &file){
    vector<string> undet_list;
    for(int i=0 ; i<faults.size() ;i++){
        size_t j=0 ;
        for(; j<testpattern.size() ; j++){
            if(cirMgr->cir_sim(faults[i].from_id, faults[i].near_id, faults[i].sa, testpattern[j]) == 1) break;
            else continue;
        }
        if(j == testpattern.size()) undet_list.push_back(faults_name[i]);
    }
    file << "DETECTED: " << faults.size()-undet_list.size() <<endl;
    file << "UNDETECTED: " << undet_list.size() << endl;
    for(size_t i=0 ;i <undet_list.size() ;i++){
        file << undet_list[i] <<endl;
    }
    undet_list.clear();
}

