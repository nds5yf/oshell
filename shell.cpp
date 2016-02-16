//@author Tyler Robinson (tlr8cn@virginia.edu) & Noah Sauber (nds5yf@virginia.edu)

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>


using namespace std;

int runCommand(vector<string> command_token_group) {
  char* command_args[command_token_group.size() + 1]; 
  for(int i = 0; i < command_token_group.size(); i++){
    command_args[i] = const_cast<char*>(command_token_group[i].c_str());
  }
  command_args[command_token_group.size()] = NULL;1

  int status;

  int pid = fork();
  if(pid == 0) {
    execvp(command_args[0], command_args);
    cout << "ERROR: Unknown command" << endl;
  } else {
    wait(&status);
  }
}

int main() {
  string input_command = "";
  vector<string> tokens;
  vector< vector<string> > token_groups;


  while(true) {
      cout << "$ ";
      getline(cin, input_command); //input command now contains entire line of user input

      if(input_command.length() > 80) { //check if line is too long
	
	     cout << "ERROR: Command exceeds maximum character length of 80 characters." << endl;
      
      } else if (input_command == "exit") { //exit the shell

	     break;

      }  else { //command length was valid and shell wasn't exited

        stringstream ss(input_command);
        string token;

        //retrieves tokens and places them into vector
        while(getline(ss, token, ' ')) { 
         tokens.push_back(token);
        }

        int last_group_indx = 0;
        //isolate the token groups of the command
        for(int i = 0; i < tokens.size(); i++) {

          if(tokens[i] == "|") {
            vector<string> temp;

            for(int j = last_group_indx; j < i; j++) {
              temp.push_back(tokens[j]);
            }
            token_groups.push_back(temp);

            last_group_indx = i + 1;

          }
        }

        //add the last token
        vector<string> temp;
        for(int j = last_group_indx; j < tokens.size(); j++) {
            temp.push_back(tokens[j]);
        }
        token_groups.push_back(temp);

        /*
        for(int k = 0; k < token_groups.size(); k++){
          for(int l = 0; l < token_groups[k].size(); l++) {
            cout << token_groups[k][l] << " ";
          }
          cout << endl;
        }
        */


        //Start running commands
        runCommand(token_groups[0]);

      }
  }
  

 
}
