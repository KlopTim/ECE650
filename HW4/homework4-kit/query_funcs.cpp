#include "query_funcs.h"

void drop_table(connection *C) {
    /* Create SQL statement */
    string sql = "DROP TABLE IF EXISTS PLAYER;"\
    "DROP TABLE IF EXISTS TEAM;" \
    "DROP TABLE IF EXISTS STATE;" \
    "DROP TABLE IF EXISTS COLOR;";

    /* Create a transactional object. */
    work W(*C);
    
    /* Execute SQL query */
    W.exec( sql );
    W.commit();
}

void create_player_table(connection *C) {
    /* Create SQL statement */
    string sql = "CREATE TABLE PLAYER("  \
      "PLAYER_ID      SERIAL PRIMARY KEY     NOT NULL," \
      "TEAM_ID        INT                    NOT NULL," \
      "UNIFORM_NUM    INT                    NOT NULL," \
      "FIRST_NAME     TEXT                   NOT NULL," \
	  "LAST_NAME      TEXT                   NOT NULL," \
      "MPG            INT                    NOT NULL,"\
      "PPG            INT                    NOT NULL,"\
      "RPG            INT                    NOT NULL,"\
      "APG            INT                    NOT NULL,"\
      "SPG            REAL                   NOT NULL,"\
      "BPG            REAL                   NOT NULL);";

    /* Create a transactional object. */
    work W(*C);
    
    /* Execute SQL query */
    W.exec( sql );
    W.commit();
}
void create_team_table(connection *C) {
    /* Create SQL statement */
    string sql = "CREATE TABLE TEAM("  \
    "TEAM_ID      SERIAL PRIMARY KEY     NOT NULL," \
    "NAME         TEXT                   NOT NULL," \
    "STATE_ID     INT                    NOT NULL," \
    "COLOR_ID     INT                    NOT NULL," \
    "WINS         INT                    NOT NULL," \
    "LOSSES       INT                    NOT NULL)";

    /* Create a transactional object. */
    work W(*C);
    
    /* Execute SQL query */
    W.exec( sql );
    W.commit();
}
void create_state_table(connection *C) {
    /* Create SQL statement */
    string sql = "CREATE TABLE STATE("  \
    "STATE_ID     SERIAL PRIMARY KEY     NOT NULL," \
    "NAME         TEXT                   NOT NULL)";

    /* Create a transactional object. */
    work W(*C);
    
    /* Execute SQL query */
    W.exec( sql );
    W.commit();
}
void create_color_table(connection *C) {
    /* Create SQL statement */
    string sql = "CREATE TABLE COLOR("  \
    "COLOR_ID     SERIAL PRIMARY KEY     NOT NULL," \
    "NAME         TEXT                   NOT NULL)";

    /* Create a transactional object. */
    work W(*C);
    
    /* Execute SQL query */
    W.exec( sql );
    W.commit();
}


void add_player(connection *C, int team_id, int jersey_num, string first_name, string last_name,
                int mpg, int ppg, int rpg, int apg, double spg, double bpg) {
    /* Create a transactional object. */
    work W(*C);

    /* Create SQL statement */
    string sql;
    stringstream ss;
    ss << "INSERT INTO PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG)";
    ss << "VALUES (" << team_id << ", " << jersey_num << ", " << W.quote(first_name) << ", " << W.quote(last_name) << ", ";
    ss << mpg << ", " << ppg << ", " << rpg << ", " << apg << ", " << spg << ", " << bpg << " );";
    sql = ss.str();

    /* Execute SQL query */
    W.exec( sql );
    W.commit();
}


void add_team(connection *C, string name, int state_id, int color_id, int wins, int losses) {
    /* Create a transactional object. */
    work W(*C);

    /* Create SQL statement */
    string sql;
    stringstream ss;
    ss << "INSERT INTO TEAM ( NAME, STATE_ID, COLOR_ID, WINS, LOSSES)";
    ss << "VALUES (" << W.quote(name) << ", " << state_id << ", " << color_id << ", ";
    ss << wins << ", " << losses << " );";
    sql = ss.str();
    
    /* Execute SQL query */
    W.exec( sql );
    W.commit();
}


void add_state(connection *C, string name) {
    /* Create a transactional object. */
    work W(*C);

    /* Create SQL statement */
    string sql;
    stringstream ss;
    ss << "INSERT INTO STATE (NAME)";
    ss << "VALUES (" << W.quote(name) <<  " );";
    sql = ss.str();
    
    /* Execute SQL query */
    W.exec( sql );
    W.commit();
}


void add_color(connection *C, string name) {
    /* Create a transactional object. */
    work W(*C);

    /* Create SQL statement */
    string sql;
    stringstream ss;
    ss << "INSERT INTO COLOR (NAME)";
    ss << "VALUES (" << W.quote(name) <<  " );";
    sql = ss.str();
    
    /* Execute SQL query */
    W.exec( sql );
    W.commit();
}


