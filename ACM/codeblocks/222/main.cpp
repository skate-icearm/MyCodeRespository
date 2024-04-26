#include <bits/stdc++.h>

using namespace std;
#define endl '\n'
typedef long long ll;
typedef pair<int,int> pii;
const int N = 1e3+10;
double g[N][N];
vector<int > tmp[N];


int main()
{
    int N, M;
    cin >> N >> M;
    map<int, map<int, double>> intimacy;

    for (int i = 0; i < M; ++i)
    {
        int K;
        cin >> K;
        vector<int> photo(K);
        for (int& p : photo)
            cin >> p;
    }

    int A, B;
    cin >> A >> B;

    cout<<A<<" " <<B<<endl;
    return 0;
}
/*
5 7 5
1 2 3 4 5 6 7
3 2 1 7 5 6 4
7 6 5 4 3 2 1
5 6 4 3 7 2 1
1 7 6 5 4 3 2
*/






