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

class EdmondsKarp : public FlowNetwork
{
private:
	Long bfs(std::size_t s, std::size_t t, std::vector<std::size_t> &parent)
	{
		std::fill(parent.begin(), parent.end(), MAX);
		std::queue<std::pair<std::size_t, Long>> q;
		parent[s] = s;
		q.push({s, INF});

		while (!q.empty())
		{
			auto [cur, cur_f] = q.front();
			q.pop();

			for (std::size_t id : adj[cur])
			{
				std::size_t nxt = edges[id].to;
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
	EdmondsKarp(std::size_t n) : FlowNetwork(n) {}

	static std::unique_ptr<FlowNetwork> create(std::size_t n)
	{
		return std::make_unique<EdmondsKarp>(n);
	}

	std::unique_ptr<FlowNetwork> make(std::size_t n) const override
	{
		return std::make_unique<EdmondsKarp>(n);
	}

	std::unique_ptr<FlowNetwork> clone() const override
	{
		return std::make_unique<EdmondsKarp>(*this);
	}

	Long compute_max_flow(std::size_t s, std::size_t t) override
	{
		Long tot_f = 0, new_f;
		std::vector<std::size_t> parent(size);
		while ((new_f = bfs(s, t, parent)) > 0)
		{
			tot_f += new_f;
			std::size_t cur = t;
			while (cur != s)
			{
				std::size_t id = parent[cur];
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
	std::size_t n, m, k;
	std::cin >> n >> m >> k;

	auto fn = EdmondsKarp::create(n + m + 2);
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
