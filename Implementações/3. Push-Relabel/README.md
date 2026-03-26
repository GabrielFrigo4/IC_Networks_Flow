# [Push-Relabel](https://cp-algorithms.com/graph/push-relabel.html)

## CSES
 1. [Download Speed](https://cses.fi/problemset/task/1694)
 2. [School Dance](https://cses.fi/problemset/task/1696)
 3. [Distinct Routes](https://cses.fi/problemset/task/1711)
 4. [Police Chase](https://cses.fi/problemset/task/1695)

## Beecrowd
 1. [Time Travel](https://judge.beecrowd.com/en/problems/view/2082)

## FlowNetwork
 ```cpp
 #include <algorithm>
 #include <iostream>
 #include <limits>
 #include <memory>
 #include <vector>

 using Long = long long;
 using Size = std::size_t;
 constexpr Long INF = std::numeric_limits<Long>::max() / 4;
 constexpr Size MAX = std::numeric_limits<Size>::max() / 4;

 class FlowNetwork
 {
 protected:
 	struct Edge
 	{
 		Size from, to;
 		Long cap, flow;
 	};
 	Size size;
 	std::vector<Edge> edges;
 	std::vector<std::vector<Size>> adj;

 public:
 	FlowNetwork(Size n) : size(n), adj(n) {}
 	virtual ~FlowNetwork() = default;

 	virtual std::unique_ptr<FlowNetwork> make(Size n) const = 0;
 	virtual std::unique_ptr<FlowNetwork> clone() const = 0;

 	virtual void add_edge(Size from, Size to, Long cap, Long rev_cap = 0)
 	{
 		adj[from].push_back(edges.size());
 		edges.push_back({from, to, cap, 0});
 		adj[to].push_back(edges.size());
 		edges.push_back({to, from, rev_cap, 0});
 	}
 
 	virtual Long compute_max_flow(Size s, Size t) = 0;
 };
 
 class PushRelabel : public FlowNetwork
 {
 private:
 	std::vector<Size> height;
 	std::vector<Long> excess;
 
 	void push(Size cur, Size id)
 	{
 		Size nxt = edges[id].to;
 		Long res = edges[id].cap - edges[id].flow;
 		Long tr = std::min(excess[cur], res);
 
 		if (tr == 0)
 			return;
 
 		edges[id].flow += tr;
 		edges[id ^ 1ULL].flow -= tr;
 		excess[cur] -= tr;
 		excess[nxt] += tr;
 	}
 
 	void relabel(Size cur)
 	{
 		Size min_h = MAX;
 		for (Size id : adj[cur])
 		{
 			Long res = edges[id].cap - edges[id].flow;
 			if (res > 0)
 			{
 				min_h = std::min(min_h, height[edges[id].to]);
 			}
 		}
 		if (min_h != MAX)
 		{
 			height[cur] = min_h + 1;
 		}
 	}
 
 public:
 	PushRelabel(Size n) : FlowNetwork(n), height(n), excess(n) {}
 
 	static std::unique_ptr<FlowNetwork> create(Size n)
 	{
 		return std::make_unique<PushRelabel>(n);
 	}
 
 	std::unique_ptr<FlowNetwork> make(Size n) const override
 	{
 		return std::make_unique<PushRelabel>(n);
 	}
 
 	std::unique_ptr<FlowNetwork> clone() const override
 	{
 		return std::make_unique<PushRelabel>(*this);
 	}
 
 	Long compute_max_flow(Size s, Size t) override
 	{
 		std::fill(height.begin(), height.end(), 0);
 		std::fill(excess.begin(), excess.end(), 0);
 
 		height[s] = size;
 		excess[s] = INF;
 
 		for (Size id : adj[s])
 		{
 			Long res = edges[id].cap - edges[id].flow;
 			if (res > 0)
 			{
 				push(s, id);
 			}
 		}
 
 		while (true)
 		{
 			Size cur = MAX;
 			for (Size i = 0; i < size; ++i)
 			{
 				if (i != s && i != t && excess[i] > 0)
 				{
 					if (cur == MAX || height[i] > height[cur])
 					{
 						cur = i;
 					}
 				}
 			}
 
 			if (cur == MAX)
 				break;
 
 			bool pushed = false;
 			for (Size id : adj[cur])
 			{
 				Size nxt = edges[id].to;
 				Long res = edges[id].cap - edges[id].flow;
 
 				if (res > 0 && height[cur] == height[nxt] + 1)
 				{
 					push(cur, id);
 					pushed = true;
 					if (excess[cur] == 0)
 						break;
 				}
 			}
 
 			if (!pushed)
 			{
 				relabel(cur);
 			}
 		}
 
 		return excess[t];
 	}
 };
 ```
