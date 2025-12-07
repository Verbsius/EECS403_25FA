// Project Identifier: 1761414855B69983BD8035097EFBD312EB2517F0
// Author: Yiwei Gui
// Date: 12/6/2025

#include "Drone.h"
#include <cmath>
#include <iostream>
#include <queue>
#include <limits>
#include <algorithm>
#include <numeric>

struct edgeCadidate {
    double dist;
    size_t idx;
};



Area Drone::classifyArea(int x, int y) {
    if ((x == 0 && y <= 0) || (y == 0 && x <= 0))
        return BORDER;
    if (x < 0 && y < 0)
        return MEDICAL;
    return MAIN;
}


void Drone::load() {
    size_t n;
    std::cin >> n;
    pts.reserve(n);

    for (size_t i = 0; i < n; i++) {
        int x, y;
        std::cin >> x >> y;
        pts.push_back({ x, y, classifyArea(x, y)});
        if (classifyArea(x, y) == BORDER && !hasBorder) hasBorder = true;
        if (classifyArea(x, y) == MEDICAL && !hasMedical) hasMedical = true;
        if (classifyArea(x, y) == MAIN && !hasMain) hasMain = true;

    }
}
double Drone::distanceBetween(const Point &a, const Point &b, bool isMST) const {

    // In MST mode, we care about boarder
    if (isMST) {
        bool crossCampus =
            ((a.area == MAIN && b.area == MEDICAL) || 
            (a.area == MEDICAL && b.area == MAIN));
        if (crossCampus && a.area != BORDER && b.area != BORDER)    
            return std::numeric_limits<double>::infinity();

        double dx = (double)a.x - b.x;
        double dy = (double)a.y - b.y;
        return sqrt(dx * dx + dy * dy);
    } 

    // In FASTTSP and OPTTSP mode, we don't care about boarder
    double dx = (double)a.x - b.x;
    double dy = (double)a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}

bool Drone::constructMST() {
    size_t n = pts.size();
    if (n < 2) return false;


    // If there is no border points and needs to cross campus, can not construct MST
    if (hasMain && hasMedical && !hasBorder)
        return false;

    std::vector<bool> k(n, false);
    std::vector<double> d(n, std::numeric_limits<double>::infinity());
    std::vector<size_t> p(n, n);



    d[0] = 0.0;
    mstEdges.clear();
    totalWeight = 0.0;

    for (size_t i = 0; i < n; i++) {
        size_t u = n;
        double best = std::numeric_limits<double>::infinity();

        for (size_t j = 0; j < n; j++) {
            if (!k[j] && d[j] < best) {
                best = d[j];
                u = j;
            }
        }

        if (u == n) return false; // graph disconnected

        k[u] = true;
        totalWeight += d[u];

        if (p[u] != n) {
            size_t a = p[u], b = u;
            if (a > b) std::swap(a, b);
            mstEdges.push_back({a, b});
        }


        for (size_t v = 0; v < n; v++) {
            if (!k[v]) {
                double w = distanceBetween(pts[u], pts[v], true);
                if (w < d[v]) {
                    d[v] = w;
                    p[v] = u;
                }
            }
        }
    }


    return true;
}

void Drone::outputMST() const {
    std::cout << totalWeight << "\n";
    for (auto &e : mstEdges) {
        std::cout << e.first << " " << e.second << "\n";
    }
}

void Drone::computeFASTTSP() {
    if (pts.empty()) return;
    tspPath.clear();
    tspLength = 0.0;

    nearestNeighbor();
    twoOpt();

    tspLength = calPathLength(tspPath);
}

void Drone::outputFASTTSP() const {

    std::cout << tspLength << "\n";
    for (size_t i = 0; i < tspPath.size() - 1; ++i) {
        std::cout << tspPath[i];
        if (i < tspPath.size() - 2) std::cout << " ";
    }
    std::cout << "\n";
}

void Drone::nearestNeighbor() {
    size_t n = pts.size();
    tspPath.reserve(n + 1);

    std::vector<bool> visited(n, false);

    size_t cur = 0;     // start from vertex 0
    tspPath.push_back(cur);
    visited[cur] = true;

    for (size_t i = 1; i < n; ++i) {
        double bestDist = std::numeric_limits<double>::max();
        size_t best = 0;

        for (size_t v = 0; v < n; ++v) {
            if (!visited[v]) {
                double d = distanceBetween(pts[cur], pts[v], false);
                if (d < bestDist) {
                    bestDist = d;
                    best = v;
                }
            }
        }

        tspPath.push_back(best);
        visited[best] = true;
        cur = best;
    }

    // form cycle
    tspPath.push_back(tspPath[0]);
}

double Drone::calPathLength(const std::vector<size_t> &path) const {
    double sum = 0.0;
    for (size_t i = 0; i + 1 < path.size(); ++i) {
        sum += distanceBetween(pts[path[i]], pts[path[i + 1]], false);
    }
    return sum;
}

