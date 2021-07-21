#include "Parser.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

void patch(Circuit G_circuit, Circuit P_circuit, string outdir){


    
    fstream file, filetmp;
    vector<string> wire_vec;
	file.open("tmp.v", ios::out | ios::trunc);
	file<<"module top (";
	for(int i = 0; i<G_circuit.PIs.size(); ++i){
		file<<G_circuit.PIs[i]<<", ";
	}
	for(int i = 0; i<G_circuit.POs.size(); ++i){
		file<<G_circuit.POs[i];
		if(i == G_circuit.POs.size() - 1)file<<");"<<endl;
		else file<<", ";
	}
	file<<"input ";
	for(int i = 0; i<G_circuit.PIs.size(); ++i){
		file<<G_circuit.PIs[i];
		if(i == G_circuit.PIs.size() - 1)file<<";"<<endl;
		else file<<", ";
	}
	file<<"output ";
	for(int i = 0; i<G_circuit.POs.size(); ++i){
		file<<G_circuit.POs[i];
		if(i == G_circuit.POs.size() - 1)file<<";"<<endl;
		else file<<", ";
	}
	file<<"wire ";
	for(int i = 0; i<G_circuit.PIs.size(); ++i){
		file<<G_circuit.PIs[i]<<", ";
	}
	for(int i = 0; i<G_circuit.POs.size(); ++i){
		file<<G_circuit.POs[i];
		if(i == G_circuit.POs.size() - 1)file<<";"<<endl;
		else file<<", ";
	}

    int gatecounter = 0;
    int WWWcounter = 0;
    for(int i = 0; i < P_circuit.gates.size(); i++){
        string o_in = P_circuit.gates[i]->out->name;
        o_in += "_in";
        bool find_in = 0;
        for (int j = 0; j< P_circuit.PIs.size(); ++j){
            if(P_circuit.PIs[j].compare(o_in) == 0) {
                find_in = 1;
                break;
            }
        }
        if(find_in){
            bool is_po = 0;
            for (int j = 0; j< G_circuit.POs.size(); ++j){
                if(P_circuit.gates[i]->out->name.compare(G_circuit.POs[j]) == 0){
                    is_po = 1;
                    break;
                }
            }
            if(is_po){
                file<<P_circuit.gates[i]->type<<" "<<"(";
                file<<P_circuit.gates[i]->out->name<<", ";
                wire_vec.push_back(P_circuit.gates[i]->out->name);
                for(int j = 0; j< P_circuit.gates[i]->fanin.size();++j){
                    size_t found = P_circuit.gates[i]->fanin[j]->name.find("_in");
                    if (found!=std::string::npos){ //find
                        bool ori_po = 0;
                        for (int k = 0; k< G_circuit.POs.size(); ++k){
                            cout<<P_circuit.gates[i]->fanin[j]->name<<" ";
                            cout<<P_circuit.gates[i]->fanin[j]->name.substr(0, P_circuit.gates[i]->fanin[j]->name.length()-3)<<endl;
                            if(P_circuit.gates[i]->fanin[j]->name.substr(0, P_circuit.gates[i]->fanin[j]->name.length()-3).compare(G_circuit.POs[k]) == 0){
                                ori_po = 1;
                                break;
                            }
                        }
                        
                        
                        if(ori_po){
                            file<<P_circuit.gates[i]->fanin[j]->name.substr(0, P_circuit.gates[i]->fanin[j]->name.length()-3)<<"_1";
                            for(int k = 0; k < G_circuit.gates.size(); k++){
                                if(G_circuit.gates[k]->out->name.compare(P_circuit.gates[i]->fanin[j]->name.substr(0, P_circuit.gates[i]->fanin[j]->name.length()-3)))
                                    G_circuit.gates[k]->out->name += "_1";
                            }
                        }
                        else{
                            file<<P_circuit.gates[i]->fanin[j]->name.substr(0, P_circuit.gates[i]->fanin[j]->name.length()-3);
                        }
                    }
                    else{
                        file<<P_circuit.gates[i]->fanin[j]->name;
                    }
                    if(j == P_circuit.gates[i]->fanin.size()-1)file<<");"<<endl;
                    else file<<", ";
                }
                for(int j = 0; j< G_circuit.gates.size();++j){
                    if(G_circuit.gates[j]->out->name.compare(P_circuit.gates[i]->out->name) == 0){
                        G_circuit.gates[j]->out->name += "_1";
                    }
                }
            }
            else{
                file<<P_circuit.gates[i]->type<<" "<<"(";
                string newname;
                for(int j = 0; j< P_circuit.gates[i]->fanin.size();++j){
                    if (P_circuit.gates[i]->fanin[j]->name.substr(0, 1).compare("1") == 0){
                        newname += "C";
                        newname += to_string(WWWcounter);
                        file<<"C"<<WWWcounter++;
                    } 
                    else {
                        newname += P_circuit.gates[i]->fanin[j]->name;
                        file<<P_circuit.gates[i]->fanin[j]->name;
                    }
                }
                newname += P_circuit.gates[i]->type;
                file<<P_circuit.gates[i]->type<<", ";
                wire_vec.push_back(newname);
                for(int j = 0; j< P_circuit.gates[i]->fanin.size();++j){
                    size_t found = P_circuit.gates[i]->fanin[j]->name.find("_in");
                    if (found!=std::string::npos){ //find
                        
                        file<<P_circuit.gates[i]->fanin[j]->name.substr(0, P_circuit.gates[i]->fanin[j]->name.length()-3);
                    }
                    else{
                        file<<P_circuit.gates[i]->fanin[j]->name;
                    }
                    if(j == P_circuit.gates[i]->fanin.size()-1)file<<");"<<endl;
                    else file<<", ";
                }
                for(int j = 0; j< G_circuit.gates.size();++j){
                    for(int k= 0; k< G_circuit.gates[j]->fanin.size();++k){
                        if(G_circuit.gates[j]->fanin[k]->name.compare(P_circuit.gates[i]->out->name) == 0){
                            G_circuit.gates[j]->fanin[k]->name = newname;
                        }
                    }
                }
            }
        }
        else{
            for(int j = 0; j < G_circuit.gates.size(); ++j){
                if(G_circuit.gates[j]->out->name.compare(P_circuit.gates[i]->out->name) == 0){
                    G_circuit.gates[j]->use = 0;
                    break;
                }
            }
            file<<P_circuit.gates[i]->type<<" "<<"(";
            file<<P_circuit.gates[i]->out->name<<", ";
            wire_vec.push_back(P_circuit.gates[i]->out->name);
            for(int j = 0; j< P_circuit.gates[i]->fanin.size();++j){
                size_t found = P_circuit.gates[i]->fanin[j]->name.find("_in");
                if (found!=std::string::npos){ //find
                    file<<P_circuit.gates[i]->fanin[j]->name.substr(0, P_circuit.gates[i]->fanin[j]->name.length()-3);
                }
                else file<<P_circuit.gates[i]->fanin[j]->name;
                if(j == P_circuit.gates[i]->fanin.size()-1)file<<");"<<endl;
                else file<<", ";
            }
        }
    }
    for(int i = 0; i < G_circuit.gates.size(); i++){
        if(G_circuit.gates[i]->use){
            file<<G_circuit.gates[i]->type<<" "<<"("<<G_circuit.gates[i]->out->name<<", ";
            wire_vec.push_back(G_circuit.gates[i]->out->name);
            for(int j = 0; j< G_circuit.gates[i]->fanin.size(); ++j){
                file<<G_circuit.gates[i]->fanin[j]->name;
                if(j == G_circuit.gates[i]->fanin.size()-1)file<<");"<<endl;
                else file<<", ";
            }
        }
    }
    file.close();
    
    /*
    for(int i = 0; i<wire_vec.size(); ++i){
        cout<<wire_vec[i]<<endl;
    }
    */
    
    filetmp.open("tmp.v", ios::in);
    file.open(outdir, ios::out | ios::trunc);
    int CCC = 0;
    char buffer[100000];
    while (!filetmp.eof()){
        
        filetmp.getline (buffer,sizeof(buffer));
        if(CCC == 3){

            file<<"wire ";
            for(int i = 0; i<wire_vec.size(); ++i){
                file<<wire_vec[i];
                if(i == wire_vec.size()-1)file<<";"<<endl;
                else file<<", ";
            }
        }
        else{
            file << buffer<<endl;  
            //cout<<buffer;
        }
        CCC++;
        //cout<<CCC<<endl;
    }
    file<<"endmodule";
    file.close();
    filetmp.close();
}

