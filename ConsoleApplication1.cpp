#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <map>

#define M_PI 3.141592653589793

class TMapPoint {

	public:
	int x, y;

	bool operator< (const TMapPoint& p) const {
		return x<p.x||(x==p.x&&y<p.y);
	}
};



bool operator== (const TMapPoint& cell1, const TMapPoint& cell2) {
	return (cell1.x==cell2.x&&cell1.y==cell2.y);
}

std::ostream& operator<< (std::ostream& out, const TMapPoint& cell) {
	out<<'('<<cell.x<<", "<<cell.y<<')';
	return out;
}

TMapPoint operator+(const TMapPoint& cell1, const TMapPoint& cell2) {
	return {{cell1.x+cell2.x}, {cell1.y+cell2.y}};
}

enum NCellType { Inner, ThreeSide, Side, BigAngle, Angl, Outside };

double Angle(TMapPoint a, TMapPoint b) {
	return atan2(b.y-a.y, b.x-a.x);
}


bool IsOutside(TMapPoint cell, int mapHeight, int mapWidth) {
	return cell.x<0||cell.x>=mapWidth||cell.y<0||cell.y>=mapHeight;
}




void Assert(bool condition, const char* msg) {
	std::cout<<msg<<" - "<<(condition ? "Ok" : "Fail")<<'\n';
}


void TestIsOutside() {

	TMapPoint cell = {3, 3};
	Assert(IsOutside(cell, 4, 4)==false, "TestIsOutsideFalse");
	Assert(IsOutside(cell, 4, 2)==true, "TestIsOutsideTrue");
	Assert(IsOutside(cell, 2, 4)==true, "TestIsOutsideTrue");

}

std::vector<TMapPoint> Neighbours(TMapPoint cell) {
	return {{cell.x+1, cell.y}, {cell.x, cell.y+1}, {cell.x-1, cell.y}, {cell.x, cell.y-1}};
}





void TestNeighbours() {
	{
		std::vector<TMapPoint> answer = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
		Assert(Neighbours({0, 0})==answer, "TestNeightbours0");
	}
	{
		std::vector<TMapPoint> answer = {{2, 2}, {1, 3}, {0, 2}, {1, 1}};
		Assert(Neighbours({1, 2})==answer, "TestNeightbours1");
	}
}


NCellType FindPattern(TMapPoint cell, std::vector<TMapPoint> CellNeighbours) {

	const auto numberOfNeighbors = CellNeighbours.size();
	if(numberOfNeighbors==1) return Angl;
	if(CellNeighbours[0].x==CellNeighbours[1].x||CellNeighbours[0].y==CellNeighbours[1].y&&numberOfNeighbors==2) {
		return Side;
	} else if(numberOfNeighbors==2) { return BigAngle; }
	if(numberOfNeighbors==3) return ThreeSide;
	if(numberOfNeighbors==4) return Inner;
	return Outside;
}

void TestFindPattern() {

	{
		/*

		* * * * * * *
		* * * * * # *
		* * # * * C *
		# * # # # # *
		# # # # # # *
		# # # # # # *

		*/
		std::vector<TMapPoint> CellNeighbours = {{5, 2}, {5, 4}};

		Assert(Side==FindPattern({5, 3}, CellNeighbours), "TestFindPatternSide");
	}

	{
		/*

		  * * * * * * *
		  * * * * * # *
		  * * C * * # *
		  # * # # # # *
		  # # # # # # *
		  # # # # # # *

		*/
		std::vector<TMapPoint> CellNeighbours = {{2, 2}};

		Assert(Angl==FindPattern({2, 3}, CellNeighbours), "TestFindPatternAngl");
	}


	{
		/*

		  * * * * * * *
		  * * * * * # *
		  * * C # * # *
		  # * # # # # *
		  # # # # # # *
		  # # # # # # *

		*/
		std::vector<TMapPoint> CellNeighbours = {{2, 2}, {3, 3}};

		Assert(BigAngle==FindPattern({2, 3}, CellNeighbours), "TestFindPatternAngl");
	}
}


template<typename T>
void RotatePoint2D(T& pointX, T& pointY, float angle, float centerPointX, float centerPointY) {

	pointX -= T(centerPointX);
	pointY -= T(centerPointY);

	float xnew = float(pointX)*cos(angle)-float(pointY)*sin(angle);
	float ynew = float(pointX)*sin(angle)+float(pointY)*cos(angle);

	pointX = T(xnew+centerPointX);
	pointY = T(ynew+centerPointY);
}

TMapPoint RotatePoint(const TMapPoint& point, float rad, const TMapPoint& center = {}) {
	auto x = (float)point.x;
	auto y = (float)point.y;
	RotatePoint2D(x, y, rad, (float)center.x, (float)center.y);

	return {(int)std::round(x), (int)std::round(y)};
}




