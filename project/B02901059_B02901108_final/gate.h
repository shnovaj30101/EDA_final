#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
friend class CirMgr;

public:
   CirGate(string i, string n) :
    id(i) , name(n) {
    input_fault_marked = 0;
    output_fault_marked = 0;
    true_output_sig =-1;
    fault_output_sig =-1;
    has_primary_output=0;
    input_num = 0;
    sim_num =0;
     }
    int sim_num;
   virtual ~CirGate() {}

   // Basic access methods
   virtual string getTypeStr() const {}
   virtual int simulation() {}
   //virtual void operate(int intput,string id) {}
   string getId() const { return id; }
   string getName() const { return name; }

   void init(){
        input_fault_marked = 0;
        output_fault_marked = 0;
        true_output_sig = -1;
        fault_output_sig = -1;
        for(size_t i = 0; i< true_input_sigs.size(); i++){
            true_input_sigs[i] = -1;
            fault_input_sigs[i] = -1;
        }
        for(size_t i=0 ; i< outputs.size(); i++){
            outputs[i]-> sim_num = 0;
        }
   }
   void printInputs() const {
       cout<<"  Inputs :"<<endl;
       for (int i =0 ; i<inputs.size() ; i++) {
           cout<<"     "<< inputs[i]->id<<endl;
       }
   }
   void printOutputs() const {
       cout<<"  Outputs :"<<endl;
       //if (has_primary_output) { cout<<"     "<<id<<endl; } ////delete
       for (int i =0 ; i<outputs.size() ; i++) {
           cout<<"     "<<outputs[i]->id<<endl;
       }
   }
   void add_input(CirGate* input) { inputs.push_back(input); true_input_sigs.push_back(-1); fault_input_sigs.push_back(-1); input_num ++;}
   void add_input_id(string id) {input_id.push_back(id);}
   void add_pi_input() { true_input_sigs.push_back(-1); fault_input_sigs.push_back(-1); input_num ++;}
   void con_output(CirGate* output) { outputs.push_back(output); }

private:

protected:
    string name;
    string id;
    vector<CirGate*> inputs;
    vector<string> input_id;
    vector<int> true_input_sigs;
    vector<int> fault_input_sigs;
    vector<CirGate*> outputs;
    int input_num;
    int true_output_sig;
    int fault_output_sig;
    bool has_primary_output;
    bool input_fault_marked;
    bool output_fault_marked;

};



class PI_gate : public CirGate
{
public:
      PI_gate(string i,string n) : CirGate(i,n) { }
    ~PI_gate() { }
     string getTypeStr() const { return "PI";}
    int simulation(){
        bool match = 1;
        if(true_input_sigs[0] == 1) true_output_sig = 1;
        else true_output_sig = 0;
        if(fault_output_sig == -1) {
            if(fault_input_sigs[0] == 1) fault_output_sig = 1;
            else fault_output_sig = 0;
        }
        if(fault_output_sig != true_output_sig) match = 0;
        for(size_t i=0 ; i<outputs.size(); i++) outputs[i]-> sim_num ++;
        if(output_fault_marked == 1){
            if(match == 0) return 2;
            else return 3;
        }
        return 1;
    }
};

class PPI_gate : public CirGate
{
public:
      PPI_gate(string i,string n) : CirGate(i,n) { }
    ~PPI_gate() { }
     string getTypeStr() const { return "PPI";}
    int simulation(){
        bool match = 1;
        if(true_input_sigs[0] == 1) true_output_sig = 1;
        else true_output_sig = 0;
        if(fault_output_sig == -1) {
            if(fault_input_sigs[0] == 1) fault_output_sig = 1;
            else fault_output_sig = 0;
        }
        if(fault_output_sig != true_output_sig) match = 0;
        for(size_t i=0 ; i<outputs.size(); i++) outputs[i]-> sim_num ++;
        if(output_fault_marked == 1){
            if(match == 0) return 2;
            else return 3;
        }
        return 1;
    }
};

class AND_gate : public CirGate {
   public:
      AND_gate(string i,string n) : CirGate(i,n) { }
    ~AND_gate() { }
     string getTypeStr() const { return "AND"; }

