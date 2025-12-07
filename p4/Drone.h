// Project Identifier: 1761414855B69983BD8035097EFBD312EB2517F0
// Author: Yiwei Gui
// Date: 12/6/2025

#ifndef DRONE
#define DRONE

#include <vector>
#include <string>
#include <limits>

enum Area { MAIN = 0, MEDICAL = 1, BORDER = 2 };

struct Point {
    int x, y;
    Area area;
};


class Drone {
public:
  Drone() = default;     
  void load(); 


  // MST mode
  bool constructMST();
  double primMST(
    const std::vector<Point>& pts, 
    std::vector<std::pair<size_t, size_t>>* mstEdgesOut = nullptr
  );
  void outputMST() const;

  // FASTTSP mode
  void computeFASTTSP();
  void outputFASTTSP() const;

  // OPTTSP mode
  void computeOPTTSP();
  void outputOPTTSP() const;

    

private:
  bool hasMain = false, hasMedical = false, hasBorder = false;

  // MST mode
  std::vector<Point> pts;
  std::vector<std::pair<size_t, size_t>> mstEdges;
  double totalWeight = 0.0;

  // FASTTSP mode 
  std::vector<size_t> tspPath;   
  double tspLength = 0.0;

  // OPTTSP mode 
  std::vector<std::vector<double>> distMatrix; // n x n distances
  std::vector<size_t> optPath;
  double optLength = std::numeric_limits<double>::infinity();
  


private:
  Area classifyArea(int x, int y);
  double distanceBetween(const Point &a, const Point &b, bool isMST) const;

  // FASTTSP helpers 
  void nearestNeighbor();    
  void twoOpt();             
  double calPathLength(const std::vector<size_t> &path) const;

  // OPTTSP helpers

  void genPerms(std::vector<size_t>& path, size_t permLength); 
  void buildDistanceMatrix();
  // 
  bool promising(const std::vector<size_t> &path, size_t permLength);

  // 
  void handleCompletePermutation(const std::vector<size_t> &path);

  // return lb based on elements that are not permutated yet 
  double primMSTonSubset(const std::vector<size_t> &subset) const;
};

#endif