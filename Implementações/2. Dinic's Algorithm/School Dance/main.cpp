#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <queue>
#include <vector>

using Long = long long;
constexpr Long INF = static_cast<Long>(1e14);
constexpr std::size_t MAX = std::numeric_limits<std::size_t>::max();

class FlowNetwork
{
protected:
	std::size_t size;
	std::vector<std::vector<std::size_t>> adj;

public:
	struct Edge
	{
		std::size_t from, to;
		Long cap, flow;
	};
	std::vector<Edge> edges;

	FlowNetwork(std::size_t n) : size(n), adj(n) {}
	virtual ~FlowNetwork() = default;

	virtual std::unique_ptr<FlowNetwork> make(std::size_t n) const = 0;
	virtual std::unique_ptr<FlowNetwork> clone() const = 0;

	virtual void add_edge(std::size_t from, std::size_t to, Long cap, Long rev_cap = 0)
	{
		adj[from].push_back(edges.size());
		edges.push_back({from, to, cap, 0});
		adj[to].push_back(edges.size());
		edges.push_back({to, from, rev_cap, 0});
	}

	virtual Long compute_max_flow(std::size_t s, std::size_t t) = 0;
};

class Dinic : public FlowNetwork
{
private:
	std::vector<std::size_t> level;
	std::vector<std::size_t> ptr;

	bool bfs(std::size_t s, std::size_t t)
	{
		std::fill(level.begin(), level.end(), MAX);
		level[s] = 0;
		std::queue<std::size_t> q;
		q.push(s);
		while (!q.empty())
		{
			std::size_t cur = q.front();
			q.pop();
			for (std::size_t id : adj[cur])
			{
				std::size_t nxt = edges[id].to;
				Long res = edges[id].cap - edges[id].flow;
				if (level[nxt] != MAX || res <= 0)
					continue;
				level[nxt] = level[cur] + 1;
				q.push(nxt);
			}
		}
		return level[t] != MAX;
	}

	Long dfs(std::size_t cur, std::size_t t, Long pushed)
	{
		if (pushed == 0 || cur == t)
			return pushed;
		for (std::size_t &cid = ptr[cur]; cid < adj[cur].size(); ++cid)
		{
			std::size_t id = adj[cur][cid];
			std::size_t nxt = edges[id].to;
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
	Dinic(std::size_t n) : FlowNetwork(n), level(n), ptr(n) {}

	static std::unique_ptr<FlowNetwork> create(std::size_t n)
	{
		return std::make_unique<Dinic>(n);
	}

	std::unique_ptr<FlowNetwork> make(std::size_t n) const override
	{
		return std::make_unique<Dinic>(n);
	}

	std::unique_ptr<FlowNetwork> clone() const override
	{
		return std::make_unique<Dinic>(*this);
	}

	Long compute_max_flow(std::size_t s, std::size_t t) override
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
	std::size_t n, m, k;
	std::cin >> n >> m >> k;

	auto fn = Dinic::create(n + m + 2);
	for (std::size_t i = 1; i <= n; i++)
	{
		fn->add_edge(0, i, 1);
	}
	for (std::size_t i = n + 1; i <= n + m; i++)
	{
		fn->add_edge(i, n + m + 1, 1);
	}
	for (std::size_t i = 0; i < k; i++)
	{
		std::size_t u, v;
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
