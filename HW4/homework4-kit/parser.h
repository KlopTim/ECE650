#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "query_funcs.h"

using namespace std;


class player_info{
public:
    int team_id;
    int jersey_num;
    string first_name;
    string last_name;
    int mpg;
    int ppg;
    int rpg;
    int apg;
    double spg;
    double bpg;
    player_info() {}
};

class team_info {
public:
    string name;
    int state_id;
    int color_id;
    int wins;
    int losses;
    team_info() {}
};

class state_info {
public:
    string name;
    state_info() {}
};

class color_info {
public:
    string name;
    color_info() {}
};


vector<string> read_file(string fname);
vector<player_info> parse_playerText(vector<string>& str_arr);
vector<team_info> parse_teamText(vector<string>& str_arr);
vector<state_info> parse_stateText(vector<string>& str_arr);
vector<color_info> parse_colorText(vector<string>& str_arr);


player_info parse_one_player(string str);
team_info parse_one_team(string str);
state_info parse_one_state(string str);
color_info parse_one_color(string str);


void write_player(connection *C, vector<player_info>& arr);
void write_team(connection *C, vector<team_info>& arr);
void write_state(connection *C, vector<state_info>& arr);
void write_color(connection *C, vector<color_info>& arr);


vector<string> str_split(string str);
void print_strings(vector<string>& str_arr);
