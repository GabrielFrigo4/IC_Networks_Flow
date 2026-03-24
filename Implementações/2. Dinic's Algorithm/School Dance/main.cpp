#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using Long = long long;
constexpr Long INF = static_cast<Long>(1e14);

class FlowNetwork {
protected:
    Long n;
    std::vector<std::vector<Long>> adj;

public:
    struct Edge {
        Long from, to;
        Long cap, flow;
    };
    std::vector<Edge> edges;

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

class Dinic : public FlowNetwork {
private:
    std::vector<Long> level, ptr;

    bool bfs(Long s, Long t) {
        std::fill(level.begin(), level.end(), -1);
        level[s] = 0;
        std::queue<Long> q;
        q.push(s);

        while (!q.empty()) {
            Long cur = q.front();
            q.pop();
            for (Long id : adj[cur]) {
                Long nxt = edges[id].to;
                Long res = edges[id].cap - edges[id].flow;
                if (level[nxt] != -1 || res <= 0) continue;
                level[nxt] = level[cur] + 1;
                q.push(nxt);
            }
        }
        return level[t] != -1;
    }

    Long dfs(Long cur, Long t, Long pushed) {
        if (pushed == 0 || cur == t) return pushed;

        for (Long& cid = ptr[cur]; cid < (Long)adj[cur].size(); ++cid) {
            Long id = adj[cur][cid];
            Long nxt = edges[id].to;
            Long res = edges[id].cap - edges[id].flow;
            if (level[cur] + 1 != level[nxt] || res <= 0) continue;

            Long pushed_nxt = dfs(nxt, t, std::min(pushed, res));
            if (pushed_nxt == 0) continue;

            edges[id].flow += pushed_nxt;
            edges[id ^ 1].flow -= pushed_nxt;
            return pushed_nxt;
        }
        return 0;
    }

public:
    Dinic(Long n) : FlowNetwork(n), level(n), ptr(n) {}

    Long compute_max_flow(Long s, Long t) override {
        Long tot_f = 0;
        while (bfs(s, t)) {
            std::fill(ptr.begin(), ptr.end(), 0);
            Long pushed;
            while ((pushed = dfs(s, t, INF)) > 0) tot_f += pushed;
        }
        return tot_f;
    }
};

void task()
{
    Long n, m, k;
    std::cin >> n >> m >> k;

    Dinic fn(n + m + 2);
    for (Long i = 1; i <= n; i++)
    {
        fn.add_edge(0, i, 1);
    }
    for (Long i = n + 1; i <= n + m; i++)
    {
        fn.add_edge(i, n + m + 1, 1);
    }
    for (Long i = 0; i < k; i++)
    {
        Long u, v;
        std::cin >> u >> v;
        v += n;
        fn.add_edge(u, v, 1);
    }

    std::cout << fn.compute_max_flow(0, n + m + 1) << std::endl;
    for (const auto& e : fn.edges) {
        if (e.from >= 1 && e.from <= n && e.to > n && e.to <= n + m && e.flow == 1) {
            std::cout << e.from << " " << e.to - n << std::endl;
        }
    }
}

int main(void)
{
    task();
    return 0;
}
