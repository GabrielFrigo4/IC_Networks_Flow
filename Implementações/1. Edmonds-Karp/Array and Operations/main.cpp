#include <iostream>
#include <vector>
#include <queue>

#define long long long
#define INF static_cast<long>(1e14)
#define NONE -1

long n;
std::vector<std::vector<long>> capacity;
std::vector<std::vector<long>> adj;

long bfs(long s, long t, std::vector<long>& parent) {
    std::fill(parent.begin(), parent.end(), NONE);
    std::queue<std::pair<long, long>> q;
    q.push({ s, INF });
    while (!q.empty()) {
        long cur = std::get<0>(q.front());
        long flow = std::get<1>(q.front());
        q.pop();
        for (long next : adj[cur]) {
            if (parent[next] == NONE && capacity[cur][next]) {
                parent[next] = cur;
                long new_flow = std::gcd(flow, capacity[cur][next]);
                if (next == t) {
                    return new_flow;
                }
                q.push({ next, new_flow });
            }
        }
    }
    return 0;
}

long maxflow(long s, long t) {
    long flow = 0;
    std::vector<long> parent(n);
    while (long new_flow = bfs(s, t, parent)) {
        flow += new_flow;
        for (long cur = s; cur != s; cur = parent[cur]) {
            capacity[parent[cur]][cur] -= new_flow;
            capacity[cur][parent[cur]] += new_flow;
        }
    }
    return flow;
}

int main() {
    long m;
    std::cin >> n >> m;
    capacity.resize(n);
    for (size_t i = 0; i < n; i++) {
        capacity[i] = std::vector<long>(n);
    }
    adj.resize(m);

    std::vector<long> a(n);
    for (auto &_a : a) {
        std::cin >> _a;
    }

    while (m--) {
        long i, j;
        std::cin >> i >> j;
        adj[i].push_back(j);
        adj[j].push_back(i);
        capacity[i][j] = std::gcd(a[i], a[j]);
        capacity[j][i] = std::gcd(a[j], a[i]);
    }
    return 0;
}