void Drone::twoOpt() {
    size_t n = tspPath.size();

    bool improved = true;

    while (improved) {
        improved = false;

        for (size_t i = 1; i < n - 2; ++i) {
            for (size_t j = i + 1; j < n - 1; ++j) {
                double before = distanceBetween(pts[tspPath[i - 1]], pts[tspPath[i]], false) +
                    distanceBetween(pts[tspPath[j]], pts[tspPath[j + 1]], false);

                double after = distanceBetween(pts[tspPath[i - 1]], pts[tspPath[j]], false) +
                    distanceBetween(pts[tspPath[i]], pts[tspPath[j + 1]], false);

                // Check if swapping the edges improves distance
                if (after < before) {
                    std::reverse(tspPath.begin() + i, tspPath.begin() + j + 1);

                    improved = true;
                }
            }

        }
    }

}

void Drone::computeOPTTSP() {
    size_t n = pts.size();
    if (n == 0) {
        optPath.clear();
        optLength = std::numeric_limits<double>::infinity();
        return;
    }
    if (n == 1) {
        optPath = {0};
        optLength = 0.0;
        return;
    }

    // Build distance matrix and some helper arrays
    buildDistanceMatrix();

    // Calculate the upper bound 
    computeFASTTSP();

    if (!tspPath.empty()) {
        optLength = tspLength;
    } else {
        optLength = std::numeric_limits<double>::infinity();
    }


    std::vector<size_t> path(n);
    std::iota(path.begin(), path.end(), 0);

    // Start branch and bound
    genPerms(path, 1);


    

}

void Drone::genPerms(std::vector<size_t>& path, size_t permLength) {
    if (permLength == path.size()) {
        handleCompletePermutation(path);
        return;
    }
    if (!promising(path, permLength)) {
        return;
    }
    for (size_t i = permLength; i < path.size(); i++) {
        std::swap(path[permLength], path[i]);
        genPerms(path, permLength + 1);
        std::swap(path[permLength], path[i]);
    }
} 
bool Drone::promising(const std::vector<size_t> &path, size_t permLength) {
    size_t n = pts.size();
    if (n == 0) return false;

    // compute prefix cost 
    double prefixCost = 0.0;
    for (size_t i = 1; i < permLength; ++i) {
        prefixCost += distMatrix[path[i-1]][path[i]];
    }

    // quick prune if already worse than best found
    if (prefixCost >= optLength) return false;

    // ensure the unchanged order in remaining subset
    std::vector<char> inPrefix(n, 0);
    for (size_t i = 0; i < permLength; ++i)
        inPrefix[path[i]] = 1;

    std::vector<size_t> remaining;
    for (size_t v = 0; v < n; ++v)
        if (!inPrefix[v])
            remaining.push_back(v);


    // compute mst on remaining
    double mst_len = primMSTonSubset(remaining);

    // compute min connection from left (start) and right (current end) to remaining
    size_t left = path[0];
    size_t right = path[permLength - 1];
    double min_left = std::numeric_limits<double>::infinity();
    double min_right = std::numeric_limits<double>::infinity();
    for (size_t v : remaining) {
        if (distMatrix[left][v] < min_left) min_left = distMatrix[left][v];
        if (distMatrix[right][v] < min_right) min_right = distMatrix[right][v];
    }

    double lb = prefixCost + mst_len + min_left + min_right;

    // if lower bound is >= current best we can prune
    return lb < optLength;
}

void Drone::handleCompletePermutation(const std::vector<size_t> &path) {
    size_t n = pts.size();
    if (path.size() != n) return;

    double total = 0.0;
    for (size_t i = 1; i < n; ++i) {
        total += distMatrix[path[i-1]][path[i]];
    }
    total += distMatrix[path[n-1]][path[0]]; 

    if (total < optLength) {
        optLength = total;
        optPath = path;
    }
}

double Drone::primMSTonSubset(const std::vector<size_t> &subset) const {
    size_t m = subset.size();
    if (m <= 1) return 0.0;

    std::vector<bool> used(m, false);
    std::vector<double> minDist(m, std::numeric_limits<double>::infinity());

    minDist[0] = 0.0;
    double total = 0.0;

    for (size_t iter = 0; iter < m; ++iter) {
        size_t u = m; 
        double best = std::numeric_limits<double>::infinity();

        // pick closest unused
        for (size_t i = 0; i < m; ++i) {
            if (!used[i] && minDist[i] < best) {
                best = minDist[i];
                u = i;
            }
        }

        if (u == m) {
            // subset is disconnected (shouldn't happen in Euclidean), treat remaining as infinite
            return std::numeric_limits<double>::infinity();
        }

        used[u] = true;
        total += minDist[u];

        // relax neighbors
        for (size_t v = 0; v < m; ++v) {
            if (!used[v]) {
                double w = distanceBetween(pts[subset[u]], pts[subset[v]], false);
                if (w < minDist[v]) {
                    minDist[v] = w;
                }
            }
        }
    }

    return total;
}




void Drone::outputOPTTSP() const {
    if (optLength == std::numeric_limits<double>::infinity()) {
        // no solution found (should not happen if pts non-empty)
        std::cout << "0\n";
        return;
    }
    // print length and the tour (as sequence of indices ending back at start)
    std::cout << optLength << "\n";
    for (size_t i = 0; i < optPath.size(); ++i) {
        std::cout << optPath[i] << " ";
    }
    std::cout << "\n";
}

void Drone::buildDistanceMatrix() {
    size_t n = pts.size();
    distMatrix.assign(n, std::vector<double>(n, 0.0));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (i == j) distMatrix[i][j] = 0.0;
            else distMatrix[i][j] = distanceBetween(pts[i], pts[j], false);
        }
    }
}


