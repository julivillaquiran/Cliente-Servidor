//    Sudoku cooperative Server side

#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <unistd.h>

//Standar for puzzle size
const int SIZE = 9;

using namespace std;

class Sudoku{
	private:
		int board[SIZE][SIZE];
		int player, column, row, value;

		
	public:
		//Initializing a 0's fill board
		Sudoku(){
			for (int i=0;i<SIZE;i++){
				for (int j=0;j<SIZE;j++){
					board[i][j] = 0;
				}
			}
		}
		
		//Initializing a Sudoku coping a given matrix
		Sudoku(int matrix[SIZE][SIZE]){
			for(int a=0; a<SIZE; a++)
				for(int b=0; b<SIZE; b++)
					board[a][b]==matrix[a][b];
		}
		
		//Allows to place a number in a given position (Columnd,Row) only if doesn't break the sudoke rules
		void Place(int col, int row, int value){
			//if(Check(col,row,value))
				board[row][col] = value; 
		}
		
		
		//Validate if a given input (Column, Row and Value) is under the restrictions. 1 for wrong column, 2 for wronw row, 3 for wrong value, 0 if everything is ok
		int Validate(int col, int row, int value){
			if (!ValidInt(col, SIZE-1)){
				return 1;
			}else{
				 if (!ValidInt(row, SIZE-1)){
					return 2;
				}else{ 
					if (!ValidInt(value, 9)){
						return 3;
					}else{
						return 0;
					}
				}
			}
		}
		
		//Auxiliar function to chek if a given number is between 0 and a given maximum
		bool ValidInt(int value, int max){
			if(value >=0  && value <= max){
				return true;
			}else{
			return false;
			}		
		}

		//Convert from String to variables and validate if a given input is under the restrictions
		int Translate(char* buffer){
				//int player, column, row, value;
				sscanf(buffer, "%d;%d;%d;%d", &player ,&column, &row, &value);
				return Validate(column, row, value);
			}

		//Checking sudoku rules (adapted from http://www.cplusplus.com/forum/lounge/29126/)
		int Check(int x, int y, int val){
			//unsigned char val=matrix[x][y];
			//Checking same row
			for (int a=0;a<SIZE;a++)
				if (board[a][y]==val)
					return 4;

			//Checking same column
			for (int a=0;a<SIZE;a++)
				if (board[x][a]==val)
					return 4;

			//Checking the grid --> the (current row||column module (SIZE/3)) 3
			int startx=x/(SIZE/3)*3,
				starty=y/(SIZE/3)*3,
				endx=startx+SIZE/3,
				endy=starty+SIZE/3;
			for (int a=startx;a<endx;a++){
				for (int b=starty;b<endy;b++){
					if (board[a][b]==val)
						return 4;
					}
				}
			return 0;
		}

		//Prints the current state of the board
		void Print(){
			for(int a=0; a<SIZE; a++){
				for(int b=0; b<SIZE; b++){
					cerr << board[a][b] << " ";
				}
				cout << endl;
			}
		}

		char* Responder(){
		}

		int Play(char* buffer){
			int responseCode = Translate(buffer);
			cout << "Values in sudoku... " << column <<";"<< row <<";" << value << endl;
			cout << "A chequear el Traductor... " << endl;
			if(responseCode == 0){
				cout << "A chequear el Checker... " << endl;
				responseCode = Check(column, row, value);
				if(responseCode == 0){
					cout << "A poner el valor... " << endl;
					Place(column, row, value);
					//---->sumar score
					Print();
					cout << endl;	
				}else{
					cout << "Jugada invalida. Codigo de error: " << responseCode << endl;
				}
			}else{
			cout << "Result especifications: " << endl;
			cout << " 0 <--  Valid results " << endl;
			cout << " 1 <--  Invalid COLUMN value " << endl;
			cout << " 2 <--  Invalid ROW value " << endl;
			cout << " 3 <--  Invalid NUMBER TO PLACE " << endl;
			cout << "Getting results... --> " << responseCode << endl;
			cout << endl;}
			//cout << "Valores invalidos. Codigo de error: " << responseCode << endl;
			//-----> restar score
			return responseCode;
		}
};


int main (void){

	//  Comunication socket
	void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555"); 
    assert (rc == 0);

	//Declarating useful variables    
	//int player, column, row, value;
	char responseCode[5] = "43";
	//Initializing a Sudoku Game   
	Sudoku sudoku;
	sudoku.Print();

    
    while(1){
		char buffer[10];
		zmq_recv (responder, buffer, 10, 0);
		
		//Extracting variable from an string input
		//sscanf(buffer, "%d;%d;%d;%d", &player ,&column, &row, &value);

		/*
		//Checking restriccions
		system("clear");
		cout << "Validating values... " << buffer << endl;
		cout << endl;
		cout << "Result especifications: " << endl;
		cout << " 0 <--  Valid results " << endl;
		cout << " 1 <--  Invalid COLUMN value " << endl;
		cout << " 2 <--  Invalid ROW value " << endl;
		cout << " 3 <--  Invalid NUMBER TO PLACE " << endl;
		cout << "Getting results... --> " << sudoku.Translate(buffer) << endl;
		cout << endl;

		//Checking Sudoku rules
		std::cout << "Checking viability of given movement..." << std::boolalpha << sudoku.Check(column, row, value) << std::endl;

		//Attempt to place the value
		sudoku.Place(column, row, value);

		sudoku.Print();
		*/

		cout << endl;
		cout << "New movement:" << endl;
		cout << "Validating values... " << buffer << endl;
		sprintf(responseCode,"%d",sudoku.Play(buffer));

		//Responding to client
		zmq_send (responder, responseCode , 5, 0);
    }
    return 0;
}
