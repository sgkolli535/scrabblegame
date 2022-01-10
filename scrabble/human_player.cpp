#include "human_player.h"

#include "exceptions.h"
#include "formatting.h"
#include "move.h"
#include "place_result.h"
#include "rang.h"
#include "tile_kind.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

using namespace std;

// This method is fully implemented.
inline string& to_upper(string& str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

Move HumanPlayer::get_move(const Board& board, const Dictionary& dictionary) const {
    // TODO: begin your implementation here.
    // print player's hand at the start of each turn
    print_hand(std::cout);
    // used for while loop to iterate until player makes a valid move
    bool invalid = true;
    // initialize make_move
    Move make_move;
    while (invalid) {
        // initialize string temp to take in player's move
        string temp;
        // ask player to enter a move
        cout << "Your move, " << this->get_name() << ": ";
        // get the player's move and put into temp
        getline(std::cin, temp);
        // use try-catch block to catch exception from parse_tiles
        try {
            // assign make_move with string from user input
            make_move = parse_move(temp);
            // if move is PLACE, check if test_place is valid and if words created are valid in dictionary
            if (make_move.kind == MoveKind::PLACE) {
                // call test_place with make_move
                PlaceResult p_test = board.test_place(make_move);
                // keeps track of if all words created are in dictionary
                bool d_test = true;
                // if p_test is valid
                if (p_test.valid) {
                    // iterate through the vector of words created by p_test
                    for (size_t i = 0; i < p_test.words.size(); i++) {
                        // check if each word is in dictionary or not
                        if (!dictionary.is_word(p_test.words[i])) {
                            // set d_test to false if a word is invalid and break out of for loop
                            d_test = false;
                            break;
                        }
                    }
                    // checks if d_test is invalid, then outputs error message that words created by place are invalid
                    if (!d_test) {
                        cerr << "One or More Words are not in the Dictionary" << endl;
                    }
                }
                // if p_test is not valid, output the error message from PlaceResult
                else if (!p_test.valid) {
                    cerr << p_test.error << endl;
                }
                // if both p_test and d_test are valid, then PLACE is valid, so set invalid to false to stop loop
                if (p_test.valid && d_test) {
                    invalid = false;
                }
            }
            // set invalid to false if moves are not PLACE, because parse_tiles worked, and nothing else needs to be
            // checked
            else if (make_move.kind == MoveKind::PASS || make_move.kind == MoveKind::EXCHANGE) {
                invalid = false;
            }
        }
        // catch exception for if the user enters invalid tiles
        catch (CommandException& e) {
            cerr << e.what() << endl;
        }
    }
    // return the valid move
    return make_move;
}

vector<TileKind> HumanPlayer::parse_tiles(string& letters, string type) const {
    // TODO: begin implementation here.
    // initialize a vector of TileKinds
    vector<TileKind> parsed;
    // create stringstream from letters
    stringstream ss(letters);
    // initialize char letter that every letter will go into
    char letter;
    // create a copy of TileCollection tiles object to find tiles
    TileCollection copy_tiles = this->tiles;
    TileKind new_tile = TileKind('\0', 0);
    // extract each char letter from ss
    while (ss >> letter) {
        // use try-catch block to see if player actually has tile with given letter
        try {
            // assign new_tile to be tile returned from lookup_tile function with given letter
            new_tile = copy_tiles.lookup_tile(letter);
        }
        // catch exception for if a tile is not in the player's hand
        catch (out_of_range& e) {
            throw CommandException("Do not Have One or More Tiles");
        }
        // remove the new_tile from copy_tiles to account for copies of tiles
        copy_tiles.remove_tile(new_tile);
        // if tile is a wildcard
        if (letter == '?') {
            // checks to see move is a place, not an exchange
            if (type == "PLACE") {
                // get the letter after the question mark and assign new_tile
                letter = ss.get();
                new_tile.assigned = std::tolower(letter);
            }
        }
        // push new_tile to vector parsed
        parsed.push_back(new_tile);
    }
    // return the vector of TileKinds
    return parsed;
}

Move HumanPlayer::parse_move(string& move_string) const {
    // TODO: begin implementation here.
    // make stringstream of move_string
    stringstream sstream(move_string);
    // determines the type of move
    string type;
    // determines the direction, if move is a PLACE
    string direction;
    // determines the starting row and column, if move is a PLACE
    size_t r;
    size_t c;
    // extract type of move from sstream
    sstream >> type;
    // change move to be all capitalized
    to_upper(type);
    // initialize a move struct
    Move parsed_move;
    // move is assigned with no parameters, if it is a PASS
    if (type == "PASS") {
        parsed_move = Move();
    }
    // if type is EXCHANGE
    else if (type == "EXCHANGE") {
        // extract the string of letters to be exchanged from sstream and into vector_letters
        string vector_letters;
        sstream >> vector_letters;
        // initialize a vector of TileKinds
        vector<TileKind> parsed_tiles;
        // catch any exceptions if the tiles the player wants to exchange are invalid
        try {
            parsed_tiles = parse_tiles(vector_letters, type);
        } catch (CommandException& e) {
            throw;
        }
        // assign move with vector of TileKinds as parameter
        parsed_move = Move(parsed_tiles);
    }
    // if type if PLACE
    else if (type == "PLACE") {
        // extract direction, starting row and column from sstream
        sstream >> direction >> r >> c;
        // extract the string of letters to be placed from sstream and into vector_letters
        string vector_letters;
        sstream >> vector_letters;
        // initialize a vector of TileKinds
        vector<TileKind> parsed_tiles;
        // catch any exceptions if the tiles the player wants to place are invalid
        try {
            parsed_tiles = parse_tiles(vector_letters, type);
        } catch (CommandException& e) {
            throw;
        }
        // if the direction is aceoss
        if (direction == "-") {
            // subtract one from r and c to maintain 0-based indexing
            parsed_move = Move(parsed_tiles, r - 1, c - 1, Direction::ACROSS);
        }
        // if direction is down
        else if (direction == "|") {
            // subtract one from r and c to maintain 0-based indexing
            parsed_move = Move(parsed_tiles, r - 1, c - 1, Direction::DOWN);
        }
    }
    // return move
    return parsed_move;
}

// This function is fully implemented.
void HumanPlayer::print_hand(ostream& out) const {
    const size_t tile_count = tiles.count_tiles();
    const size_t empty_tile_count = this->get_hand_size() - tile_count;
    const size_t empty_tile_width = empty_tile_count * (SQUARE_OUTER_WIDTH - 1);

    for (size_t i = 0; i < HAND_TOP_MARGIN - 2; ++i) {
        out << endl;
    }

    out << repeat(SPACE, HAND_LEFT_MARGIN) << FG_COLOR_HEADING << "Your Hand: " << endl << endl;

    // Draw top line
    out << repeat(SPACE, HAND_LEFT_MARGIN) << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE;
    print_horizontal(tile_count, L_TOP_LEFT, T_DOWN, L_TOP_RIGHT, out);
    out << repeat(SPACE, empty_tile_width) << BG_COLOR_OUTSIDE_BOARD << endl;

    // Draw middle 3 lines
    for (size_t line = 0; line < SQUARE_INNER_HEIGHT; ++line) {
        out << FG_COLOR_LABEL << BG_COLOR_OUTSIDE_BOARD << repeat(SPACE, HAND_LEFT_MARGIN);
        for (auto it = tiles.cbegin(); it != tiles.cend(); ++it) {
            out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL << BG_COLOR_PLAYER_HAND;

            // Print letter
            if (line == 1) {
                out << repeat(SPACE, 2) << FG_COLOR_LETTER << (char)toupper(it->letter) << repeat(SPACE, 2);

                // Print score in bottom right
            } else if (line == SQUARE_INNER_HEIGHT - 1) {
                out << FG_COLOR_SCORE << repeat(SPACE, SQUARE_INNER_WIDTH - 2) << setw(2) << it->points;

            } else {
                out << repeat(SPACE, SQUARE_INNER_WIDTH);
            }
        }
        if (tiles.count_tiles() > 0) {
            out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL;
            out << repeat(SPACE, empty_tile_width) << BG_COLOR_OUTSIDE_BOARD << endl;
        }
    }

    // Draw bottom line
    out << repeat(SPACE, HAND_LEFT_MARGIN) << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE;
    print_horizontal(tile_count, L_BOTTOM_LEFT, T_UP, L_BOTTOM_RIGHT, out);
    out << repeat(SPACE, empty_tile_width) << rang::style::reset << endl;
}
