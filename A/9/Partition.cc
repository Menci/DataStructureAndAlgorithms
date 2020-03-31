#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cstdlib>
#include <cmath>

const double PI = acos(-1);

struct RealPoint {
	int x, y;
};

struct Point {
	int x, y, count;
	double angle;
	std::vector<RealPoint> realPoints;
};

inline bool isSameAngle(int x1, int y1, int x2, int y2) {
	int g1 = std::gcd(x1, y1), g2 = std::gcd(x2, y2);
	return x1 / g1 == x2 / g2 && y1 / g1 == y2 / g2;
}

inline bool isOnSameLine(int x1, int y1, int x2, int y2) {
	return isSameAngle(abs(x1), abs(y1), abs(x2), abs(y2));
}

inline double angle(int x, int y) {
	// angle from 0 to 2PI: from positive x-axis, anti-clockwise, to positive x-axis
	double tmp = atan2(y, x);
	if (tmp < 0) {
		tmp += 2 * PI;
	}
	return tmp;
}

inline void transform(int &x, int &y, int w, int h, bool reverse = false) {
	if (!reverse) {
		x = x * 2 - w;
		y = y * 2 - h;
	} else {
		x = (x + w) / 2;
		y = (y + h) / 2;
	}
}

inline void printPoint(const Point *point, int w, int h, size_t &count) {
	for (const RealPoint &realPoint : point->realPoints) {
		if (count == 0) return;

		int x = realPoint.x, y = realPoint.y;
		transform(x, y, w, h, true);
		std::cout << x << " " << y << std::endl;

		count--;
	}
}

int main() {
	for (int n, w, h; std::cin >> n >> w >> h, n != 0; ) {
		std::vector<Point> points(n);
		for (Point &point : points) {
			std::cin >> point.x >> point.y;
			transform(point.x, point.y, w, h);
			point.angle = angle(point.x, point.y);
		}

		std::sort(points.begin(), points.end(), [](const Point &lhs, const Point &rhs) {
			return lhs.angle < rhs.angle;
		});

		Point *equalRangeStart = nullptr;
		for (Point &point : points) {
			// Do NOT use .angle == .angle since there's floating error
			if (equalRangeStart && isSameAngle(equalRangeStart->x, equalRangeStart->y, point.x, point.y)) {
				equalRangeStart->count++;
				point.count = 0;
			} else {
				equalRangeStart = &point;
				point.count = 1;
			}
			equalRangeStart->realPoints.push_back({point.x, point.y});
		}

		// Imagine we're starting with a line on x-axis but have a little offset
		// so it doesn't cover any points on x-axis
		// 
		// We call the two side of the rotating line "above" and "below" according the
		// initial state, above or below the x-axis, regardless of the real up/down direction
		size_t aboveCount = 0, belowCount = 0;

		// Starting with the horizontal line
		std::queue<Point *> qAbove, qBelow;
		for (Point &point : points) {
			if (point.count == 0) continue;

			// Above or on positive x-axis: qAvobe
			// Below or on negative x-axis: qBelow
			if (point.y > 0 || (point.y == 0 && point.x > 0)) {
				qAbove.push(&point);
				aboveCount += point.count;
			} else {
				qBelow.push(&point);
				belowCount += point.count;
			}
		}

		// Along with the rotation of the line, the points on the line which are previously
		// above the line will go below the line, otherwise go above the line
		Point *onLineFromAbove = nullptr, *onLineFromBelow = nullptr;

		// The angle of each of two sides of the line and positive x-axis, up and down still
		// only describes the initial state
		double currentAngleUp = angle(1, 0), currentAngleDown = angle(-1, 0);

		// The points passed by the dividing line
		std::vector<Point *> passedAbove, passedBelow;
	
		while (!qAbove.empty() || !qBelow.empty()) {
			if (onLineFromBelow) {
				aboveCount += onLineFromBelow->count;
				passedAbove.push_back(onLineFromBelow);
			}

			if (onLineFromAbove) {
				belowCount += onLineFromAbove->count;
				passedBelow.push_back(onLineFromAbove);
			}

			onLineFromAbove = onLineFromBelow = nullptr;

			Point *nearestAbove = qAbove.empty() ? nullptr : qAbove.front(),
				  *nearestBelow = qBelow.empty() ? nullptr : qBelow.front();

			// Choose a smaller angle increment from two sides of the line to rotate
			Point *nearestAnglePoint = (!nearestBelow || (nearestAbove && nearestAbove->angle - currentAngleUp < nearestBelow->angle - currentAngleDown))
									 ? nearestAbove
									 : nearestBelow;

			// Check if nearest point above / below is on the rotated line
			if (nearestAbove && isOnSameLine(nearestAbove->x, nearestAbove->y, nearestAnglePoint->x, nearestAnglePoint->y)) {
				onLineFromAbove = nearestAbove;
				aboveCount -= nearestAbove->count;
				qAbove.pop();
			}

			if (nearestBelow && isOnSameLine(nearestBelow->x, nearestBelow->y, nearestAnglePoint->x, nearestAnglePoint->y)) {
				onLineFromBelow = nearestBelow;
				belowCount -= nearestBelow->count;
				qBelow.pop();
			}

			// Check if the current position of the dividing line is a valid solution
			// since we can choose which half to assign each point that lies exactly on the dividing
			// line, points "above" and "below" only needs to be less than or equal to half of number of points
			if (aboveCount <= n / 2 && belowCount <= n / 2) {
				// Print the solution (points "above" the line only)
				// It's not required, but we can print with the ascending angle order
				size_t count = n / 2, countAbove = aboveCount, countNotAbove = count - aboveCount;

				if (onLineFromAbove) printPoint(onLineFromAbove, w, h, countNotAbove);

				while (!qAbove.empty() && countAbove > 0) {
					Point *point = qAbove.front();
					qAbove.pop();
					printPoint(point, w, h, countAbove);
				}

				for (Point *point : passedAbove) {
					printPoint(point, w, h, countAbove);
					if (countAbove == 0) break;
				}

				if (onLineFromBelow) printPoint(onLineFromBelow, w, h, countNotAbove);

				break;
			}
		}
	}
}