void query1(connection *C,
	    int use_mpg, int min_mpg, int max_mpg,
        int use_ppg, int min_ppg, int max_ppg,
        int use_rpg, int min_rpg, int max_rpg,
        int use_apg, int min_apg, int max_apg,
        int use_spg, double min_spg, double max_spg,
        int use_bpg, double min_bpg, double max_bpg
            ) {
    /* Create SQL statement */
    string sql = "SELECT * from PLAYER WHERE ";
    bool first = true;
    if (use_mpg != 0) {
        if (first) first = false;
        else sql += " AND ";
        sql +=  "MPG >= " + to_string(min_mpg) + " AND MPG <= " + to_string(max_mpg) + " ";
    }
    if (use_ppg != 0) {
        if (first) first = false;
        else sql += " AND ";
        sql += "PPG >= " + to_string(min_ppg) + " AND PPG <= " + to_string(max_ppg) + " ";
    }
    if (use_rpg != 0) {
        if (first) first = false;
        else sql += " AND ";
        sql +=  "RPG >= " + to_string(min_rpg) + " AND RPG <= " + to_string(max_rpg) + " ";
    }
    if (use_apg != 0) {
        if (first) first = false;
        else sql += " AND ";
        sql +=  "APG >= " + to_string(min_apg) + " AND APG <= " + to_string(max_apg) + " ";
    }
    if (use_spg != 0) {
        if (first) first = false;
        else sql += " AND ";
        sql +=  "SPG >= " + to_string(min_spg) + " AND SPG <= " + to_string(max_spg) + " ";
    }
    if (use_bpg != 0) {
        if (first) first = false;
        else sql += " AND ";
        sql +=  "BPG >= " + to_string(min_bpg) + " AND BPG <= " + to_string(max_bpg) + " ";
    }
    sql +=  ";";

    /* Create a non-transactional object. */
    nontransaction N(*C);
      
    /* Execute SQL query */
    result R( N.exec( sql ));
      
    cout << "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG"<<endl;
    /* List down all the records */
    for (result::const_iterator c = R.begin(); c != R.end(); c++) {
      cout << c[0].as<int>();
      cout << " " << c[1].as<int>();
      cout << " " << c[2].as<int>();
      cout << " " << c[3].as<string>();
      cout << " " << c[4].as<string>();
      cout << " " << c[5].as<int>();
      cout << " " << c[6].as<int>();
      cout << " " << c[7].as<int>();
      cout << " " << c[8].as<int>();
      cout << " " << c[9].as<double>();
      cout << " " << c[10].as<double>();
      cout << endl;
    }
}


void query2(connection *C, string team_color) {
    work W(*C);
    /* Create SQL statement */
    string sql = "SELECT TEAM.NAME from TEAM, COLOR ";
    sql += "WHERE COLOR.NAME = " + W.quote(team_color) + " AND TEAM.COLOR_ID = COLOR.COLOR_ID;";
    W.commit();

    /* Create a non-transactional object. */
    nontransaction N(*C);
      
    /* Execute SQL query */
    result R( N.exec( sql ));
      
    cout << "NAME" << endl;
    /* List down all the records */
    for (result::const_iterator c = R.begin(); c != R.end(); c++) {
      cout << c[0].as<string>();
      cout << endl;
    }
}


void query3(connection *C, string team_name) {
    work W(*C);
    /* Create SQL statement */
    string sql = "SELECT FIRST_NAME, LAST_NAME from PLAYER, TEAM "; 
    sql += "WHERE TEAM.NAME = " + W.quote(team_name) + " AND PLAYER.TEAM_ID = TEAM.TEAM_ID"; 
    sql += " ORDER BY PLAYER.PPG DESC;";
    W.commit();

    /* Create a non-transactional object. */
    nontransaction N(*C);
      
    /* Execute SQL query */
    result R( N.exec( sql ));
      
    cout << "FIRST_NAME LAST_NAME" << endl;
    /* List down all the records */
    for (result::const_iterator c = R.begin(); c != R.end(); c++) {
      cout << c[0].as<string>() << " ";
      cout << c[1].as<string>();
      cout << endl;
    }
}


void query4(connection *C, string team_state, string team_color) {
    work W(*C);
    /* Create SQL statement */
    string sql = "SELECT FIRST_NAME, LAST_NAME, UNIFORM_NUM FROM PLAYER, TEAM, STATE, COLOR "; 
    sql += "WHERE STATE.NAME = " + W.quote(team_state) + " AND COLOR.NAME = " + W.quote(team_color) + " ";
    sql += " AND PLAYER.TEAM_ID = TEAM.TEAM_ID ";
    sql += " AND TEAM.STATE_ID = STATE.STATE_ID "; 
    sql += " AND TEAM.COLOR_ID = COLOR.COLOR_ID;"; 
    W.commit();

    /* Create a non-transactional object. */
    nontransaction N(*C);
      
    /* Execute SQL query */
    result R( N.exec( sql ));
      
    cout<<"FIRST_NAME LAST_NAME UNIFORM_NUM"<<endl;
    /* List down all the records */
    for (result::const_iterator c = R.begin(); c != R.end(); c++) {
      cout << c[0].as<string>() << " ";
      cout << c[1].as<string>() << " ";
      cout << c[2].as<int>();
      cout << endl;
    }
}


void query5(connection *C, int num_wins) {
    work W(*C);
    /* Create SQL statement */
    string sql = "SELECT FIRST_NAME, LAST_NAME, TEAM.NAME, WINS FROM PLAYER, TEAM "; 
    sql += " WHERE PLAYER.TEAM_ID = TEAM.TEAM_ID ";
    sql += " AND WINS > " + W.quote(num_wins);
    sql += " ;"; 
    W.commit();

    /* Create a non-transactional object. */
    nontransaction N(*C);
      
    /* Execute SQL query */
    result R( N.exec( sql ));
      
    cout<<"FIRST_NAME LAST_NAME TEAM_NAME WIN"<<endl;
    /* List down all the records */
    for (result::const_iterator c = R.begin(); c != R.end(); c++) {
      cout << c[0].as<string>() << " ";
      cout << c[1].as<string>() << " ";
      cout << c[2].as<string>() << " ";
      cout << c[3].as<int>();
      cout << endl;
    }
}
