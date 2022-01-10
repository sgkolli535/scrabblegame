#include "board.h"

#include "board_square.h"
#include "exceptions.h"
#include "formatting.h"
#include <fstream>
#include <iomanip>

using namespace std;

bool Board::Position::operator==(const Board::Position& other) const {
    return this->row == other.row && this->column == other.column;
}

bool Board::Position::operator!=(const Board::Position& other) const {
    return this->row != other.row || this->column != other.column;
}

Board::Position Board::Position::translate(Direction direction) const { return this->translate(direction, 1); }

Board::Position Board::Position::translate(Direction direction, ssize_t distance) const {
    if (direction == Direction::DOWN) {
        return Board::Position(this->row + distance, this->column);
    } else {
        return Board::Position(this->row, this->column + distance);
    }
}

Board Board::read(const string& file_path) {
    ifstream file(file_path);
    if (!file) {
        throw FileException("cannot open board file!");
    }

    size_t rows;
    size_t columns;
    size_t starting_row;
    size_t starting_column;
    file >> rows >> columns >> starting_row >> starting_column;
    Board board(rows, columns, starting_row, starting_column);

    // TODO: complete implementation of reading in board from file here.
    // char that determines the multipliers needed for a particular BoardSquare
    char temp;
    // nested for loops that iterates for the number of rows and number of columns
    for (size_t i = 0; i < rows; i++) {
        // vector to get all BoardSquares for a column
        std::vector<BoardSquare> v1;
        for (size_t j = 0; j < columns; j++) {
            // extract temp char from file
            if (file >> temp) {
                // depending on the char, push a particular BoardSquare to the vector
                if (temp == '.') {
                    v1.push_back(BoardSquare(1, 1));
                } else if (temp == '2') {
                    v1.push_back(BoardSquare(2, 1));
                } else if (temp == '3') {
                    v1.push_back(BoardSquare(3, 1));
                } else if (temp == 'd') {
                    v1.push_back(BoardSquare(1, 2));
                } else if (temp == 't') {
                    v1.push_back(BoardSquare(1, 3));
                }
            }
        }
        // push the vector of a column to the main vector
        board.squares.push_back(v1);
    }
    return board;
}

