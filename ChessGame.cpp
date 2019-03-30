#include <string>
#include <iostream>
#include <vector>
#include <windows.system.h>
#include <windows.system.display.h>

using namespace std;
using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Drawing2D;

// ENUMARATORS

enum ENUM_COLOR{
	NONE_COLOR, //=0
	BLACK_COLOR = -1,
	WHITE_COLOR = 1,
};

enum ENUM_MOVES{
	LEFT1,
	LEFT7,
	RIGHT1,
	RIGHT7,
	DOWN1,
	DOWN7,
	UP1,
	UP7,
	DIAGONAL_UP_LEFT1,
	DIAGONAL_UP_LEFT7,
	DIAGONAL_UP_RIGHT1,
	DIAGONAL_UP_RIGHT7,
	DIAGONAL_DOWN_LEFT1,
	DIAGONAL_DOWN_LEFT7,
	DIAGONAL_DOWN_RIGHT1,
	DIAGONAL_DOWN_RIGHT7
};

// CLASSES and types
// Declarations

class ChessPosition;
class ChessMove;
class ChessTool;
class ChessGame;

typedef ChessTool* ChessBOARD[8][8];

class ChessToolPawn;
class chessMoveUp1_NoKillCleanPath;
 
// *********************************

class ChessPosition{
	
	private:
		/*****/

	protected:

		int _x; // x=row
		int _y; // y=col

	public:

		ChessPosition(int x=-1, int y=-1): _x(x), _y(y){}

		int GetX() { return _x; }

		int GetY() { return _y; }

		void SetX(int x) { _x=x; }

		void SetY(int y) { _y=y; }

		void SetXY(int x, int y) { _x=x; _y=y; }

};

class ChessPlayer{

	private:
		/*****/

	protected:
		string _playerName;
		int _id;
		ENUM_COLOR _playerToolsColor;

	public:
		
		ChessPlayer(ENUM_COLOR playerToolsColor, string playerName="Anonymous", int id=-1): _playerToolsColor(playerToolsColor), _playerName(playerName), _id(id) {
		}

		ENUM_COLOR GetPlayerColor(){
			return _playerToolsColor;
		}

		string GetPlayerName(){
			return _playerName;
		}

		int GetPlayerId(){
			return _id;
		}

		void PrintPlayerDetails(){
			cout<<endl;
			cout<<"Player Name:\t"<<_playerName<<endl;
			cout<<"\tID:\t"<<_id<<endl;
			cout<<"Tools Color:\t"<<_playerToolsColor<<endl;
		}

		// Output: positionArray[0] = curPosition, positionArray[1] = nextPosition.
		// Retruns false if positions are not in boarders of the board. Otherwise return true.
		bool virtual ChooseMove(ChessPosition positionArray[2])
		{
			int curX;
			int curY;
			int nextX;
			int nextY;

			cout<<endl<<"enter current Position (x,y) to move from:";
			cin>>curX>>curY;
			cout<<endl<<"enter nextPosition (x_next, y_next) to move to:";
			cin>>nextX>>nextY;

			// Incrementing by 1 Since the user talks in language [1..8]x[1..8], 
			// therefore transformed to [0..7]x[0..7].
			curX--;
			curY--;
			nextX--;
			nextY--;

			positionArray[0] = ChessPosition(curX,curY);
			positionArray[1] = ChessPosition(nextX,nextY);

			// verify legals next position and current position.
			if(	curX>=8 || curX<0 || curY>=8 || curY<0 ||
				nextX>=8 || nextX<0 || nextY>=8 || nextY<0 )
			{
				cout<<"Illegal move (positions are not in boarders)!"<<endl;
				return false;
			}

			return true;
		}

		virtual ~ChessPlayer(){
		}

};

// Abstract Class ChessTool.
class ChessTool{
	
	private:
		/*****/

	protected:

		ENUM_COLOR _toolColor;
		vector<ChessMove*> _toolMoves;
		int _toolRank; // BlackRank < 0. WhiteRank > 0.
		bool _enableFlag;

	public:

		ChessTool(ENUM_COLOR toolColor = BLACK_COLOR, int toolRank = -1): _toolColor(toolColor), _toolRank(toolRank*toolColor/* T.B.D */), _enableFlag(true){}

		ENUM_COLOR GetToolColor(){
			return _toolColor;
		}

		int GetRank(){ return _toolRank; }

		// enabled --> disabled, disabled --> enabled.
		void ChangeEnableStatus()
		{
			_enableFlag = !_enableFlag;
		}

		bool GetEnabledStatus()
		{
			return _enableFlag;
		}

		void virtual DrawTool(bool showToolRank = false) = 0; // Pure Virtual

		bool isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board);

		virtual ~ChessTool(){
			vector<ChessMove*>::size_type sz = _toolMoves.size();
			//delete all moves.
			for(int i = 0; i < sz; i++)
			{
				delete(_toolMoves[i]);
				//flag = _toolMoves.at(i)->isValidMove(curPosition, nextPosition, board); // An alternative way.
			}
		}

};

// Abstract Class ChessMove.
class ChessMove{
	
	private:
		/*****/

	protected:

	public:

		// GetNextValidPositions returned in vector<ChessPosition*> ChessNextPositions. assumptions: ChessNextPositions is emepty.
		void virtual GetNextValidPositions(ChessPosition& curPosition,  const ChessBOARD& board, vector<ChessPosition*> ChessNextPositions) = 0; // Pure Virtual.

		bool virtual isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board) = 0; // Pure Virtual.

		virtual ~ChessMove(){
		}
};

// Concrete ChessMove inherited Classes

// can move up Not for killing.
class ChessMoveUp1_NoKill: public ChessMove{
	
	private:
		/*****/

	protected:

	public:

		bool virtual isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board){

			// verify legal current position.
			if (curPosition.GetX()>=8 || curPosition.GetX()<0 || curPosition.GetY()>=8 || curPosition.GetY()<0)
			{
				return false;
			}
			// verify legal next position.
			if (nextPosition.GetX()>=8 || nextPosition.GetX()<0 || nextPosition.GetY()>=8 || nextPosition.GetY()<0)
			{
				return false;
			}

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return false;
			}

			int curX = curPosition.GetX();
			int curY = curPosition.GetY();
			int nextX = nextPosition.GetX();
			int nextY = nextPosition.GetY();

			/*
				0| - - -   -   - - - |    ^        y ==>
				1| - - - nextX - - - |   /|\ Up
				2| - - - curX  - - - |    |	x
			*/

			// Not 1 step Up direction
			if( ! ((nextX == (curX-1)) && (nextY == curY)) ){
				return false;
			}

			// NextPosition is occupied by myself
			if( (board[nextX][nextY] != NULL) && // nextPosition is occupied
				(board[nextX][nextY]->GetToolColor() == board[curX][curY]->GetToolColor()) // nextPosition is myself.
			) 
			{
				return false;
			} 

			// No Killing allowed.
			if( (board[nextX][nextY] != NULL) && // nextPosition is occupied
				(board[nextX][nextY]->GetToolColor() != board[curX][curY]->GetToolColor()) // nextPosition is the oppnent.
			) 
			{
				return false;
			} 

			return true;
		}

		// GetNextValidPositions returned in vector<ChessPosition*> ChessNextPositions. Note: ChessNextPositions is handeled by the caller, ChessNextPositions will be added to it.
		void virtual GetNextValidPositions(ChessPosition& curPosition,  const ChessBOARD& board, vector<ChessPosition*> ChessNextPositions){

			// T.B.D

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return;
			}

			int NextX = curPosition.GetX()+1;
			int NextY = curPosition.GetY();

			if (board[NextX][NextY] == NULL)
			{
				ChessNextPositions.push_back(new ChessPosition(NextX, NextY));
			}

		}

		~ChessMoveUp1_NoKill(){
		}

};

// can move up Not for killing.
class ChessMoveDown1_NoKill: public ChessMove{
	
