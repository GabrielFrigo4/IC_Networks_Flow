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
	struct Edge
	{
		std::size_t from, to;
		Long cap, flow;
	};
	std::size_t size;
	std::vector<Edge> edges;
	std::vector<std::vector<std::size_t>> adj;

public:
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
	std::size_t n, m;
	std::cin >> n >> m;

	std::vector<std::vector<Long>> cap(n, std::vector<Long>(n, 0));
	for (std::size_t k = 0; k < m; k++)
	{
		std::size_t u, v;
		Long c;
		std::cin >> u >> v >> c;
		u--;
		v--;
		cap[u][v] = c;
		cap[v][u] = c;
	}

	auto fn = Dinic::create(n);
	for (std::size_t i = 0; i < n; i++)
	{
		for (std::size_t j = i + 1; j < n; j++)
		{
			if (cap[i][j] > 0)
			{
				fn->add_edge(i, j, cap[i][j], cap[i][j]);
			}
		}
	}

	Long global_min_cut = INF;
	for (std::size_t t = 1; t < n; t++)
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
