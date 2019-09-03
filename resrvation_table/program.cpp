#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <numeric>

using namespace std;

void drawline()
{
    cout << endl
         << endl;
    for (int i = 0; i < 30; i++)
        cout << "-";
    cout << endl
         << endl;
}

string operator|(string &a, string &b)
{
    string ans = "";
    for (int i = 0; i < a.length(); i++)
    {
        ans += ((a[i] - '0') | (b[i] - '0')) + '0';
    }
    return ans;
}

ostream &operator<<(ostream &out, vector<int> &v)
{
    out << "(";
    for (int i = 0; i < v.size() - 1; i++)
        out << v[i] << ", ";
    out << v[v.size() - 1] << ")";
    return out;
}

string get_new_state(string state, string collison_vector, int r)
{
    for (int i = state.length() - r - 1; i >= 0; i--)
        state[i + r] = state[i];
    for (int i = 0; i < r; i++)
        state[i] = '0';
    return state | collison_vector;
}

void print_simple_path_util(string start_state, string curr_state, vector<int> &path,
                            unordered_map<string, vector<pair<string, int>>> &next_state_map, unordered_set<string> &visited)
{
    if (curr_state == start_state)
    {
        cout << path << endl;
        return;
    }
    for (auto it = next_state_map[curr_state].begin(); it != next_state_map[curr_state].end(); it++)
    {
        if (visited.find(it->first) == visited.end())
        {
            path.push_back(it->second);
            visited.insert(it->first);
            print_simple_path_util(start_state, it->first, path, next_state_map, visited);
            visited.erase(it->first);
            path.pop_back();
        }
    }
}

void print_simple_path(string start_state,
                       unordered_map<string, vector<pair<string, int>>> &next_state_map)
{
    unordered_set<string> visited;
    vector<int> path;
    for (auto it = next_state_map[start_state].begin(); it != next_state_map[start_state].end(); it++)
    {
        path.push_back(it->second);
        visited.insert(it->first);
        print_simple_path_util(start_state, it->first, path, next_state_map, visited);
        visited.erase(it->first);
        path.pop_back();
    }
}

float get_greedy_path_util(string start_state, string curr_state, vector<int> &path,
                           unordered_map<string, vector<pair<string, int>>> &next_state_map, unordered_set<string> &visited)
{
    if (curr_state == start_state)
    {
        cout << path << endl;
        return accumulate(path.begin(), path.end(), 0.0) / (1.0 * path.size());
    }
    auto it = next_state_map[curr_state].begin();
    if (visited.find(it->first) != visited.end())
        return INT16_MAX;
    path.push_back(it->second);
    visited.insert(it->first);
    float mal = get_greedy_path_util(start_state, it->first, path, next_state_map, visited);
    visited.erase(it->first);
    path.pop_back();
    return mal;
}

float get_greedy_path(string start_state,
                      unordered_map<string, vector<pair<string, int>>> &next_state_map)
{
    float mal = start_state.length() + 1;
    unordered_set<string> visited;
    vector<int> path;
    auto it = next_state_map[start_state].begin();
    path.push_back(it->second);
    visited.insert(it->first);
    mal = min(mal, get_greedy_path_util(start_state, it->first, path, next_state_map, visited));
    visited.erase(it->first);
    path.pop_back();
    return mal;
}

