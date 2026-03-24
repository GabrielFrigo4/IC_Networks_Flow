#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using Long = long long;
constexpr Long INF = static_cast<Long>(1e14);

class FlowNetwork {
protected:
    struct Edge {
        Long from, to;
        Long cap, flow;
    };
    Long n;
    std::vector<Edge> edges;
    std::vector<std::vector<Long>> adj;

public:
    FlowNetwork(Long n) : n(n), adj(n) {}
    virtual ~FlowNetwork() = default;

    virtual void add_edge(Long from, Long to, Long cap, Long rev_cap = 0) {
        adj[from].push_back(edges.size());
        edges.push_back({from, to, cap, 0});
        adj[to].push_back(edges.size());
        edges.push_back({to, from, rev_cap, 0});
    }

    virtual Long compute_max_flow(Long s, Long t) = 0;
};

class EdmondsKarp : public FlowNetwork {
private:
    Long bfs(Long s, Long t, std::vector<Long>& parent) {
        std::fill(parent.begin(), parent.end(), -1);
        std::queue<std::pair<Long, Long>> q;
        parent[s] = -2;
        q.push({s, INF});

        while (!q.empty()) {
            Long cur = q.front().first;
            Long cur_f = q.front().second;
            q.pop();

            for (Long id : adj[cur]) {
                Long nxt = edges[id].to;
                Long res = edges[id].cap - edges[id].flow;
                if (parent[nxt] != -1 || res <= 0) continue;

                parent[nxt] = id;
                Long min_f = std::min(cur_f, res);
                if (nxt == t) return min_f;
                q.push({nxt, min_f});
            }
        }
        return 0;
    }

public:
    EdmondsKarp(Long n) : FlowNetwork(n) {}

    Long compute_max_flow(Long s, Long t) override {
        Long tot_f = 0, new_f;
        std::vector<Long> parent(n);

        while ((new_f = bfs(s, t, parent)) > 0) {
            tot_f += new_f;
            Long cur = t;
            while (cur != s) {
                Long id = parent[cur];
                edges[id].flow += new_f;
                edges[id ^ 1].flow -= new_f;
                cur = edges[id].from;
            }
        }
        return tot_f;
    }
};

void task()
{
    Long n, m;
    std::cin >> n >> m;

    EdmondsKarp fn(n);
    for (Long k = 0; k < m; k++)
    {
        Long u, v, c;
        std::cin >> u >> v >> c;
        u--;
        v--;
        fn.add_edge(u, v, c);
    }

    std::cout << fn.compute_max_flow(0, n - 1) << std::endl;
}

int main(void)
{
    task();
    return 0;
}
