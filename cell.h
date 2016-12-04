#ifndef __CELL_H
#define __CELL_H


const int row = 10;
const int col = 8;

extern char maze[row][col];


class Cell
{

public:

	Cell(int row, int col);
	~Cell();
	int getRow() const;
	int getCol() const;
	int getDirection() const;
	char getValue() const;

	Cell getNextCell();

private:
	int row_;
	int col_;
	char value_;
	int direction_;
	enum { arrRow = 4, arrCol = 2 };
	enum {totalRow = 10, totalCol = 8};
	void setValue(int totalRow, int totalCol);
};


Cell::Cell(int initRow, int initCol)
{
	row_ = initRow;
	col_ = initCol;
	direction_ = 0;
	setValue( totalRow, totalCol);
}

// destructor, takes no action
Cell::~Cell()
{
}

Cell Cell::getNextCell()
{
	static const int ds[arrRow][arrCol] = { {1, 0 },
			    					 {0, 1 },
			  				   	     {-1, 0},
			  					     {0, -1} };

	++direction_;
	// return Cell(constrain( row_ + ds[direction_-  1][0], 0, totalRow - 1 ), constrain( col_ + ds[direction_ - 1][1], 0, totalCol - 1) );
	return Cell( row_ + ds[direction_ -  1][0], col_ + ds[direction_ - 1][1] );
}

void Cell::setValue(int totalRow, int totalCol)
{
	if ( (row_ < totalRow && row_ >= 0 ) && (col_ < totalCol && col_ >= 0) )
	{
		value_ = maze[row_][col_];
	}
	else
	{
		value_ = '&';	// virtual outer boundaries
	}
}

int Cell::getRow() const
{
	return row_;
}


int Cell::getCol() const
{
	return col_;
}

int Cell::getDirection() const
{
	return direction_;
}


char Cell::getValue() const
{
	return value_;
}


#endif