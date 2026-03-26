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
	Size size;
	std::vector<std::vector<Size>> adj;

public:
	struct Edge
	{
		Size from, to;
		Long cap, flow;
	};
	std::vector<Edge> edges;

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

void task()
{
	Size n, m, k;
	std::cin >> n >> m >> k;

	auto fn = PushRelabel::create(n + m + 2);
	for (Size i = 1; i <= n; i++)
	{
		fn->add_edge(0, i, 1);
	}
	for (Size i = n + 1; i <= n + m; i++)
	{
		fn->add_edge(i, n + m + 1, 1);
	}
	for (Size i = 0; i < k; i++)
	{
		Size u, v;
		std::cin >> u >> v;
		v += n;
		fn->add_edge(u, v, 1);
	}

	std::cout << fn->compute_max_flow(0, n + m + 1) << std::endl;
	for (const auto &e : fn->edges)
	{
		if (e.from >= 1 && e.from <= n && e.to > n && e.to <= n + m && e.flow == 1)
		{
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
