
#include "TKHexagonGrid.h"
#include <algorithm>

void Grid::updateZ()
{
	this->z = -(this->x + this->y);
}

int Grid::getYIdx() const
{
	//y is an offset data,equal row sub col
	return this->y + this->x;
}

double Grid::distance(const Grid& grid) 
{
	double dis = abs(grid.x - x) + abs(grid.y - y) + abs(grid.z - z);
	return dis / 2;
}

Grid Grid::neighbor(int direction)
{
	return this->add(GridHelper::direction(direction));
}

bool Grid::posInGrid(const double& posX, const double& posY, bool& xMoreThanHalf)
{
	const double dx = std::abs(this->posX - posX);
	const double dy = std::abs(this->posY - posY);
	if (dx <= halfSideLen)
	{
		return dy <= halfSideLen_X_mathSqrt3;
	}
	else
	{
		xMoreThanHalf = true;
		const double maxY = -mathSqrt3 * (dx - halfSideLen) + halfSideLen_X_mathSqrt3;
		return dy < maxY;
	}
}

/********************************/
/********************************/
/********************************/

double GridHelper::m_centerPosX = 0.0;
double GridHelper::m_centerPosY = 0.0;

const std::vector<Grid> _neighbourVec = {
	Grid( 1, 0,-1),
	Grid( 1,-1, 0),
	Grid( 0,-1, 1),
	Grid(-1, 0, 1),
	Grid(-1, 1, 0),
	Grid( 0, 1,-1),
};

Grid GridHelper::direction(unsigned idx)
{
	return _neighbourVec[idx];
}

void GridHelper::getGridRingsByRadius(const Grid& centenrGrid, const int radius, std::vector<Grid>& ringsVec)
{
	ringsVec.clear();
	Grid grid = GridHelper::direction(4).scale(radius);
	for (int side = 0; side < 6; side++) {
		for (int step = 0; step < radius; step++) {
			ringsVec.push_back(grid + centenrGrid);
			grid = grid.neighbor(side);
		}
	}
}

void GridHelper::getPosByGrid(const int gridX, const int gridY, Grid& pos)
{
	const int y = gridY - gridX / 2;
	pos.x = gridX;
	pos.y = gridY - gridX;
	pos.posX = gridX * oneAndAHalfSideLen;
	if (gridX % 2 == 0)
	{
		pos.posY = cubeSideLen_X_mathSqrt3 * y;
	}
	else
	{
		if (gridX>=0)
		{
			pos.posY = cubeSideLen_X_mathSqrt3 * (y - 0.5);
		}
		else
		{
			pos.posY = cubeSideLen_X_mathSqrt3 * (y + 0.5);
		}
	}
	pos.posX += m_centerPosX;
	pos.posY += m_centerPosY;
}

void GridHelper::getGridY(const int gridX, const double posX,const double posY, int& gridY)
{
	const bool xGreatThanZero = posX >= 0;
	const bool yGreatThanZero = posY >= 0;
	gridY = 0;
	if (gridX % 2 == 0)
	{
		if (yGreatThanZero)
		{
			gridY = posY / cubeSideLen_X_mathSqrt3 + 0.5;
		}
		else 
		{
			gridY = posY / cubeSideLen_X_mathSqrt3 - 0.5;
		}
	}
	else
	{
		if (yGreatThanZero)
		{
			gridY = posY / cubeSideLen_X_mathSqrt3 + 1;
		}
		else
		{
			gridY = posY / cubeSideLen_X_mathSqrt3;
		}
	}
	
	if (xGreatThanZero)
	{
		gridY -= (gridX + 1) / 2;
	}
	else 
	{
		gridY -= gridX / 2;
	}
}

void GridHelper::getGridByPos(const double px, const double py, Grid& grid)
{
	//获得相对地图中心点的坐标
	const double posX = px - m_centerPosX;
	const double posY = py - m_centerPosY;

	const bool xGreatThanZero = posX >= 0;

	const double gridD = posX/ oneAndAHalfSideLen + (xGreatThanZero ? 0.5 : -0.5);
	int gridX = gridD;
	int gridY = 0;
	GridHelper::getGridY(gridX, posX,posY, gridY);

	grid.x = gridX;
	grid.y = gridY;

	GridHelper::getPosByGrid(gridX, grid.getYIdx(), grid);
	bool xMoreThanHalf = false;
	if (!grid.posInGrid(px, py, xMoreThanHalf))
	{
		if (xMoreThanHalf)
		{
			if (xGreatThanZero)
			{
				if (gridD > gridX + 0.5)
				{
					gridX += 1;
				}
				else
				{
					gridX -= 1;
				}
			}
			else
			{
				if (gridD > gridX - 0.5)
				{
					gridX += 1;
				}
				else
				{
					gridX -= 1;
				}
			}
			GridHelper::getGridY(gridX, posX, posY, gridY);
		}
		else
		{
			if (grid.posY > py)
			{
				gridY -= 1;
			}
			else
			{
				gridY += 1;
			}
		}
	}

	grid.x = gridX;
	grid.y = gridY;
	grid.updateZ();
	//posX/posY 需要记录传入的坐标
	grid.posX = px;
	grid.posY = py;;
}

int GridHelper::getTwoGridDistance(const double startPosX, const double startPosY, const double endPosX, const double endPosY, double& distance)
{
	Grid startGrid, endGrid;
	GridHelper::getGridByPos(startPosX, startPosY, startGrid);
	GridHelper::getGridByPos(endPosX, endPosY, endGrid);
	const int dx = std::abs(startGrid.x - endGrid.x);
	const int dy = std::abs(startGrid.y - endGrid.y);
	const int dz = std::abs(startGrid.z - endGrid.z);
	int radius = std::max(dx, std::max(dy, dz));

	//两个网格间的距离【移动消耗】【无阻挡点有效】
	//对角线消耗2，对边消耗1.73，边界消耗1
	//通过对角线必然经过边界
	distance = 0;
	int diagonalStep = std::min(dx, std::min(dy, dz));
	distance = diagonalStep * (2/*对角线消耗*/+1/*边界消耗*/) + (radius - diagonalStep*2) * mathSqrt3;
	return radius;
}

void GridHelper::setCenterPos(const double posX, const double posY)
{
	m_centerPosX = posX;
	m_centerPosY = posY;
}