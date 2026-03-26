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

class Dinic : public FlowNetwork
{
private:
	std::vector<Size> level;
	std::vector<Size> ptr;

	bool bfs(Size s, Size t)
	{
		std::fill(level.begin(), level.end(), MAX);
		level[s] = 0;
		std::queue<Size> q;
		q.push(s);
		while (!q.empty())
		{
			Size cur = q.front();
			q.pop();
			for (Size id : adj[cur])
			{
				Size nxt = edges[id].to;
				Long res = edges[id].cap - edges[id].flow;
				if (level[nxt] != MAX || res <= 0)
					continue;
				level[nxt] = level[cur] + 1;
				q.push(nxt);
			}
		}
		return level[t] != MAX;
	}

	Long dfs(Size cur, Size t, Long pushed)
	{
		if (pushed == 0 || cur == t)
			return pushed;
		for (Size &cid = ptr[cur]; cid < adj[cur].size(); ++cid)
		{
			Size id = adj[cur][cid];
			Size nxt = edges[id].to;
			Long res = edges[id].cap - edges[id].flow;
			if (level[cur] + 1 != level[nxt] || res <= 0)
				continue;
			Long tr = dfs(nxt, t, std::min(pushed, res));
			if (tr == 0)
				continue;
			edges[id].flow += tr;
			edges[id ^ 1ULL].flow -= tr;
			return tr;
		}
		return 0;
	}

public:
	Dinic(Size n) : FlowNetwork(n), level(n), ptr(n) {}

	static std::unique_ptr<FlowNetwork> create(Size n)
	{
		return std::make_unique<Dinic>(n);
	}

	std::unique_ptr<FlowNetwork> make(Size n) const override
	{
		return std::make_unique<Dinic>(n);
	}

	std::unique_ptr<FlowNetwork> clone() const override
	{
		return std::make_unique<Dinic>(*this);
	}

	Long compute_max_flow(Size s, Size t) override
	{
		Long tot_f = 0;
		while (bfs(s, t))
		{
			std::fill(ptr.begin(), ptr.end(), 0);
			while (Long pushed = dfs(s, t, INF))
				tot_f += pushed;
		}
		return tot_f;
	}
};

void task()
{
	Size n, m, k;
	std::cin >> n >> m >> k;

	auto fn = Dinic::create(n + m + 2);
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