	private:
		/*****/

	protected:

	public:

		bool virtual isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board){

			// verify legal current position.
			if (curPosition.GetX()>=8 || curPosition.GetX()<0 || curPosition.GetY()>=8 || curPosition.GetY()<0)
			{
				return false;
			}
			// verify legal next position.
			if (nextPosition.GetX()>=8 || nextPosition.GetX()<0 || nextPosition.GetY()>=8 || nextPosition.GetY()<0)
			{
				return false;
			}

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return false;
			}

			int curX = curPosition.GetX();
			int curY = curPosition.GetY();
			int nextX = nextPosition.GetX();
			int nextY = nextPosition.GetY();

			/*
				0| - - -   -     - - - |   y ==>
				1| - - - curX=1  - - - |   
				2| - - - nextX=2 - - - |    
			*/

			// Not 1 step down direction
			if( ! ((nextX == (curX+1)) && (nextY == curY)) ){
				return false;
			}

			// NextPosition is occupied by myself
			if( (board[nextX][nextY] != NULL) && // nextPosition is occupied
				(board[nextX][nextY]->GetToolColor() == board[curX][curY]->GetToolColor()) // nextPosition is the myself.
			) 
			{
				return false;
			} 

			// No Killing allowed.
			if( (board[nextX][nextY] != NULL) && // nextPosition is occupied
				(board[nextX][nextY]->GetToolColor() != board[curX][curY]->GetToolColor()) // nextPosition is the oppnent.
			) 
			{
				return false;
			} 

			return true;
		}

		// GetNextValidPositions returned in vector<ChessPosition*> ChessNextPositions. Note: ChessNextPositions is handeled by the caller, ChessNextPositions will be added to it.
		void virtual GetNextValidPositions(ChessPosition& curPosition,  const ChessBOARD& board, vector<ChessPosition*> ChessNextPositions){

			// T.B.D

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return;
			}

			int NextX = curPosition.GetX()+1;
			int NextY = curPosition.GetY();

			if (board[NextX][NextY] == NULL)
			{
				ChessNextPositions.push_back(new ChessPosition(NextX, NextY));
			}

		}

		~ChessMoveDown1_NoKill(){
		}

};

// can move 1 step diagonal up-right or up-left to kill.
class ChessMoveDiagonalUpRightLeft1_OnlyForKill: public ChessMove{
	
	private:
		/*****/

	protected:

	public:

		bool virtual isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board){

			// verify legal current position.
			if (curPosition.GetX()>=8 || curPosition.GetX()<0 || curPosition.GetY()>=8 || curPosition.GetY()<0)
			{
				return false;
			}
			// verify legal next position.
			if (nextPosition.GetX()>=8 || nextPosition.GetX()<0 || nextPosition.GetY()>=8 || nextPosition.GetY()<0)
			{
				return false;
			}

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return false;
			}

			int curX = curPosition.GetX();
			int curY = curPosition.GetY();
			int nextX = nextPosition.GetX();
			int nextY = nextPosition.GetY();

			/*	   0 1 2         3     4    5  6
				0| - -    -      -     -     - - |   y ==>
				1| - - nextX=1   -   nextX=1 - - |   
				2| - -    -   curX=2   -     - - |    
			*/

			// Not 1 step diagonal up-left/right
			if( ! ( 
					(nextX == (curX-1)) && 
				    (  (nextY == (curY+1)) || (nextY == (curY-1)) )
				)
			)
			{
				return false;
			} 

			// only Killing allowed
			if( ! (
					(board[nextX][nextY] != NULL) && // nextPosition is occupied
					(board[nextX][nextY]->GetToolColor() != board[curX][curY]->GetToolColor()) // nextPosition is the oppnent.
				)
			) 
			{
				return false;
			} 

			// We get here only if in boarders, 1 step-up-left/right, and killing.
			return true;
		}

		// GetNextValidPositions returned in vector<ChessPosition*> ChessNextPositions. Note: ChessNextPositions is handeled by the caller, ChessNextPositions will be added to it.
		void virtual GetNextValidPositions(ChessPosition& curPosition,  const ChessBOARD& board, vector<ChessPosition*> ChessNextPositions)
		{
			//T.B.D
		}

		~ChessMoveDiagonalUpRightLeft1_OnlyForKill(){
		}

};

	// can move 1 step diagonal down-right or down-left to kill.
class ChessMoveDiagonalDownRightLeft1_OnlyForKill: public ChessMove{
	
	private:
		/*****/

	protected:

	public:

		bool virtual isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board){

			// verify legal current position.
			if (curPosition.GetX()>=8 || curPosition.GetX()<0 || curPosition.GetY()>=8 || curPosition.GetY()<0)
			{
				return false;
			}
			// verify legal next position.
			if (nextPosition.GetX()>=8 || nextPosition.GetX()<0 || nextPosition.GetY()>=8 || nextPosition.GetY()<0)
			{
				return false;
			}

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return false;
			}

			int curX = curPosition.GetX();
			int curY = curPosition.GetY();
			int nextX = nextPosition.GetX();
			int nextY = nextPosition.GetY();

			/*	   0 1 2         3     4    5  6
				0| - -    -      -     -     - - |   y ==>
				1| - -    -   curX=1   -     - - |   
				2| - - nextX=2   -   nextX=2 - - |    
			*/

			// Not 1 step diagonal down-left/right
			if( ! ( 
					(nextX == (curX+1)) && 
				    (  (nextY == (curY+1)) || (nextY == (curY-1)) )
				)
			)
			{
				return false;
			} 

			// Only Killing allowed
			if( ! (
					(board[nextX][nextY] != NULL) && // nextPosition is occupied
					(board[nextX][nextY]->GetToolColor() != board[curX][curY]->GetToolColor()) // nextPosition is the oppnent.
				)
			) 
			{
				return false;
			} 

			// We get here only if in boarders, 1 step-up-left/right, and killing.
			return true;
		}

		// GetNextValidPositions returned in vector<ChessPosition*> ChessNextPositions. Note: ChessNextPositions is handeled by the caller, ChessNextPositions will be added to it.
		void virtual GetNextValidPositions(ChessPosition& curPosition,  const ChessBOARD& board, vector<ChessPosition*> ChessNextPositions)
		{
			//T.B.D
		}

		~ChessMoveDiagonalDownRightLeft1_OnlyForKill(){
		}

};

// can move up 1 step.
class ChessMoveUp1: public ChessMove{
	
	private:
		/*****/

	protected:

	public:

		bool virtual isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board){

			// verify legal current position.
			if (curPosition.GetX()>=8 || curPosition.GetX()<0 || curPosition.GetY()>=8 || curPosition.GetY()<0)
			{
				return false;
			}
			// verify legal next position.
			if (nextPosition.GetX()>=8 || nextPosition.GetX()<0 || nextPosition.GetY()>=8 || nextPosition.GetY()<0)
			{
				return false;
			}

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return false;
			}

			int curX = curPosition.GetX();
			int curY = curPosition.GetY();
			int nextX = nextPosition.GetX();
			int nextY = nextPosition.GetY();

			/*
				0| - - -    -    - - - |    ^        y ==>
				1| - - - nextX=1 - - - |   /|\ Up
				2| - - - curX=2  - - - |    |	x
			*/

			// Not 1 step Up direction
			if( ! ((nextX == (curX-1)) && (nextY == curY)) ){
				return false;
			}

			// NextPosition is occupied by myself
			if( (board[nextX][nextY] != NULL) && // nextPosition is occupied
				(board[nextX][nextY]->GetToolColor() == board[curX][curY]->GetToolColor()) // nextPosition is myself.
			) 
			{
				return false;
			}  

			return true;
		}

		// GetNextValidPositions returned in vector<ChessPosition*> ChessNextPositions. Note: ChessNextPositions is handeled by the caller, ChessNextPositions will be added to it.
		void virtual GetNextValidPositions(ChessPosition& curPosition,  const ChessBOARD& board, vector<ChessPosition*> ChessNextPositions)
		{
			//T.B.D
		}

		~ChessMoveUp1(){
		}

};

