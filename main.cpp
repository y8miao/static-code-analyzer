#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <string>
using namespace std;

// Data structures
class Node {
public:
    string function_name;
    vector<string> functions_called;
};

// Global variables
int T_SUPPORT = 3;
double T_CONFIDENCE = 0.65;
map<string,Node> callgraph;
map<string,vector<string> > individual_occurences;
map<pair<string,string>,vector<string> > pair_occurences;

int main(int argc, char* argv[]) {
    if (argc > 1) {
	T_SUPPORT = stoi(argv[1]);
	T_CONFIDENCE = stoi(argv[2]) / 100.0;
    }

    string line;
    ifstream infile("callgraph.txt");
    
    bool is_null_function = false;
    vector<Node> nodes_created;
    
    // Read in data from callgraph file and store it appropriately
    while (getline(infile, line)) {
        
        //*********************** Tokenize line read in from input *****************************
        istringstream iss(line);
        istream_iterator<string> beg(iss), end;
        vector<string> tokens(beg, end);
        
        
        //************************ Create nodes from tokens *************************
        
        // Skip empty lines in input
        if (tokens.size() == 0) continue;
        
        // Mark currend node being parsed as null function
        if (tokens[3] == "<<null") {
            is_null_function = true;
            continue;
        }
        
        // Create node for new function call
        if (tokens[0] == "Call") {
            
            // Indicate that we're parsing a non-null function node now
            is_null_function = false;
            
            Node curr_node;
            
            int first_quot_ind = tokens[5].find("'") + 1;
            int second_quot_ind = tokens[5].find("'", first_quot_ind) - 1;
            curr_node.function_name = tokens[5].substr(first_quot_ind, second_quot_ind);
            
            nodes_created.push_back(curr_node);
        }
        
        // Add called functions to newly created node
        if (tokens[0].substr(0, 2) == "CS" && tokens[2] != "external") {
            
            // Skip called functions inside null function nodes
            if (is_null_function) {
                continue;
            }
            
            string called_function = tokens[3].substr(1, tokens[3].size() - 2);
            vector<string> functions_called = nodes_created.back().functions_called;
            
            // Don't add duplicates
            if (find(functions_called.begin(), functions_called.end(), called_function) == functions_called.end()) {
                nodes_created.back().functions_called.push_back(called_function);
            }
        }
    }
    
    // Store nodes in callgraph map
    for (int i = 0; i < nodes_created.size(); i++) {
        Node node = nodes_created[i];
        callgraph[node.function_name] = node;
        
        // DEBUG: Print each node parsed
//        cout << "Node for function: " << node.function_name << endl;
//        cout << "Calls functions:" << endl;
//        for (int j = 0; j < node.functions_called.size(); j++) {
//            cout << node.functions_called[j] << endl;
//        }
    }
    
    for (map<string,Node>::iterator map_it = callgraph.begin(); map_it != callgraph.end(); map_it++) {
        Node curr_node = map_it->second;
        for (int j = 0; j < curr_node.functions_called.size(); j++) {
            string called_function = curr_node.functions_called[j];
            
            // Update number of individual occurrences for called function
            if (individual_occurences.find(called_function) == individual_occurences.end()) {
                individual_occurences[called_function] = vector<string>();
            }
            if (find(individual_occurences[called_function].begin(), individual_occurences[called_function].end(), curr_node.function_name) == individual_occurences[called_function].end()) {
                individual_occurences[called_function].push_back(curr_node.function_name);
            }
        }
    }
    
    // DEBUG: Print out individual occurrences
    /*for (map<string,vector<string> >::iterator map_it = individual_occurences.begin(); map_it != individual_occurences.end(); map_it++) {
        cout << "Scopes for function " << map_it->first << endl;
        vector<string> scopes = map_it->second;
        for (int k = 0; k < scopes.size(); k++) {
            cout << scopes[k] << endl;
        }
    } */
    

    //*********************** Second Pass *****************************
    vector<pair<string,string> > pairs_created;
    // Go through the call graph
    for(map<string,Node>::iterator map_it = callgraph.begin(); map_it != callgraph.end(); map_it++){
	// Go through every combination of functions in the nested for loop
//	cout<<map_it->first<<endl;
	if(((map_it->second).functions_called).empty()) {/*cout<<"found empty vector."<<endl;*/ continue;}
	else{
		for(vector<string>::iterator vec_it1 = ((map_it->second).functions_called).begin(); vec_it1 != ((map_it->second).functions_called).end();){
//			cout<<"vec_it1"<<(*vec_it1)<<endl;
			vector<string>::iterator vec_dummy = vec_it1;
			vec_it1++;
			vector<string>::iterator vec_it2 = vec_it1;
			if(vec_it2 == ((map_it->second).functions_called).end()) {/*cout<<"end of array reached."<<endl;*/ continue;}
			else{
				for(; vec_it2 != ((map_it->second).functions_called).end(); vec_it2 ++){
//					cout<<"vec_it2"<<(*vec_it2)<<endl;
					// Two possibilities that pair may already be in the map
					pair<string,string> curr_pair (*vec_dummy, *vec_it2);
//					cout<<"curr_pair:"<<curr_pair.first<<","<<curr_pair.second<<endl;
					pair<string,string> curr_pair_mirror (*vec_it2, *vec_dummy);
//					cout<<"curr_pair_mirror"<<curr_pair_mirror.first<<","<<curr_pair_mirror.second<<endl;
					// Both keys not found, insert curr_pair
					if(pair_occurences.find(curr_pair) == pair_occurences.end() && pair_occurences.find(curr_pair_mirror) == pair_occurences.end()){
						vector<string> temp;
						temp.push_back(map_it->first);
						pair_occurences[curr_pair] = temp;
					}
					// curr_pair not found, increment mirror by 1
					else if(pair_occurences.find(curr_pair) == pair_occurences.end()){
						(pair_occurences[curr_pair_mirror]).push_back(map_it->first);
					}
					// Mirror not found, increment curr_pair by 1
					else if(pair_occurences.find(curr_pair_mirror) == pair_occurences.end()){
						pair_occurences[curr_pair].push_back(map_it->first);
					}
					// Both found should never happen
					else throw "Not supposed to happen";
				}
			}
		}
	}
    }
/*    cout<<"out of the insertion loop."<<endl;
    if(pair_occurences.empty()){
	cout<<"pair occurences is empty."<<endl;
    }
    else{
	for(map<pair<string,string>, vector<string> >::iterator map_it = (pair_occurences.begin()); map_it != pair_occurences.end(); map_it++){
		cout<<"pair:"<<(map_it->first).first<<","<<(map_it->first).second<<" value:"<<endl;
		for(vector<string>::iterator vec_it = (map_it->second).begin(); vec_it != (map_it->second).end(); vec_it++){
			cout<<*vec_it<<endl;
		}
    	}
    }*/

    //*********************** Third Pass *****************************

    for(map<pair<string,string>,vector<string> >::iterator it_pair = pair_occurences.begin(); it_pair != pair_occurences.end(); it_pair++) {
        //DEBUGGING CODE
        /*cout << "pair:";
        cout << it_pair->first.first; 
        cout << " "; 
        cout << it_pair->first.second << endl; */
       
        vector<string> pairScopes = it_pair->second;
        
        
        
        if(pairScopes.size() < T_SUPPORT) { continue; }
        //cout << pairScopes.size() << endl;

        pair<string, string> cur_pair = it_pair->first; 
    
        vector<string> firstOfPairScopes = individual_occurences[cur_pair.first];
        double firstConfidence = it_pair->second.size()/(double)firstOfPairScopes.size();

        

        /*cout << it_pair->second.size() << endl;
        cout << firstOfPairScopes.size() << endl;
        cout << firstConfidence << endl;

        cout << endl;

        cout << it_pair->second.size() << endl;
        cout << secondOfPairScopes.size() << endl;
        cout << secondConfidence << endl; */

        if (firstConfidence >= T_CONFIDENCE) {
        
            for(int i = 0; i < firstOfPairScopes.size(); i++) {
                string curr_scope = firstOfPairScopes[i];
                if (find(pairScopes.begin(), pairScopes.end(), curr_scope) == pairScopes.end() ) {
                    cout << "bug: ";
                    cout << cur_pair.first;
                    cout << " in " << curr_scope;
                    cout << ", pair: (";
                    if(cur_pair.first >= cur_pair.second) {
                        cout << cur_pair.second;
                        cout << ", ";
                        cout << cur_pair.first;
                    } else {
                        cout << cur_pair.first;
                        cout << ", ";
                        cout << cur_pair.second;
                    }                    
                    cout << "), support: ";
                    cout << pairScopes.size();
                    cout << ", confidence: ";
                    cout << fixed << setprecision(2) << fixed << firstConfidence * 100.0;
                    cout << "%" << endl;
                } 
            }       
        } 

        vector<string> secondOfPairScopes = individual_occurences[cur_pair.second];
        double secondConfidence = it_pair->second.size()/(double)secondOfPairScopes.size();

        if(secondConfidence >= T_CONFIDENCE) {
            for(int i = 0; i < secondOfPairScopes.size(); i++) {
                string curr_scope = secondOfPairScopes[i];
                if(find(pairScopes.begin(), pairScopes.end(), curr_scope) == pairScopes.end() ) {
                    cout << "bug: ";
                    cout << cur_pair.second;
                    cout << " in ";
                    cout << curr_scope;
                    cout << ", pair: (";
                    if(cur_pair.first >= cur_pair.second) {
                        cout << cur_pair.second;
                        cout << ", ";
                        cout << cur_pair.first;
                    } else {
                        cout << cur_pair.first;
                        cout << ", ";
                        cout << cur_pair.second;
                    }                    
                    cout << "), support: ";
                    cout << pairScopes.size();
                    cout << ", confidence: ";
                    cout << setprecision(2) << fixed << secondConfidence * 100.0;
                    cout << "%" << endl;
                }
            }

        } 
        
    }


}
