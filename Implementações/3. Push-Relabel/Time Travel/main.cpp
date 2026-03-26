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

void task()
{
	Size n, m;
	std::cin >> n >> m;

	std::vector<std::vector<Long>> cap(n, std::vector<Long>(n, 0));
	for (Size k = 0; k < m; k++)
	{
		Size u, v;
		Long c;
		std::cin >> u >> v >> c;
		u--;
		v--;
		cap[u][v] = c;
		cap[v][u] = c;
	}

	auto fn = PushRelabel::create(n);
	for (Size i = 0; i < n; i++)
	{
		for (Size j = i + 1; j < n; j++)
		{
			if (cap[i][j] > 0)
			{
				fn->add_edge(i, j, cap[i][j], cap[i][j]);
			}
		}
	}

	Long global_min_cut = INF;
	for (Size t = 1; t < n; t++)
	{
		auto fn_clone = fn->clone();
		Long current_flow = fn_clone->compute_max_flow(0, t);
		global_min_cut = std::min(global_min_cut, current_flow);
	}

	std::cout << global_min_cut << std::endl;
}

int main(void)
{
	std::ios_base::sync_with_stdio(false);
	std::cout.tie(nullptr);
	std::cin.tie(nullptr);

	Long t;
	std::cin >> t;
	while (t--)
	{
		task();
	}
	return 0;
}
