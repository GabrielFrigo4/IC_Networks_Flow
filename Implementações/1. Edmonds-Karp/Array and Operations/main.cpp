#include <iostream>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <queue>

#define NONE -1
#define INF static_cast<int32_t>(1e7)

int32_t n;
std::vector<std::vector<int64_t>> capacity;
std::vector<std::vector<int32_t>> adj;

int32_t bfs(int32_t s, int32_t t, std::vector<int32_t>& parent) {
    std::fill(parent.begin(), parent.end(), NONE);
    std::queue<std::pair<int32_t, int64_t>> q;
    q.push({ s, INF });
    while (!q.empty()) {
        int32_t cur = std::get<0>(q.front());
        int64_t flow = std::get<1>(q.front());
        q.pop();
        for (int32_t next : adj[cur]) {
            if (parent[next] == NONE && capacity[cur][next]) {
                parent[next] = cur;
                int32_t new_flow = std::gcd(flow, capacity[cur][next]);
                if (next == t) {
                    return new_flow;
                }
                q.push({ next, new_flow });
            }
        }
    }
    return 0;
}

int32_t maxflow(int32_t s, int32_t t) {
    int32_t flow = 0;
    std::vector<int32_t> parent(n);
    while (int32_t new_flow = bfs(s, t, parent)) {
        flow += new_flow;
        for (int32_t cur = s; cur != s; cur = parent[cur]) {
            capacity[parent[cur]][cur] -= new_flow;
            capacity[cur][parent[cur]] += new_flow;
        }
    }
    return flow;
}

int main() {
    int32_t m;
    std::cin >> n >> m;
    for (size_t i = 0; i < n; i++) {
        capacity.push_back(std::vector<int32_t>(n));
    }
    adj.resize(m);

    std::vector<int64_t> a(n);
    for (auto &_a : a) {
        std::cin >> _a;
    }

    while (m--) {
        int32_t i, j;
        std::cin >> i >> j;
        adj[i].push_back(j);
        adj[j].push_back(i);
        capacity[i][j] = std::gcd(a[i], a[j]);
        capacity[j][i] = std::gcd(a[j], a[i]);
    }
    return 0;
}