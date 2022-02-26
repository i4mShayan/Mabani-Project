#include <iostream>
#include "clui.h"
#include <string>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <sstream>
using namespace std;

int ground[20][20]; // -1=mine, -2=not selected, -3=exploded mine, -4=flagged, -5=flagged a mine
int ground_row, ground_col, mines;
int flags, selected_cells;
string name;
int points=0;
int players_count;
string *players; //array of players
int *players_points; //array of players' points
int *saved_games_count; //array of players' saved games count
int player_id, player_saved_games, playing_game;
bool file_loaded_before=0;

void menu();
void make_new_game(int, int, int);
void select(int, int);
void load_game();

void load_files(){
    if(file_loaded_before){
        delete[] players;
        delete[] players_points;
        delete[] saved_games_count;
    }
    bool error=0;
    fstream players_file;
    players_file.open(".\\data\\players.txt", ios::in);
    if(players_file.is_open()){
        players_count=0;
        string player_name;
        while(getline(players_file, player_name)){
            players_count++;
        }
        players_file.close();
        players_file.open(".\\data\\players.txt", ios::in);
        players=new string[players_count];
        int i=0;
        while(getline(players_file, player_name)){
            players[i]=player_name;
            i++;
        }
        players_file.close();
        players_points=new int[players_count];
        fstream players_points_file;
        players_points_file.open(".\\data\\players_points.txt", ios::in);
        if(players_points_file.is_open()){
            int i=0;
            string player_point;
            while(getline(players_points_file, player_point)){
                players_points[i]=atoi(player_point.c_str());
                i++;
            }
            players_points_file.close();
            file_loaded_before=1;
        }
        else{
            error=1;
        }
    }
    else{
        error=1;
    }
    saved_games_count=new int[players_count];
    fstream games_count_file;
    games_count_file.open(".\\data\\saved_games_count.txt", ios::in);
    if(games_count_file.is_open()){
        int i=0;
        string saved_count;
        while(getline(games_count_file, saved_count)){
            saved_games_count[i]=atoi(saved_count.c_str());
            i++;
        }
        games_count_file.close();
    }
    else{
        error=1;
    }
    if(error){
        clear_screen();
        cout << "error while opening files!";
        exit(0);
    }
}


void add_player(string player_name, int player_points){
    bool error=0;
    fstream players_file;
    players_file.open(".\\data\\players.txt", ios::app);
    if(players_file.is_open()){
        players_file << player_name << '\n' ;
        players_file.close();
        fstream players_points_file;
        players_points_file.open(".\\data\\players_points.txt", ios::app);
        if(players_points_file.is_open()){
            players_points_file << player_points << '\n';
            players_points_file.close();
        }
        else{
            error=1;
        }
    }
    else{
        error=1;
    }
    fstream games_count_file;
    games_count_file.open(".\\data\\saved_games_count.txt", ios::app);
    if(games_count_file.is_open()){
        games_count_file << 0 << '\n' ;
        games_count_file.close();
    }
    else{
        error=1;
    }
    if(error){
        clear_screen();
        cout << "error while opening files!";
        exit(0);
    }
    points=player_points;
    player_id=players_count;
    players_count++;
    player_saved_games=0;
    load_files();
}

void update_player_points(int new_points, int id){
    fstream file;
    file.open(".\\data\\players_points.txt", ios::out);
    if(file.is_open()){
        for(int i=0; i<id; i++){
            file << players_points[i] << '\n';
        }
        file << new_points << '\n';
        for(int i=id+1; i<players_count; i++){
            file << players_points[i] << '\n';
        }
        file.close();
    }
    else{
        clear_screen();
        cout << "error while opening files!";
        exit(0);
    }
    load_files();
}

void update_player_name(string new_name, int id){
    fstream file;
    file.open(".\\data\\players.txt", ios::out);
    if(file.is_open()){
        for(int i=0; i<id; i++){
            file << players[i] << '\n';
        }
        file << new_name << '\n';
        for(int i=id+1; i<players_count; i++){
            file << players[i] << '\n';
        }
        file.close();
    }
    else{
        clear_screen();
        cout << "error while opening files!";
        exit(0);
    }
    load_files();
}