// can move down 1 step.
class ChessMoveDown1: public ChessMove{
	
	private:
		/*****/

	protected:

	public:

		bool virtual isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board){

			// verify legal current position.
			if (curPosition.GetX()>=8 || curPosition.GetX()<0 || curPosition.GetY()>=8 || curPosition.GetY()<0)
			{
				return false;
			}
			// verify legal next position.
			if (nextPosition.GetX()>=8 || nextPosition.GetX()<0 || nextPosition.GetY()>=8 || nextPosition.GetY()<0)
			{
				return false;
			}

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return false;
			}

			int curX = curPosition.GetX();
			int curY = curPosition.GetY();
			int nextX = nextPosition.GetX();
			int nextY = nextPosition.GetY();

			/*
				0| - - - curX=0  - - - |       y ==>
				1| - - - nextX=1 - - - |   
				2| - - -    -    - - - |  
			*/

			// Not down direction
			if( ! ((nextX == (curX+1)) && (nextY == curY)) ){
				return false;
			}

			// NextPosition is occupied by myself
			if( (board[nextX][nextY] != NULL) && // nextPosition is occupied
				(board[nextX][nextY]->GetToolColor() == board[curX][curY]->GetToolColor()) // nextPosition is the oppnent.
			) 
			{
				return false;
			}  

			return true;
		}

		// GetNextValidPositions returned in vector<ChessPosition*> ChessNextPositions. Note: ChessNextPositions is handeled by the caller, ChessNextPositions will be added to it.
		void virtual GetNextValidPositions(ChessPosition& curPosition,  const ChessBOARD& board, vector<ChessPosition*> ChessNextPositions)
		{
			//T.B.D
		}

		~ChessMoveDown1(){
		}

};

// can move Left 1 step.
class ChessMoveLeft1: public ChessMove{
	
	private:
		/*****/

	protected:

	public:

		bool virtual isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board){

			// verify legal current position.
			if (curPosition.GetX()>=8 || curPosition.GetX()<0 || curPosition.GetY()>=8 || curPosition.GetY()<0)
			{
				return false;
			}
			// verify legal next position.
			if (nextPosition.GetX()>=8 || nextPosition.GetX()<0 || nextPosition.GetY()>=8 || nextPosition.GetY()<0)
			{
				return false;
			}

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return false;
			}

			int curX = curPosition.GetX();
			int curY = curPosition.GetY();
			int nextX = nextPosition.GetX();
			int nextY = nextPosition.GetY();

			/*
				0| - - nextY=1 curY=2  - - |       y ==>
				1| - -   -         -   - - |   
				2| - -   -         -   - - |  
			*/

			// Not 1 step Left direction
			if( ! ((nextX == curX) && (nextY == (curY-1) )) ){
				return false;
			}

			// NextPosition is occupied by myself
			if( (board[nextX][nextY] != NULL) && // nextPosition is occupied
				(board[nextX][nextY]->GetToolColor() == board[curX][curY]->GetToolColor()) // nextPosition is the oppnent.
			) 
			{
				return false;
			}  

			return true;
		}

		// GetNextValidPositions returned in vector<ChessPosition*> ChessNextPositions. Note: ChessNextPositions is handeled by the caller, ChessNextPositions will be added to it.
		void virtual GetNextValidPositions(ChessPosition& curPosition,  const ChessBOARD& board, vector<ChessPosition*> ChessNextPositions)
		{
			//T.B.D
		}

		~ChessMoveLeft1(){
		}

};

// can move Right 1 step.
class ChessMoveRight1: public ChessMove{
	
	private:
		/*****/

	protected:

	public:

		bool virtual isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board){

			// verify legal current position.
			if (curPosition.GetX()>=8 || curPosition.GetX()<0 || curPosition.GetY()>=8 || curPosition.GetY()<0)
			{
				return false;
			}
			// verify legal next position.
			if (nextPosition.GetX()>=8 || nextPosition.GetX()<0 || nextPosition.GetY()>=8 || nextPosition.GetY()<0)
			{
				return false;
			}

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return false;
			}

			int curX = curPosition.GetX();
			int curY = curPosition.GetY();
			int nextX = nextPosition.GetX();
			int nextY = nextPosition.GetY();

			/*
				0| - - curY=1 nextY=2  - - |       y ==>
				1| - -   -         -   - - |   
				2| - -   -         -   - - |  
			*/

			// Not 1 step Right direction
			if( ! ((nextX == curX) && (nextY == (curY+1) )) ){
				return false;
			}

			// NextPosition is occupied by myself
			if( (board[nextX][nextY] != NULL) && // nextPosition is occupied
				(board[nextX][nextY]->GetToolColor() == board[curX][curY]->GetToolColor()) // nextPosition is the oppnent.
			) 
			{
				return false;
			}  

			return true;
		}

		// GetNextValidPositions returned in vector<ChessPosition*> ChessNextPositions. Note: ChessNextPositions is handeled by the caller, ChessNextPositions will be added to it.
		void virtual GetNextValidPositions(ChessPosition& curPosition,  const ChessBOARD& board, vector<ChessPosition*> ChessNextPositions)
		{
			//T.B.D
		}

		~ChessMoveRight1(){
		}

};

// can move 1 step diagonal to all 4 directions: up-left, up-right, down-left, down-right.
class ChessMoveDiagonalAllDirections1: public ChessMove{
	
	private:
		/*****/

	protected:

	public:

		bool virtual isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board){

			// verify legal current position.
			if (curPosition.GetX()>=8 || curPosition.GetX()<0 || curPosition.GetY()>=8 || curPosition.GetY()<0)
			{
				return false;
			}
			// verify legal next position.
			if (nextPosition.GetX()>=8 || nextPosition.GetX()<0 || nextPosition.GetY()>=8 || nextPosition.GetY()<0)
			{
				return false;
			}

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return false;
			}

			int curX = curPosition.GetX();
			int curY = curPosition.GetY();
			int nextX = nextPosition.GetX();
			int nextY = nextPosition.GetY();

			/*	     0 1 2         3     4    5  6
			  x	  0| - -    -      -     -     - - |   y ==>
			  x	  1| - - nextX=1   -   nextX=1 - - |   
			  x	  2| - -    -   curX=2   -     - - |
			  x	  3| - - nextX=3   -   nextX=3 - - |
			*/

			// Not 1 step diagonal up-left, up-right, down-left, down-right.
			if( ! ( 
					(
						(nextX == (curX-1)) && 
						(  (nextY == (curY+1)) || (nextY == (curY-1)) )
					)
					||
					( 
						(nextX == (curX+1)) && 
						(  (nextY == (curY+1)) || (nextY == (curY-1)) )
					)

				)
			)
			{
				return false;
			} 

			// NextPosition is occupied by myself
			if( (board[nextX][nextY] != NULL) && // nextPosition is occupied
				(board[nextX][nextY]->GetToolColor() == board[curX][curY]->GetToolColor()) // nextPosition is myself.
			) 
			{
				return false;
			}

			// We get here only if in boarders, 1 step-up-left/right, and killing.
			return true;
		}

		// GetNextValidPositions returned in vector<ChessPosition*> ChessNextPositions. Note: ChessNextPositions is handeled by the caller, ChessNextPositions will be added to it.
		void virtual GetNextValidPositions(ChessPosition& curPosition,  const ChessBOARD& board, vector<ChessPosition*> ChessNextPositions)
		{
			//T.B.D
		}

		~ChessMoveDiagonalAllDirections1(){
		}

};

// can move 2-1 steps: 2up-1left, 2up-1right, 2down-1left, 2down-1right, 2left-1up, 2left-1down, 2right-1up, 2right-1down
class ChessMoveAllDirections_2_1_Steps: public ChessMove{
	
