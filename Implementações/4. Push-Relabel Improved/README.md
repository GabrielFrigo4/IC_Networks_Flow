# [Push-Relabel Improved](https://cp-algorithms.com/graph/push-relabel-faster.html)

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
 constexpr Long INF = std::numeric_limits<Long>::max() << 8;
 constexpr Size MAX = std::numeric_limits<Size>::max() << 8;
 
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
 
 class PushRelabelImproved : public FlowNetwork
 {
 private:
 	std::vector<Size> height;
 	std::vector<Long> excess;
 	std::vector<Size> count;
 	std::vector<Size> ptr;
 	std::vector<std::vector<Size>> active;
 	Size highest;
 
 	void enqueue(Size v, Size s, Size t)
 	{
 		if (v != s && v != t && excess[v] > 0)
 		{
 			active[height[v]].push_back(v);
 			highest = std::max(highest, height[v]);
 		}
 	}
 
 	void push(Size cur, Size id, Size s, Size t)
 	{
 		Size nxt = edges[id].to;
 		Long res = edges[id].cap - edges[id].flow;
 
 		if (res == 0 || height[cur] <= height[nxt])
 			return;
 
 		Long tr = std::min(excess[cur], res);
 		edges[id].flow += tr;
 		edges[id ^ 1ULL].flow -= tr;
 		excess[cur] -= tr;
 		excess[nxt] += tr;
 
 		if (excess[nxt] == tr)
 		{
 			enqueue(nxt, s, t);
 		}
 	}
 
 	void gap(Size k, Size s, Size t)
 	{
 		for (Size i = 0; i < size; ++i)
 		{
 			if (i != s && height[i] > k && height[i] < size)
 			{
 				count[height[i]]--;
 				height[i] = std::max(height[i], size + 1);
 				count[height[i]]++;
 				enqueue(i, s, t);
 			}
 		}
 	}
 
 	void relabel(Size cur, Size s, Size t)
 	{
 		Size min_h = MAX;
 		for (Size id : adj[cur])
 		{
 			if (edges[id].cap - edges[id].flow > 0)
 			{
 				min_h = std::min(min_h, height[edges[id].to]);
 			}
 		}
 
 		if (min_h != MAX)
 		{
 			count[height[cur]]--;
 			Size old_h = height[cur];
 
 			height[cur] = min_h + 1;
 			count[height[cur]]++;
 			enqueue(cur, s, t);
 
 			if (count[old_h] == 0 && old_h < size)
 			{
 				gap(old_h, s, t);
 			}
 		}
 	}
 
 	void discharge(Size cur, Size s, Size t)
 	{
 		while (excess[cur] > 0)
 		{
 			if (ptr[cur] < adj[cur].size())
 			{
 				Size id = adj[cur][ptr[cur]];
 				Size nxt = edges[id].to;
 				Long res = edges[id].cap - edges[id].flow;
 
 				if (res > 0 && height[cur] == height[nxt] + 1)
 				{
 					push(cur, id, s, t);
 				}
 				else
 				{
 					ptr[cur]++;
 				}
 			}
 			else
 			{
 				relabel(cur, s, t);
 				ptr[cur] = 0;
 			}
 		}
 	}
 
 public:
 	PushRelabelImproved(Size n) : FlowNetwork(n) {}
 
 	static std::unique_ptr<FlowNetwork> create(Size n)
 	{
 		return std::make_unique<PushRelabelImproved>(n);
 	}
 
 	std::unique_ptr<FlowNetwork> make(Size n) const override
 	{
 		return std::make_unique<PushRelabelImproved>(n);
 	}
 
 	std::unique_ptr<FlowNetwork> clone() const override
 	{
 		return std::make_unique<PushRelabelImproved>(*this);
 	}
 
 	Long compute_max_flow(Size s, Size t) override
 	{
 		Size max_height_limit = size * 2 + 1;
 
 		height.assign(size, 0);
 		excess.assign(size, 0);
 		count.assign(max_height_limit, 0);
 		ptr.assign(size, 0);
 		active.assign(max_height_limit, std::vector<Size>());
 		highest = 0;
 
 		height[s] = size;
 		count[size] = 1;
 		count[0] = size - 1;
 
 		excess[s] = INF;
 		for (Size id : adj[s])
 		{
 			push(s, id, s, t);
 		}
 
 		while (true)
 		{
 			if (active[highest].empty())
 			{
 				if (highest == 0)
 					break;
 				highest--;
 				continue;
 			}
 
 			Size cur = active[highest].back();
 			active[highest].pop_back();
 
 			if (height[cur] == highest)
 			{
 				discharge(cur, s, t);
 			}
 		}
 
 		return excess[t];
 	}
 };
 ```
