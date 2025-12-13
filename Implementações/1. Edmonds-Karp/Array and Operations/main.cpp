#include <iostream>
#include <utility>
#include <vector>
#include <queue>

#define long long long
#define INF static_cast<long>(1e14)
#define NONE -1

long bfs(long s, long t, std::vector<long> &parent, std::vector<std::vector<long>> &adj, std::vector<std::vector<long>> &capacity) {
    std::fill(parent.begin(), parent.end(), NONE);
    parent[s] = s;
    std::queue<std::pair<long, long>> q;
    q.push({ s, INF });
    while (!q.empty()) {
        long cur = std::get<0>(q.front());
        long flow = std::get<1>(q.front());
        q.pop();
        for (long next : adj[cur]) {
            if (parent[next] == NONE && capacity[cur][next]) {
                parent[next] = cur;
                long new_flow = std::min(flow, capacity[cur][next]);
                if (next == t) {
                    return new_flow;
                }
                q.push({ next, new_flow });
            }
        }
    }
    return 0;
}

long maxflow(long s, long t, std::vector<std::vector<long>> &adj, std::vector<std::vector<long>> &capacity) {
    long flow = 0;
    std::vector<long> parent(adj.size());
    while (long new_flow = bfs(s, t, parent, adj, capacity)) {
        flow += new_flow;
        for (long cur = t; cur != s; cur = parent[cur]) {
            capacity[parent[cur]][cur] -= new_flow;
            capacity[cur][parent[cur]] += new_flow;
        }
    }
    return flow;
}

int main() {
    long n, m;
    std::cin >> n >> m;
    std::vector<long> a(n);
    for (auto &_a : a) {
        std::cin >> _a;
    }
    std::vector<std::vector<long>> adj(n, std::vector<long>());
    std::vector<std::vector<long>> capacity(n, std::vector<long>(n, 0));
    while (m--) {
        long i, j, c;
        std::cin >> i >> j >> c;
        --i, --j;
        adj[i].push_back(j);
        capacity[i][j] += c;
    }
    return 0;
}