	private:
		/*****/

	protected:

	public:

		bool virtual isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board){

			// verify legal current position.
			if (curPosition.GetX()>=8 || curPosition.GetX()<0 || curPosition.GetY()>=8 || curPosition.GetY()<0)
			{
				return false;
			}
			// verify legal next position.
			if (nextPosition.GetX()>=8 || nextPosition.GetX()<0 || nextPosition.GetY()>=8 || nextPosition.GetY()<0)
			{
				return false;
			}

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return false;
			}

			int curX = curPosition.GetX();
			int curY = curPosition.GetY();
			int nextX = nextPosition.GetX();
			int nextY = nextPosition.GetY();

			/*	    
			  x	  0|  -   -   -   -   -   -   -   -   - |   y ==>
			  x	  1|  -   -   -   -   -  next -  next - |
			  x	  2|  -   -   -   -  next -   -   - next|
			  x	  3|  -   -   -   -   -   -  cur  -   - |
			  x	  4|  -   -   -   -  next -   -   - next|
			  x	  5|  -   -   -   -   -  next -  next - |
			  x	  6|  -   -   -   -   -   -   -   -   - |
			  x	  7|  -   -   -   -   -   -   -   -   - |
			*/

			// Not Neither 2up-1left nor 2up-1right nor 2down-1left nor 2down-1right 
			//		   nor 2left-1up nor 2left-1down nor 2right-1up nor 2right-1down.
			//	Note:	when next is on the edge conditions of points adding/subtracing 
			//			1/2 can't be satisfied since cur is also inside (we cheked earlier). 
			if( ! ( 
					(
						(curX == (nextX+1)) && (curY == (nextY+2))
					)
					||
					( 
						(curX == (nextX+1)) && (curY == (nextY-2))
					)
					||
					(
						(curX == (nextX-1)) && (curY == (nextY+2))
					)
					||
					(
						(curX == (nextX-1)) && (curY == (nextY-2))
					)
					||
					(
						(curX == (nextX+2)) && (curY == (nextY-1))
					)
					||
					(
						(curX == (nextX+2)) && (curY == (nextY+1))
					)
					||
					(
						(curX == (nextX-2)) && (curY == (nextY+1))
					)
					||
					(
						(curX == (nextX-2)) && (curY == (nextY-1))
					)
				)
			)
			{
				return false;
			} 

			// NextPosition is occupied by myself
			if( (board[nextX][nextY] != NULL) && // nextPosition is occupied
				(board[nextX][nextY]->GetToolColor() == board[curX][curY]->GetToolColor()) // nextPosition is myself.
			) 
			{
				return false;
			}

			// We get here only if in boarders, 1 step-up-left/right, and killing.
			return true;
		}

		// GetNextValidPositions returned in vector<ChessPosition*> ChessNextPositions. Note: ChessNextPositions is handeled by the caller, ChessNextPositions will be added to it.
		void virtual GetNextValidPositions(ChessPosition& curPosition,  const ChessBOARD& board, vector<ChessPosition*> ChessNextPositions)
		{
			//T.B.D
		}

		~ChessMoveAllDirections_2_1_Steps(){
		}

};

// can move up to 7 - for a position if path upward is clean.
class ChessMoveUp7_CleanPath: public ChessMove{
	
	private:
		/*****/

	protected:

	public:

		bool virtual isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board){

			// verify legal current position.
			if (curPosition.GetX()>=8 || curPosition.GetX()<0 || curPosition.GetY()>=8 || curPosition.GetY()<0)
			{
				return false;
			}
			// verify legal next position.
			if (nextPosition.GetX()>=8 || nextPosition.GetX()<0 || nextPosition.GetY()>=8 || nextPosition.GetY()<0)
			{
				return false;
			}

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return false;
			}

			int curX = curPosition.GetX();
			int curY = curPosition.GetY();
			int nextX = nextPosition.GetX();
			int nextY = nextPosition.GetY();

			/*
				0| - - - nextX=0 - - - |    ^        y ==>
				1| - - -    -    - - - |   /|\ Up
				2| - - - curX=2  - - - |    |	x
			*/

			// Not Up direction
			if( ! ((nextX < curX) && (nextY == curY)) ){
				return false;
			}

			// Path up (not including nextPosition) is not clean of tools
			bool flag = true;
			for( int iX=1 ; (iX < abs(nextX-curX) ) && (flag == true) ; iX++ )
			{ // iX is the next node in path.
				if (board[curX-iX][curY] != NULL){
					flag = false; // Next Node in path is occupied. thus, Path up (not including nextPosition) is not clean of tools
				}
			}
			if (flag == false)
			{
				return false;
			}

			/* A nother way for the rule: Path (not including nextPosition) is not clean from tools
			int iX;
			for( iX=1 ; (iX < (nextX-curX) ) && (board[curX+iX][curY] != NULL) ; iX++ );
			if ( iX < (nextX-curX) )
			{
				return false;
			}
			*/

			// NextPosition is occupied by myself
			if( (board[nextX][nextY] != NULL) && // nextPosition is occupied
				(board[nextX][nextY]->GetToolColor() == board[curX][curY]->GetToolColor()) // nextPosition is myself.
			) 
			{
				return false;
			}  

			return true;
		}

		// GetNextValidPositions returned in vector<ChessPosition*> ChessNextPositions. Note: ChessNextPositions is handeled by the caller, ChessNextPositions will be added to it.
		void virtual GetNextValidPositions(ChessPosition& curPosition,  const ChessBOARD& board, vector<ChessPosition*> ChessNextPositions)
		{
			//T.B.D
		}

		~ChessMoveUp7_CleanPath(){
		}

};

// can move down 7 - for a position if path is clean.
class ChessMoveDown7_CleanPath: public ChessMove{
	
	private:
		/*****/

	protected:

	public:

		bool virtual isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board){

			// verify legal current position.
			if (curPosition.GetX()>=8 || curPosition.GetX()<0 || curPosition.GetY()>=8 || curPosition.GetY()<0)
			{
				return false;
			}
			// verify legal next position.
			if (nextPosition.GetX()>=8 || nextPosition.GetX()<0 || nextPosition.GetY()>=8 || nextPosition.GetY()<0)
			{
				return false;
			}

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return false;
			}

			int curX = curPosition.GetX();
			int curY = curPosition.GetY();
			int nextX = nextPosition.GetX();
			int nextY = nextPosition.GetY();

			/*
				0| - - - curX=0  - - - |    ^        y ==>
				1| - - -    -    - - - |   /|\ Up
				2| - - - nextX=2 - - - |    |	x
			*/

			// Not down direction
			if( ! ((nextX > curX) && (nextY == curY)) ){
				return false;
			}

			// Path down (not including nextPosition) is not clean of tools
			bool flag = true;
			for( int iX=1 ; (iX < abs(nextX-curX) ) && (flag == true) ; iX++ )
			{ // iX is the next node in path.
				if (board[curX+iX][curY] != NULL){
					flag = false; // Next Node in path is occupied. thus, Path down (not including nextPosition) is not clean of tools
				}
			}
			if (flag == false)
			{
				return false;
			}

			/* A nother way for the rule: Path (not including nextPosition) is not clean from tools
			int iX;
			for( iX=1 ; (iX < (nextX-curX) ) && (board[curX+iX][curY] != NULL) ; iX++ );
			if ( iX < (nextX-curX) )
			{
				return false;
			}
			*/

			// NextPosition is occupied by myself
			if( (board[nextX][nextY] != NULL) && // nextPosition is occupied
				(board[nextX][nextY]->GetToolColor() == board[curX][curY]->GetToolColor()) // nextPosition is the oppnent.
			) 
			{
				return false;
			}  

			return true;
		}

		// GetNextValidPositions returned in vector<ChessPosition*> ChessNextPositions. Note: ChessNextPositions is handeled by the caller, ChessNextPositions will be added to it.
		void virtual GetNextValidPositions(ChessPosition& curPosition,  const ChessBOARD& board, vector<ChessPosition*> ChessNextPositions)
		{
			//T.B.D
		}

		~ChessMoveDown7_CleanPath(){
		}

};