void merge_users(int deleting_player_id, int destination_player_id){
    points=players_points[destination_player_id]+players_points[deleting_player_id];
    name=players[destination_player_id];
    update_player_points(points, destination_player_id);
    fstream file;
    file.open(".\\data\\players.txt", ios::out);
    if(file.is_open()){
        for(int i=0; i<deleting_player_id; i++){
            file << players[i] << '\n';
        }
        for(int i=deleting_player_id+1; i<players_count; i++){
            file << players[i] << '\n';
        }
        file.close();
    }
    else{
        clear_screen();
        cout << "error while opening files!";
        exit(0);
    }
    file.open(".\\data\\players_points.txt", ios::out);
    if(file.is_open()){
        for(int i=0; i<deleting_player_id; i++){
            file << players_points[i] << '\n';
        }
        for(int i=deleting_player_id+1; i<players_count; i++){
            file << players_points[i] << '\n';
        }
        file.close();
    }
    else{
        clear_screen();
        cout << "error while opening files!";
        exit(0);
    }
    player_id=destination_player_id;
    player_saved_games=saved_games_count[player_id];
    if(destination_player_id > deleting_player_id) player_id--;
    players_count--;
    load_files();
}

void change_name(){
    clear_screen();
    cout << "Your Current User is ";
    change_color(13);
    cout << name << endl;
    reset_color();
    cout << "Please Enter The User You Want To Switch To (enter nothing to go back): ";
    string x;
    getline(cin, x);
    if(x[0]!='\0'){
        name = x;
        bool old_player=0;
        for(int i=0; i<players_count; i++){
            if(name==players[i]){
                name = players[i];
                points = players_points[i];
                player_saved_games=saved_games_count[i];
                player_id=i;
                old_player=1;
                break;
            }
        }
        if(!old_player){
            add_player(name, 0);
        }
    }
    menu();
}

int main(){
    load_files();
    init_clui();
    cout << "Hi There! Please Enter Your Name: ";
    getline(cin, name);
    bool old_player=0;
    for(int i=0; i<players_count; i++){
        if(name==players[i]){
            points=players_points[i];
            player_saved_games=saved_games_count[i];
            player_id=i;
            old_player=1;
            break;
        }
    }
    if(!old_player){
        player_id=players_count;
        points=0;
        player_saved_games=0;
        add_player(name, points);
    }
    menu();
    quit();
    return 0;
}

void print_a_line(){
    int col=get_window_cols();
    for(int i=0; i<col; i++){
        cout << char(196);
    }
}

void menu_footage(){
    int row=get_window_rows(), col=get_window_cols();
    save_cursor();
    cursor_to_pos(row-2, 1);
    print_a_line();
    cursor_to_pos(row-1, col/2-41);
    cout << "use WASD keys to move" << "    "
    << char(179) << "    " << "press SPACE to select an item" << "    "
    << char(179) << "    " << "press ESC to exit";
    restore_cursor();
}

void game_footage(){
    save_cursor();
    int row=get_window_rows(), col=get_window_cols();
    cursor_to_pos(row-6, 1);
    print_a_line();
    cursor_to_pos(row-5, col/2-42);
    cout << char(254) << " : not selected cell"
    << "     " << char(179) << "     " << char(251) << " : flag"
    << "     " << char(179) << "     " << "* : mine" << "     " << char(179) << "     " <<  "X : exploded mine" << endl;
    print_a_line();
    cursor_to_pos(row-3, col/2-42);
    cout << "use WASD keys to move" << "    "
    << char(179) << "    " << "SPACE : select a cell!" << "    "
    << char(179) << "    " << "ESC : exit without saving" << endl;
    print_a_line();
    cursor_to_pos(row-1, col/2-43);
    cout << "F : flag/unflag" << "   "
    << char(179) << "   " << "O : save and go to the menu" << "   "
    << char(179) << "   " << "Q : go to the menu without saving";
    restore_cursor();
}

void header(){
    save_cursor();
    cursor_to_pos(1,1);
    int row=get_window_rows(), col=get_window_cols();
    cout << " Points: ";
    change_color(13);
    cout << points;
    reset_color();
    cursor_to_pos(1,col/2-8);
    cout << "Minesweeper game!";
    cursor_to_pos(1,col-11-name.size());
    cout << "Playing as ";
    change_color(13);
    cout << name << endl;
    reset_color();
    cout << " Notification: " << endl;
    print_a_line();
    restore_cursor();
}