int main(int argc, char *argv[])
{
    // string a = "1010";
    // string b = "1010";
    // cout << get_new_state(a, b, 1) << endl;
    // if(argc < 2)
    // {
    //     exit(-1);
    // }
    // string filename(argv[1]);
    string filename = "table.csv";
    fstream reader;
    reader.open(filename, ios::in | ios::out);
    string row;
    int rownum = 0;
    int execution_time;
    vector<vector<string>> table;
    while (getline(reader, row))
    {
        int n = row.length();
        execution_time = (n - 1) / 2;
        table.push_back(vector<string>());
        string symbol;
        stringstream iss(row);
        while (getline(iss, symbol, ','))
            table[rownum].push_back(symbol);
        rownum++;
    }

    vector<int> forb_latencies, perm_latencies;
    for (vector<string> &row : table)
        for (int i = 0; i < row.size(); i++)
            if (row[i] == "x")
                for (int j = i + 1; j < row.size(); j++)
                    if (row[j] == "x")
                        forb_latencies.push_back(j - i);

    sort(forb_latencies.begin(), forb_latencies.end());
    forb_latencies.resize(distance(forb_latencies.begin(), unique(forb_latencies.begin(), forb_latencies.end())));

    drawline();
    cout << "forbidden latencies : ";
    for (int i = 0; i < forb_latencies.size(); i++)
        cout << forb_latencies[i] << " ";
    // cout << endl << endl;

    drawline();
    cout << "Permissible Latencies : ";
    for (int i = 1; i <= execution_time; i++)
    {
        if (find(forb_latencies.begin(), forb_latencies.end(), i) == forb_latencies.end())
            perm_latencies.push_back(i), cout << i << " ";
    }
    // cout << endl << endl;

    string collision_vector(*max_element(forb_latencies.begin(), forb_latencies.end()), '0');
    for (int i = 0; i < forb_latencies.size(); i++)
    {
        collision_vector[collision_vector.length() - forb_latencies[i]] = '1';
    }

    drawline();
    cout << "Collision Vector : " << collision_vector;

    // STATE TRANSITION DIAGRAM
    unordered_map<string, string> state_name_map;
    unordered_map<string, vector<pair<string, int>>> next_state_map;
    queue<string> states_not_done;
    int num_states = 1;
    states_not_done.push(collision_vector);
    state_name_map[collision_vector] = "N1";

    while (!states_not_done.empty())
    {
        string state = states_not_done.front();
        states_not_done.pop();
        for (int i = 1; i <= state.length(); i++)
        {
            if (state[state.length() - i] == '0')
            {
                string new_state = get_new_state(state, collision_vector, i);
                if (state_name_map.find(new_state) == state_name_map.end())
                {
                    state_name_map[new_state] = "N" + to_string(++num_states);
                    states_not_done.push(new_state);
                }
                next_state_map[state].push_back({new_state, i});
            }
        }
    }

    for (auto it = state_name_map.begin(); it != state_name_map.end(); it++)
        if (next_state_map.find(it->first) == next_state_map.end())
            next_state_map[it->first] = vector<pair<string, int>>();

    for (auto it = state_name_map.begin(); it != state_name_map.end(); it++)
        next_state_map[it->first].push_back({collision_vector, collision_vector.length() + 1});

    drawline();
    cout << "number of states : " << num_states << endl;
    for (auto it = state_name_map.begin(); it != state_name_map.end(); it++)
        cout << it->first << " : " << it->second << endl;
    cout << endl;

    // for_each(next_state_map.begin(), next_state_map.end(), [](auto it) -> void
    // {
    //     sort((it->second).begin(), (it->second).end(), [](const pair<string, int>& p1, const pair<string, int>& p2) -> bool{
    //         return p1.second > p2.second;
    //     });
    // });

    for (auto it = next_state_map.begin(); it != next_state_map.end(); it++)
    {
        cout << state_name_map[it->first] << " -> ";
        for (pair<string, int> p : it->second)
            cout << "(" << state_name_map[p.first] << " , " << p.second << (p.second == collision_vector.length() + 1 ? "+" : "") << ") , ";
        cout << endl;
    }

    // SIMPLE CYCLES
    // for_each(state_name_map.begin(), state_name_map.end(), [](auto it){
    //     print_simple_path(it->first, next_state_map);
    // });

    drawline();
    cout << "Simple Cycles" << endl;
    for (auto it = state_name_map.begin(); it != state_name_map.end(); it++)
        print_simple_path(it->first, next_state_map);

    drawline();
    cout << "Greedy Cycles" << endl;
    float mal = collision_vector.length() + 1;
    for (auto it = state_name_map.begin(); it != state_name_map.end(); it++)
        mal = min(mal, get_greedy_path(it->first, next_state_map));

    drawline();
    cout << "Minimum Average Latency : " << mal << endl;

    // for(vector<string> row : table)
    // {
    //     for(string sym : row)
    //     {
    //         cout << sym << " ";
    //     }
    //     cout << endl;
    // }
    return 0;
}