// can move left 7 - for a position if path is clean.
class ChessMoveLeft7_CleanPath: public ChessMove{
	
	private:
		/*****/

	protected:

	public:

		bool virtual isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board){

			// verify legal current position.
			if (curPosition.GetX()>=8 || curPosition.GetX()<0 || curPosition.GetY()>=8 || curPosition.GetY()<0)
			{
				return false;
			}
			// verify legal next position.
			if (nextPosition.GetX()>=8 || nextPosition.GetX()<0 || nextPosition.GetY()>=8 || nextPosition.GetY()<0)
			{
				return false;
			}

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return false;
			}

			int curX = curPosition.GetX();
			int curY = curPosition.GetY();
			int nextX = nextPosition.GetX();
			int nextY = nextPosition.GetY();

			/*
				0| - nextY=1 - curY=3  - - |      y ==>
				1| - - -     -    -    - - |   
				2| - - -     -    -    - - |    
			*/

			// Not Left direction
			if( ! ((nextX == curX) && (nextY < curY)) ){
				return false;
			}

			// Path down (not including nextPosition) is not clean of tools
			bool flag = true;
			for( int iY=1 ; (iY < abs(nextY-curY) ) && (flag == true) ; iY++ )
			{ // iX is the next node in path.
				if (board[curX][curY-iY] != NULL){
					flag = false; // Next Node in path is occupied. thus, Path down (not including nextPosition) is not clean of tools
				}
			}
			if (flag == false)
			{
				return false;
			}

			/* A nother way for the rule: Path (not including nextPosition) is not clean from tools
			int iX;
			for( iX=1 ; (iX < (nextX-curX) ) && (board[curX+iX][curY] != NULL) ; iX++ );
			if ( iX < (nextX-curX) )
			{
				return false;
			}
			*/

			// NextPosition is occupied by myself
			if( (board[nextX][nextY] != NULL) && // nextPosition is occupied
				(board[nextX][nextY]->GetToolColor() == board[curX][curY]->GetToolColor()) // nextPosition is the oppnent.
			) 
			{
				return false;
			}  

			return true;
		}

		// GetNextValidPositions returned in vector<ChessPosition*> ChessNextPositions. Note: ChessNextPositions is handeled by the caller, ChessNextPositions will be added to it.
		void virtual GetNextValidPositions(ChessPosition& curPosition,  const ChessBOARD& board, vector<ChessPosition*> ChessNextPositions)
		{
			//T.B.D
		}

		~ChessMoveLeft7_CleanPath(){
		}

};

// can move right 7 - for a position if path is clean.
class ChessMoveRight7_CleanPath: public ChessMove{
	
	private:
		/*****/

	protected:

	public:

		bool virtual isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board){

			// verify legal current position.
			if (curPosition.GetX()>=8 || curPosition.GetX()<0 || curPosition.GetY()>=8 || curPosition.GetY()<0)
			{
				return false;
			}
			// verify legal next position.
			if (nextPosition.GetX()>=8 || nextPosition.GetX()<0 || nextPosition.GetY()>=8 || nextPosition.GetY()<0)
			{
				return false;
			}

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return false;
			}

			int curX = curPosition.GetX();
			int curY = curPosition.GetY();
			int nextX = nextPosition.GetX();
			int nextY = nextPosition.GetY();

			/*
				0| -  curY=1 - nextY=3 - - |      y ==>
				1| - - -     -    -    - - |   
				2| - - -     -    -    - - |    
			*/

			// Not Left direction
			if( ! ((nextX == curX) && (nextY > curY)) ){
				return false;
			}

			// Path down (not including nextPosition) is not clean of tools
			bool flag = true;
			for( int iY=1 ; (iY < abs(nextY-curY) ) && (flag == true) ; iY++ )
			{ // iX is the next node in path.
				if (board[curX][curY+iY] != NULL){
					flag = false; // Next Node in path is occupied. thus, Path down (not including nextPosition) is not clean of tools
				}
			}
			if (flag == false)
			{
				return false;
			}

			/* A nother way for the rule: Path (not including nextPosition) is not clean from tools
			int iX;
			for( iX=1 ; (iX < (nextX-curX) ) && (board[curX+iX][curY] != NULL) ; iX++ );
			if ( iX < (nextX-curX) )
			{
				return false;
			}
			*/

			// NextPosition is occupied by myself
			if( (board[nextX][nextY] != NULL) && // nextPosition is occupied
				(board[nextX][nextY]->GetToolColor() == board[curX][curY]->GetToolColor()) // nextPosition is the oppnent.
			) 
			{
				return false;
			}  

			return true;
		}

		// GetNextValidPositions returned in vector<ChessPosition*> ChessNextPositions. Note: ChessNextPositions is handeled by the caller, ChessNextPositions will be added to it.
		void virtual GetNextValidPositions(ChessPosition& curPosition,  const ChessBOARD& board, vector<ChessPosition*> ChessNextPositions)
		{
			//T.B.D
		}

		~ChessMoveRight7_CleanPath(){
		}

};

// can move right 7 - for a position if path is clean.
class ChessMoveDiagonalAllDirections7_CleanPath: public ChessMove{
	
	private:
		/*****/

	protected:

	public:

		bool virtual isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board){

			// verify legal current position.
			if (curPosition.GetX()>=8 || curPosition.GetX()<0 || curPosition.GetY()>=8 || curPosition.GetY()<0)
			{
				return false;
			}
			// verify legal next position.
			if (nextPosition.GetX()>=8 || nextPosition.GetX()<0 || nextPosition.GetY()>=8 || nextPosition.GetY()<0)
			{
				return false;
			}

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return false;
			}

			int curX = curPosition.GetX();
			int curY = curPosition.GetY();
			int nextX = nextPosition.GetX();
			int nextY = nextPosition.GetY();
			int diffX = nextX-curX;
			int diffY = nextY-curY;

			/*
				0| - - next -  -  -  next  - - |      y ==>
				1| - -   -  -  -  -   -    - - |
				2| - -   -  - cur -   -    - - |
				3| - -   -  -  -  -   -    - - |
				4| - - next -  -  -  next  - - |    
			*/

			// Not Diag direction
			if( ! ((abs(diffX) == abs(diffY)) && (diffX != 0)) )
			{ // Note that if (abs(diffX) == abs(diffY)) then if diffX!=0 then also diffY!=0
				return false;
			}
			
			// Path diagonal (not including nextPosition) is not clean of tools
			bool flag = true;

			if ( (diffX > 0) && (diffY > 0) )
			{				
				for( int iX=1, iY=1 ; (iY < abs(diffY)) && (flag == true) ; iY++, iX++ )
				{ // (iX,iY) is for the next node in path.
					if (board[curX+iX][curY+iY] != NULL)
					{
						flag = false; // Next Node in path is occupied. thus, Path down (not including nextPosition) is not clean of tools
					}
				}
			}
			else if ( (diffX > 0) && (diffY < 0) ) 
			{
				for( int iX=1, iY=1 ; (iY < abs(diffY)) && (flag == true) ; iY++, iX++ )
				{ // (iX,iY) is for the next node in path.
					if (board[curX+iX][curY-iY] != NULL)
					{
						flag = false; // Next Node in path is occupied. thus, Path down (not including nextPosition) is not clean of tools
					}
				}
			}
			else if ( (diffX < 0) && (diffY > 0) ) 
			{
				for( int iX=1, iY=1 ; (iY < abs(diffY)) && (flag == true) ; iY++, iX++ )
				{ // (iX,iY) is for the next node in path.
					if (board[curX-iX][curY+iY] != NULL)
					{
						flag = false; // Next Node in path is occupied. thus, Path down (not including nextPosition) is not clean of tools
					}
				}
			}
			else // i.e. if( (diffX < 0) && (diffY < 0) )
			{
				for( int iX=1, iY=1 ; (iY < abs(diffY)) && (flag == true) ; iY++, iX++ )
				{ // (iX,iY) is for the next node in path.
					if (board[curX-iX][curY-iY] != NULL)
					{
						flag = false; // Next Node in path is occupied. thus, Path down (not including nextPosition) is not clean of tools
					}
				}
			}

			if (flag == false)
			{
				return false;
			}

			// NextPosition is occupied by myself
			if( (board[nextX][nextY] != NULL) && // nextPosition is occupied
				(board[nextX][nextY]->GetToolColor() == board[curX][curY]->GetToolColor()) // nextPosition is the oppnent.
			) 
			{
				return false;
			}  

			return true;
		}

		// GetNextValidPositions returned in vector<ChessPosition*> ChessNextPositions. Note: ChessNextPositions is handeled by the caller, ChessNextPositions will be added to it.
		void virtual GetNextValidPositions(ChessPosition& curPosition,  const ChessBOARD& board, vector<ChessPosition*> ChessNextPositions)
		{
			//T.B.D
		}

		~ChessMoveDiagonalAllDirections7_CleanPath(){
		}

};

