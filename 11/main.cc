#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>
#include <numeric>

struct Cell {
    bool isHospital;
    int dist, minDist = std::numeric_limits<int>::max();
};

std::vector<std::vector<Cell>> cells;

struct Point {
    int i, j;

    Point move(int di, int dj) const {
        return Point{i + di, j + dj};
    }

    Cell *operator->() const {
        return &cells[i][j];
    }

    bool isValid(int r, int c) const {
        return i >= 0 && j >= 0 && i < r && j < c;
    }
};

int main() {
    int r, c, k;
    std::cin >> r >> c >> k;

    cells = std::vector<std::vector<Cell>>(r, std::vector<Cell>(c));

    std::vector<Point> hospitals;
    while (k--) {
        int i, j;
        std::cin >> i >> j;
        hospitals.push_back({i, j});

        cells[i][j].isHospital = true;
        cells[i][j].minDist = 0;
    }

    std::queue<Point> q;
    for (Point &hospital : hospitals) {
        for (int i = 0; i < r; i++)
            for (int j = 0; j < c; j++) {
                cells[i][j].dist = -1;
            }
        
        hospital->dist = 0;
        q.push(hospital);

        while (!q.empty()) {
            Point v = q.front();
            q.pop();

            for (int di = -1; di <= 1; di++)
                for (int dj = -1; dj <= 1; dj++) {
                    if (di * dj != 0) continue;

                    Point u = v.move(di, dj);
                    if (!u.isValid(r, c)) continue;

                    if (u->dist == -1) {
                        u->dist = v->dist + 1;
                        u->minDist = std::min(u->minDist, u->dist);
                        q.push(u);
                    }
                }
        }
    }

    int answer = 0;
    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++) {
            answer = std::max(answer, cells[i][j].minDist);
        }

    std::cout << answer << std::endl;
}
