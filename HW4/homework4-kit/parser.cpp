#include "parser.h"
vector<string> read_file(string fname) {
  ifstream ifs(fname);
  vector<string> str_arr;
  if (!ifs.is_open()) {
    cerr << fname << " read fail" << endl;
    exit(EXIT_FAILURE);
  }
  string str;
  while (getline(ifs, str)) {
      str_arr.push_back(str);
  }
  ifs.close();
  return str_arr;
}

vector<player_info> parse_playerText(vector<string>& str_arr) {
    vector<player_info> arr;
    for (int i = 0; i < str_arr.size(); i++) {
        arr.push_back(parse_one_player(str_arr[i]));
    }
    return arr;
}
vector<team_info> parse_teamText(vector<string>& str_arr) {
    vector<team_info> arr;
    for (int i = 0; i < str_arr.size(); i++) {
        arr.push_back(parse_one_team(str_arr[i]));
    }
    return arr;
}
vector<state_info> parse_stateText(vector<string>& str_arr) {
    vector<state_info> arr;
    for (int i = 0; i < str_arr.size(); i++) {
        arr.push_back(parse_one_state(str_arr[i]));
    }
    return arr;
}
vector<color_info> parse_colorText(vector<string>& str_arr){
    vector<color_info> arr;
    for (int i = 0; i < str_arr.size(); i++) {
        arr.push_back(parse_one_color(str_arr[i]));
    }
    return arr;
}

player_info parse_one_player(string str) {
    vector<string> elements = str_split(str);
    player_info info;
    info.team_id = stoi(elements[1]);
    info.jersey_num = stoi(elements[2]);
    info.first_name = elements[3];
    info.last_name = elements[4];
    info.mpg = stoi(elements[5]);
    info.ppg = stoi(elements[6]);
    info.rpg = stoi(elements[7]);
    info.apg = stoi(elements[8]);
    info.spg = stod(elements[9]);
    info.bpg = stod(elements[10]);
    return info;
}
team_info parse_one_team(string str) {
    vector<string> elements = str_split(str);
    team_info info;
    info.name = elements[1];
    info.state_id = stoi(elements[2]);
    info.color_id = stoi(elements[3]);
    info.wins = stoi(elements[4]);
    info.losses = stoi(elements[5]);
    return info;
}
state_info parse_one_state(string str) {
    vector<string> elements = str_split(str);
    state_info info;
    info.name = elements[1];
    return info;
}
color_info parse_one_color(string str) {
    vector<string> elements = str_split(str);
    color_info info;
    info.name = elements[1];
    return info;
}


void write_player(connection *C, vector<player_info>& arr) {
    for (int i = 0; i < arr.size(); i++) {
        add_player(C, arr[i].team_id, arr[i].jersey_num, arr[i].first_name,
        arr[i].last_name, arr[i].mpg, arr[i].ppg, arr[i].rpg, arr[i].apg,
        arr[i].spg, arr[i].bpg);
    }
}
void write_team(connection *C, vector<team_info>& arr) {
    for (int i = 0; i < arr.size(); i++) {
        add_team(C, arr[i].name, arr[i].state_id, arr[i].color_id,
        arr[i].wins, arr[i].losses);
    }
}
void write_state(connection *C, vector<state_info>& arr) {
    for (int i = 0; i < arr.size(); i++) {
        add_state(C, arr[i].name);
    }
}
void write_color(connection *C, vector<color_info>& arr) {
    for (int i = 0; i < arr.size(); i++) {
        add_color(C, arr[i].name);
    }
}



vector<string> str_split(string str) {
    vector<string> words;
    string space_delimiter = " ";
    int pos = 0;
    while ((pos = str.find(space_delimiter)) != string::npos) {
        words.push_back(str.substr(0, pos));
        str.erase(0, pos + space_delimiter.length());
    }
    if (str != "") words.push_back(str);
    return words;
}

void print_strings(vector<string>& str_arr) {
    for (int i = 0; i < str_arr.size(); i++) {
        cout << str_arr[i];
        cout << endl;
    }
}

