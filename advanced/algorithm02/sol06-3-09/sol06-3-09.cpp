// sol06-3-09.cpp : 10진수를 2진수로 변환하기

#include <iostream>
#include <stack>
#include <string>

using namespace std;

string solution(int decimal) {
    stack<int> stack;
    string answer;

    while (decimal > 0) {
        stack.push(decimal % 2);
        decimal /= 2;
    }

    while (!stack.empty()) {
        answer.append(to_string(stack.top()));
        stack.pop();
    }
    return answer;
}

int main()
{
    cout << "TEST CASE 1" << endl;
    int cases1 = 10;
    cout << solution(cases1) << endl;

    cout << endl;

    cout << "TEST CASE 2" << endl;
    int cases2 = 27;
    cout << solution(cases2) << endl;

    cout << endl;

    cout << "TEST CASE 3" << endl;
    int cases3 = 12345;
    cout << solution(cases3) << endl;
}
