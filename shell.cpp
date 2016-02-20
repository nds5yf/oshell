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
  string outfile_string, infile_string; //used for storing the name of the output file if one exists
  bool output_redirect = false;
  bool input_redirect = false;
  int outfile, infile;
  int command_size = -1;
  int max_indx;



  for (int j = 0; j < command_token_group.size(); j++) {
    
    if(command_token_group[j] == ">") {
      outfile_string = command_token_group[j+1];
      output_redirect = true;

      //ensuring the original size doesn't get overwritten in the 
      //case that there is a '<' followed by a '>'
      if(command_size == -1)
        command_size = j;

    } else if (command_token_group[j] == "<") {

      infile_string = command_token_group[j+1];
      input_redirect = true;

      if(command_size == -1)
        command_size = j;
    }
  }

  max_indx = ((command_size != -1) ? command_size : command_token_group.size());

  char* command_args[max_indx + 1]; 

  for(int i = 0; i < max_indx; i++){

    command_args[i] = const_cast<char*>(command_token_group[i].c_str());

  }
  //sets the null arg
  command_args[max_indx] = NULL;

  int status;

  if(output_redirect){
    //open file with write permissions and create if not created
    outfile = open(const_cast<char*>(outfile_string.c_str()), O_CREAT | O_RDWR, 0777);
  }

  if(input_redirect) {
    infile = open(const_cast<char*>(infile_string.c_str()), O_CREAT | O_RDONLY, 0777);
  }

  int pid = fork();

  if(pid == 0) {
    //need to redirect the output to the specified file
    if(output_redirect) {

      //replace stdout
      dup2(outfile, 1); 
    }

    if(input_redirect) {

      dup2(infile, 0);
    }

    execvp(command_args[0], command_args);
    cout << "ERROR: Unknown command" << endl;

  } else {
    wait(&status);

    if(output_redirect)
      close(outfile);

    if(input_redirect)
      close(infile);
    
  }
}




int main() {
  string input_command = "";
  vector<string> tokens;
  vector< vector<string> > token_groups;


  while(true) {
      input_command = "";
      tokens.clear();
      token_groups.clear();

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