double FindAngel(std::vector<TMapPoint> CellNeighbour, TMapPoint cell, NCellType type) {

	double rad = 2*M_PI;

	if(type==Inner) {
		return 2*M_PI;
	} else if(type==Side) {
		return 3*M_PI/2;
	} else if(type==ThreeSide) {

		if(CellNeighbour[1].x==CellNeighbour[2].x&&CellNeighbour[0].x==cell.x+1) {
			return 3*M_PI/2;
		} else if(CellNeighbour[0].y==cell.y+1) {
			return M_PI/2;
		}

		while(RotatePoint(CellNeighbour[0], rad, cell).y!=RotatePoint(CellNeighbour[2], rad, cell).y&&
			  RotatePoint(CellNeighbour[1], rad, cell).x!=cell.x+1)
			rad += M_PI/2;

		return 2*M_PI-rad;

	} else if(type==Angl) {
		while(RotatePoint(CellNeighbour[0], rad, cell).x!=cell.x+1)
			rad += M_PI/2;

		return rad-2*M_PI;
	} else {
		if(CellNeighbour[1].y==cell.y-1&&CellNeighbour[0].x==cell.x+1) return 2*M_PI-2*M_PI;
		while(RotatePoint(CellNeighbour[0], rad, cell).y!=cell.y-1)
			rad += M_PI/2;

		return rad;
	}

	return -1;
}


void TestFindAngel1() {

	Assert(FindAngel({{5, 4}}, {5, 3}, Side)==3*M_PI/2, "TestFindAngel1Side90");
	Assert(FindAngel({{5, 3}}, {5, 4}, Angl)==M_PI/2, "TestFindAngel2Angl90");
	Assert(FindAngel({{4, 4}}, {5, 4}, Angl)==M_PI, "TestFindAngel3Angl180");
	Assert(FindAngel({{5, 4}}, {4, 4}, Angl)==0.0, "TestFindAngel4Angl180");
}


static std::map<int, std::vector<TMapPoint>> s_mCellPatternData = {

	{NCellType::Inner, {{1, 0}, {0, 1}, {-1, 0}, {0, -1}}},
	{NCellType::ThreeSide, {{1, 0}, {0, 1}, {-1, 0}}},
	{NCellType::Side, {{1, 0}, {-1, 0}}},
	{NCellType::BigAngle, {{1, 0}, {0, 1}}},
	{NCellType::Angl, {{1, 0}}},
};

std::pair<int, double> CellPatternAndRotation(const TMapPoint& cell,
											  const std::vector<TMapPoint>& cache,
											  const std::map<int, std::vector<TMapPoint>>& patternData) {
	auto sideAngles = {M_PI/2.0, M_PI, 3.0*M_PI/2.0, 2*M_PI};

	for(auto& [pattern, initialOffsets]:patternData) {
		for(auto& angle:sideAngles) {

			auto offsetsWithRotation = initialOffsets;
			for(auto& offsetWithRotation:offsetsWithRotation) {

				auto offsetWithRotationAs2DPoint = RotatePoint({offsetWithRotation.x, offsetWithRotation.y}, (float)angle);
				offsetWithRotation = {offsetWithRotationAs2DPoint.x, offsetWithRotationAs2DPoint.y};
			}

			std::vector<TMapPoint> cellNeighbours;
			for(auto& offsetWithRotation:offsetsWithRotation) {

				auto cellWithOffset = offsetWithRotation+cell;
				auto filter = [&cellWithOffset](const auto& point) { return point.x==cellWithOffset.x&&point.y==cellWithOffset.y; };

				if(std::find_if(cache.begin(), cache.end(), filter)!=cache.end()) {
					cellNeighbours.push_back(offsetWithRotation);
				}
			}

			std::sort(cellNeighbours.begin(), cellNeighbours.end());
			std::sort(offsetsWithRotation.begin(), offsetsWithRotation.end());

			if(cellNeighbours==offsetsWithRotation) {
				//_LOG(cell<<" match pattern: "<<magic_enum::enum_name(pattern)<<" and angle: "<<angle);
				return std::make_pair(pattern, angle);
			}
		} // for(angle)
	} // for(pattern, offsets)

	//_ERROR("No available pattern for given point: " << cell);
	return std::pair<int, double>{};
}






//std::pair<NCellType, double> CellDetection(TMapPoint cell, std::vector<TMapPoint>& cache, int mapHeight, int mapWidth) {
//
//
//	std::vector<TMapPoint> AllCellNeighbours;
//	std::vector<TMapPoint> CellNeighbours(4);
//
//	if(!IsOutside(cell, mapHeight, mapWidth))
//		AllCellNeighbours = Neighbours(cell);
//	else
//		return std::make_pair(Outside, -1);
//
//	int count = 0;
//	for(int k = 0; k<AllCellNeighbours.size(); ++k)
//	{
//		for(int i = 0; i<cache.size(); ++i)
//		{
//			if(AllCellNeighbours[k]==cache[i]) {
//
//				CellNeighbours[count] = AllCellNeighbours[k];
//				++count;
//
//			}
//		}
//	}
//
//	CellNeighbours.resize(count);
//
//	NCellType type = FindPattern(cell, CellNeighbours);
//	double angle = FindAngel(CellNeighbours, cell, type);
//	std::cout<<"Angle == "<<angle<<std::endl;
//
//	//return std::make_pair(Outside, 1 );
//	return std::make_pair(type, angle);
//}

