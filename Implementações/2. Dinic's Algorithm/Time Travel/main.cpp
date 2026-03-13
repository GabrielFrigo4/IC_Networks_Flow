#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using Long = long long;
constexpr Long INF = static_cast<Long>(1e14);

struct Edge
{
    Long from, to, cap, flow;
};

class Dinic
{
private:
    Long n;
    std::vector<Edge> edges;
    std::vector<std::vector<Long>> adj;
    std::vector<Long> level;
    std::vector<Long> ptr;

    bool bfs(Long s, Long t)
    {
        std::fill(level.begin(), level.end(), -1);
        level[s] = 0;
        std::queue<Long> q;
        q.push(s);

        while (!q.empty())
        {
            Long cur = q.front();
            q.pop();

            for (Long id : adj[cur])
            {
                Long nxt = edges[id].to;
                Long res = edges[id].cap - edges[id].flow;

                if (level[nxt] != -1 || res <= 0)
                    continue;

                level[nxt] = level[cur] + 1;
                q.push(nxt);
            }
        }
        return level[t] != -1;
    }

    Long dfs(Long cur, Long t, Long pushed)
    {
        if (pushed == 0 || cur == t)
            return pushed;

        for (Long &cid = ptr[cur]; cid < adj[cur].size(); ++cid)
        {
            Long id = adj[cur][cid];
            Long nxt = edges[id].to;
            Long res = edges[id].cap - edges[id].flow;

            if (level[cur] + 1 != level[nxt] || res <= 0)
                continue;

            Long pushed_nxt = dfs(nxt, t, std::min(pushed, res));

            if (pushed_nxt == 0)
                continue;

            edges[id].flow += pushed_nxt;
            edges[id ^ 1].flow -= pushed_nxt;
            return pushed_nxt;
        }
        return 0;
    }

public:
    Dinic(Long n) : n(n), adj(n), level(n), ptr(n) {}

    void add_edge(Long from, Long to, Long cap, bool is_directed = true)
    {
        adj[from].push_back(edges.size());
        edges.push_back({from, to, cap, 0});

        adj[to].push_back(edges.size());
        edges.push_back({to, from, is_directed ? 0 : cap, 0});
    }

    Long get_max_flow(Long s, Long t)
    {
        Long tot_f = 0;

        while (bfs(s, t))
        {
            std::fill(ptr.begin(), ptr.end(), 0);
            Long pushed;
            while ((pushed = dfs(s, t, INF)) > 0)
            {
                tot_f += pushed;
            }
        }
        return tot_f;
    }
};

void task()
{
    Long n, m;
    std::cin >> n >> m;

    std::vector<std::vector<Long>> cap(n, std::vector<Long>(n, 0));
    for (Long k = 0; k < m; k++)
    {
        Long u, v, c;
        std::cin >> u >> v >> c;
        u--;
        v--;
        cap[u][v] = c;
        cap[v][u] = c;
    }

    Dinic dn(n);
    for (Long i = 0; i < n; i++)
    {
        for (Long j = i + 1; j < n; j++)
        {
            if (cap[i][j] > 0)
            {
                dn.add_edge(i, j, cap[i][j], false);
            }
        }
    }

    Long global_min_cut = INF;
    for (Long t = 1; t < n; t++)
    {
        Dinic dn_copy = dn;
        Long current_flow = dn_copy.get_max_flow(0, t);
        global_min_cut = std::min(global_min_cut, current_flow);
    }

    std::cout << global_min_cut << std::endl;
}

int main(void)
{
    Long t;
    std::cin >> t;
    while (t--)
    {
        task();
    }
    return 0;
}