    int simulation(){
        bool match = 1;
        true_output_sig = 1;
        if(input_fault_marked == 1) output_fault_marked = 1;
        for(int i=0; i<input_num; i++){
            if(true_input_sigs[i] != fault_input_sigs[i]) match =0;
            if(true_input_sigs[i] == 0) true_output_sig = 0;
            if(fault_output_sig == -1) {
                if(fault_input_sigs[i] == 0) fault_output_sig = 0;
            }
        }
        if(fault_output_sig == -1) fault_output_sig = 1;
        if(fault_output_sig != true_output_sig) match = 0;
        for(size_t i=0 ; i<outputs.size(); i++) outputs[i]-> sim_num ++;
        if(output_fault_marked == 1){
            if(match == 0) return 2;
            else return 3;
        }
        return 1;
    }
};

class NAND_gate : public CirGate {
   public:
      NAND_gate(string i,string n) : CirGate(i,n) { }
    ~NAND_gate() {}
     string getTypeStr() const { return "NAND"; }

    int simulation(){
        bool match = 1;
        true_output_sig = 0;
        if(input_fault_marked == 1) output_fault_marked = 1;
        for(int i=0; i<input_num; i++){
            if(true_input_sigs[i] != fault_input_sigs[i]) match =0;
            if(true_input_sigs[i] == 0) true_output_sig = 1;
            if(fault_output_sig == -1) {
                if(fault_input_sigs[i] == 0) fault_output_sig = 1;
            }
        }
        if(fault_output_sig == -1) fault_output_sig = 0;
        if(fault_output_sig != true_output_sig) match = 0;
        for(size_t i=0 ; i<outputs.size(); i++) outputs[i]-> sim_num ++;
        if(output_fault_marked == 1){
            if(match == 0) return 2;
            else return 3;
        }
        return 1;
    }
};

class OR_gate : public CirGate {
   public:
      OR_gate(string i,string n) : CirGate(i,n) { }
    ~OR_gate() {}
     string getTypeStr() const { return "OR"; }

    int simulation(){
        bool match = 1;
        true_output_sig = 0;
        if(input_fault_marked == 1) output_fault_marked = 1;
        for(int i=0; i<input_num; i++){
            if(true_input_sigs[i] != fault_input_sigs[i]) match =0;
            if(true_input_sigs[i] == 1) true_output_sig = 1;
            if(fault_output_sig == -1) {
                if(fault_input_sigs[i] == 1) fault_output_sig = 1;
            }
        }
        if(fault_output_sig == -1) fault_output_sig = 0;
        if(fault_output_sig != true_output_sig) match = 0;
        for(size_t i=0 ; i<outputs.size(); i++) outputs[i]-> sim_num ++;
        if(output_fault_marked == 1){
            if(match == 0) return 2;
            else return 3;
        }
        return 1;
    }
};

class NOR_gate : public CirGate {
   public:
      NOR_gate(string i,string n) : CirGate(i,n) { }
    ~NOR_gate() {}
     string getTypeStr() const { return "NOR"; }

    int simulation(){
        bool match = 1;
        true_output_sig = 1;
        if(input_fault_marked == 1) output_fault_marked = 1;
        for(int i=0; i<input_num; i++){
            if(true_input_sigs[i] != fault_input_sigs[i]) match =0;
            if(true_input_sigs[i] == 1) true_output_sig = 0;
            if(fault_output_sig == -1) {
                if(fault_input_sigs[i] == 1) fault_output_sig = 0;
            }
        }
        if(fault_output_sig == -1) fault_output_sig = 1;
        if(fault_output_sig != true_output_sig) match = 0;
        for(size_t i=0 ; i<outputs.size(); i++) outputs[i]-> sim_num ++;
        if(output_fault_marked == 1){
            if(match == 0) return 2;
            else return 3;
        }
        return 1;
    }
};

class NOT_gate : public CirGate {
   public:
      NOT_gate(string i,string n) : CirGate(i,n) { }
    ~NOT_gate() {}
     string getTypeStr() const { return "NOT"; }
    int simulation(){
        bool match = 1;
        if(input_fault_marked == 1) output_fault_marked = 1;
        if(true_input_sigs[0] != fault_input_sigs[0]) match =0;
        if(true_input_sigs[0] == 1) true_output_sig = 0;
        else true_output_sig = 1;
        if(fault_output_sig == -1) {
            if(fault_input_sigs[0] == 1) fault_output_sig = 0;
            else fault_output_sig = 1;
        }
        if(fault_output_sig != true_output_sig) match = 0;
        for(size_t i=0 ; i<outputs.size(); i++) outputs[i]-> sim_num ++;
        if(output_fault_marked == 1){
            if(match == 0) return 2;
            else return 3;
        }
        return 1;
    }

};


#endif // CIR_GATE_H
