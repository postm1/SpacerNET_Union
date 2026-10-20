// Supported with union (c) 2020 Union team

// User API for zCView
// Add your methods here

int zCView::ClipLine_Union(int& x0, int& y0, int& x1, int& y1);


void SetCenterX(int y)
{
	SetPos(8192 / 2 - this->vsizex / 2, y);
}

void SetY(int y)
{
	SetPos(this->vposx, y);
}

void SetCenterXY()
{
	SetPos(8192 / 2 - this->vsizex / 2, 8192 / 2 - this->vsizey / 2);
}


void SetSizeSquare(int size)
{
	int nSizeY = size;
	int nSizeX = this->anx(this->nay(nSizeY));

	this->SetSize(nSizeX, nSizeY);
}
