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


#define ARRAY_SIZE(array) (sizeof((array))/sizeof(array[0]))


int runCommand(vector< vector<string> > the_token_groups, int pipe_ends[], int pipe_counter, int pipe_indx) {

//  the_token_groups.erase(the_token_groups.begin());
 vector<string> command_token_group = the_token_groups[the_token_groups.size() - 1];
 the_token_groups.pop_back();

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

    if(pipe_indx > 0) {



       

      if(pipe_counter == 0) {

        dup2(pipe_ends[pipe_counter + 1], 1);

       
      } else if (pipe_counter == pipe_indx) {

        dup2(pipe_ends[pipe_counter - 2], 0);


        
      } else {
        dup2(pipe_ends[pipe_counter - 2], 0);
        dup2(pipe_ends[pipe_counter + 1], 1);
      }
    }

    //do recursion
      if(the_token_groups.size() > 0) {
        runCommand(the_token_groups, pipe_ends, pipe_counter - 2, pipe_indx);
      }

    
    execvp(command_args[0], command_args);
    cout << "ERROR: Unknown command" << endl;

  } else {

      for (int k = 0; k < pipe_indx; k++) {
        close(pipe_ends[k]);
      }

    wait(&status);


    if(output_redirect)
      close(outfile);

    if(input_redirect)
      close(infile);
    
  }
}

//Bank of valid characters
char bank[65] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '-', '_'};

bool validIn(string x){
  bool valid;
  for(int i = 0; i < x.length(); i++){ //For each character in string
    valid = false;
    for(int j = 0; j < 65; j++){  // For each character in bank
      if(x[i] == bank[j]){  //If char matches, break and move to next
        valid = true;
        break;
      }
    }
    if(valid == false){ //char not found in bank, break and return false
      break;
    }
  }
  return valid;
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

      } else if (!validIn(input_command)) { //check if characters are valid
        cout << "ERROR: Command contains an invalid character. Valid characters: A-Z, a-z, 0-9, ., -, _" << endl;

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

        //Start running commands
        int pipes[(token_groups.size() - 1)*2];


        for(int k = 0; k < ARRAY_SIZE(pipes); k+=2) {
          pipe(pipes + k);
        }

        runCommand(token_groups, pipes, ARRAY_SIZE(pipes), ARRAY_SIZE(pipes));

      }
  }
}