void check_patch_valid(Circuit P_circuit){

    fstream file, filetmp;
    vector<string> wire_vec;
	file.open("Ptmp.v", ios::out | ios::trunc);
	file<<"module top (";
	for(int i = 0; i<P_circuit.PIs.size(); ++i){
		file<<P_circuit.PIs[i]<<", ";
	}
	for(int i = 0; i<P_circuit.POs.size(); ++i){
		file<<P_circuit.POs[i];
		if(i == P_circuit.POs.size() - 1)file<<");"<<endl;
		else file<<", ";
	}
	file<<"input ";
	for(int i = 0; i<P_circuit.PIs.size(); ++i){
		file<<P_circuit.PIs[i];
		if(i == P_circuit.PIs.size() - 1)file<<";"<<endl;
		else file<<", ";
	}
	file<<"output ";
	for(int i = 0; i<P_circuit.POs.size(); ++i){
		file<<P_circuit.POs[i];
		if(i == P_circuit.POs.size() - 1)file<<";"<<endl;
		else file<<", ";
	}
	file<<"wire ";
	for(int i = 0; i<P_circuit.PIs.size(); ++i){
		file<<P_circuit.PIs[i]<<", ";
	}
	for(int i = 0; i<P_circuit.POs.size(); ++i){
		file<<P_circuit.POs[i];
		if(i == P_circuit.POs.size() - 1)file<<";"<<endl;
		else file<<", ";
	}

    for(int i = 0; i < P_circuit.gates.size(); i++){
        if(P_circuit.gates[i]->use){
            file<<P_circuit.gates[i]->type<<" "<<"("<<P_circuit.gates[i]->out->name<<", ";
            wire_vec.push_back(P_circuit.gates[i]->out->name);
            for(int j = 0; j< P_circuit.gates[i]->fanin.size(); ++j){
                file<<P_circuit.gates[i]->fanin[j]->name;
                if(j == P_circuit.gates[i]->fanin.size()-1)file<<");"<<endl;
                else file<<", ";
            }
        }
    }
    file.close();
    
    
    filetmp.open("Ptmp.v", ios::in);
    file.open("P_check.v", ios::out | ios::trunc);
    int CCC = 0;
    char buffer[100000];
    while (!filetmp.eof()){
        
        filetmp.getline (buffer,sizeof(buffer));
        if(CCC == 3){

            file<<"wire ";
            for(int i = 0; i<wire_vec.size(); ++i){
                file<<wire_vec[i];
                if(i == wire_vec.size()-1)file<<";"<<endl;
                else file<<", ";
            }
        }
        else{
            file << buffer<<endl;  
            //cout<<buffer;
        }
        CCC++;
        //cout<<CCC<<endl;
    }
    file<<"endmodule";
    file.close();
    filetmp.close();
}
void genterate_R2_circuit(Circuit R_circuit, string R2_dir){

    fstream file, filetmp;
    vector<string> wire_vec;
	file.open("Rtmp.v", ios::out | ios::trunc);
	file<<"module top (";
	for(int i = 0; i<R_circuit.PIs.size(); ++i){
		file<<R_circuit.PIs[i]<<", ";
	}
	for(int i = 0; i<R_circuit.POs.size(); ++i){
		file<<R_circuit.POs[i];
		if(i == R_circuit.POs.size() - 1)file<<");"<<endl;
		else file<<", ";
	}
	file<<"input ";
	for(int i = 0; i<R_circuit.PIs.size(); ++i){
		file<<R_circuit.PIs[i];
		if(i == R_circuit.PIs.size() - 1)file<<";"<<endl;
		else file<<", ";
	}
	file<<"output ";
	for(int i = 0; i<R_circuit.POs.size(); ++i){
		file<<R_circuit.POs[i];
		if(i == R_circuit.POs.size() - 1)file<<";"<<endl;
		else file<<", ";
	}
	file<<"wire ";
	for(int i = 0; i<R_circuit.PIs.size(); ++i){
		file<<R_circuit.PIs[i]<<", ";
	}
	for(int i = 0; i<R_circuit.POs.size(); ++i){
		file<<R_circuit.POs[i];
		if(i == R_circuit.POs.size() - 1)file<<";"<<endl;
		else file<<", ";
	}

    for(int i = 0; i < R_circuit.gates.size(); i++){
        if(R_circuit.gates[i]->use){
            file<<R_circuit.gates[i]->type<<" "<<"("<<R_circuit.gates[i]->out->name<<", ";
            wire_vec.push_back(R_circuit.gates[i]->out->name);
            for(int j = 0; j< R_circuit.gates[i]->fanin.size(); ++j){
                file<<R_circuit.gates[i]->fanin[j]->name;
                if(j == R_circuit.gates[i]->fanin.size()-1)file<<");"<<endl;
                else file<<", ";
            }
        }
    }
    file.close();
    
    string for_R2 = R2_dir.substr(0, R2_dir.length()-2);
    for_R2 += "ABC.v";
    filetmp.open("Rtmp.v", ios::in);
    file.open(for_R2, ios::out | ios::trunc);
    int CCC = 0;
    char buffer[100000];
    while (!filetmp.eof()){
        
        filetmp.getline (buffer,sizeof(buffer));
        if(CCC == 3){

            file<<"wire ";
            for(int i = 0; i<wire_vec.size(); ++i){
                file<<wire_vec[i];
                if(i == wire_vec.size()-1)file<<";"<<endl;
                else file<<", ";
            }
        }
        else{
            file << buffer<<endl;  
            //cout<<buffer;
        }
        CCC++;
        //cout<<CCC<<endl;
    }
    file<<"endmodule";
    file.close();
    filetmp.close();
}