size_t Board::get_move_index() const { return this->move_index; }
// helper function to find adjacent words created when placing a new tile
// takes in the Position to place the tile, the string of words created, TileKind of tile to be placed, direction fo
// move, and points created by move
void Board::look_around(
        const Board::Position& traverse,
        std::vector<std::string>& words,
        TileKind curr,
        Direction direction,
        unsigned int& points) const {
    // position created by copying Position to place tile
    Position temp = Position(traverse.row, traverse.column);
    // points created from the adjacent words
    unsigned int adj_points = 0;
    // word bonus to multiply the adjacent words, if tile being placed is on a word multiplier
    unsigned int w_b = 1;
    // if the move direction is across
    // if move direction is across, then the adjacent words created will be vertical
    if (direction == Direction::ACROSS) {
        // decrement the row of Position temp
        temp.row--;
        // keep decrementing the row of temp as long as it is in bounds and has a tile
        while (in_bounds_and_has_tile(temp)) {
            temp.row--;
        }
        // create new string to store adjacent word created
        string adj;
        // as long as temp is in bounds of the board
        while (is_in_bounds(temp)) {
            // check if BoardSquare has a tile that can be added to the word
            if (at(temp).has_tile()) {
                // get the tile at temp Position
                TileKind add = at(temp).get_tile_kind();
                // if the tile is a wildcard, add the assigned letter to adj string
                if (add.letter == '?') {
                    adj += add.assigned;
                }
                // add the letter from Tile add to the adj string
                else {
                    adj += add.letter;
                }
                // add the points from the tile to the points
                adj_points += add.points;
            }
            // if temp Position is where the new tile needs to be placed
            else if (temp.row == traverse.row) {
                // if the new tile is a wildcard, add the assigned letter to adj string
                if (curr.letter == '?') {
                    adj += curr.assigned;
                }
                // add the letter from the new tile to adj string
                else {
                    adj += curr.letter;
                }
                // add the points from multiplying points on new tile with the letter multiplier at that Position
                adj_points += (curr.points * at(traverse).letter_multiplier);
            }
            // if temp is below the position to place new tile, and it does not have a tile, then break out of the loop
            // this means there is a gap, which means a word cannot be created
            else if (temp.row > traverse.row && !at(temp).has_tile()) {
                break;
            }
            // increment temp row to keep traversing down
            temp.row++;
        }
        // get the word multiplier from placing the new tile
        w_b *= at(traverse).word_multiplier;
        // multiply the points from adj word by the word multiplier
        adj_points *= w_b;
        // add the points from adj word to the total points
        points += adj_points;
        // push back adj word to the total vector of words
        words.push_back(adj);
    }
    // if the move direction is down
    // if move direction is down, then the adjacent words created will be horizontal
    else if (direction == Direction::DOWN) {
        // decrement column of position temp
        temp.column--;
        // keep decrementing the row of temp as long as it is in bounds and has a tile
        while (in_bounds_and_has_tile(temp)) {
            temp.column--;
        }
        // create new string to store adjacent word created
        string adj;
        // as long as temp is in bounds of the board
        while (is_in_bounds(temp)) {
            // check if BoardSquare has a tile that can be added to the word
            if (at(temp).has_tile()) {
                // get the tile at temp Position
                TileKind add = at(temp).get_tile_kind();
                // if the tile is a wildcard, add the assigned letter to adj string
                if (add.letter == '?') {
                    adj += add.assigned;
                }
                // add the letter from Tile add to the adj string
                else {
                    adj += add.letter;
                }
                // add the points from the tile to the points
                adj_points += add.points;
            }
            // check if temp Position is where the new tile needs to be placed
            else if (temp.column == traverse.column) {
                // if the new tile is a wildcard, add the assigned letter to adj string
                if (curr.letter == '?') {
                    adj += curr.assigned;
                }
                // add the letter from the new tile to adj string
                else {
                    adj += curr.letter;
                }
                // add the points from multiplying points on new tile with the letter multiplier at that Position
                adj_points += (curr.points * at(traverse).letter_multiplier);
            }
            // if temp is right of the position to place new tile, and it does not have a tile, then break out of the
            // loop this means there is a gap, which means a word cannot be created
            else if (temp.column > traverse.column && !at(temp).has_tile()) {
                break;
            }
            // increment temp column to keep traversing across
            temp.column++;
        }
        // get the word multiplier from placing the new tile
        w_b *= at(traverse).word_multiplier;
        // multiply the points from adj word by the word multiplier
        adj_points *= w_b;
        // add the points from adj word to the total points
        points += adj_points;
        // push back adj word to the total vector of words
        words.push_back(adj);
    }
}
// tests placement on board
PlaceResult Board::test_place(const Move& move) const {
    // TODO: complete implementation here
    // initialize placeresult as invalid
    PlaceResult temp = PlaceResult("Unable to place move");
    // if move is the first placement on the board
    if (this->get_move_index() == 0) {
        // bool checks if the first word is placed correctly on the starting square
        bool start_point = false;
        // position to keep track of where to place tiles starting at the row and column of move
        Position check = Position(move.row, move.column);
        // keeps track of number of tiles placed on board
        size_t num_tiles = 0;
        // keeps track of points from move
        unsigned int points = 0;
        // keeps track of word multiplier
        unsigned int word_m = 1;
        // keeps track of all words created by placement
        vector<string> words;
        // string to keep track of word created
        string current;
        // as long as position is in bounds and all the tiles have not been placed
        while (is_in_bounds(check) && num_tiles != move.tiles.size()) {
            // if current position is at starting position, then starting square is covered
            if (check == start) {
                start_point = true;
            }
            // if the move is across
            if (move.direction == Direction::ACROSS) {
                // add the points from tile multiplied by letter multiplier at position
                points += (move.tiles[num_tiles].points * at(check).letter_multiplier);
                // multiply by word multiplier at position
                word_m *= at(check).word_multiplier;
                // add assigned letter of tile to string if a wildcard
                if (move.tiles[num_tiles].letter == '?') {
                    current += move.tiles[num_tiles].assigned;
                }
                // add letter of tile to string
                else {
                    current += move.tiles[num_tiles].letter;
                }
                // increment column to traverse across
                check.column++;
            }
            // if the move is down
            else if (move.direction == Direction::DOWN) {
                // add the points from tile multiplied by letter multiplier at position
                points += (move.tiles[num_tiles].points * at(check).letter_multiplier);
                // multiply by word multiplier at position
                word_m *= at(check).word_multiplier;
                // add assigned letter of tile to string if a wildcard
                if (move.tiles[num_tiles].letter == '?') {
                    current += move.tiles[num_tiles].assigned;
                }
                // add letter of tile to string
                else {
                    current += move.tiles[num_tiles].letter;
                }
                // increment row to traverse down
                check.row++;
            }
            num_tiles++;
        }
        // push back the word created from placing the tiles
        words.push_back(current);
        // if the starting square is crossed, all tiles are placed, and move is not a single tile on empty board
        if (start_point && num_tiles == move.tiles.size() && move.tiles.size() > 1) {
            // multiply points by word multiplier
            points *= word_m;
            // assign placeresult with vector of words and points created
            temp = PlaceResult(words, points);
        }
        // assign PlaceResult with appropriate error, depending on error in command
        // if first move does not cross the starting square
        else if (!start_point) {
            temp = PlaceResult("First Move Does not Start on Start Square");
        }
        // if starting point is out of bounds, or placing tiles goes out of bounds
        else if (num_tiles != move.tiles.size()) {
            temp = PlaceResult("Placing One or More Tiles Goes Out of Bounds");
        }
        // if single tile is being placed on an empty board
        else if (move.tiles.size() <= 1) {
            temp = PlaceResult("Cannot Place Single Tile on Empty Board");
        }
    }
    // if move is not the first placement on the board
    else if (this->get_move_index() != 0) {
        // checks if move goes out of bounds
        bool bounds = true;
        // checks if move is adjacent to existing tiles
        bool adjacent = false;
        // total points accumulated from move
        unsigned int points = 0;
        // points from placing tiles and creating current word, not adjacent words
        unsigned int curr_points = 0;
        // keeps track of word multipliers
        unsigned int word_m = 1;
        // keeps track of all words created from move
        vector<string> words;
        // keeps track of how many tiles are placed
        size_t all_placed = 0;
        // position used to traverse move
        Position traverse = Position(move.row, move.column);
        // string to keep track of current word created by placing tiles, not adjacent words
        string current;
        // if the direction is across, the starting position of move is in bounds, and the starting position does not
        // have a tile already
        if (move.direction == Direction::ACROSS && is_in_bounds(traverse) && !at(traverse).has_tile()) {
            // traverse to the left, as long as position is in bounds and has tile, so that left tiles can be added to
            // word created
            traverse.column--;
            while (in_bounds_and_has_tile(traverse)) {
                traverse.column--;
                adjacent = true;
            }
            // until the starting point of move is reached
            while (traverse.column != move.column) {
                // as long as square is in bounds and has a tile
                if (in_bounds_and_has_tile(traverse)) {
                    // get tile at square
                    TileKind add = at(traverse).get_tile_kind();
                    // add assigned letter, if wildcard, to current word
                    if (add.letter == '?') {
                        current += add.assigned;
                    }
                    // add letter to current word
                    else {
                        current += add.letter;
                    }
                    // add points of tile to current points of word
                    curr_points += add.points;
                }
                // keep traversing back right
                traverse.column++;
            }
            // until all tiles are placed
            while (all_placed != move.tiles.size()) {
                // if position goes out of bounds while placing tiles, break out of loop and set bounds to false
                if (!is_in_bounds(traverse)) {
                    bounds = false;
                    break;
                }
                // if current square does not have a tile
                if (!at(traverse).has_tile()) {
                    // make positions one above and one above current position to place tile
                    Position above = Position(traverse.row + 1, traverse.column);
                    Position below = Position(traverse.row - 1, traverse.column);
                    // add points of tile multiplied by letter multiplier at current square
                    curr_points += (move.tiles[all_placed].points * at(traverse).letter_multiplier);
                    // multipy by word multiplier at current square
                    word_m *= at(traverse).word_multiplier;
                    // if the tile is a wildcard, add assigned letter to current word
                    if (move.tiles[all_placed].letter == '?') {
                        current += move.tiles[all_placed].assigned;
                    }
                    // add letter to current word
                    else {
                        current += move.tiles[all_placed].letter;
                    }
                    // if the squares above or below have tiles and are in bounds
                    if (in_bounds_and_has_tile(above) || in_bounds_and_has_tile(below)) {
                        // call helper function to find adjacent words created
                        look_around(traverse, words, move.tiles[all_placed], Direction::ACROSS, points);
                        // set adjacent to be true
                        adjacent = true;
                    }
                    // increment the number of tiles that are placed
                    all_placed++;
                }
                // if current square does have a tile
                else if (at(traverse).has_tile()) {
                    // set adjacent to be true
                    adjacent = true;
                    // get TileKind of square that has tile
                    TileKind exist = at(traverse).get_tile_kind();
                    // add assigned letter to current word, if tile is a wildcard
                    if (exist.letter == '?') {
                        current += exist.assigned;
                    }
                    // add letter to current word
                    else {
                        current += exist.letter;
                    }
                    // add points of tile to current points
                    curr_points += exist.points;
                }
                // increment column to traverse right
                traverse.column++;
            }
            // traverse to the right to add any letters to extend current word
            while (in_bounds_and_has_tile(traverse)) {
                // get tile beyond placing new tiles
                TileKind add = at(traverse).get_tile_kind();
                // add assigned letter to current word, if a wildcard
                if (add.letter == '?') {
                    current += add.assigned;
                }
                // add letter to current word
                else {
                    current += add.letter;
                }
                // add points of tile to current points
                curr_points += add.points;
                // set adjacent to be true
                adjacent = true;
                // keep traversing to the right
                traverse.column++;
            }
            // if the move is placing more than one tile, since a single tile by itself is not a valid word
            if (current.length() > 1) {
                // push back the current word to total vector
                words.push_back(current);
                // multiply current points by total word multiplier
                curr_points *= word_m;
                // add current points to total points
                points += curr_points;
            }
        }
        // if the direction is down, the starting position of move is not out of bounds, and the starting position does
        // not have a tile already
        else if (move.direction == Direction::DOWN && is_in_bounds(traverse) && !at(traverse).has_tile()) {
            // traverse up, as long as position is in bounds and has tile, so that tiles above can be added to word
            // created
            traverse.row--;
            while (in_bounds_and_has_tile(traverse)) {
                adjacent = true;
                traverse.row--;
            }
            // until the starting point of move is reached
            while (traverse.row != move.row) {
                // as long as square is in bounds and has a tile
                if (in_bounds_and_has_tile(traverse)) {
                    // get tile at square
                    TileKind add = at(traverse).get_tile_kind();
                    // add assigned letter, if wildcard, to current word
                    if (add.letter == '?') {
                        current += add.assigned;
                    }
                    // add letter to current word
                    else {
                        current += add.letter;
                    }
                    // add points of tile to current points of word
                    curr_points += add.points;
                }
                // keep traversing back down
                traverse.row++;
            }
            // until all tiles are placed
            while (all_placed != move.tiles.size()) {
                // if position goes out of bounds while placing tiles, break out of loop and set bounds to false
                if (!is_in_bounds(traverse)) {
                    bounds = false;
                    break;
                }
                // if current square does not have a tile
                if (!at(traverse).has_tile()) {
                    // make positions one to the left and one to the right of current position to place tile
                    Position left = Position(traverse.row, traverse.column - 1);
                    Position right = Position(traverse.row, traverse.column + 1);
                    // add points of tile multiplied by letter multiplier at current square
                    curr_points += (move.tiles[all_placed].points * at(traverse).letter_multiplier);
                    // multipy by word multiplier at current square
                    word_m *= at(traverse).word_multiplier;
                    // if the tile is a wildcard, add assigned letter to current word
                    if (move.tiles[all_placed].letter == '?') {
                        current += move.tiles[all_placed].assigned;
                    }
                    // add letter to current word
                    else {
                        current += move.tiles[all_placed].letter;
                    }
                    // if the squares above or below have tiles and are in bounds
                    if (in_bounds_and_has_tile(left) || in_bounds_and_has_tile(right)) {
                        // call helper function to find adjacent words created
                        look_around(traverse, words, move.tiles[all_placed], Direction::DOWN, points);
                        // set adjacent to be true
                        adjacent = true;
                    }
                    // increment the number of tiles that are placed
                    all_placed++;
                }
                // if current square does have a tile
                else if (at(traverse).has_tile()) {
                    // set adjacent to be true
                    adjacent = true;
                    // get TileKind of square that has tile
                    TileKind exist = at(traverse).get_tile_kind();
                    // add assigned letter to current word, if tile is a wildcard
                    if (exist.letter == '?') {
                        current += exist.assigned;
                    }
                    // add letter to current word
                    else {
                        current += exist.letter;
                    }
                    // add points of tile to current points
                    curr_points += exist.points;
                }
                // increment row to traverse down
                traverse.row++;
            }
            // traverse down to add any letters to extend current word
            while (in_bounds_and_has_tile(traverse)) {
                // get tile beyond placing new tiles
                TileKind add = at(traverse).get_tile_kind();
                // add assigned letter to current word, if a wildcard
                if (add.letter == '?') {
                    current += add.assigned;
                }
                // add letter to current word
                else {
                    current += add.letter;
                }
                // add points of tile to current points
                curr_points += add.points;
                // set adjacent to be true
                adjacent = true;
                // keep traversing down
                traverse.row++;
            }
            // if the move is placing more than one tile, since a single tile by itself is not a valid word
            if (current.length() > 1) {
                // push back the current word to total vector
                words.push_back(current);
                // multiply current points by total word multiplier
                curr_points *= word_m;
                // add current points to total points
                points += curr_points;
            }
        }
        // check if move is within bounds, is adjacent to existing tiles, and all tiles are placed
        if (bounds && adjacent && all_placed == move.tiles.size()) {
            // assign PlaceResult with vector of total words and total points earned
            temp = PlaceResult(words, points);
        }
        // assign PlaceResult with appropriate error in command
        // if placing tiles goes out of bounds of board
        else if (!bounds) {
            temp = PlaceResult("Placing One or More Tiles Goes Out of Bounds");
        }
        // if starting row or column is out of bounds of board or already has a tile on it
        else if (all_placed != move.tiles.size()) {
            temp = PlaceResult("Starting Row and Column Entered is Not Valid");
        }
        // if placement is not adjacent to at least one tile already placed on board
        else if (!adjacent) {
            temp = PlaceResult("Move is not Adjacent to at Least One Previously Placed Tile");
        }
    }
    // return PlaceResult temp
    return temp;
}
// places tile on board depending on validity of test_place
PlaceResult Board::place(const Move& move) {
    // TODO: Complete implementation here
    // call test_place to check if move is valid
    PlaceResult work = test_place(move);
    // if move is valid
    if (work.valid) {
        // if move direction is across
        if (move.direction == Direction::ACROSS) {
            // keeps track of number of tiles that are placed
            size_t tiles_placed = 0;
            // traverses across board
            size_t j = 0;
            // while not all tiles are placed
            while (tiles_placed != move.tiles.size()) {
                // check that current square does not have a tile
                if (!squares[move.row][move.column + j].has_tile()) {
                    // set tile of square from move.tiles
                    squares[move.row][move.column + j].set_tile_kind(move.tiles[tiles_placed]);
                    // increment number of tiles placed
                    tiles_placed++;
                }
                // keep traversing across board
                j++;
            }
        }
        // if move direction is down
        else if (move.direction == Direction::DOWN) {
            // keeps track of number of tiles that are placed
            size_t tiles_placed = 0;
            // traverses down board
            size_t j = 0;
            // while not all tiles are placed
            while (tiles_placed != move.tiles.size()) {
                // check that current square does not have a tile
                if (!squares[move.row + j][move.column].has_tile()) {
                    // set tile of square from move.tiles
                    squares[move.row + j][move.column].set_tile_kind(move.tiles[tiles_placed]);
                    // increment number of tiles placed
                    tiles_placed++;
                }
                // keep traversing down board
                j++;
            }
        }
        // increment move_index if move is valid
        move_index++;
    }
    // return PlaceResult work
    return work;
}

