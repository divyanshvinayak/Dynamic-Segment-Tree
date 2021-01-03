/*
* Copyright (c) 2020 Divyansh Vinayak
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "../dynamic-segment-tree/dynamic_segment_tree.hpp"
#include <chrono>
#include <iostream>
#include <vector>
#include <tuple>
#include <random>
#include <algorithm>

using namespace std;
using namespace __rb_tree;

const int MAXN = 1e5, MAXM = 10, ITER = 10;

void bench_naive(vector<vector<pair<int, int>>> &insert_data, vector<vector<pair<pair<int, int>, int>>> &range_updates, vector<vector<int>> &delete_positions)
{
	vector<int> vec;
	for (auto &data : insert_data.back())
	{
		vec.insert(vec.begin() + data.first - 1, data.second);
	}
	for (int i = 0; i < MAXN; i++)
	{
		for (auto &update : range_updates[i])
		{
			for (int k = update.first.first - 1; k < update.first.second; k++)
			{
				vec[k] += update.second;
			}
		}
		for (auto &data : insert_data[i])
		{
			vec.insert(vec.begin() + data.first - 1, data.second);
		}
		for (auto &pos : delete_positions[i])
		{
			vec.erase(vec.begin() + pos - 1);
		}
	}
}

void bench_dynamic_segment_tree(vector<vector<pair<int, int>>> &insert_data, vector<vector<pair<pair<int, int>, int>>> &range_updates, vector<vector<int>> &delete_positions)
{
	dynamic_segment_tree<int> tree;
	for (auto &data : insert_data.back())
	{
		tree.insert(data.first, data.second);
	}
	for (int i = 0; i < MAXN; i++)
	{
		for (auto &update : range_updates[i])
		{
			tree.update(update.first.first, update.first.second, update.second);
		}
		for (auto &data : insert_data[i])
		{
			tree.insert(data.first, data.second);
		}
		for (auto &pos : delete_positions[i])
		{
			tree.erase(pos);
		}
	}
}

int main(int argc, char **argv)
{
	cout << "*** Dynamic-Segment-Tree Benchmark ***" << endl;

	cout << "Generating insert data ...";
	vector<vector<pair<int, int>>> insert_data;
	insert_data.emplace_back();
	while (insert_data.back().size() < MAXN)
	{
		insert_data.back().emplace_back(rand() % (insert_data.back().size() + 1) + 1, rand() % 5 + 1);
	}
	while (insert_data.size() <= MAXN)
	{
		insert_data.emplace_back();
		for (int i = 0; i < MAXM - 1; i++)
		{
			insert_data.back().emplace_back(rand() % (insert_data.size() + i) + 1, rand() % 5 + 1);
		}
	}
	reverse(insert_data.begin(), insert_data.end());
	cout << " done" << endl;

	cout << "Generating range updates ...";
	vector<vector<pair<pair<int, int>, int>>> range_updates;
	while (range_updates.size() < MAXN)
	{
		range_updates.emplace_back();
		for (int i = 1; i <= MAXM; i++)
		{
			int p = rand() % range_updates.size() + 1, q = rand() % range_updates.size() + 1, r = rand() % 5 + 1;
			range_updates.back().emplace_back(make_pair(min(p, q), max(p, q)), r);
		}
	}
	reverse(range_updates.begin(), range_updates.end());
	cout << " done" << endl;

	cout << "Generating delete positions ...";
	vector<vector<int>> delete_positions;
	while (delete_positions.size() < MAXN)
	{
		delete_positions.emplace_back();
		for (int i = MAXM - 1; i >= 0; i--)
		{
			delete_positions.back().emplace_back(rand() % (delete_positions.size() + i) + 1);
		}
	}
	reverse(delete_positions.begin(), delete_positions.end());
	cout << " done" << endl;

	vector<tuple<chrono::high_resolution_clock::duration, chrono::high_resolution_clock::duration>> timings;

	cout << "Running ...";
	for (int i = 0; i < ITER; i++)
	{
		auto start_time = chrono::high_resolution_clock::now();
		bench_naive(insert_data, range_updates, delete_positions);
		auto end_time = chrono::high_resolution_clock::now();
		auto time_1 = end_time - start_time;

		start_time = chrono::high_resolution_clock::now();
		bench_dynamic_segment_tree(insert_data, range_updates, delete_positions);
		end_time = chrono::high_resolution_clock::now();
		auto time_2 = end_time - start_time;

		timings.emplace_back(time_1, time_2);
	}
	cout << " done" << endl;

	cout << "Results:" << endl;
	for (int i = 0; i < ITER; i++)
	{
		cout << "  loop #" << i + 1;
		cout << ", naive: " << chrono::duration_cast<chrono::milliseconds>(get<0>(timings[i])).count() << "ms";
		cout << ", dynamic segment tree: " << chrono::duration_cast<chrono::milliseconds>(get<1>(timings[i])).count() << "ms";
		cout << endl;
	}

	return 0;
}