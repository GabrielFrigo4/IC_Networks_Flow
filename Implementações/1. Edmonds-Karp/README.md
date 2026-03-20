# [Edmonds-Karp](https://cp-algorithms.com/graph/edmonds_karp.html)
 1. [Time Travel](https://judge.beecrowd.com/en/problems/view/2082)
 2. [Maximum Flow](https://codeforces.com/problemset/problem/843/E)
 3. [Fast Maximum Flow](https://www.spoj.com/problems/FASTFLOW/)
 4. [Download Speed](https://cses.fi/problemset/task/1694)
 5. [Array and Operations](https://codeforces.com/contest/498/problem/c)
 6. [Red-Blue Graph](https://codeforces.com/contest/1288/problem/f)

 ```cpp
 #include <iostream>
 #include <vector>
 #include <queue>
 #include <algorithm>

 using Long = long long;
 constexpr Long INF = static_cast<Long>(1e14);

 struct Edge
 {
     Long from, to;
     Long cap, flow;
 };

 class EdmondsKarp
 {
 private:
     Long n;
     std::vector<Edge> edges;
     std::vector<std::vector<Long>> adj;

     Long bfs(Long s, Long t, std::vector<Long> &parent)
     {
         std::fill(parent.begin(), parent.end(), -1);
         std::queue<std::pair<Long, Long>> q;

         parent[s] = -2;
         q.push({s, INF});

         while (!q.empty())
         {
             Long cur = q.front().first;
             Long cur_f = q.front().second;
             q.pop();

             for (Long id : adj[cur])
             {
                 Long nxt = edges[id].to;
                 Long res = edges[id].cap - edges[id].flow;

                 if (parent[nxt] != -1 || res <= 0)
                     continue;

                 parent[nxt] = id;
                 Long min_f = std::min(cur_f, res);

                 if (nxt == t)
                     return min_f;

                 q.push({nxt, min_f});
             }
         }
         return 0;
     }

 public:
     EdmondsKarp(Long n) : n(n), adj(n) {}

     void add_edge(Long from, Long to, Long cap, bool is_directed = true)
     {
         adj[from].push_back(edges.size());
         edges.push_back({from, to, cap, 0});

         adj[to].push_back(edges.size());
         edges.push_back({to, from, is_directed ? 0 : cap, 0});
     }

     Long get_max_flow(Long s, Long t)
     {
         Long tot_f = 0, new_f;
         std::vector<Long> parent(n);

         while ((new_f = bfs(s, t, parent)) > 0)
         {
             tot_f += new_f;

             Long cur = t;
             while (cur != s)
             {
                 Long id = parent[cur];
                 edges[id].flow += new_f;
                 edges[id ^ 1].flow -= new_f;
                 cur = edges[id].from;
             }
         }
         return tot_f;
     }
 };
 ```
