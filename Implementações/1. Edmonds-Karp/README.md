# [Edmonds-Karp](https://cp-algorithms.com/graph/edmonds_karp.html)
 1. [Time Travel](https://judge.beecrowd.com/en/problems/view/2082)
 2. [Array and Operations](https://codeforces.com/contest/498/problem/c)
 3. [Red-Blue Graph](https://codeforces.com/contest/1288/problem/f)

 ```cpp
 #include <iostream>
 #include <vector>
 #include <queue>
 #include <algorithm>

 #define long long long
 const long INF = 1e18;

 struct Edge {
     int from, to;
     long cap, flow;
 };

 struct EdmondsKarp {
     int n;
     std::vector<Edge> edges;
     std::vector<std::vector<int>> adj;

     EdmondsKarp(int n) : n(n), adj(n) {}

     void add_edge(int from, int to, long cap, bool is_directed = true) {
         adj[from].push_back(edges.size());
         edges.push_back({from, to, cap, 0});
        
         adj[to].push_back(edges.size());
         edges.push_back({to, from, is_directed ? 0 : cap, 0}); 
     }

     long get_max_flow(int s, int t) {
         long total_flow = 0;
         std::vector<int> parent(n);

         while (true) {
             std::fill(parent.begin(), parent.end(), -1);
             std::queue<std::pair<int, long>> q;
            
             parent[s] = -2;
             q.push({s, INF});
             long new_flow = 0;

             while (!q.empty()) {
                 int cur = q.front().first;
                 long current_flow = q.front().second;
                 q.pop();

                 for (int edge_idx : adj[cur]) {
                     int next = edges[edge_idx].to;
                     long residual_cap = edges[edge_idx].cap - edges[edge_idx].flow;

                     if (parent[next] == -1 && residual_cap > 0) {
                         parent[next] = edge_idx; 
                         long min_flow = std::min(current_flow, residual_cap);
                        
                         if (next == t) {
                             new_flow = min_flow;
                             break;
                         }
                         q.push({next, min_flow});
                     }
                 }
                 if (new_flow > 0) break;
             }

             if (new_flow == 0) break;
             total_flow += new_flow;
            
             int curr_node = t;
             while (curr_node != s) {
                 int edge_idx = parent[curr_node];
                 edges[edge_idx].flow += new_flow;
                 edges[edge_idx ^ 1].flow -= new_flow;
                 curr_node = edges[edge_idx].from;
             }
         }
         return total_flow;
     }
 };
 ```
