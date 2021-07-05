

#ifndef __TKGridAStar__
#define __TKGridAStar__

#include "TKHexagonGrid.h"
#include <vector>
#include <map>

#define ASTAR (TKGridAStar::getInstance())

class TKGridAStar
{
public:
	TKGridAStar();
	~TKGridAStar();

	static TKGridAStar& getInstance();
	struct Directs
	{
		Directs();
		Directs(int x, int y, int z, double g = 1.73)
		{
			grid.x = x;
			grid.y = y;
			grid.z = z;
			this->g = g;
		}
		Grid grid=Grid(0,0,0);
		double g = 0;

		std::vector<Grid> conditionVec;
	};

	struct MapData
	{
		Grid grid;
		bool visited = false;
		bool isSpace = true;
		void reset() {
			visited = false;
		}
	};

	void setMapData(const std::map<int,std::vector<MapData>>& data);
	MapData* getMapDataByGrid(const Grid& grid);
	void getDirects(const Grid& curPos,std::vector<Directs>& directs);
	void findPath(const Grid & startPos,const Grid &endPos,std::vector<Grid> &pathVec);
protected:
	void initData();
	void resetMapData();
private:
	std::vector<Directs> m_neighbourVec,m_diagonalVec;
	std::map<int,std::vector<MapData>> m_mapData;
};

#endif /* defined(__TKGridAStar__) */