bool ChessTool:: isValidMove(ChessPosition& curPosition, ChessPosition& nextPosition, const ChessBOARD& board){
			
			// verify legal current position.
			if (curPosition.GetX()>=8 || curPosition.GetX()<0 || curPosition.GetY()>=8 || curPosition.GetY()<0)
			{
				return false;
			}

			// verify legal next position.
			if (nextPosition.GetX()>=8 || nextPosition.GetX()<0 || nextPosition.GetY()>=8 || nextPosition.GetY()<0)
			{
				return false;
			}

			// Verify attendance in current position.
			if(board[curPosition.GetX()][curPosition.GetY()] == NULL)
			{
				return false;
			}

			bool flag = false;
			vector<ChessMove*>::size_type sz = _toolMoves.size();

			//for every move in _tool_moves and while flag == false
			for (int i = 0; (i < sz) && (flag == false); i++)
			{
				flag = _toolMoves[i]->isValidMove(curPosition, nextPosition, board);
				//flag = _toolMoves.at(i)->isValidMove(curPosition, nextPosition, board); // An alternative way.
			}

			return flag;

		}

// Concrete ChessTool inherited Classes

class ChessToolPawn: public ChessTool{
	
	private:
		/*****/

	protected:
		
	public:

		// Assumption: Black located up the board, White located down the board.
		ChessToolPawn(ENUM_COLOR toolColor = BLACK_COLOR, int toolRank = 1): ChessTool(toolColor, toolRank)
		{
			if( toolColor == BLACK_COLOR)
			{ // BLACK_COLOR PAWN
				_toolMoves.push_back(new ChessMoveDown1_NoKill);
				_toolMoves.push_back(new ChessMoveDiagonalDownRightLeft1_OnlyForKill);
			}
			else
			{ // WHITE_COLOR PAWN
				_toolMoves.push_back(new ChessMoveUp1_NoKill);	
				_toolMoves.push_back(new ChessMoveDiagonalUpRightLeft1_OnlyForKill);
			}
		}

		void virtual DrawTool(bool showToolRank = false)
		{
			cout<<"Pawn  ";
			if (showToolRank == true)
			{
				cout<<" r="<<GetRank();
			}
		}

		virtual ~ChessToolPawn(){
		}
};

// Concrete ChessTool inherited Classes

class ChessToolQueen: public ChessTool{
	
	private:
		/*****/

	protected:
		
	public:

		// Assumption: Black located up the board, White located down the board.
		ChessToolQueen(ENUM_COLOR toolColor = BLACK_COLOR, int toolRank = 9): ChessTool(toolColor, toolRank)
		{
			_toolMoves.push_back(new ChessMoveDiagonalAllDirections7_CleanPath);
			_toolMoves.push_back(new ChessMoveDown7_CleanPath);
			_toolMoves.push_back(new ChessMoveUp7_CleanPath);
			_toolMoves.push_back(new ChessMoveLeft7_CleanPath);
			_toolMoves.push_back(new ChessMoveRight7_CleanPath);
		}

		void virtual DrawTool(bool showToolRank = false)
		{
			cout<<"Queen ";
			if (showToolRank == true)
			{
				cout<<" r="<<GetRank();
			}
		}

		virtual ~ChessToolQueen(){
		}
};

class ChessToolKing: public ChessTool{
	
	private:
		/*****/

	protected:
		
	public:

		// Assumption: Black located up the board, White located down the board.
		ChessToolKing(ENUM_COLOR toolColor = BLACK_COLOR, int toolRank = 1000): ChessTool(toolColor, toolRank)
		{
			_toolMoves.push_back(new ChessMoveDiagonalAllDirections1);
			_toolMoves.push_back(new ChessMoveLeft1);
			_toolMoves.push_back(new ChessMoveRight1);
			_toolMoves.push_back(new ChessMoveUp1);
			_toolMoves.push_back(new ChessMoveDown1);
		}

		void virtual DrawTool(bool showToolRank = false)
		{
			cout<<"King  ";
			if (showToolRank == true)
			{
				cout<<" r="<<GetRank();
			}
		}

		virtual ~ChessToolKing(){
		}
};

class ChessToolRook: public ChessTool{
	
	private:
		/*****/

	protected:
		
	public:

		// Assumption: Black located up the board, White located down the board.
		ChessToolRook(ENUM_COLOR toolColor = BLACK_COLOR, int toolRank = 5): ChessTool(toolColor, toolRank)
		{
			_toolMoves.push_back(new ChessMoveLeft7_CleanPath);
			_toolMoves.push_back(new ChessMoveRight7_CleanPath);
			_toolMoves.push_back(new ChessMoveUp7_CleanPath);
			_toolMoves.push_back(new ChessMoveDown7_CleanPath);
		}

		void virtual DrawTool(bool showToolRank = false)
		{
			cout<<"Rook  ";
			if (showToolRank == true)
			{
				cout<<" r="<<GetRank();
			}
		}

		virtual ~ChessToolRook(){
		}
};

class ChessToolBishop: public ChessTool{
	
	private:
		/*****/

	protected:
		
	public:

		// Assumption: Black located up the board, White located down the board.
		ChessToolBishop(ENUM_COLOR toolColor = BLACK_COLOR, int toolRank = 3): ChessTool(toolColor, toolRank)
		{
			_toolMoves.push_back(new ChessMoveDiagonalAllDirections7_CleanPath);
		}

		void virtual DrawTool(bool showToolRank = false)
		{
			cout<<"Bishop";
			if (showToolRank == true)
			{
				cout<<" r="<<GetRank();
			}
		}

		virtual ~ChessToolBishop(){
		}
};

class ChessToolKnight: public ChessTool{
	
	private:
		/*****/

	protected:
		
	public:

		// Assumption: Black located up the board, White located down the board.
		ChessToolKnight(ENUM_COLOR toolColor = BLACK_COLOR, int toolRank = 3): ChessTool(toolColor, toolRank)
		{
			_toolMoves.push_back(new ChessMoveAllDirections_2_1_Steps);
		}

		void virtual DrawTool(bool showToolRank = false)
		{
			cout<<"Knight";
			if (showToolRank == true)
			{
				cout<<" r="<<GetRank();
			}
		}

		virtual ~ChessToolKnight(){
		}
};

class ChessGame{

	private:

		/*****/

	protected:

		ChessBOARD _board;
		vector<ChessTool*> _blackChessTools;
		vector<ChessTool*> _whiteChessTools;
		ChessPlayer* _player1;
		ChessPlayer* _player2;
		ChessPlayer* _curTurnPlayer;
		int _turnNum;