void clear_notify(){
    save_cursor();
    cursor_to_pos(2, 16);
    for(int i=0; i<get_window_cols()-16; i++){
        cout << ' ';
    }
    restore_cursor();
    flush();
}

void notify(string msg){
    clear_notify();
    save_cursor();
    cursor_to_pos(2, 16);
    change_color(11);
    cout << msg;
    reset_color();
    restore_cursor();
    play_beep();
    flush();
}

void put_mines(){
    for(int i=0; i<ground_row; i++){
        for(int j=0; j<ground_col; j++){
            ground[i][j]=-2;
        }
    }
    srand(time(NULL));
    int **mines_position = new int*[mines];
    for(int i=0; i<mines; i++){
        mines_position[i] = new int [2];
    }
    for(int i=0; i<mines; i++){
        int r = rand()%ground_row;
        int c = rand()%ground_col;
        bool repeated=0;
        for(int j=0; j<i; j++){
            if(r == mines_position[j][0] && c == mines_position[j][1]){
                repeated=1;
                i--;
                break;
            }
        }
        if(!repeated){
            mines_position[i][0]=r;
            mines_position[i][1]=c;
            ground[r][c]=-1;
        }
    }

    for(int i=0; i<mines; i++){
        delete[] mines_position[i];
    }
    delete[] mines_position;
}

void show_ground(bool show_bombs){
    save_cursor();
    clear_screen();
    header();
    game_footage();
    for(int i=0; i<ground_row; i++){
        cursor_to_pos(4+i,2);
        for(int j=0; j<ground_col; j++){
             // -1=mine, -2=not selected, -3=exploded mine, -4=flagged, -5=flagged a mine
            if(show_bombs && ground[i][j]==-1) cout << "* ";
            else if(show_bombs && ground[i][j]==-5){
                cout << "* ";
            }
            else if(ground[i][j]==-1){
                cout << char(254) << " ";
            }
            else if(ground[i][j]==-5){
                cout << char(251) << " ";
            }
            else if(ground[i][j]==-2) cout << char(254) << " ";
            else if(ground[i][j]==-3) cout << "X ";
            else if(ground[i][j]==-4 || ground[i][j]==-5)cout << char(251) << " ";
            else cout << ground[i][j] << " ";
        }
    }
    restore_cursor();
    flush();
}

void select(int r, int c){
    if(ground[r][c] == -1){
        ground[r][c]=-3;
        show_ground(1);
        return;
    }
    else if(ground[r][c] == -2){
        int nearby_mines=0;
        for(int i=-1; i<=1; i++){
            for(int j=-1; j<=1; j++){
                int r_checker=r+i;
                int c_checker=c+j;
                if(r_checker < 0 || r_checker >= ground_row || c_checker < 0 || c_checker >= ground_col) continue;
                if(ground[r_checker][c_checker] == -1 || ground[r_checker][c_checker] == -5) nearby_mines++;
            }
        }
        ground[r][c]=nearby_mines;
        selected_cells++;
        if(nearby_mines==0){
            for(int i=-1; i<=1; i++){
                for(int j=-1; j<=1; j++){
                    int r_checker=r+i;
                    int c_checker=c+j;
                    if(r_checker < 0 || r_checker >= ground_row || c_checker < 0
                       || c_checker >= ground_col || ground[r_checker][c_checker] != -2) continue;
                    select(r_checker,c_checker);
                }
            }
        }
    }
    else{
        notify("this cell has been selected!");
    }
}

void flag(int x, int y){ // -1=mine, -2=not selected, -3=exploded mine, -4=flagged, -5=flagged a mine
    if(ground[x][y]==-4){
        ground[x][y]=-2;
        flags++;
    }
    else if(ground[x][y]==-5){
        ground[x][y]=-1;
        flags++;
    }
    else if(flags > 0){
        if(ground[x][y]==-2){
            ground[x][y]=-4;
            flags--;
        }
        else if(ground[x][y]==-1){
            ground[x][y]=-5;
            flags--;
        }
    }
}

