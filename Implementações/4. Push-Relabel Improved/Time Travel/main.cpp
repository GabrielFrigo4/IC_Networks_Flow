#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
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

class PushRelabelImproved : public FlowNetwork
{
private:
	std::vector<Size> height;
	std::vector<Long> excess;
	std::vector<Size> height_count;
	std::vector<Size> next_edge_ptr;
	std::vector<std::vector<Size>> active_buckets;
	Size highest_active;

	void enqueue_active_node(Size node, Size source, Size sink)
	{
		if (node != source && node != sink && excess[node] > 0)
		{
			active_buckets[height[node]].push_back(node);
			highest_active = std::max(highest_active, height[node]);
		}
	}

	void push_preflow(Size current_node, Size edge_id, Size source, Size sink)
	{
		Size next_node = edges[edge_id].to;
		Long residual_capacity = get_residual_capacity(edge_id);

		if (residual_capacity == 0 || height[current_node] <= height[next_node])
			return;

		Long flow_to_push = std::min(excess[current_node], residual_capacity);
		push_flow(edge_id, flow_to_push);
		excess[current_node] -= flow_to_push;
		excess[next_node] += flow_to_push;

		if (excess[next_node] == flow_to_push)
		{
			enqueue_active_node(next_node, source, sink);
		}
	}

	void apply_gap_heuristic(Size gap_height, Size source, Size sink)
	{
		for (Size i = 0; i < size; ++i)
		{
			if (i != source && height[i] > gap_height && height[i] < size)
			{
				height_count[height[i]]--;
				height[i] = std::max(height[i], size + 1);
				height_count[height[i]]++;
				enqueue_active_node(i, source, sink);
			}
		}
	}

	void relabel_node(Size current_node, Size source, Size sink)
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
			Size old_height = height[current_node];
			height_count[old_height]--;

			height[current_node] = min_height + 1;
			height_count[height[current_node]]++;
			enqueue_active_node(current_node, source, sink);

			if (height_count[old_height] == 0 && old_height < size)
			{
				apply_gap_heuristic(old_height, source, sink);
			}
		}
	}

	void discharge_node(Size current_node, Size source, Size sink)
	{
		while (excess[current_node] > 0)
		{
			if (next_edge_ptr[current_node] < adj[current_node].size())
			{
				Size edge_id = adj[current_node][next_edge_ptr[current_node]];
				Size next_node = edges[edge_id].to;
				Long residual_capacity = get_residual_capacity(edge_id);

				if (residual_capacity > 0 &&
				    height[current_node] == height[next_node] + 1)
				{
					push_preflow(current_node, edge_id, source, sink);
				}
				else
				{
					next_edge_ptr[current_node]++;
				}
			}
			else
			{
				relabel_node(current_node, source, sink);
				next_edge_ptr[current_node] = 0;
			}
		}
	}

	void initialize_preflow(Size source, Size sink)
	{
		Size max_height_limit = size * 2 + 1;

		height.assign(size, 0);
		excess.assign(size, 0);
		height_count.assign(max_height_limit, 0);
		next_edge_ptr.assign(size, 0);
		active_buckets.assign(max_height_limit, std::vector<Size>());
		highest_active = 0;

		height[source] = size;
		height_count[size] = 1;
		height_count[0] = size - 1;

		excess[source] = INF;
		for (Size edge_id : adj[source])
		{
			push_preflow(source, edge_id, source, sink);
		}
	}

public:
	explicit PushRelabelImproved(Size n) : FlowNetwork(n) {}

	static std::unique_ptr<FlowNetwork> create(Size n)
	{
		return std::make_unique<PushRelabelImproved>(n);
	}

	std::unique_ptr<FlowNetwork> make(Size n) const override
	{
		return std::make_unique<PushRelabelImproved>(n);
	}

	std::unique_ptr<FlowNetwork> clone() const override
	{
		return std::make_unique<PushRelabelImproved>(*this);
	}

	Long compute_max_flow(Size source, Size sink) override
	{
		initialize_preflow(source, sink);

		while (true)
		{
			if (active_buckets[highest_active].empty())
			{
				if (highest_active == 0)
					break;
				highest_active--;
				continue;
			}

			Size current_node = active_buckets[highest_active].back();
			active_buckets[highest_active].pop_back();

			if (height[current_node] == highest_active)
			{
				discharge_node(current_node, source, sink);
			}
		}

		return excess[sink];
	}
};

void task()
{
	Size num_nodes, num_edges;
	std::cin >> num_nodes >> num_edges;

	std::vector<std::vector<Long>> capacity_matrix(
	    num_nodes, std::vector<Long>(num_nodes, 0)
	);

	for (Size k = 0; k < num_edges; k++)
	{
		Size from_node, to_node;
		Long capacity;
		std::cin >> from_node >> to_node >> capacity;

		from_node--;
		to_node--;

		capacity_matrix[from_node][to_node] = capacity;
		capacity_matrix[to_node][from_node] = capacity;
	}

	auto fn = PushRelabelImproved::create(num_nodes);

	for (Size i = 0; i < num_nodes; i++)
	{
		for (Size j = i + 1; j < num_nodes; j++)
		{
			if (capacity_matrix[i][j] > 0)
			{
				fn->add_edge(
				    i, j, capacity_matrix[i][j], capacity_matrix[i][j]
				);
			}
		}
	}

	Long global_min_cut = INF;
	for (Size current_sink = 1; current_sink < num_nodes; current_sink++)
	{
		auto fn_clone = fn->clone();
		Long current_flow = fn_clone->compute_max_flow(0, current_sink);
		global_min_cut = std::min(global_min_cut, current_flow);
	}

	std::cout << global_min_cut << "\n";
}

int main(void)
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	Long num_test_cases;
	if (std::cin >> num_test_cases)
	{
		while (num_test_cases--)
		{
			task();
		}
	}
	return 0;
}
