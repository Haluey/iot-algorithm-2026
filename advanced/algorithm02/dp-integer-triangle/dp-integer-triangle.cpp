#include <iostream>
#include <vector>

using namespace std;

int solution(vector<vector<int>> triangle) {
    int n = triangle.size();

    // 각 위치에서 바닥까지 내려갔을 때 얻을 수 있는 최대 합을 저장한다.
    vector<vector<int>> dp(n, vector<int>(n, 0));

    // 맨 아래 행은 더 이상 이동할 수 없으므로 원래 값으로 초기화한다.
    for (int i = 0; i < n; i++) {
        dp[n - 1][i] = triangle[n - 1][i];
    }

    // 아래에서 두 번째 행부터 꼭대기까지 올라가며 계산한다.
    for (int i = n - 2; i >= 0; i--) {
        for (int j = 0; j <= i; j++) {
            dp[i][j] =
                triangle[i][j]
                + max(dp[i + 1][j], dp[i + 1][j + 1]);
        }
    }

    // 꼭대기에서 바닥까지 내려갔을 때 얻을 수 있는 최대 합
    return dp[0][0];
}

int main() {
    vector<vector<int>> triangle = {
        {7},
        {3, 8},
        {8, 1, 0},
        {2, 7, 4, 4},
        {4, 5, 2, 6, 5}
    };

    cout << solution(triangle) << endl;  // 30

    return 0;
}