void update_player_saved_games_count(int count, int id){
    fstream file;
    file.open(".\\data\\saved_games_count.txt", ios::out);
    if(file.is_open()){
        for(int i=0; i<id; i++){
            file << saved_games_count[i] << '\n';
        }
        file << count << '\n';
        for(int i=id+1; i<players_count; i++){
            file << saved_games_count[i] << '\n';
        }
        file.close();
    }
    else{
        clear_screen();
        cout << "error while opening files!";
        exit(0);
    }
    load_files();
}

void save_ground_data(bool finished){
    if(playing_game==0){
        saved_games_count[player_id]++;
        player_saved_games++;
        playing_game=player_saved_games;
    }
    string number;
    stringstream ss;
    ss << playing_game;
    ss >> number;
    string file_name_str=".\\data\\"+name+"_game "+number+".txt";
    char* file_name = new char[file_name_str.size()+1];
    strcpy(file_name, file_name_str.c_str());
    fstream file;
    file.open(file_name, ios::out);
    delete[] file_name;
    if(file.is_open()){
        file << finished << '\n';
        file << mines << '\n';
        file << flags << '\n';
        file << ground_row << '\n';
        file << ground_col << '\n';
        for(int i=0; i<ground_row; i++){
            for(int j=0; j<ground_col; j++){
                file << ground[i][j] << '\n';
            }
        }
        update_player_saved_games_count(player_saved_games, player_id);
        file.close();
    }
    else{
        clear_screen();
        cout << "error while opening files!";
        exit(0);
    }
    load_files();
}

void play_game(bool allowed){ //allowed = allowed to play
    if(allowed){
        show_ground(0);
    }
    else{
        show_ground(1);
        notify("game has finished!");
    }
    cursor_to_pos(4,2);
    while(true){
        char key=nb_getch();
        if(key!=0){
            int x=get_cursor_x();
            int y=get_cursor_y();
            clear_notify();
            if(key==27){ //esc
                quit();
            }
            if(key==32 && allowed){ //space
                int r=(x-4), c=(y-2)/2;
                if(ground[r][c]==-1 || ground[r][c]==-2){ // -1=mine, -2=not selected, -3=exploded mine, -4=flagged, -5=flagged a mine
                    select(r, c);
                    show_ground(0);
                    cursor_to_pos(x,y);
                }
                else{
                    notify("this cell has been selected!");
                }
                if(ground[r][c]==-3){
                    show_ground(1);
                    notify("you lost! press O to go to the menu!");
                    cursor_to_pos(x,y);
                    do{
                        key=nb_getch();
                    }while(key!='o' && key!='O');
                    //here we save game as mode 1 (finished game)
                    save_ground_data(1);
                    break;
                }
                else if(ground_row*ground_col-selected_cells==mines){
                    points += ground_row*ground_col*mines;
                    update_player_points(points, player_id);
                    show_ground(1);
                    cursor_to_pos(2, 16);
                    change_color(11);
                    cout << "you won! you got " << ground_row*ground_col*mines << " points from this game! press O to go back.";
                    reset_color();
                    play_beep();
                    cursor_to_pos(x,y);
                    flush();
                    do{
                        key=nb_getch();
                    }while(key!='o' && key!='O');
                    //here we save game as mode 1 (finished game)
                    save_ground_data(1);
                    break;
                }
            }
            else if((key=='f' || key=='F') && allowed){ //flag
                int r=(x-4), c=(y-2)/2;
                flag(r, c);
                show_ground(0);
                cursor_to_pos(2, 16);
                change_color(11);
                cout << "you have " << flags << " flags left!";
                reset_color();
                play_beep();
                cursor_to_pos(x,y);
                flush();
            }
            else if(key=='o' || key=='O'){
                if(allowed){
                    save_ground_data(0);
                }
                break;
            }
            else if(key=='q' || key=='Q'){
                    break;
            }
            else if(x>=4 && x<=ground_row+3 && y>=2 && y<=2*ground_col && allowed){
                if(key=='w' || key=='W'){
                    if(x!=4){
                        cursor_up(1);
                    }
                }
                else if(key=='s' || key=='S'){
                    if(x!=ground_row+3){
                        cursor_down(1);
                    }
                }
                else if(key=='d' || key=='D'){
                    if(y!=2*ground_col){
                        cursor_forward(2);
                    }
                }
                else if(key=='a' || key=='A'){
                    if(y!=2){
                        cursor_backward(2);
                    }
                }
            }
        }
    }
    menu();
}

