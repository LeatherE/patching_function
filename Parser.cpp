#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iostream>
#include "Parser.h"
#include <assert.h>

using namespace std;



void trim_comments(string* str){
    size_t found = str->find("//");
    while(found != string::npos){
        size_t found_2 = str->find("\n", found + 2);   //find the fist \n after //
        str->erase(found, found_2 - found + 1);

        found = str->find("//");
    }


    size_t found_3 = str->find("/*");
    while(found_3 != string::npos){
        size_t found_4 = str->find("*/", found_3 + 2);   //find the fist */ after /* 
        str->erase(found_3, found_4 - found_3 + 2);

        found_3 = str->find("/*");
    }

}

bool parse_circuit(char* filename, Circuit* circuit){
    ifstream file;
    file.open(filename);
    string line_str;

    //for trim the comments---------
    stringstream file_ss;
    file_ss << file.rdbuf();
    string file_str;
    file_str = file_ss.str();

    trim_comments(&file_str);

    file_ss.str(file_str);
    //------------------------------

    while(getline(file_ss, line_str, ';')){    //get line until meeting a ';'

        line_str = line_str + ';';      //store back ';' to allow more flexible coding format

        stringstream line_ss(line_str);
        string keyword;
        line_ss >> keyword;

        getline(line_ss, line_str, '\0');    //convert stringstream (with keyword removed) to string

        transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);  //lowercase the keyword

        if(keyword == "module" || keyword == "endmodule" || keyword == "wire"){
            //wire object will be created only if it is connected to a gate, so I skipped this condition.
            continue;

        }else if(keyword == "input"){
            string signal_name = "";

            int from = 0, to = 0;  //handle the [from:to] condition
            bool from_to_format = false; //handle the [from:to] condition
            
            for(int i = 0; i < line_str.size(); i++){  //iterate each char to skip unwanted chars.
                if(line_str[i] == '['){
                    if(!signal_name.empty()){
                        from_to_format = false;
                        signal_name += line_str[i];
                    }else{
                        from_to_format = true;
                    }
                }else if(line_str[i] == ':'){     //handle the [from:to] condition
                    assert(from_to_format);
                    from = stoi(signal_name);
                    signal_name = "";
                }else if(line_str[i] == ']'){
                    if(from_to_format){
                        to = stoi(signal_name);     //handle the [from:to] condition
                        signal_name = "";
                    }else{
                        signal_name += line_str[i];
                    }
                }else if(line_str[i] == ',' || line_str[i] == ';'){
                    if(!signal_name.empty()){
                        for(int k = 0; k <= abs(from-to); k++){     //expand [7:0] S to S[7], S[6], S[5], ...
                            Wire* new_w = new Wire();   //store input as wire object

                            if(from_to_format)
                                new_w->name = signal_name + "[" + to_string(k) + "]";
                            else
                                new_w->name = signal_name;

                            circuit->wireName_2_wireObject[new_w->name] = new_w;
                            circuit->PIs.push_back(new_w->name);
                            circuit->wires.push_back(new_w);
                        }
                        signal_name = "";
                    }
                }else if(line_str[i] == ' ' || line_str[i] == '\n' || line_str[i] == '\t' || line_str[i] == '\\'){
                    continue;  //skip the blank
                }else{
                    signal_name += line_str[i];
                }
            }
        }else if(keyword == "output"){
            string signal_name = "";

            int from = 0, to = 0;  //handle the [from:to] condition
            bool from_to_format = false; //handle the [from:to] condition
            
            for(int i = 0; i < line_str.size(); i++){  //iterate each char to skip unwanted chars.
                if(line_str[i] == '['){
                    if(!signal_name.empty()){
                        from_to_format = false;
                        signal_name += line_str[i];
                    }else{
                        from_to_format = true;
                    }
                }else if(line_str[i] == ':'){     //handle the [from:to] condition
                    assert(from_to_format);
                    from = stoi(signal_name);
                    signal_name = "";
                }else if(line_str[i] == ']'){
                    if(from_to_format){
                        to = stoi(signal_name);     //handle the [from:to] condition
                        signal_name = "";
                    }else{
                        signal_name += line_str[i];
                    }
                }else if(line_str[i] == ',' || line_str[i] == ';'){
                    if(!signal_name.empty()){
                        for(int k = 0; k <= abs(from-to); k++){     //expand [7:0] S to S[7], S[6], S[5], ...
                            Wire* new_w = new Wire();   //store input as wire object

                            if(from_to_format)
                                new_w->name = signal_name + "[" + to_string(k) + "]";
                            else
                                new_w->name = signal_name;

                            circuit->wireName_2_wireObject[new_w->name] = new_w;
                            circuit->POs.push_back(new_w->name);
                            circuit->wires.push_back(new_w);
                        }
                        signal_name = "";
                    }
                }else if(line_str[i] == ' ' || line_str[i] == '\n' || line_str[i] == '\t' || line_str[i] == '\\'){
                    continue;  //skip the blank
                }else{
                    signal_name += line_str[i];
                }
            }

        }else if(keyword == "not" || keyword == "buf" || keyword == "and" || keyword == "nand"
                || keyword == "or" || keyword == "nor" || keyword == "xor" || keyword =="xnor"){

            Gate* new_g = new Gate();
            new_g->type = keyword;
            new_g->use = 1;
            circuit->gates.push_back(new_g);

            string signal_name = "";
            for(int i = 0; i < line_str.size(); i++){   //iterate each char to skip unwanted chars.
                //cout << line_str[i] << endl;
                if(line_str[i] == '('){
                    if(!signal_name.empty()){       //the gate has a gate name
                        //since we dont care the gate name, I skipped this feature for now.
                        signal_name = "";
                        continue;
                    }
                }else if(line_str[i] == ')'  || line_str[i] == ','){
                    if(!signal_name.empty()){
                        Wire* w;
                        if(circuit->wireName_2_wireObject.find(signal_name) == circuit->wireName_2_wireObject.end()){
                            //the wire is not yet defined.
                            w = new Wire();
                            w->name = signal_name;
                            circuit->wireName_2_wireObject[signal_name] = w;

                            if(w->name == "1'b1") 
                                circuit->const_1 = true;
                            else if(w->name == "1'b0")
                                circuit->const_0 = true;
                            else
                                circuit->wires.push_back(w);

                        }
                        w = circuit->wireName_2_wireObject[signal_name];
                        if(new_g->out == NULL){   //the wire conntected to the output of the gate
                            new_g->out = w;
                            w->in = new_g;
                        }else{                    //the wire connected to the input of the gate
                            new_g->fanin.push_back(w);
                            w->fanout.push_back(new_g);
                        }    
                        signal_name = "";
                    }
                }else if(line_str[i] == ' ' || line_str[i] == '\n' || line_str[i] == '\t' || line_str[i] == '\\'){
                    continue;  //skip the blank
                }else{
                    signal_name += line_str[i];
                }
            }
        }else if(keyword == "assign"){
            //assign a = b; --> make a gate: (GATE.gate_type = BUF, fanin = b, and out = a)
            string signal_name = "";
            string assigned_to_signal = "", assigned_from_signal = "";

            Gate* new_g = new Gate();
            new_g->use = 1;
            new_g->type = "buf";
            circuit->gates.push_back(new_g);

            for(int i = 0; i < line_str.size(); i++){   //iterate each char to skip unwanted chars.
                if(line_str[i] == '='){
                    assigned_to_signal = signal_name;
                    signal_name = "";
                }else if(line_str[i] == ';'){
                    assigned_from_signal = signal_name;
                    if(circuit->wireName_2_wireObject.find(assigned_to_signal) == circuit->wireName_2_wireObject.end()){
                        //the wire is not yet defined.
                        Wire* w = new Wire();
                        w->name = assigned_to_signal;
                        circuit->wireName_2_wireObject[assigned_to_signal] = w;

                        if(w->name == "1'b1") 
                            circuit->const_1 = true;
                        else if(w->name == "1'b0")
                            circuit->const_0 = true;
                        else
                            circuit->wires.push_back(w);

                    }
                    if(circuit->wireName_2_wireObject.find(assigned_from_signal) == circuit->wireName_2_wireObject.end()){
                        //the wire is not yet defined.
                        Wire* w = new Wire();
                        w->name = assigned_from_signal;
                        circuit->wireName_2_wireObject[assigned_from_signal] = w;

                        if(w->name == "1'b1") 
                            circuit->const_1 = true;
                        else if(w->name == "1'b0")
                            circuit->const_0 = true;
                        else
                            circuit->wires.push_back(w);
                    }
                    Wire* w_to = circuit->wireName_2_wireObject[assigned_to_signal];
                    Wire* w_from = circuit->wireName_2_wireObject[assigned_from_signal];

                    new_g->out = w_to;
                    w_to->in = new_g;
                    new_g->fanin.push_back(w_from);
                    w_from->fanout.push_back(new_g);


                }else if(line_str[i] == ' ' || line_str[i] == '\n' || line_str[i] == '\t' || line_str[i] == '\\'){
                    continue;  //skip the blank
                }else{
                    signal_name += line_str[i];
                }
            }
        }else{
            cout << "unknown type: " << keyword << endl;
            //file.close();
            //return false;
        }
    }

    file.close();
    return true;
}