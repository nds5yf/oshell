//@author Tyler Robinson (tlr8cn@virginia.edu) & Noah Sauber (nds5yf@virginia.edu)

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <string.h>


using namespace std;


int main() {
  string input_command = "";
  vector<string> tokens;


  while(true) {
      cout << "$ ";
      getline(cin, input_command); //input command now contains entire line of user input

      if(input_command.length() > 80) { //check if line is too long
	
	cout << "ERROR: Command exceeds maximum character length of 80 characters." << endl;
      
      } else if (input_command == "exit") { //exit the shell

	break;

      }  else { //command length was valid and shell wasn't exited


      }
  }
  

 
}