		int _gameScore; // Black leads if _gameScore < 0. White leads if _gameScore > 0.

	public:

		ChessGame(ChessPlayer* player1, ChessPlayer* player2): _curTurnPlayer(player1), _player1(player1), _player2(player2), _turnNum(0) 
		{
			InitBoard();
			_gameScore = CalculateGameScore();
		}

		void InitBoard()
		{
			for(int i=0; i<8 ; i++)
			{
				for(int j=0; j<8 ; j++)
				{
					_board[i][j]=NULL;
				}
			}

			//Black Tools
			
			//first row
			_board[0][0] = new ChessToolRook(BLACK_COLOR);
			_blackChessTools.push_back(_board[0][0]);
			_board[0][1] = new ChessToolKnight(BLACK_COLOR);
			_blackChessTools.push_back(_board[0][1]);
			_board[0][2] = new ChessToolBishop(BLACK_COLOR);
			_blackChessTools.push_back(_board[0][2]);
			_board[0][3] = new ChessToolQueen(BLACK_COLOR);
			_blackChessTools.push_back(_board[0][3]);
			_board[0][4] = new ChessToolKing(BLACK_COLOR);
			_blackChessTools.push_back(_board[0][4]);
			_board[0][5] = new ChessToolBishop(BLACK_COLOR);
			_blackChessTools.push_back(_board[0][5]);
			_board[0][6] = new ChessToolKnight(BLACK_COLOR);
			_blackChessTools.push_back(_board[0][6]);
			_board[0][7] = new ChessToolRook(BLACK_COLOR);
			_blackChessTools.push_back(_board[0][7]);

			// second row
			_board[1][0] = new ChessToolPawn(BLACK_COLOR);
			_blackChessTools.push_back(_board[1][0]);
			_board[1][1] = new ChessToolPawn(BLACK_COLOR);
			_blackChessTools.push_back(_board[1][1]);
			_board[1][2] = new ChessToolPawn(BLACK_COLOR);
			_blackChessTools.push_back(_board[1][2]);
			_board[1][3] = new ChessToolPawn(BLACK_COLOR);
			_blackChessTools.push_back(_board[1][3]);
			_board[1][4] = new ChessToolPawn(BLACK_COLOR);
			_blackChessTools.push_back(_board[1][4]);
			_board[1][5] = new ChessToolPawn(BLACK_COLOR);
			_blackChessTools.push_back(_board[1][5]);
			_board[1][6] = new ChessToolPawn(BLACK_COLOR);
			_blackChessTools.push_back(_board[1][6]);
			_board[1][7] = new ChessToolPawn(BLACK_COLOR);
			_blackChessTools.push_back(_board[1][7]);

			// White Tools
			//first row
			_board[8-1][0] = new ChessToolRook(WHITE_COLOR);
			_whiteChessTools.push_back(_board[8-1][0]);
			_board[8-1][1] = new ChessToolKnight(WHITE_COLOR);
			_whiteChessTools.push_back(_board[8-1][1]);
			_board[8-1][2] = new ChessToolBishop(WHITE_COLOR);
			_whiteChessTools.push_back(_board[8-1][2]);
			_board[8-1][3] = new ChessToolQueen(WHITE_COLOR);
			_whiteChessTools.push_back(_board[8-1][3]);
			_board[8-1][4] = new ChessToolKing(WHITE_COLOR);
			_whiteChessTools.push_back(_board[8-1][4]);
			_board[8-1][5] = new ChessToolBishop(WHITE_COLOR);
			_whiteChessTools.push_back(_board[8-1][5]);
			_board[8-1][6] = new ChessToolKnight(WHITE_COLOR);
			_whiteChessTools.push_back(_board[8-1][6]);
			_board[8-1][7] = new ChessToolRook(WHITE_COLOR);
			_whiteChessTools.push_back(_board[8-1][7]);

			// second row
			_board[8-1-1][0] = new ChessToolPawn(WHITE_COLOR);
			_whiteChessTools.push_back(_board[8-1-1][0]);
			_board[8-1-1][1] = new ChessToolPawn(WHITE_COLOR);
			_whiteChessTools.push_back(_board[8-1-1][1]);
			_board[8-1-1][2] = new ChessToolPawn(WHITE_COLOR);
			_whiteChessTools.push_back(_board[8-1-1][2]);
			_board[8-1-1][3] = new ChessToolPawn(WHITE_COLOR);
			_whiteChessTools.push_back(_board[8-1-1][3]);
			_board[8-1-1][4] = new ChessToolPawn(WHITE_COLOR);
			_whiteChessTools.push_back(_board[8-1-1][4]);
			_board[8-1-1][5] = new ChessToolPawn(WHITE_COLOR);
			_whiteChessTools.push_back(_board[8-1-1][5]);
			_board[8-1-1][6] = new ChessToolPawn(WHITE_COLOR);
			_whiteChessTools.push_back(_board[8-1-1][6]);
			_board[8-1-1][7] = new ChessToolPawn(WHITE_COLOR);
			_whiteChessTools.push_back(_board[8-1-1][7]);
		}

		void DrawGame(bool showToolsRanks = false)
		{
			cout<<endl;

			// Print Columns Header (Horizontal Header).
			cout<<" row(X) \\ col(y): |  1";
			for(int i=1; i<8; i++)
			{
				cout<<"   |  "<<i+1;
			}
			cout<<"   |"<<endl;
			cout<<"------------------|-------------------------------------------------------|---|"<<endl;

			// Print Board and Row Header (vertical Header).
			for(int i=0; i<8 ; i++) // Rows
			{
				cout<<"  "<<i+1<<"                ";

				for(int j=0 ; j<8 ; j++) // Columns
				{
					if (_board[i][j]!=NULL)
					{ // There is a tool to draw.
						_board[i][j]->DrawTool(showToolsRanks); cout<<"|";
					}
					else
					{
						cout<<"      |";
					}
				}
				cout<<" "<<i+1<<" |";
				cout<<endl<<"------------------|-------------------------------------------------------|---|"<<endl;
			}

			cout<<" row(X) \\ col(y): |  1";
			for(int i=1; i<8; i++)
			{
				cout<<"   |  "<<i+1;
			}
			cout<<"   |"<<endl;

		}



		int CalculateGameScore()
		{
			int sum=0;
			for(int i=0; i<8 ; i++)
			{
				for(int j=0 ; j<8 ; j++)
				{
					if (_board[i][j]!=NULL)
					{
						sum += _board[i][j]->GetRank();
					}
					else
					{
						// Do Nothing (Skip to next Position) because 0 should be added.
					}
				}
			}

			return sum;
		}

		int GetGameScore()
		{
			return _gameScore;
		}

		// Black leads if _gameScore < 0. White leads if _gameScore > 0.
		ENUM_COLOR whoLeads()
		{
			if (_gameScore < 0)
			{
				return BLACK_COLOR;
			}
			if (_gameScore > 0)
			{
				return WHITE_COLOR;
			}
			return NONE_COLOR;
		}

		void PrintCurrentTurnPlayerDetails(){
			_curTurnPlayer->PrintPlayerDetails();
		}

		void PrintAllPlayersDetails(){
			cout<<"Player1 Details:";
			_player1->PrintPlayerDetails();
			cout<<endl<<"Player2 Details:";
			_player2->PrintPlayerDetails();
		}

		int GetTurnNum(){
			return _turnNum;
		}

		void ManageTurns()
		{
			if( _curTurnPlayer == _player1 )
			{
				_curTurnPlayer = _player2;
			}
			else
			{
				_curTurnPlayer = _player1;
			}
			_turnNum++;
		}

		void UpdateGameScore(int rankOfToolKilled){
			_gameScore -= rankOfToolKilled; // We Subtract because it was killed.
		}

