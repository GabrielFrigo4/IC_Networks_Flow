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

class PushRelabel : public FlowNetwork
{
private:
	std::vector<Size> height;
	std::vector<Long> excess;

	void push_preflow(const Size current_node, const Size edge_id)
	{
		const Size next_node = edges[edge_id].to;
		const Long residual_capacity = get_residual_capacity(edge_id);
		const Long flow_to_push = std::min(excess[current_node], residual_capacity);

		if (flow_to_push == 0)
			return;

		push_flow(edge_id, flow_to_push);
		excess[current_node] -= flow_to_push;
		excess[next_node] += flow_to_push;
	}

	void relabel_node(const Size current_node)
	{
		Size min_height = MAX;
		for (const Size edge_id : adj[current_node])
		{
			if (get_residual_capacity(edge_id) <= 0)
			{
				continue;
			}
			min_height = std::min(min_height, height[edges[edge_id].to]);
		}

		if (min_height != MAX)
		{
			height[current_node] = min_height + 1;
		}
	}

	void discharge_node(
	    const Size current_node, std::queue<Size> &active_queue,
	    std::vector<bool> &in_queue, const Size source, const Size sink
	)
	{
		while (excess[current_node] > 0)
		{
			bool has_pushed = false;
			for (const Size edge_id : adj[current_node])
			{
				const Size next_node = edges[edge_id].to;
				const Long residual_capacity = get_residual_capacity(edge_id);

				if (residual_capacity <= 0 ||
				    height[current_node] != height[next_node] + 1)
				{
					continue;
				}

				push_preflow(current_node, edge_id);
				has_pushed = true;

				if (next_node != source && next_node != sink &&
				    !in_queue[next_node] && excess[next_node] > 0)
				{
					active_queue.push(next_node);
					in_queue[next_node] = true;
				}

				if (excess[current_node] == 0)
					break;
			}

			if (!has_pushed)
			{
				relabel_node(current_node);
			}
		}
	}

	void initialize_preflow(
	    const Size source, const Size sink, std::queue<Size> &active_queue,
	    std::vector<bool> &in_queue
	)
	{
		std::fill(height.begin(), height.end(), 0);
		std::fill(excess.begin(), excess.end(), 0);

		height[source] = size;
		excess[source] = INF;

		for (const Size edge_id : adj[source])
		{
			if (get_residual_capacity(edge_id) <= 0)
			{
				continue;
			}

			push_preflow(source, edge_id);
			const Size next_node = edges[edge_id].to;

			if (next_node == source || next_node == sink || in_queue[next_node])
			{
				continue;
			}

			active_queue.push(next_node);
			in_queue[next_node] = true;
		}
	}

public:
	explicit PushRelabel(const Size n) : FlowNetwork(n), height(n), excess(n) {}

	static std::unique_ptr<FlowNetwork> create(const Size n)
	{
		return std::make_unique<PushRelabel>(n);
	}

	std::unique_ptr<FlowNetwork> make(const Size n) const override
	{
		return std::make_unique<PushRelabel>(n);
	}

	std::unique_ptr<FlowNetwork> clone() const override
	{
		return std::make_unique<PushRelabel>(*this);
	}

	Long compute_max_flow(const Size source, const Size sink) override
	{
		std::queue<Size> active_queue;
		std::vector<bool> in_queue(size, false);

		initialize_preflow(source, sink, active_queue, in_queue);

		while (!active_queue.empty())
		{
			const Size current_node = active_queue.front();
			active_queue.pop();
			in_queue[current_node] = false;
			discharge_node(current_node, active_queue, in_queue, source, sink);
		}

		return excess[sink];
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

	const auto fn = PushRelabel::create(total_nodes);

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
