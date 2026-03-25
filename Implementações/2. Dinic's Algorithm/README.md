# [Dinic's Algorithm](https://cp-algorithms.com/graph/dinic.html)
 1. [Download Speed](https://cses.fi/problemset/task/1694)
 2. [School Dance](https://cses.fi/problemset/task/1696)
 3. [Distinct Routes](https://cses.fi/problemset/task/1711)
 4. [Time Travel](https://judge.beecrowd.com/en/problems/view/2082)

 ```cpp
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
 ```
