#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <queue>
#include <vector>

using Long = long long;
using Size = std::size_t;

constexpr Long INF = std::numeric_limits<Long>::max() >> 8;
constexpr Size MAX = std::numeric_limits<Size>::max() >> 8;

class FlowNetwork
{
protected:
	struct Edge
	{
		Size from, to;
		Long capacity, flow;
	};
	Size size;
	std::vector<Edge> edges;
	std::vector<std::vector<Size>> adj;

	[[nodiscard]] Long get_residual_capacity(const Size edge_id) const
	{
		return edges[edge_id].capacity - edges[edge_id].flow;
	}

	void push_flow(const Size edge_id, const Long flow_amount)
	{
		edges[edge_id].flow += flow_amount;
		edges[edge_id ^ 1ULL].flow -= flow_amount;
	}

public:
	explicit FlowNetwork(const Size n) : size(n), adj(n) {}
	virtual ~FlowNetwork() = default;

	virtual std::unique_ptr<FlowNetwork> make(const Size n) const = 0;
	virtual std::unique_ptr<FlowNetwork> clone() const = 0;

	virtual void add_edge(
	    const Size from, const Size to, const Long capacity,
	    const Long reverse_capacity = 0
	)
	{
		adj[from].push_back(edges.size());
		edges.push_back({from, to, capacity, 0});
		adj[to].push_back(edges.size());
		edges.push_back({to, from, reverse_capacity, 0});
	}

	virtual Long compute_max_flow(const Size source, const Size sink) = 0;

	[[nodiscard]] Size get_size() const
	{
		return size;
	}

	[[nodiscard]] const std::vector<Edge> &get_edges() const
	{
		return edges;
	}

	[[nodiscard]] const std::vector<std::vector<Size>> &get_adj() const
	{
		return adj;
	}
};

class EdmondsKarp : public FlowNetwork
{
private:
	Long bfs(const Size source, const Size sink, std::vector<Size> &parent_edge)
	{
		std::fill(parent_edge.begin(), parent_edge.end(), MAX);
		std::queue<std::pair<Size, Long>> queue;

		parent_edge[source] = source;
		queue.push({source, INF});

		while (!queue.empty())
		{
			const auto [current_node, current_flow] = queue.front();
			queue.pop();

			for (const Size edge_id : adj[current_node])
			{
				const Size next_node = edges[edge_id].to;
				const Long residual_capacity = get_residual_capacity(edge_id);

				if (parent_edge[next_node] != MAX || residual_capacity <= 0)
					continue;

				parent_edge[next_node] = edge_id;
				const Long pushed_flow = std::min(current_flow, residual_capacity);

				if (next_node == sink)
					return pushed_flow;

				queue.push({next_node, pushed_flow});
			}
		}
		return 0;
	}

	void update_path_flow(
	    const Size source, const Size sink, const std::vector<Size> &parent_edge,
	    const Long new_flow
	)
	{
		Size current_node = sink;
		while (current_node != source)
		{
			const Size edge_id = parent_edge[current_node];
			push_flow(edge_id, new_flow);
			current_node = edges[edge_id].from;
		}
	}

public:
	explicit EdmondsKarp(const Size n) : FlowNetwork(n) {}

	static std::unique_ptr<FlowNetwork> create(const Size n)
	{
		return std::make_unique<EdmondsKarp>(n);
	}

	std::unique_ptr<FlowNetwork> make(const Size n) const override
	{
		return std::make_unique<EdmondsKarp>(n);
	}

	std::unique_ptr<FlowNetwork> clone() const override
	{
		return std::make_unique<EdmondsKarp>(*this);
	}

	Long compute_max_flow(const Size source, const Size sink) override
	{
		Long total_flow = 0;
		Long new_flow = 0;
		std::vector<Size> parent_edge(size);

		while ((new_flow = bfs(source, sink, parent_edge)) > 0)
		{
			total_flow += new_flow;
			update_path_flow(source, sink, parent_edge, new_flow);
		}

		return total_flow;
	}
};

void print_matches(
    const std::unique_ptr<FlowNetwork> &fn, const Size num_boys, const Size num_girls
)
{
	for (const auto &edge : fn->get_edges())
	{
		const Size num_total = num_boys + num_girls;
		const bool from_boy = (edge.from >= 1 && edge.from <= num_boys);
		const bool to_girl = (edge.to > num_boys && edge.to <= num_total);

		if (!from_boy || !to_girl || edge.flow != 1)
		{
			continue;
		}

		std::cout << edge.from << " " << edge.to - num_boys << std::endl;
	}
}

void task()
{
	Size num_boys, num_girls, num_potential_pairs;
	if (!(std::cin >> num_boys >> num_girls >> num_potential_pairs))
		return;

	const Size total_nodes = num_boys + num_girls + 2;
	const Size source = 0;
	const Size sink = total_nodes - 1;

	const auto fn = EdmondsKarp::create(total_nodes);

	for (Size i = 1; i <= num_boys; i++)
	{
		fn->add_edge(source, i, 1);
	}

	for (Size i = 1; i <= num_girls; i++)
	{
		fn->add_edge(num_boys + i, sink, 1);
	}

	for (Size i = 0; i < num_potential_pairs; i++)
	{
		Size boy, girl;
		std::cin >> boy >> girl;
		fn->add_edge(boy, num_boys + girl, 1);
	}

	std::cout << fn->compute_max_flow(source, sink) << std::endl;
	print_matches(fn, num_boys, num_girls);
}

int main(void)
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	task();
	return 0;
}