// The rest of this file is provided for you. No need to make changes.

BoardSquare& Board::at(const Board::Position& position) { return this->squares.at(position.row).at(position.column); }

const BoardSquare& Board::at(const Board::Position& position) const {
    return this->squares.at(position.row).at(position.column);
}

bool Board::is_in_bounds(const Board::Position& position) const {
    return position.row < this->rows && position.column < this->columns;
}

bool Board::in_bounds_and_has_tile(const Position& position) const {
    return is_in_bounds(position) && at(position).has_tile();
}
// returns letter or assigned letter at position
char Board::letter_at(Position p) const {
    TileKind p_tile = squares[p.row][p.column].get_tile_kind();
    if (p_tile.letter == '?') {
        return p_tile.assigned;
    }
    return p_tile.letter;
}
// checks if position is an anchor spot
bool Board::is_anchor_spot(Position p) const {
    // return true if position is starting square
    if (p == start && !squares[p.row][p.column].has_tile()) {
        return true;
    }
    // check that position is in bounds and is empty
    if (is_in_bounds(p) && !squares[p.row][p.column].has_tile()) {
        // if adjacent to a filled square above return true
        p.row--;
        if (in_bounds_and_has_tile(p)) {
            return true;
        }
        p.row++;
        p.row++;
        // if adjacent to a filled square below return true
        if (in_bounds_and_has_tile(p)) {
            return true;
        }
        p.row--;
        p.column--;
        // if adjacent to a filled square to the left return true
        if (in_bounds_and_has_tile(p)) {
            return true;
        }
        p.column++;
        p.column++;
        // if adjacent to a filled square to the right return true
        if (in_bounds_and_has_tile(p)) {
            p.column--;
            return true;
        }
    }
    // otherwise return false
    return false;
}
// returns a vector of all anchor spots
std::vector<Board::Anchor> Board::get_anchors() const {
    // initialize vector of anchors
    std::vector<Anchor> anchors;
    // nested for loops to iterate through whole board
    for (size_t i = 0; i < squares.size(); i++) {
        for (size_t j = 0; j < squares[i].size(); j++) {
            // get position of square
            Position temp(i, j);
            // check if position is anchor spot
            if (is_anchor_spot(temp)) {
                Position traverse_across = temp;
                size_t limit1 = 0;
                traverse_across.column--;
                // while loop to find the limit of anchor spot across
                while (is_in_bounds(traverse_across) && !squares[traverse_across.row][traverse_across.column].has_tile()
                       && !is_anchor_spot(traverse_across)) {
                    limit1++;
                    traverse_across.column--;
                }
                // create anchor for across and push to vector
                Anchor a(temp, Direction::ACROSS, limit1);
                anchors.push_back(a);
                Position traverse_down = temp;
                size_t limit2 = 0;
                traverse_down.row--;
                // while loop to find limit of anchor spot down
                while (is_in_bounds(traverse_down) && !squares[traverse_down.row][traverse_down.column].has_tile()
                       && !is_anchor_spot(traverse_down)) {
                    limit2++;
                    traverse_down.row--;
                }
                // create anchor for down and push to vector
                Anchor d(temp, Direction::DOWN, limit2);
                anchors.push_back(d);
            }
        }
    }
    return anchors;
}

