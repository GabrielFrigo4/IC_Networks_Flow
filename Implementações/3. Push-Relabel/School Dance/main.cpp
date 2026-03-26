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

	[[nodiscard]] Long get_residual_capacity(Size edge_id) const
	{
		return edges[edge_id].capacity - edges[edge_id].flow;
	}

	void push_flow(Size edge_id, Long flow_amount)
	{
		edges[edge_id].flow += flow_amount;
		edges[edge_id ^ 1ULL].flow -= flow_amount;
	}

public:
	explicit FlowNetwork(Size n) : size(n), adj(n) {}
	virtual ~FlowNetwork() = default;

	virtual std::unique_ptr<FlowNetwork> make(Size n) const = 0;
	virtual std::unique_ptr<FlowNetwork> clone() const = 0;

	virtual void add_edge(
	    Size from, Size to, Long capacity, Long reverse_capacity = 0
	)
	{
		adj[from].push_back(edges.size());
		edges.push_back({from, to, capacity, 0});
		adj[to].push_back(edges.size());
		edges.push_back({to, from, reverse_capacity, 0});
	}

	virtual Long compute_max_flow(Size source, Size sink) = 0;

	[[nodiscard]] const std::vector<Edge> &get_edges() const
	{
		return edges;
	}
};

class PushRelabel : public FlowNetwork
{
private:
	std::vector<Size> height;
	std::vector<Long> excess;

	void push_preflow(Size current_node, Size edge_id)
	{
		Size next_node = edges[edge_id].to;
		Long residual_capacity = get_residual_capacity(edge_id);
		Long flow_to_push = std::min(excess[current_node], residual_capacity);

		if (flow_to_push == 0)
			return;

		push_flow(edge_id, flow_to_push);
		excess[current_node] -= flow_to_push;
		excess[next_node] += flow_to_push;
	}

	void relabel_node(Size current_node)
	{
		Size min_height = MAX;
		for (Size edge_id : adj[current_node])
		{
			if (get_residual_capacity(edge_id) > 0)
			{
				min_height = std::min(min_height, height[edges[edge_id].to]);
			}
		}
		if (min_height != MAX)
		{
			height[current_node] = min_height + 1;
		}
	}

	void initialize_preflow(
	    Size source, Size sink, std::queue<Size> &active_queue,
	    std::vector<bool> &in_queue
	)
	{
		std::fill(height.begin(), height.end(), 0);
		std::fill(excess.begin(), excess.end(), 0);

		height[source] = size;
		excess[source] = INF;

		for (Size edge_id : adj[source])
		{
			if (get_residual_capacity(edge_id) > 0)
			{
				push_preflow(source, edge_id);
				Size next_node = edges[edge_id].to;
				if (next_node != source && next_node != sink &&
				    !in_queue[next_node])
				{
					active_queue.push(next_node);
					in_queue[next_node] = true;
				}
			}
		}
	}

public:
	explicit PushRelabel(Size n) : FlowNetwork(n), height(n), excess(n) {}

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

	Long compute_max_flow(Size source, Size sink) override
	{
		std::queue<Size> active_queue;
		std::vector<bool> in_queue(size, false);

		initialize_preflow(source, sink, active_queue, in_queue);

		while (!active_queue.empty())
		{
			Size current_node = active_queue.front();
			active_queue.pop();
			in_queue[current_node] = false;

			while (excess[current_node] > 0)
			{
				bool has_pushed = false;
				for (Size edge_id : adj[current_node])
				{
					Size next_node = edges[edge_id].to;
					Long residual_capacity = get_residual_capacity(edge_id);

					if (residual_capacity > 0 &&
					    height[current_node] == height[next_node] + 1)
					{
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
				}

				if (!has_pushed)
				{
					relabel_node(current_node);
				}
			}
		}

		return excess[sink];
	}
};

void task()
{
	Size num_boys, num_girls, num_potential_pairs;
	if (!(std::cin >> num_boys >> num_girls >> num_potential_pairs))
		return;

	Size total_nodes = num_boys + num_girls + 2;
	Size source = 0;
	Size sink = total_nodes - 1;

	auto fn = PushRelabel::create(total_nodes);

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

	std::cout << fn->compute_max_flow(source, sink) << "\n";

	for (const auto &edge : fn->get_edges())
	{
		if (edge.from >= 1 && edge.from <= num_boys && edge.to > num_boys &&
		    edge.to < sink && edge.flow == 1)
		{
			std::cout << edge.from << " " << edge.to - num_boys << "\n";
		}
	}
}

int main(void)
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	task();
	return 0;
}
