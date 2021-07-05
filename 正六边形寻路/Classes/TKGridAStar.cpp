
#include "TKGridAStar.h"
#include <iostream>
#include <map>
#include <algorithm>

struct Data
{
	Data() {}
	Data(const Grid &pos, double h, double g, Data* parent) {
		this->grid = pos;
		this->h = h;
		this->g = g;
		this->parent = parent;
	}

	Grid grid = Grid(0, 0, 0);
	double h = 0;
	double g = 0;
	Data* parent = nullptr;

	double f() {
		return this->g + this->h;
	}
};

struct MinHeap
{
	std::vector<Data*> m_vec;
	std::map<std::string, Data*> m_map;
	int index = 0;

	Data* getMinAndRemove() {
		if (isEmpty()) {
			return nullptr;
		}
		sort();
		auto first = m_vec.at(0);
		auto last = m_vec.at(index - 1);
		m_vec[0] = last;

		--index;
		return first;
	}

	bool isEmpty() {
		return index <= 0;
	}

	std::string getKey(const Grid& grid) {
		char buff[32] = { 0 };
		sprintf(buff, "%d-%d", grid.x, grid.y);
		return buff;
	}

	Data* find(const Grid& grid) {
		auto it = m_map.find(getKey(grid));
		if (it != m_map.end())
		{
			return it->second;
		}
		return nullptr;
	}

	void add(Data* data) {
		if (index < m_vec.size())
		{
			m_vec[index] = data;
		}
		else
		{
			m_vec.push_back(data);
		}
		index = m_vec.size();
		m_map[getKey(data->grid)] = data;
	}

	void sort() {
		std::sort(m_vec.begin(), m_vec.end(), [](Data* a, Data* b) {return a->f() < b->f(); });
	}

	void release() {
		for (auto it = m_map.begin(); it != m_map.end();)
		{
			Data* tmp = it->second;
			it = m_map.erase(it);
			delete tmp;
		}
	}
};



static TKGridAStar* instance=nullptr;
TKGridAStar& TKGridAStar::getInstance()
{
	if (!instance)
	{
		instance = new TKGridAStar;
	}
	return *instance;
}

TKGridAStar::TKGridAStar()
{
	this->initData();
}

TKGridAStar::~TKGridAStar()
{
}

void TKGridAStar::initData()
{
	m_neighbourVec = {
		Directs(1,-1, 0),
		Directs(-1, 1, 0),
		Directs(1, 0,-1),
		Directs(-1, 0, 1),
		Directs(0, 1,-1),
		Directs(0,-1, 1)
	};

	Directs leftDown = Directs(-1, -1, 2, 3);
	leftDown.conditionVec.push_back(Grid(-1, 0, 1));
	leftDown.conditionVec.push_back(Grid(0, -1, 1));

	Directs left = Directs(-2, 1, 1, 3);
	left.conditionVec.push_back(Grid(-1, 0, 1));
	left.conditionVec.push_back(Grid(-1, 1, 0));

	Directs leftUp = Directs(-1, 2, -1, 3);
	leftUp.conditionVec.push_back(Grid(-1, 1, 0));
	leftUp.conditionVec.push_back(Grid(0, 1, -1));

	Directs rightDown = Directs(1, -2, 1, 3);
	rightDown.conditionVec.push_back(Grid(1, -1, 0));
	rightDown.conditionVec.push_back(Grid(0, -1, 1));

	Directs right = Directs(2, -1, -1, 3);
	right.conditionVec.push_back(Grid(1, 0, -1));
	right.conditionVec.push_back(Grid(1, -1, 0));

	Directs rightUp = Directs(1, 1, -2, 3);
	rightUp.conditionVec.push_back(Grid(0, 1, -1));
	rightUp.conditionVec.push_back(Grid(1, 0, -1));

	m_diagonalVec.push_back(leftDown);
	m_diagonalVec.push_back(left);
	m_diagonalVec.push_back(leftUp);
	m_diagonalVec.push_back(rightDown);
	m_diagonalVec.push_back(right);
	m_diagonalVec.push_back(rightUp);
}

void TKGridAStar::setMapData(const std::map<int,std::vector<MapData>>& data)
{
	m_mapData = data;
}

void TKGridAStar::resetMapData()
{
	for (auto& a : m_mapData)
	{
		for (auto& b : a.second)
		{
			b.reset();
		}
	}
}

TKGridAStar::MapData* TKGridAStar::getMapDataByGrid(const Grid& grid)
{
	auto& vec = m_mapData[grid.x];
	for (auto &a:vec)
	{
		if (a.grid.y==grid.y)
		{
			return &a;
		}
	}
	return nullptr;
}

void TKGridAStar::getDirects(const Grid& curPos,std::vector<Directs> &directs)
{
	directs.clear();
	for (auto goalGrid : m_diagonalVec)
	{
		bool isWall = false;
		for (auto a : goalGrid.conditionVec)
		{
			a += curPos;
			auto data = this->getMapDataByGrid(a);
			if (data && !data->isSpace)
			{
				isWall = true;
				break;
			}
		}

		if (!isWall)
		{
			directs.push_back(goalGrid);
		}
	}

	directs.insert(directs.begin() + directs.size(), m_neighbourVec.begin(), m_neighbourVec.end());
}

void TKGridAStar::findPath(const Grid& startPos, const Grid& endPos, std::vector<Grid>& pathVec)
{
	this->resetMapData();
	//------可以拓展的六个方向
	std::vector<Directs> directs;

	MinHeap heap;
	heap.add(new Data(startPos, 0, 0, nullptr));
	bool finish = false;
	Data* lastData = nullptr;// 记录最后一个点的数据,用来反推路径 若为空则无路径可到达

	int max_l = 0;
	int halfX = ceil(m_mapData.size() * 0.5);
	while (!finish && !heap.isEmpty())
	{
		Data* data = heap.getMinAndRemove();// 取出f值最小的点
		auto mapData = getMapDataByGrid(data->grid);
		if (mapData->isSpace)
		{
			mapData->visited = true;// 将取出的点标识为已访问点
		}

		max_l = m_mapData[data->grid.x].size();

		getDirects(data->grid, directs);
		for (auto dir : directs)// 遍历六个方向的点
		{
			Grid pos = mapData->grid;
			pos += dir.grid;
			if (pos.x >=-halfX && pos.x <= halfX
				&& pos.y < max_l)
			{
				auto c = getMapDataByGrid(pos);
				if (!c)
				{
					continue;
				}
				if (endPos==pos)// 如果是终点,则跳出循环,不用再找
				{
					finish = true;
					lastData = data;
					break;
				}
				if (!c->isSpace||c->visited)// 如果不是空地,就不需要再扩展
				{
					continue;
				}

				auto nn = heap.find(pos);
				if (nn)
				{
					if (nn->g > data->g + dir.g)
					{
						nn->g = data->g + dir.g;
						nn->parent = data;
					}
				}
				else
				{
					heap.add(new Data(pos, pos.distance(endPos), data->g + dir.g, data));
				}
			}
		}
	}

	if (lastData)
	{
		// 反向找出路径
		pathVec.clear();
		auto mapData = getMapDataByGrid(lastData->grid);
		if (mapData)
		{
			pathVec.push_back(mapData->grid);
		}
		while (lastData->parent)
		{
			lastData = lastData->parent;
			mapData = getMapDataByGrid(lastData->grid);
			pathVec.push_back(mapData->grid);
		}
	}
	else
	{
		std::cout << "no path" << std::endl;
	}
	heap.release();
}