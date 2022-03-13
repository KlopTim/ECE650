#include <iostream>
#include <fstream>
#include <pqxx/pqxx>

#include "exerciser.h"
#include "parser.h"

using namespace std;
using namespace pqxx;

int main (int argc, char *argv[]) 
{

  //Allocate & initialize a Postgres connection object
  connection *C;

  try{
    //Establish a connection to the database
    //Parameters: database name, user name, user password
    C = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
    if (C->is_open()) {
      cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return 1;
    }
  } catch (const std::exception &e){
    cerr << e.what() << std::endl;
    return 1;
  }


  // drop all table before creating
  drop_table(C);
  //create PLAYER, TEAM, STATE, and COLOR tables in the ACC_BBALL database
  create_player_table(C);
  create_state_table(C);
  create_color_table(C);
  create_team_table(C);
  //load each table with rows from the provided source txt files
  vector<string> player_strings = read_file("player.txt");
  vector<string> test = str_split(player_strings[4]);
  vector<string> team_strings = read_file("team.txt");
  vector<string> state_strings = read_file("state.txt");
  vector<string> color_strings = read_file("color.txt");
  // parsing
  vector<player_info> player_info_list = parse_playerText(player_strings);
  vector<team_info> team_info_list = parse_teamText(team_strings);
  vector<state_info> state_info_list = parse_stateText(state_strings);
  vector<color_info> color_info_list = parse_colorText(color_strings);
  // write into database
  write_player(C, player_info_list);
  write_team(C, team_info_list);
  write_state(C, state_info_list);
  write_color(C, color_info_list);


  exercise(C);

  //Close database connection
  C->disconnect();

  return 0;
}



