#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <memory>

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

    virtual std::unique_ptr<FlowNetwork> make(Long n) const = 0;

    virtual std::unique_ptr<FlowNetwork> clone() const = 0;

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

    static std::unique_ptr<FlowNetwork> create(Long n) {
        return std::make_unique<EdmondsKarp>(n);
    }

    std::unique_ptr<FlowNetwork> make(Long n) const override {
        return std::make_unique<EdmondsKarp>(n);
    }

    std::unique_ptr<FlowNetwork> clone() const override {
        return std::make_unique<EdmondsKarp>(*this);
    }

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
    Long n, m, k;
    std::cin >> n >> m >> k;

    auto fn = EdmondsKarp::create(n + m + 2);
    for (Long i = 1; i <= n; i++)
    {
        fn->add_edge(0, i, 1);
    }
    for (Long i = n + 1; i <= n + m; i++)
    {
        fn->add_edge(i, n + m + 1, 1);
    }
    for (Long i = 0; i < k; i++)
    {
        Long u, v;
        std::cin >> u >> v;
        v += n;
        fn->add_edge(u, v, 1);
    }

    std::cout << fn->compute_max_flow(0, n + m + 1) << std::endl;
    for (const auto& e : fn->edges) {
        if (e.from >= 1 && e.from <= n && e.to > n && e.to <= n + m && e.flow == 1) {
            std::cout << e.from << " " << e.to - n << std::endl;
        }
    }
}

int main(void)
{
    std::ios_base::sync_with_stdio(false);
    std::cout.tie(nullptr);
    std::cin.tie(nullptr);

    task();
    return 0;
}
