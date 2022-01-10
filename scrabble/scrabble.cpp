#include "scrabble.h"

#include "formatting.h"
#include <iomanip>
#include <iostream>
#include <map>

using namespace std;

// Given to you. this does not need to be changed
Scrabble::Scrabble(const ScrabbleConfig& config)
        : hand_size(config.hand_size),
          minimum_word_length(config.minimum_word_length),
          tile_bag(TileBag::read(config.tile_bag_file_path, config.seed)),
          board(Board::read(config.board_file_path)),
          dictionary(Dictionary::read(config.dictionary_file_path)) {}

// Game Loop should cycle through players and get and execute that players move
// until the game is over.
void Scrabble::game_loop() {
    // TODO: implement this.
    // keeps track of how many human players pass
    size_t passes = 0;
    // keeps track of the number of turns that have happened
    size_t i = 0;
    // keeps track of which player's turn it is
    size_t loop = i % players.size();
    // while loop that runs until all players pass in one round or at least one player has 0 tiles in their hand and
    // there are no more tiles left in tile bag
    while (passes != num_human_players && (players[loop]->count_tiles() != 0 && tile_bag.count_tiles() != 0)) {
        // print out the board to show player
        board.print(std::cout);
        // call get_move to find the move the player wants to make
        Move player_move = players[loop]->get_move(board, dictionary);
        // player's current score before points from move are calculated
        unsigned int current_score = players[loop]->get_points();
        // increment passes if the player wants to pass on their move
        if (player_move.kind == MoveKind::PASS && players[loop]->is_human()) {
            passes++;
        }
        // if the player wants to exchange tiles
        else if (player_move.kind == MoveKind::EXCHANGE) {
            // remove the tiles that the player wants to exchange
            // catch any exceptions from remove_tiles in player.cpp
            try {
                players[loop]->remove_tiles(player_move.tiles);
            } catch (out_of_range& e) {
                cerr << e.what() << endl;
            }
            // add each of the removed tiles back to tile bag
            for (size_t i = 0; i < player_move.tiles.size(); i++) {
                tile_bag.add_tile(player_move.tiles[i]);
            }
            // remove random tiles from tile bag
            vector<TileKind> result = tile_bag.remove_random_tiles(player_move.tiles.size());
            // add these tiles to player's hand
            players[loop]->add_tiles(result);
            if (players[loop]->is_human()) {
                // set passes to 0
                passes = 0;
            }
        }
        // if the player wants to place tiles
        else if (player_move.kind == MoveKind::PLACE) {
            // place the tiles on the board
            PlaceResult new_place = board.place(player_move);
            // double checks to make sure new_place is valid
            if (new_place.valid) {
                // add the points from placing tiles to player's score
                players[loop]->add_points(new_place.points);
                // if the player places their whole hand, add the empty hand bonus to player's score
                if (player_move.tiles.size() == players[loop]->get_hand_size()) {
                    players[loop]->add_points(EMPTY_HAND_BONUS);
                }
                // remove the placed tiles from the player's hand
                // catch any exceptions from remove_tiles in player.cpp
                try {
                    players[loop]->remove_tiles(player_move.tiles);
                } catch (out_of_range& e) {
                    cerr << e.what() << endl;
                }
                // remove random tiles from tile bag
                vector<TileKind> result = tile_bag.remove_random_tiles(player_move.tiles.size());
                // add these tiles to player's hand
                players[loop]->add_tiles(result);
            }
            if (players[loop]->is_human()) {
                // set passes to 0
                passes = 0;
            }
        }
        // get the player's score after move
        unsigned int new_score = players[loop]->get_points();
        // shows change in player's score from placing tiles
        if (player_move.kind == MoveKind::PLACE) {
            // shows how many points players gained by subtracting old score from new score
            cout << "You gained " << SCORE_COLOR << new_score - current_score << rang::style::reset << " points!"
                 << endl;
        }
        // shows player their score after move
        cout << "Your current score: " << SCORE_COLOR << new_score << rang::style::reset << endl;
        // allows game to move on to next player
        cout << endl << "Press [enter] to continue.";
        // ignore the cin to not interfere with next player's input
        cin.ignore();
        // increment i and reset loop to iterate through each player properly
        i++;
        loop = i % players.size();
    }
}

