#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <queue>
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

class EdmondsKarp : public FlowNetwork
{
private:
	Long bfs(Size s, Size t, std::vector<Size> &parent)
	{
		std::fill(parent.begin(), parent.end(), MAX);
		std::queue<std::pair<Size, Long>> q;
		parent[s] = s;
		q.push({s, INF});

		while (!q.empty())
		{
			auto [cur, cur_f] = q.front();
			q.pop();

			for (Size id : adj[cur])
			{
				Size nxt = edges[id].to;
				Long res = edges[id].cap - edges[id].flow;
				if (parent[nxt] != MAX || res <= 0)
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
	EdmondsKarp(Size n) : FlowNetwork(n) {}

	static std::unique_ptr<FlowNetwork> create(Size n)
	{
		return std::make_unique<EdmondsKarp>(n);
	}

	std::unique_ptr<FlowNetwork> make(Size n) const override
	{
		return std::make_unique<EdmondsKarp>(n);
	}

	std::unique_ptr<FlowNetwork> clone() const override
	{
		return std::make_unique<EdmondsKarp>(*this);
	}

	Long compute_max_flow(Size s, Size t) override
	{
		Long tot_f = 0, new_f;
		std::vector<Size> parent(size);
		while ((new_f = bfs(s, t, parent)) > 0)
		{
			tot_f += new_f;
			Size cur = t;
			while (cur != s)
			{
				Size id = parent[cur];
				edges[id].flow += new_f;
				edges[id ^ 1ULL].flow -= new_f;
				cur = edges[id].from;
			}
		}
		return tot_f;
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

	auto fn = EdmondsKarp::create(n);
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
