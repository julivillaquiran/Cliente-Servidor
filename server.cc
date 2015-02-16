//    Sudoku cooperative Server side
/*
 Julian Villaquiran
 Universidad Tecnologica de Pereria
 Arquitectura Cliente/Servidor
 Github: julivillaquiran
*/

/*
All messages between Client & Server must be send as a char*

Initial message form client MUST BE "init", this allows new player to be add to the current game.
Response would be one of the followings:
Number between 0 & NPLAYERS(Maximum of players allowed for each game) <-- Code for the player
991 <-- If Maximum of players have been add

Result especifications:
0 <--  Valid results. Used if the movement is correct and the number has been placed
1 <--  Invalid COLUMN value
2 <--  Invalid ROW value
3 <--  Invalid NUMBER TO PLACE
4 <--  Invalid MOVEMENT, if movement doesnt obey Sudoku rules
5 <--  Invalid PLAYER code	
*/

#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <unistd.h>
#include <string.h>

//Standar for puzzle size
const int SIZE = 9;
//Number of players allow
const int NPLAYERS = 5;

using namespace std;

class Sudoku{
	private:
		int board[SIZE][SIZE];
		int score[NPLAYERS];
		int player, column, row, value, currentPlayers;

		
	public:
		//Initializing a 0's fill board
		Sudoku(){
			for (int i=0; i < SIZE; i++){
				for (int j=0; j < SIZE; j++){
					board[i][j] = 0;
				}
			}
			
			for(int i=0; i < NPLAYERS; i++){
				score[i] = 0;
			}
			currentPlayers = 0;
		}
		
		//Initializing a Sudoku coping a given matrix
		Sudoku(int matrix[SIZE][SIZE]){
			for(int a=0; a<SIZE; a++)
				for(int b=0; b<SIZE; b++)
					board[a][b]==matrix[a][b];
	
			for(int i=0; i < NPLAYERS; i++){
				score[i] = 0;
			}

			currentPlayers = 0;
		}
		
		//Allows to place a number in a given position (Columnd,Row) only if doesn't break the sudoke rules
		void Place(int col, int row, int value){
			if(Check(col,row,value)==0)
				board[row][col] = value; 
		}
		
		
		//Validate if a given input (Column, Row and Value) is under the restrictions. 1 for wrong column, 2 for wronw row, 3 for wrong value, 0 if everything is ok
		int Validate(int col, int row, int value){
			//validates the player
			if(player >= currentPlayers)
				return 5;
			if (!ValidInt(col, SIZE-1))
				return 1;
			if (!ValidInt(row, SIZE-1))
				return 2;
			if (!ValidInt(value, 9))
				return 3;
			return 0;
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

		//Executes de main thread of the game, validating values, and sudoku rules, it adds and substract point to the current player
		int Play(char* buffer){
			int responseCode = Translate(buffer);
			cout << "Validating values in sudoku... " << player <<";"<< column <<";"<< row <<";" << value << endl;
			//cout << "A chequear el Traductor... " << endl;
			if(responseCode == 0){
				//cout << "A chequear el Checker... " << endl;
				responseCode = Check(column, row, value);
				if(responseCode == 0){
					//cout << "A poner el valor... " << endl;
					Place(column, row, value);
					//-----> sumar score
					AddScore(2);
					Print();
					cout << endl;	
				}else{
					cout << "Jugada invalida. Codigo de error: " << responseCode << endl;
					//-----> restar score
					SubbScore(2);
				}
			}else{
			cout << "Result especifications: " << endl;
			cout << " 0 <--  Valid results " << endl;
			cout << " 1 <--  Invalid COLUMN value " << endl;
			cout << " 2 <--  Invalid ROW value " << endl;
			cout << " 3 <--  Invalid NUMBER TO PLACE " << endl;
			cout << " 5 <--  Invalid PLAYER code " << endl;
			cout << "Getting results... --> " << responseCode << endl;
			cout << endl;
			//-----> restar score
			SubbScore(1);
			}
			//Imprime la tabla de puntuacion
			ShowScore();
			return responseCode;
		}

		//Manejo del Score
		//Adding given points to the current player
		void AddScore(int points){
			score[player] += points;		
		}

		//Substracting given points to the current player
		void SubbScore(int points){
			score[player] -= points;		
		}
	
		//Show score table
		void ShowScore(){
			cout << "Tabla de Puntaje:  " << endl;
			for(int i=0; i < currentPlayers-1; i++)
				cerr << "Jugador " << i << ": " << score[i] << " -- ";
			if(currentPlayers >=1)
				cout << "Jugador " << currentPlayers-1 << ": " << score[currentPlayers-1]<< endl;
		}

		//Manejo de jugadores
		//If there is lees players than the maximun allowed add a new player and return the new player code otherwise returns error code 991
		int NewPlayer(){
			if(currentPlayers < NPLAYERS){
				currentPlayers +=1;
				return 	currentPlayers-1;		
			}else{
				return 991;
			}
		}

		int ValidPlayer(int playerCode){
			return playerCode < currentPlayers;
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

	//Showing initial board
	sudoku.Print();

    
    while(1){
		char buffer[10];
		zmq_recv (responder, buffer, 10, 0);
		
		//Either initialize a new player or make a move
		if(strcmp(buffer, "init")==0){
			cout << endl;
			cout << "Iniciando Jugador..." << endl;
			cout << "Inyectando nano recpetores clase beta..." << endl;
			int newPlayer = sudoku.NewPlayer();
			cout << "Bienvenido Jugador..." << newPlayer << endl;
			sudoku.ShowScore();
			sprintf(responseCode, "%d", newPlayer);
		}else{
		cout << endl;
		cout << "New movement:" << endl;
		sprintf(responseCode,"%d",sudoku.Play(buffer));
		}

		//Responding to client
		zmq_send (responder, responseCode , 5, 0);
    }
    return 0;
}
