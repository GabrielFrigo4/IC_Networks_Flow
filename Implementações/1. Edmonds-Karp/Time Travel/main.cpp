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

void task() {
    int n, m;
    std::cin >> n >> m;
    
    EdmondsKarp ek(n); 
    
    for (int k = 0; k < m; k++) {
        int u, v;
        long long c;
        std::cin >> u >> v >> c;
        u--; v--; 
        
        ek.add_edge(u, v, c, false); 
    }
    
    long long global_min_cut = INF;
    for (int t = 1; t < n; t++) {
        EdmondsKarp ek_copy = ek; 
        long long current_flow = ek_copy.get_max_flow(0, t);
        global_min_cut = std::min(global_min_cut, current_flow);
    }
    
    std::cout << global_min_cut << std::endl;
}

int main(void) {
    int32_t t;
    std::cin >> t;
    while(t--) {
        task();
    }
    return 0;
}