		bool SimulateSingleMove(ChessPosition& curPosition, ChessPosition& nextPosition){

			int curX = curPosition.GetX();
			int curY = curPosition.GetY();
			int nextX = nextPosition.GetX(); 
			int nextY = nextPosition.GetY();

			// verify legal current position - in boarders.
			if (curX>=8 || curX<0 || curY>=8 || curY<0)
			{
				cout<<"Ilegal move (cur position not in borders)!"<<endl;
				return false;
			}

			// verify legal next position - in boarders.
			if (nextX>=8 || nextX<0 || nextY>=8 || nextY<0)
			{
				cout<<"Ilegal move (next position not in borders)!"<<endl;
				return false;
			}

			if (_board[curX][curY] == NULL)
			{ // No tool in current position.
				cout<<"Ilegal move (No tool in current position)!"<<endl;
				return false;
			}

			if (_board[curX][curY]->GetToolColor() != _curTurnPlayer->GetPlayerColor())
			{ // ilegal move - trying to move opponent tool.
				cout<<"Ilegal move (trying to move opponent tool)!"<<endl;
				return false;
			}

			if(_board[curX][curY]->isValidMove(curPosition, nextPosition,_board) == false)
			{ // ilegal move according to tool legitimate defined moves.
				cout<<"Ilegal move (not legitimate for tool)!"<<endl;
				return false;
			}

			if (_board[nextX][nextY]!=NULL){ // Kill was done this move.
				UpdateGameScore( _board[nextX][nextY]->GetRank() ); // Udate Game Score.
				_board[nextX][nextY]->ChangeEnableStatus();
			}
			_board[nextX][nextY] = _board[curX][curY];
			_board[curX][curY] = NULL;

			return true;
		}

		bool EndGame(){
			// T.B.D
			return false;
		}

		void PlaySingleTurn()
		{
			playKTurns(1);
		}

		void playKTurns(int k=10)
		{
			DrawGame();

			for(int i=0; i<k && EndGame() == false; i++)
			{
				ChessPosition ChessPositionArray[2];

				while ( (_curTurnPlayer->ChooseMove(ChessPositionArray) == false) ||
						(SimulateSingleMove(ChessPositionArray[0], ChessPositionArray[1]) == false)
				)
				{
					// Do nothing - All checks and actions are done in the loop condition.
					// Note:	ChooseMove is the first condition in order for (in C++/Cpp) second condition of 
					//			SimulateSingleMove to be avoided (since not relevant because ChessPositionArray is irrelevant).
				}

				ManageTurns();
				DrawGame();
			}
		}

		void PlayEntireGame()
		{
			DrawGame();

			while (EndGame() == false)
			{
				ChessPosition ChessPositionArray[2];

				while ( (_curTurnPlayer->ChooseMove(ChessPositionArray) == false) ||
						(SimulateSingleMove(ChessPositionArray[0], ChessPositionArray[1]) == false)
				)
				{
					// Do nothing - All checks and actions are done in the loop condition.
					// Note:	ChooseMove is the first condition in order for (in C++/Cpp) second condition of 
					//			SimulateSingleMove to be avoided (since not relevant because ChessPositionArray is irrelevant).
				}

				ManageTurns();
				DrawGame();
			}
		}

		~ChessGame(){

			// Delete Black Tools
			vector<ChessTool*>::size_type sz = _blackChessTools.size();
			//delete all tools.
			for(int i = 0; i < sz; i++)
			{
				delete(_blackChessTools[i]);
				//flag = _toolMoves.at(i)->isValidMove(curPosition, nextPosition, board); // An alternative way.
			}

			// Delete White Tools
			sz = _whiteChessTools.size();
			//delete all tools.
			for(int i = 0; i < sz; i++)
			{
				delete(_whiteChessTools[i]);
				//flag = _toolMoves.at(i)->isValidMove(curPosition, nextPosition, board); // An alternative way.
			}

		}
};

void main()
{

	//System::Drawing::Graphics  a;
	//Drawing::Graphics b;

	//Graphics c;
	
	Color col;

	//c.Clear(col);

	/*
	ChessToolPawn pawn1(BLACK_COLOR);
	ChessToolPawn* p1 = new ChessToolPawn(BLACK_COLOR);
	pawn1.DrawTool();
	p1->DrawTool();

	ChessTool* p2 = new ChessToolPawn(BLACK_COLOR);

	p2->DrawTool();

	ChessTool* p3;
	p3 = new ChessToolPawn(BLACK_COLOR);

	p3->DrawTool();
	cout<<p3->GetRank();
	cout<<p3->GetToolColor();
	
	if (p3->GetToolColor() == p3->GetToolColor())
		cout<<"hi"<<endl;
	*/

	ChessPlayer* p1 = new ChessPlayer(BLACK_COLOR, "Shalev", 1);
	ChessPlayer* p2 = new ChessPlayer(WHITE_COLOR, "Shalev", 2);

	ChessGame G1(p1,p2);

	G1.PrintAllPlayersDetails();
	G1.PrintCurrentTurnPlayerDetails();

	//G1.PlaySingleTurn();
	//G1.playKTurns(3);
	G1.PlayEntireGame();

	/*
	G1.DrawGame();
	
	ChessPosition curPos(0,0);
	ChessPosition nextPos(1,0);

	cout<<"step1:"<<endl;

	G1.SimulateSingleMove(curPos, nextPos);
	G1.DrawGame();
	cout<<endl;

	cout<<"CalcGameScoreG1 = "<<G1.CalculateGameScore()<<"\tGetGameScore = "<<G1.GetGameScore()<<endl;

	cout<<"step2:"<<endl;

	curPos=nextPos;
	nextPos.SetX(curPos.GetX()+1);
	G1.SimulateSingleMove(curPos, nextPos);
	G1.DrawGame();
	cout<<endl;

	cout<<"CalcGameScoreG1 = "<<G1.CalculateGameScore()<<"\tGetGameScore = "<<G1.GetGameScore()<<endl;

	cout<<"step3:"<<endl;

	curPos=nextPos;
	nextPos.SetX(curPos.GetX()+1);
	G1.SimulateSingleMove(curPos, nextPos);
	G1.DrawGame();
	cout<<endl;
	
	cout<<"CalcGameScoreG1 = "<<G1.CalculateGameScore()<<"\tGetGameScore = "<<G1.GetGameScore()<<endl;

	cout<<"step4:"<<endl;

	curPos=nextPos;
	nextPos.SetX(curPos.GetX()+1);
	G1.SimulateSingleMove(curPos, nextPos);
	G1.DrawGame();
	cout<<endl;

	cout<<"CalcGameScoreG1 = "<<G1.CalculateGameScore()<<"\tGetGameScore = "<<G1.GetGameScore()<<endl;

	cout<<"step5:"<<endl;

	curPos=nextPos;
	nextPos.SetX(curPos.GetX()+1);
	G1.SimulateSingleMove(curPos, nextPos);
	G1.DrawGame();
	cout<<endl;

	cout<<"CalcGameScoreG1 = "<<G1.CalculateGameScore()<<"\tGetGameScore = "<<G1.GetGameScore()<<endl;

	cout<<"step6:"<<endl;

	curPos=nextPos;
	nextPos.SetX(curPos.GetX()+1);
	G1.SimulateSingleMove(curPos, nextPos);
	G1.DrawGame();
	cout<<endl;

	cout<<"CalcGameScoreG1 = "<<G1.CalculateGameScore()<<"\tGetGameScore = "<<G1.GetGameScore()<<endl;

	cout<<"step7:"<<endl;

	curPos=nextPos;
	nextPos.SetX(curPos.GetX()+1);
	G1.SimulateSingleMove(curPos, nextPos);
	G1.DrawGame();
	cout<<endl;

	cout<<"CalcGameScoreG1 = "<<G1.CalculateGameScore()<<"\tGetGameScore = "<<G1.GetGameScore()<<endl;

	cout<< G1.whoLeads()<<endl;

	*/
	//G1.DrawGame(true);

}
