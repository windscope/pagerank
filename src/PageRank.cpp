#include <iostream>
#include <vector>
#include <limits>
#include <sstream>
#include <algorithm>
#include <string>
#include <cassert>
#include <fstream>
#include <unordered_map>
using namespace std;

using BoolMatrix = vector<vector<bool>>;
using IntMatrix = vector<vector<int>>;

class PageRank{

public:
	PageRank(const string& data_path): data_path(data_path), threshold(0.001) {
		stop_iteration = numeric_limits<int>::max();
		init();
	}

	PageRank(const string& data_path, const int stop_iteration): data_path(data_path), stop_iteration(stop_iteration), threshold(0.001) {
		init();
	}

	PageRank(const string& data_path, const double threshold): data_path(data_path), threshold(threshold) {
		stop_iteration = numeric_limits<int>::max();
		init();
	}
	void run(); // run the iteration
	void print_rank(); // print out the page rank

private:
	void load_data();// load the data into adjacency_matrix
	void init(); // Initialize all the variables
	void make_child_map(); // construct the child map which the parent is pointing to.
	void iterate(); // one iteration
	void print_matrix();

private:
	const string data_path;
	const double dumping_factor = 0.85; // dumping factor
	int iteration_count = 0; // iteration count
	int coverged_node = 0; // number of converged nodes
	int stop_iteration; // If assigned, stop at the stop_iteration.
	double prefix; // (1 - d) / N
	double threshold; // stop point
	int num_nodes; // number of nodes of this page rank. Get from the matrix
	BoolMatrix adjacency_matrix; // the correspond matrix read from the data_path. Eg: test.data
	IntMatrix child_map; // The out link graph
	vector<double> prev_weight; // previous weight
	vector<double> current_weight; // curent weight
	const char sep = ',';

};

void PageRank::init() {
	load_data();
	num_nodes = int(adjacency_matrix.size()); // this should be a square matrix
	make_child_map();
	double N_inverse = 1 / double(num_nodes);
	prefix = (1 - dumping_factor) * N_inverse;
	prev_weight = vector<double>(num_nodes, N_inverse);
	current_weight = vector<double>(num_nodes, 0);
}

void PageRank::load_data() {
	// read the data into adjacency_matrix
	ifstream file(data_path);
	if (file.is_open()) {
		string line, token;
		vector<bool> tmp;
		while(getline(file, line)){
			stringstream line_stream(line);
			while(getline(line_stream, token, sep)){
				tmp.push_back(token == "1");
			}
			adjacency_matrix.push_back(tmp);
			tmp.clear();
		}
		file.close();
	} else {
		cout << "Failed to read file " << data_path << endl;
	}
}

void PageRank::make_child_map() {
	child_map = vector<vector<int>> (num_nodes, {});
	for (int i = 0; i < num_nodes; ++i) {
		auto& curent_vector = adjacency_matrix[i];
		assert(num_nodes == curent_vector.size());
		for (int j = 0; j < curent_vector.size(); ++j) {
			if (i != j) {
				if (curent_vector[j]) {
					child_map[i].push_back(j); // construct the graph
				}
			}
		}
	}
}

void PageRank::iterate() {
	++iteration_count;
	// First iteration calculate the sum of s(vj) / out(vij), where i represent for different out links
	for (int i = 0; i < num_nodes; ++i) {
		// for simplicity, for link that has no child, we manualy set its child num as 1 for code to run
        // The reason we can do that is because it would not have any impact on its kids' score, since it has no children
        int num_childs = max(1, int(child_map[i].size()));
        double parent_avg_weight = double(prev_weight[i]) / double(num_childs);
        for (const auto& node : child_map[i]) {
        	current_weight[node] += parent_avg_weight;
        }
	}
	for (int i = 0; i < num_nodes; ++i) {
        current_weight[i] = prefix + dumping_factor * current_weight[i];
        if (abs(current_weight[i] - prev_weight[i]) < threshold) {
        	++coverged_node;
        }
        prev_weight[i] = 0;
	}
	swap(prev_weight, current_weight);
}

void PageRank::run() {
	while (coverged_node < num_nodes && iteration_count < stop_iteration) {
		cout << "iterate number: " << iteration_count << " converged nodes/all nodes: " << coverged_node << "/" <<num_nodes << endl;
		coverged_node = 0;
		iterate();
	}
}

void PageRank::print_matrix() {
	for (const auto& row : adjacency_matrix) {
		for (const auto col : row) {
			cout << col << ", ";
		}
		cout << endl;
	}
}

void PageRank::print_rank() {
	for (size_t i = 0; i < num_nodes; ++i) {
		cout << "node: " << i << ", rank value: " << prev_weight[i] << endl;
	}
}

int main(int argc, char** argv) {
	if (argc == 2) {
		string data_path(argv[1]);
		auto ranker = PageRank(data_path);
		ranker.run();
		ranker.print_rank();
	}

}