void make_new_game(int given_ground_row, int given_ground_col, int given_mines_count){
    if(given_ground_row==-1){ //custom
        clear_screen();
        header();
        while(true){
            cursor_to_pos(4,1);
            cout << "Please Enter 3 numbers for row, col and number of mines: " << endl;
            cout << "two first numbers must be between 5-20" << endl;
            cout << "and mines must be less than (row*col) and more than 0 : ";
            cin >> given_ground_row >> given_ground_col >> given_mines_count;
            flush();
            if(given_ground_row>=5 && given_ground_row<=20 && given_ground_col>=5 && given_ground_col<=20 && given_mines_count < given_ground_row*given_ground_col && given_mines_count > 0){
                break;
            }
            clear_screen();
            header();
            notify("Please Enter Valid Numbers!");
        }
    }
    ground_row=given_ground_row;
    ground_col=given_ground_col;
    mines=given_mines_count;
    flags=mines;
    selected_cells=0;
    playing_game=0;
    put_mines();
    play_game(1);
}



void print_menu(int colorful_item){
    save_cursor();
    cursor_to_pos(4,1);
    if(colorful_item==1){
        change_color(3);
    }
    cout << "-> New Game" << endl;
    reset_color();
    if(colorful_item==2){
        change_color(3);
    }
    cout << "-> Load Game" << endl;
    reset_color();
    if(colorful_item==3){
        change_color(3);
    }
    cout << "-> Change User" << endl;
    reset_color();
    if(colorful_item==4){
        change_color(3);
    }
    cout << "-> Leaderboard" << endl;
    reset_color();
    if(colorful_item==5){
        change_color(3);
    }
    cout << "-> Quit" << endl;
    reset_color();
    restore_cursor();
    flush();
}

void print_new_game(int colorful_item){
    save_cursor();
    cursor_to_pos(4,1);
    change_color(10);
    cout << "~ Select a Level:" << endl;
    reset_color();
    if(colorful_item==1){
        change_color(3);
    }
    cout << "-> Easy" << endl;
    reset_color();
    if(colorful_item==2){
        change_color(3);
    }
    cout << "-> Medium" << endl;
    reset_color();
    if(colorful_item==3){
        change_color(3);
    }
    cout << "-> Hard" << endl;
    reset_color();
    if(colorful_item==4){
        change_color(3);
    }
    cout << "-> Custom" << endl;
    reset_color();
    if(colorful_item==5){
        change_color(3);
    }
    cout << "-> Go Back To The Menu" << endl;
    reset_color();
    restore_cursor();
    flush();
}

void new_game_keyboard(){
    cursor_to_pos(5,1);
    while(true){
        char key=nb_getch();
        if(key!=0){
            clear_notify();
            if(key==27){ //esc
                quit();
            }
            else if(key==32){ //space
                switch(get_cursor_x()){
                case 5: make_new_game(5,5,4); return;
                case 6: make_new_game(12,12,28); return;
                case 7: make_new_game(20,20,96); return;
                case 8: make_new_game(-1,-1,-1); return;
                case 9: menu(); return;
                }
            }
            else if(get_cursor_x()>=5 && get_cursor_x()<=9){
                if(key=='w' || key=='W'){
                    if(get_cursor_x()!=5){
                        cursor_up(1);
                    }
                }
                else if(key=='s' || key=='S'){
                    if(get_cursor_x()!=9){
                        cursor_down(1);
                    }
                }
                print_new_game(get_cursor_x()-4);
            }
        }
    }
}

void new_game(){
    clear_screen();
    header();
    menu_footage();
    print_new_game(1);
    flush();
    new_game_keyboard();
}

