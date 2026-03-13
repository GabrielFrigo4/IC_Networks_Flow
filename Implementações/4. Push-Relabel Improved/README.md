# [Push-Relabel Improved](https://cp-algorithms.com/graph/push-relabel-faster.html)
 ```cpp
 #include <iostream>
 #include <utility>
 #include <vector>
 #include <queue>

 #define long long long
 #define INF static_cast<long>(1e14)
 #define NONE -1

 long maxflow(long s, long t, std::vector<std::vector<long>> &adj, std::vector<std::vector<long>> &capacity) {
     long flow = 0;
     return flow;
 }

 void digraph() {
     long n, m, s, t;
     std::cin >> n >> m >> s >> t;
     --s, --t;
     std::vector<std::vector<long>> adj(n, std::vector<long>());
     std::vector<std::vector<long>> capacity(n, std::vector<long>(n, 0));
     while (m--) {
         long i, j, c;
         std::cin >> i >> j >> c;
         --i, --j;
         adj[i].push_back(j);
         adj[j].push_back(i);
         capacity[i][j] += c;
     }
     long flow = maxflow(s, t, adj, capacity);
     std::cout << flow << std::endl;
 }

 void graph() {
     long n, m, s, t;
     std::cin >> n >> m >> s >> t;
     --s, --t;
     std::vector<std::vector<long>> adj(n, std::vector<long>());
     std::vector<std::vector<long>> capacity(n, std::vector<long>(n, 0));
     while (m--) {
         long i, j, c;
         std::cin >> i >> j >> c;
         --i, --j;
         adj[i].push_back(j);
         adj[j].push_back(i);
         capacity[i][j] += c;
         capacity[j][i] += c;
     }
     long flow = maxflow(s, t, adj, capacity);
     std::cout << flow << std::endl;
 }
 ```