//This scipte is for generate the modified r2.v(r2ABC.v) and patched file
//usage: ./ver <g1.v> <patch.v> <patched.v> <r2.v>
//Modified r2.v(r2ABC.v) wll generate at the the same directory of the original r2.v.

int main(int argc, char* argv[]){
    
    //parse the circuit
    Circuit G_circuit;
    Circuit P_circuit;
    Circuit R_circuit;
    parse_circuit(argv[1], &G_circuit);
    parse_circuit(argv[2], &P_circuit);
    parse_circuit(argv[4], &R_circuit);

    /*
    int cost_1 = circuit.wires.size();
    int cost_2 = 0;
    for(int i = 0; i < circuit.gates.size(); i++){
        int cost_gates = (circuit.gates[i]->fanin).size() - 2;
        cost_2 += cost_gates;
    }
    
    int cost_3 = 0;  //TO-DO.....
    if(circuit.const_1) cost_3++;
    if(circuit.const_0) cost_3++;

    cout << "number of wires: " << cost_1 << endl;
    cout << "cost of primitives: " << cost_2 << endl;
    cout << "number of constant: " << cost_3 << endl;
    cout << "total cost: " << cost_1 + cost_2 + cost_3 << endl;
    cout << circuit.gates.size()<<endl;
    */
    /*
    cout<<"==========G=========="<<endl;
    for(int i = 0; i < G_circuit.gates.size(); i++){
        cout<<"G"<<i<<": "<<G_circuit.gates[i]->type<<endl<<"FI: ";
        for(int j = 0; j < G_circuit.gates[i]->fanin.size(); ++j){
            cout<<G_circuit.gates[i]->fanin[j]->name;
            if(j == G_circuit.gates[i]->fanin.size() -1)cout<<endl;
            else cout<< ", ";
        }
        cout<<"FO: "<<G_circuit.gates[i]->out->name<<endl;
    }
    cout<<"==========P=========="<<endl;
    for(int i = 0; i < P_circuit.gates.size(); i++){
        cout<<"P"<<i<<": "<<P_circuit.gates[i]->type<<endl<<"FI: ";
        for(int j = 0; j < P_circuit.gates[i]->fanin.size(); ++j){
            cout<<P_circuit.gates[i]->fanin[j]->name;
            if(j == P_circuit.gates[i]->fanin.size() -1)cout<<endl;
            else cout<< ", ";
        }
        cout<<"FO: "<<P_circuit.gates[i]->out->name<<endl;
    }
    */
    //check_patch_valid(P_circuit);
    genterate_R2_circuit(R_circuit, argv[4]);
    patch(G_circuit, P_circuit, argv[3]);
}