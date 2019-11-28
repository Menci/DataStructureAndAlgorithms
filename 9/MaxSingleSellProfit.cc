#include <iostream>
#include <algorithm>

int main() {
	for (int n; std::cin >> n, n != 0; ) {
		int min = std::numeric_limits<int>::max(), ans = 0;
		for (int i = 0; i < n; i++) {
			int x;
			std::cin >> x;
			min = std::min(min, x);
			ans = std::max(ans, x - min);
		}

		std::cout << ans << std::endl;
	}
}
