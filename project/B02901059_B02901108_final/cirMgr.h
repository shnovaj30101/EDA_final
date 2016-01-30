#ifndef CIR_MGR_H
#define CIR_MGR_H

#define CODER 100

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;

typedef vector<CirGate*> GateList;
typedef map<string,CirGate*> GateMap;

struct Net {
   public:
   string out_id;
   string out_name;
   vector<string> in_id;
   vector<string> id_name;
   Net() {}
   Net(string id,string name) { out_id=id; out_name=name; }
   ~Net() {}
};//// add

class CirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
friend class CirGate;

public:
   CirMgr(){}
   ~CirMgr()
   {
        cout<<"release cirmgr!"<<endl;
        for(size_t i = 0 ; i< _TOTAL_gatelist.size() ; i++)
           delete _TOTAL_gatelist[i] ;
    }

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   /*CirGate* getGate(unsigned gid) const;

   // Member functions about circuit construction
   bool readCircuit(const string&);
    bool check_head_error(string&);
    bool string_parsing(string& , int);
    void construct_cirgraph();
    bool connecting( CirGate* , CirGate* , bool & );
    bool define_id(string& );
    bool add_symbolic_name(string& ,bool &);
    bool check_available_id(string&, unsigned , int );

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;*/
   void printGate() {
       for (size_t i = 0 ; i< _TOTAL_gatelist.size() ; i++) {
           cout<<_TOTAL_gatelist[i]->getTypeStr()<<" "
               <<_TOTAL_gatelist[i]->getName()<<" "
               <<_TOTAL_gatelist[i]->getId()<<" :"<<endl;
           string temp=_TOTAL_gatelist[i]->getTypeStr();

           if (temp=="PPI" || temp=="PI")
                 _TOTAL_gatelist[i]->printOutputs();

           else {
                 _TOTAL_gatelist[i]->printInputs();
                 _TOTAL_gatelist[i]->printOutputs();
           }

       }
   }

   void add_dic_order(string id) {
       for (vector<string>::iterator it=dic_order_input_id.begin() ; it!=dic_order_input_id.end() ; it++) {
           if (id<(*it)) {
              dic_order_input_id.insert(it,1,id);
              return;
           }
       }
       dic_order_input_id.push_back(id);
   }////add

   void addGate(string id,string name,string gateType) {
          CirGate* gate;
          if (gateType=="PPI") {////revise
             gate = new PPI_gate(id,name);
             add_dic_order(id);
          }////add
          else if (gateType=="PI") {////revise
             gate = new PI_gate(id,name);
             add_dic_order(id);
          }////add
          else if (gateType=="not")
             gate = new NOT_gate(id,name);
          else if (gateType=="and")
             gate = new AND_gate(id,name);
          else if (gateType=="nand")
             gate = new NAND_gate(id,name);
          else if (gateType=="or")
             gate = new OR_gate(id,name);
          else if (gateType=="nor")
             gate = new NOR_gate(id,name);
          _TOTAL_gatelist.push_back(gate);
          //cout<<_TOTAL_gatelist.size();
          IdMap.insert(pair<string,CirGate*>(id,gate));
          //if (gateType!="PI") ////delete
             NameMap.insert(pair<string,CirGate*>(name,gate));
          if (primary_output.count(id)>0) {////revise
             gate->has_primary_output=1;
             gate->outputs.push_back(gate);////add
          } ////add

          nets.insert(pair<string,Net>(id,Net(id,name)));////add

       if (Buf_output.count(id)>0) {
          //cout<<id<<endl;
          for (vector<string>::iterator it=Buf_output[id].begin(); it!=Buf_output[id].end(); ++it) {
              //cout<<*it<<endl;
              IdMap[id]->con_output(IdMap[*it]);
              nets[id].in_id.push_back(*it);////add
              nets[id].in_id.push_back(IdMap[*it]->name);////add
          }
       }
   }
    void add_piinput(string id){
        IdMap[id]->add_pi_input();
    }
   void addinput(string id,string input) {
        IdMap[id]->add_input_id(input);
        if (IdMap.count(input)>0) {
           IdMap[input]->con_output(IdMap[id]);
           nets[input].in_id.push_back(id);////add
           nets[input].in_id.push_back(IdMap[id]->name);////add
        }
        else if (Buf_output.count(input)==0) {
           Buf_output.insert(pair<string,vector<string> >(input,vector<string>()));
           Buf_output[input].push_back(id);
        }
        else { Buf_output[input].push_back(id); }
   }
   void connect_input(){
       for(size_t i=0; i<_TOTAL_gatelist.size(); i++){
            for(size_t j=0; j<_TOTAL_gatelist[i]->input_id.size(); j++){
                if (IdMap.count(_TOTAL_gatelist[i]->input_id[j])>0) {
                _TOTAL_gatelist[i] -> add_input(IdMap[_TOTAL_gatelist[i]->input_id[j]]);
                }
            }
       }
   }

   void add_primary_Out(string id,string name) {
       primary_output.insert(pair<string,string>(id,name));
   }

   void print_buf() {
        for (map<string,vector<string> >::iterator it=Buf_output.begin() ; it!=Buf_output.end() ; it++) {
            cout<<it->first<<" :"<<endl;
            for (int i=0 ; i<(it->second).size() ; i++) {
                cout<<"   "<<(it->second)[i]<<endl;
            }
        }
   }
   void print_dic_order() {
      for (int i=0 ; i<dic_order_input_id.size() ; i++)
          cout<<dic_order_input_id[i]<<endl;
   }////add

   string Name_to_Id (string name) {
      return NameMap[name]->id;
   }////add

   bool has_primary_output(string id) {
      return IdMap[id]->has_primary_output;
   }////add

   bool is_FF_name(string name) {
      if (NameMap[name]->getTypeStr()=="PPI") return true;
      return false;
   }////add

   bool cir_sim(string from_id, string near_id, int sa, vector<int> testpattern ){
       if(from_id.compare(near_id) == 0){
            IdMap[from_id]->output_fault_marked = 1;
            IdMap[from_id]->fault_output_sig = sa;
       }
       else{
            IdMap[near_id]->input_fault_marked = 1;
            for(size_t i=0; i<IdMap[near_id]->inputs.size();i++){
                if(IdMap[near_id]->inputs[i]->id.compare(from_id)==0){
                    IdMap[near_id]->fault_input_sigs[i] = sa;
                    break;
                }
            }
       }

       for(size_t i=0; i<dic_order_input_id.size(); i++){
            IdMap[dic_order_input_id[i]]->true_input_sigs[0] = testpattern[i];
            IdMap[dic_order_input_id[i]]->fault_input_sigs[0] = testpattern[i];
            sim_id_list.push_back(dic_order_input_id[i]);
       }

        bool stop_sim = 0;
        bool success = 0;
        bool start_marked =0;
        while(stop_sim == 0){
            for(size_t i=0; i<IdMap[sim_id_list[0]]->true_input_sigs.size() ; i++){
                if(IdMap[sim_id_list[0]]->getTypeStr() == "PI" ||IdMap[sim_id_list[0]]->getTypeStr() == "PPI") break;
                IdMap[sim_id_list[0]]->true_input_sigs[i] = IdMap[sim_id_list[0]]->inputs[i]-> true_output_sig;
                if(IdMap[sim_id_list[0]]->fault_input_sigs[i] == -1)
                    IdMap[sim_id_list[0]]->fault_input_sigs[i] = IdMap[sim_id_list[0]]->inputs[i]-> fault_output_sig;
                if(IdMap[sim_id_list[0]]->inputs[i]->output_fault_marked == 1)
                    IdMap[sim_id_list[0]]->input_fault_marked = 1;
            }
            int result = IdMap[sim_id_list[0]]->simulation();
            if( result == 1){
                for(size_t i=0; i<IdMap[sim_id_list[0]]-> outputs.size() ; i++){
                    if(IdMap[sim_id_list[0]]-> outputs[i]->sim_num == IdMap[sim_id_list[0]]-> outputs[i]->input_num)
                        sim_id_list.push_back(IdMap[sim_id_list[0]]-> outputs[i]->id);
                }
            }
            else {
                start_marked = 1;
                if(result == 2){
                    if(IdMap[sim_id_list[0]]->has_primary_output == 1) success =1;
                    else{
                        for(vector<string>::iterator it = marked_id_list.begin() ; it!= marked_id_list.end() ; it++){
                            if(IdMap[sim_id_list[0]]->id.compare(*it)== 0){
                                marked_id_list.erase(it);
                                break;
                            }
                        }
                        for(size_t i=0; i<IdMap[sim_id_list[0]]-> outputs.size() ; i++){
                            marked_id_list.push_back(IdMap[sim_id_list[0]]->outputs[i]->id);
                            if(IdMap[sim_id_list[0]]-> outputs[i]->sim_num == IdMap[sim_id_list[0]]-> outputs[i]->input_num)
                                sim_id_list.push_back(IdMap[sim_id_list[0]]-> outputs[i]->id);
                        }
                    }
                }
                else{
                    for(vector<string>::iterator it = marked_id_list.begin() ; it!= marked_id_list.end() ; it++){
                        if(IdMap[sim_id_list[0]]->id.compare(*it)== 0){
                            marked_id_list.erase(it);
                            break;
                        }
                    }
                    for(size_t i=0; i<IdMap[sim_id_list[0]]-> outputs.size() ; i++){
                        if(IdMap[sim_id_list[0]]-> outputs[i]->sim_num == IdMap[sim_id_list[0]]-> outputs[i]->input_num)
                            sim_id_list.push_back(IdMap[sim_id_list[0]]-> outputs[i]->id);
                    }
                }
            }
            sim_passby_list.push_back(sim_id_list[0]);
            sim_id_list.erase(sim_id_list.begin());

            if(success == 1 ) stop_sim = 1;
            if(start_marked == 1 && marked_id_list.empty() == 1) stop_sim = 1;

        }

        for(size_t i=0; i<sim_passby_list.size() ; i++){
            IdMap[sim_passby_list[i]]-> init();
        }
        sim_id_list.clear();
        marked_id_list.clear();
        sim_passby_list.clear();

        if(success == 1) return 1;
        else return 0;
   }


private:
    GateList                              _TOTAL_gatelist;
    map<string,vector<string> > Buf_output;
    map<string,string> primary_output;
    map<string,Net> nets;//// add
    vector<string> dic_order_input_id;////add
    vector<string> sim_id_list;
    vector<string> marked_id_list;
    vector<string> sim_passby_list;
    /*GateList                              _PI_gatelist;
    GateList                              _PO_gatelist;
    GateList                              _AIG_gatelist;
    GateList                               DFS_AIG_gatelist;
    //GateList                              _CONST_gatelist;
    //GateList                              _UNDEF_gatelist;
    IdList                                  _TOTAL_idlist;
    IdList                                  _PI_idlist;
    IdList                                  _PO_idlist;
    IdList                                  _AIG_idlist;
    //IdList                                  _CONST_idlist;
    IdList                                  _UNDEF_idlist;
    vector < pair< unsigned ,string> >                  _strlist;
    unsigned  TOTAL_num , PI_num , PO_num , L_num , AND_num , Maxvar_num;*/
    GateMap IdMap;
    GateMap NameMap;

};

#endif // CIR_MGR_H