void Board::print(ostream& out) const {
    // Draw horizontal number labels
    for (size_t i = 0; i < BOARD_TOP_MARGIN - 2; ++i) {
        out << std::endl;
    }
    out << FG_COLOR_LABEL << repeat(SPACE, BOARD_LEFT_MARGIN);
    const size_t right_number_space = (SQUARE_OUTER_WIDTH - 3) / 2;
    const size_t left_number_space = (SQUARE_OUTER_WIDTH - 3) - right_number_space;
    for (size_t column = 0; column < this->columns; ++column) {
        out << repeat(SPACE, left_number_space) << std::setw(2) << column + 1 << repeat(SPACE, right_number_space);
    }
    out << std::endl;

    // Draw top line
    out << repeat(SPACE, BOARD_LEFT_MARGIN);
    print_horizontal(this->columns, L_TOP_LEFT, T_DOWN, L_TOP_RIGHT, out);
    out << endl;

    // Draw inner board
    for (size_t row = 0; row < this->rows; ++row) {
        if (row > 0) {
            out << repeat(SPACE, BOARD_LEFT_MARGIN);
            print_horizontal(this->columns, T_RIGHT, PLUS, T_LEFT, out);
            out << endl;
        }

        // Draw insides of squares
        for (size_t line = 0; line < SQUARE_INNER_HEIGHT; ++line) {
            out << FG_COLOR_LABEL << BG_COLOR_OUTSIDE_BOARD;

            // Output column number of left padding
            if (line == 1) {
                out << repeat(SPACE, BOARD_LEFT_MARGIN - 3);
                out << std::setw(2) << row + 1;
                out << SPACE;
            } else {
                out << repeat(SPACE, BOARD_LEFT_MARGIN);
            }

            // Iterate columns
            for (size_t column = 0; column < this->columns; ++column) {
                out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL;
                const BoardSquare& square = this->squares.at(row).at(column);
                bool is_start = this->start.row == row && this->start.column == column;

                // Figure out background color
                if (square.word_multiplier == 2) {
                    out << BG_COLOR_WORD_MULTIPLIER_2X;
                } else if (square.word_multiplier == 3) {
                    out << BG_COLOR_WORD_MULTIPLIER_3X;
                } else if (square.letter_multiplier == 2) {
                    out << BG_COLOR_LETTER_MULTIPLIER_2X;
                } else if (square.letter_multiplier == 3) {
                    out << BG_COLOR_LETTER_MULTIPLIER_3X;
                } else if (is_start) {
                    out << BG_COLOR_START_SQUARE;
                }

                // Text
                if (line == 0 && is_start) {
                    out << "  \u2605  ";
                } else if (line == 0 && square.word_multiplier > 1) {
                    out << FG_COLOR_MULTIPLIER << repeat(SPACE, SQUARE_INNER_WIDTH - 2) << 'W' << std::setw(1)
                        << square.word_multiplier;
                } else if (line == 0 && square.letter_multiplier > 1) {
                    out << FG_COLOR_MULTIPLIER << repeat(SPACE, SQUARE_INNER_WIDTH - 2) << 'L' << std::setw(1)
                        << square.letter_multiplier;
                } else if (line == 1 && square.has_tile()) {
                    char l = square.get_tile_kind().letter == TileKind::BLANK_LETTER ? square.get_tile_kind().assigned
                                                                                     : ' ';
                    out << repeat(SPACE, 2) << FG_COLOR_LETTER << square.get_tile_kind().letter << l
                        << repeat(SPACE, 1);
                } else if (line == SQUARE_INNER_HEIGHT - 1 && square.has_tile()) {
                    out << repeat(SPACE, SQUARE_INNER_WIDTH - 1) << FG_COLOR_SCORE << square.get_points();
                } else {
                    out << repeat(SPACE, SQUARE_INNER_WIDTH);
                }
            }

            // Add vertical line
            out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL << BG_COLOR_OUTSIDE_BOARD << std::endl;
        }
    }

    // Draw bottom line
    out << repeat(SPACE, BOARD_LEFT_MARGIN);
    print_horizontal(this->columns, L_BOTTOM_LEFT, T_UP, L_BOTTOM_RIGHT, out);
    out << endl << rang::style::reset << std::endl;
}
