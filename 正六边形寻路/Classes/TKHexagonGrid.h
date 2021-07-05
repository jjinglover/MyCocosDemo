

#ifndef __TKHexagonGrid__
#define __TKHexagonGrid__

#include <vector>

const double cubeSideLen = 50;
const double mathSqrt3 = 1.732051;
const double cubeSideLen_X_mathSqrt3 = cubeSideLen * mathSqrt3;
const double halfSideLen = cubeSideLen * 0.5;
const double halfSideLen_X_mathSqrt3 = halfSideLen * mathSqrt3;
const double oneAndAHalfSideLen = cubeSideLen * 1.5;

class Grid
{
public:
	Grid() {}
	Grid(int x, int y,int z=0) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	int x = 0;
	int y = 0;
	int z = 0;
	int uid = 0;

	double posX = 0;
	double posY = 0;

	void updateZ();
	int getYIdx() const;

	double distance(const Grid& grid);

	bool operator==(const Grid& grid) const {
		return this->x == grid.x && this->y == grid.y;
	}

	void operator+=(const Grid& grid) {
		this->x += grid.x;
		this->y += grid.y;
		this->z += grid.z;
	}

	Grid operator+(const Grid& grid) {
		return add(grid);
	}

	Grid scale(int k) {
		return Grid(this->x*k,this->y*k,this->z*k);
	}

	Grid add(const Grid& grid) {
		return Grid(this->x + grid.x, this->y + grid.y, this->z + grid.z);
	}

	Grid neighbor(int direction);
	bool posInGrid(const double& posX, const double& posY, bool& xMoreThanHalf);
};


class GridHelper
{
public:
	static Grid direction(unsigned idx);
	/*
		centenrGrid：中心六边形网格
		radius：距离centenrGrid的步长
		ringsVec：距离centenrGrid步长为radius的所有网格信息
	*/
	static void getGridRingsByRadius(const Grid& centenrGrid,const int radius, std::vector<Grid>& ringsVec);
	static void getPosByGrid(const int gridX, const int gridY, Grid& pos);
	static void getGridByPos(const double posX, const double posY, Grid& grid);
	static int getTwoGridDistance(const double startPosX, const double startPosY, const double endPosX, const double endPosY,double &distance);
	//设置平铺的中心坐标
	static void setCenterPos(const double posX, const double posY);
protected:
	static void getGridY(const int gridX, const double posX,const double posY,int &gridY);
	//平铺中心的世界坐标
	static double m_centerPosX;
	static double m_centerPosY;
};

#endif /* defined(__TKHexagonGrid__) */