//#define make_pair pair<int, double>

void TestCellDetection() {


	std::vector<TMapPoint> cache{
		{5, 4},
		{2, 3}, {5, 3},
		{0, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2},
		{0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1},
		{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}
	};

	int Width = 6;
	int Height = 6;

	/*

	  5  * * * * * * *
	  4  * * * * * # *
	  3  * * # * * # *
	  2  # * # # # # *
	  1  # # # # # # *
	  0  # # # # # # *

		 0 1 2 3 4 5 6

	 */
	Assert(CellPatternAndRotation({0, 0}, cache, s_mCellPatternData)==std::make_pair((int)BigAngle, 2*M_PI), "TestCellDetection BigAngle90");
	Assert(CellPatternAndRotation({1, 0}, cache, s_mCellPatternData)==std::make_pair((int)ThreeSide, 2*M_PI), "TestCellDetection ThreeSide0");
	Assert(CellPatternAndRotation({2, 0}, cache, s_mCellPatternData)==std::make_pair((int)ThreeSide, 2*M_PI), "TestCellDetection ThreeSide0");
	Assert(CellPatternAndRotation({3, 0}, cache, s_mCellPatternData)==std::make_pair((int)(ThreeSide), 2*M_PI), "TestCellDetection ThreeSide0");
	Assert(CellPatternAndRotation({4, 0}, cache, s_mCellPatternData)==std::make_pair((int)(ThreeSide), 2*M_PI), "TestCellDetection ThreeSide0");
	Assert(CellPatternAndRotation({5, 0}, cache, s_mCellPatternData)==std::make_pair((int)(BigAngle), M_PI/2), "TestCellDetection BigAngle180");
	Assert(CellPatternAndRotation({0, 1}, cache, s_mCellPatternData)==std::make_pair((int)(ThreeSide), 3*M_PI/2), "TestCellDetection BigAngle270");
	Assert(CellPatternAndRotation({1, 1}, cache, s_mCellPatternData)==std::make_pair((int)(ThreeSide), M_PI), "TestCellDetectionThreeSide180");
	Assert(CellPatternAndRotation({2, 2}, cache, s_mCellPatternData)==std::make_pair((int)(ThreeSide), 3*M_PI/2), "TestCellDetection ThreeSide270");
	Assert(CellPatternAndRotation({2, 1}, cache, s_mCellPatternData).first==Inner, "TestCellDetection Inner0");
	Assert(CellPatternAndRotation({3, 1}, cache, s_mCellPatternData).first==Inner, "TestCellDetection Inner0");
	Assert(CellPatternAndRotation({4, 1}, cache, s_mCellPatternData).first==Inner, "TestCellDetection Inner0");
	Assert(CellPatternAndRotation({5, 1}, cache, s_mCellPatternData)==std::make_pair((int)(ThreeSide), M_PI/2), "TestCellDetection ThreeSide90");
	Assert(CellPatternAndRotation({0, 2}, cache, s_mCellPatternData)==std::make_pair((int)(Angl), 3*M_PI/2), "TestCellDetection Angl270");
	Assert(CellPatternAndRotation({2, 2}, cache, s_mCellPatternData)==std::make_pair((int)(ThreeSide), 3*M_PI/2), "TestCellDetection ThreeSide270");
	Assert(CellPatternAndRotation({3, 2}, cache, s_mCellPatternData)==std::make_pair((int)(ThreeSide), M_PI), "TestCellDetection ThreeSide180");
	Assert(CellPatternAndRotation({4, 2}, cache, s_mCellPatternData)==std::make_pair((int)(ThreeSide), M_PI), "TestCellDetection ThreeSide180");
	Assert(CellPatternAndRotation({5, 2}, cache, s_mCellPatternData)==std::make_pair((int)(ThreeSide), M_PI/2), "TestCellDetection ThreeSide90");
	Assert(CellPatternAndRotation({2, 3}, cache, s_mCellPatternData)==std::make_pair((int)(Angl), 3*M_PI/2), "TestCellDetection Angl90");
	Assert(CellPatternAndRotation({5, 3}, cache, s_mCellPatternData)==std::make_pair((int)(Side), M_PI/2), "TestCellDetection Side90");
	Assert(CellPatternAndRotation({5, 4}, cache, s_mCellPatternData)==std::make_pair((int)(Angl), 3*M_PI/2), "TestCellDetection Angl270");
}


int main() {

	TestNeighbours();
	TestIsOutside();
	TestFindPattern();
	TestFindAngel1();
	TestCellDetection();
	return 0;
}