void leaderboard(){
    clear_screen();
    header();
    int row=get_window_rows(), col=get_window_cols();
    string *names_for_leaderboard=new string[players_count];
    int *points_for_leaderboard=new int[players_count];
    for(int i=0; i<players_count; i++){
        names_for_leaderboard[i]=players[i];
        points_for_leaderboard[i]=players_points[i];
    }
    for (int i = 0; i < players_count-1; i++){
        bool swapped=0;
        for (int j = 0; j < players_count-i-1; j++){
            if (points_for_leaderboard[j] < points_for_leaderboard[j+1]){
                swap(points_for_leaderboard[j], points_for_leaderboard[j+1]);
                swap(names_for_leaderboard[j], names_for_leaderboard[j+1]);
                swapped=1;
            }
        }
        if(!swapped) break;
    }
    notify("Here is the leaderboard!");
    cursor_to_pos(4,3);
    change_background_color(6);
    change_color_rgb(0,0,0);
    cout << "rank";
    reset_color();
    cout << "            ";
    change_background_color(6);
    change_color_rgb(0,0,0);
    cout << "name of player";
    reset_color();
    cout << "            ";
    change_background_color(6);
    change_color_rgb(0,0,0);
    cout << "points";
    reset_color();
    change_color_rgb(255,223,0);
    int leaderboard_players=row-6>players_count ? players_count:row-6;
    for(int i=0; i<leaderboard_players; i++){
        if(i==5){
            reset_color();
        }
        cursor_to_pos(5+i,3);
        cout << i+1;
        cursor_to_pos(5+i,19);
        cout << names_for_leaderboard[i];
        cursor_to_pos(5+i,45);
        cout << points_for_leaderboard[i];
    }
    reset_color();
    delete[] names_for_leaderboard;
    delete[] points_for_leaderboard;
    cursor_to_pos(row-2, 1);
    print_a_line();
    cursor_to_pos(row-1, col/2-29);
    cout << "press M to go to the menu" << "     "
    << char(179) << "     " << "press ESC to exit";
    while(true){
        char key=nb_getch();
        if(key!=0){
            if(key==27){ //esc
                quit();
            }
            else if(key=='m' || key=='M'){
                break;
            }
        }
    }
    menu();
}

void load_game(){
    clear_screen();
    header();
    cursor_to_pos(4,2);
    cout << "You have ";
    change_color(13);
    cout << player_saved_games;
    reset_color();
    cout << " saved games!" << endl;
    cout << " enter number of the game you want to play" << endl;
    cout << " e.g. enter ";
    change_color(13);
    cout << player_saved_games;
    reset_color();
    cout << " for playing the last saved game (or enter 0 to go back): ";
    flush();
    int game_num;
    cin >> game_num;
    if(game_num==0){
        menu();
        return;
    }
    playing_game=game_num;
    string number;
    stringstream ss;
    ss << game_num;
    ss >> number;
    string file_name_str=".\\data\\"+name+"_game "+number+".txt";
    char* file_name = new char[file_name_str.size()+1];
    strcpy(file_name, file_name_str.c_str());
    fstream file;
    file.open(file_name, ios::in);
    delete[] file_name;
    if(file.is_open()){
        string x;
        getline(file, x);
        int game_mode=atoi(x.c_str());
        getline(file, x);
        mines=atoi(x.c_str());
        getline(file, x);
        flags=atoi(x.c_str());
        getline(file, x);
        ground_row=atoi(x.c_str());
        getline(file, x);
        ground_col=atoi(x.c_str());
        for(int i=0; i<ground_row; i++){
            for(int j=0; j<ground_col; j++){
                getline(file, x);
                ground[i][j]=atoi(x.c_str());
            }
        }

        if(game_mode==1){ //game finished
            play_game(0);
        }
        else{
            play_game(1);
        }
        file.close();
    }
    else{
        clear_screen();
        cout << "error while opening files!";
        exit(0);
    }
}

void menu_keyboard(){
    cursor_to_pos(4,1);
    while(true){
        char key=nb_getch();
        if(key!=0){
            clear_notify();
            if(key==27){ //esc
                quit();
            }
            else if(key==32){ //space
                switch(get_cursor_x()){
                case 4: new_game(); return;
                case 5: load_game(); return;
                case 6: change_name(); return;
                case 7: leaderboard(); return;
                case 8: quit(); return;
                }
            }
            else if(get_cursor_x()>=4 && get_cursor_x()<=8){
                if(key=='w' || key=='W'){
                    if(get_cursor_x()!=4){
                        cursor_up(1);
                    }
                }
                else if(key=='s' || key=='S'){
                    if(get_cursor_x()!=8){
                        cursor_down(1);
                    }
                }
                print_menu(get_cursor_x()-3);
            }
        }
    }
}

void menu(){
    flush();
    clear_screen();
    header();
    print_menu(1);
    menu_footage();
    notify("welcome to the menu!");
    flush();
    menu_keyboard();
}