// Performs final score subtraction. Players lose points for each tile in their
// hand. The player who cleared their hand receives all the points lost by the
// other players.
void Scrabble::final_subtraction(vector<shared_ptr<Player>>& plrs) {
    // TODO: implement this method.
    // Do not change the method signature.
    // iterate through the vector of plrs
    for (size_t i = 0; i < plrs.size(); i++) {
        // if a player has 0 tiles in their hand
        if (plrs[i]->count_tiles() == 0) {
            // iterates through the other players and adds their hand values to this player's points
            for (size_t j = 0; j < plrs.size(); j++) {
                plrs[i]->add_points(plrs[j]->get_hand_value());
            }
        }
        // if the player does not have 0 tiles in their hand
        else {
            // if the player's hand value is greater than the player's points
            if (plrs[i]->get_hand_value() > plrs[i]->get_points()) {
                // set the player's points to 0
                plrs[i]->subtract_points(plrs[i]->get_points());
            }
            // if the player's hand value is less than the player's points
            else {
                // subtract the hand value from the points
                plrs[i]->subtract_points(plrs[i]->get_hand_value());
            }
        }
    }
}

void Scrabble::add_players() {
    int num_players;
    cout << "Please enter number of players: ";
    // extract the num_players
    cin >> num_players;
    // checks to see that the num_players is valid
    if (num_players < 1 || num_players > 8) {
        throw FileException("Invalid Number of Players");
    }
    // print how many players have been confirmed
    cout << num_players << " Players have been confirmed." << endl;
    // initialize a vector of the names of the players
    vector<string> names;
    // keeps track of if player is a computer
    vector<bool> types;
    // initialize a string of names
    string name;
    // keeps track of if player is a computer
    string type;
    // loop for num_players
    for (int i = 1; i <= num_players; i++) {
        // ask for the name of each player
        cout << "Please enter name for player " << i << ": ";
        // to avoid any input from previous statement
        if (i == 1) {
            cin.ignore();
        }
        // get the name of the player and put in string name
        getline(cin, name);
        // push back name into vector of names
        names.push_back(name);
        // ask if player is a computer
        cout << "Is the player a computer? ";
        getline(cin, type);
        // push true if player is a computer and false if not
        if (type == "y") {
            types.push_back(true);
        } else {
            types.push_back(false);
        }
        // print out confirmation that player has been added to game
        cout << "Player " << i << " named '" << name << "' has been added." << endl;
    }
    // create shared_ptr for new player
    shared_ptr<Player> new_player;
    // loops for num_players
    for (int i = 0; i < num_players; i++) {
        // if player is a computer
        if (types[i]) {
            new_player = make_shared<ComputerPlayer>(names[i], hand_size);
        }
        // if player is a human
        else {
            // make shared_ptr to HumanPlayer with name and hand_size
            new_player = make_shared<HumanPlayer>(names[i], hand_size);
            num_human_players++;
        }
        // get a vector of random tiles from tile bag
        vector<TileKind> result = tile_bag.remove_random_tiles(hand_size);
        // add the random tiles to new player's hand
        new_player->add_tiles(result);
        // push new player to the vector pf players
        players.push_back(new_player);
    }
}

// You should not need to change this function.
void Scrabble::print_result() {
    // Determine highest score
    size_t max_points = 0;
    for (auto player : this->players) {
        if (player->get_points() > max_points) {
            max_points = player->get_points();
        }
    }

    // Determine the winner(s) indexes
    vector<shared_ptr<Player>> winners;
    for (auto player : this->players) {
        if (player->get_points() >= max_points) {
            winners.push_back(player);
        }
    }

    cout << (winners.size() == 1 ? "Winner:" : "Winners: ");
    for (auto player : winners) {
        cout << SPACE << PLAYER_NAME_COLOR << player->get_name();
    }
    cout << rang::style::reset << endl;

    // now print score table
    cout << "Scores: " << endl;
    cout << "---------------------------------" << endl;

    // Justify all integers printed to have the same amount of character as the high score, left-padding with spaces
    cout << setw(static_cast<uint32_t>(floor(log10(max_points) + 1)));

    for (auto player : this->players) {
        cout << SCORE_COLOR << player->get_points() << rang::style::reset << " | " << PLAYER_NAME_COLOR
             << player->get_name() << rang::style::reset << endl;
    }
}

// You should not need to change this.
void Scrabble::main() {
    add_players();
    game_loop();
    final_subtraction(this->players);
    print_result();
}
