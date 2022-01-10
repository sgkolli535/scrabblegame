
#include "computer_player.h"

#include <memory>
#include <stdexcept>
#include <string>

void ComputerPlayer::left_part(
        Board::Position anchor_pos,
        std::string partial_word,
        Move partial_move,
        std::shared_ptr<Dictionary::TrieNode> node,
        size_t limit,
        TileCollection& remaining_tiles,
        std::vector<Move>& legal_moves,
        const Board& board) const {
    // HW5: IMPLEMENT THIS
    // call extend right as base case
    extend_right(anchor_pos, partial_word, partial_move, node, remaining_tiles, legal_moves, board);
    // if limit is greater than 0
    if (limit > 0) {
        // iterate through all the children of current node
        for (std::map<char, std::shared_ptr<Dictionary::TrieNode>>::iterator it = node->nexts.begin();
             it != node->nexts.end();
             it++) {
            // check if current node is available as a tile in player's hand
            try {
                // get tile of char of node
                TileKind new_tile = remaining_tiles.lookup_tile(it->first);
                // copy current move to new move
                Move new_partial_move = partial_move;
                // translate starting position of move left or up depending on move direction
                if (new_partial_move.direction == Direction::DOWN) {
                    new_partial_move.row--;
                } else {
                    new_partial_move.column--;
                }
                // push new tile into move's tiles
                new_partial_move.tiles.push_back(new_tile);
                // remove new tile from player's hand
                remaining_tiles.remove_tile(new_tile);
                // call left part recursively with new partial word, partial move, limit decremented, one less tile, and
                // new node
                left_part(
                        anchor_pos,
                        partial_word + it->first,
                        new_partial_move,
                        it->second,
                        limit - 1,
                        remaining_tiles,
                        legal_moves,
                        board);
                // back track by adding new tile back to player's hand
                remaining_tiles.add_tile(new_tile);
            } catch (std::out_of_range& e) {
            }
            // check if blank tile is available in player's hand
            try {
                // get blank tile
                TileKind new_tile = remaining_tiles.lookup_tile(TileKind::BLANK_LETTER);
                // set assigned of tile to be char of node
                new_tile.assigned = it->first;
                // copy current move to new move
                Move new_partial_move = partial_move;
                // translate starting position of move left or up depending on move direction
                if (new_partial_move.direction == Direction::DOWN) {
                    new_partial_move.row--;
                } else {
                    new_partial_move.column--;
                }
                // push new tile into move's tiles
                new_partial_move.tiles.push_back(new_tile);
                // remove new tile from player's hand
                remaining_tiles.remove_tile(new_tile);
                // call left part recursively with new partial word, partial move, limit decremented, one less tile, and
                // new node
                left_part(
                        anchor_pos,
                        partial_word + it->first,
                        new_partial_move,
                        it->second,
                        limit - 1,
                        remaining_tiles,
                        legal_moves,
                        board);
                // back track by adding new tile back to player's hand
                remaining_tiles.add_tile(new_tile);
            } catch (std::out_of_range& e) {
            }
        }
    }
}

void ComputerPlayer::extend_right(
        Board::Position square,
        std::string partial_word,
        Move partial_move,
        std::shared_ptr<Dictionary::TrieNode> node,
        TileCollection& remaining_tiles,
        std::vector<Move>& legal_moves,
        const Board& board) const {
    // HW5: IMPLEMENT THIS
    // check if current node marks a valid word and push move into vector of moves
    if (node->is_final) {
        legal_moves.push_back(partial_move);
    }
    // check that square is in bounds
    if (board.is_in_bounds(square)) {
        // if square has a tile
        if (board.in_bounds_and_has_tile(square)) {
            // if letter at square is a child of current node
            if (node->nexts.find(board.letter_at(square)) != node->nexts.end()) {
                // set node to child with letter at square
                node = node->nexts.find(board.letter_at(square))->second;
                // translate square right or down depending on move direction
                if (partial_move.direction == Direction::ACROSS) {
                    square.column++;
                } else {
                    square.row++;
                }
                // if node is a valid word then push move into vector of moves
                if (node->is_final) {
                    legal_moves.push_back(partial_move);
                }
                // call extend right with new partial word and node
                extend_right(
                        square,
                        partial_word + board.letter_at(square),
                        partial_move,
                        node,
                        remaining_tiles,
                        legal_moves,
                        board);
            }
            // return if letter at square is not a child of current node
            else {
                return;
            }
        }
        // if square is empty
        else {
            // if node is a valid word then push move into vector of moves
            if (node->is_final) {
                legal_moves.push_back(partial_move);
            }
            // iterate through all children of current node
            for (std::map<char, std::shared_ptr<Dictionary::TrieNode>>::iterator it = node->nexts.begin();
                 it != node->nexts.end();
                 it++) {
                // check if current node is available as a tile in player's hand
                try {
                    // get tile of char of node
                    TileKind new_tile = remaining_tiles.lookup_tile(it->first);
                    // translate square right or down depending on move direction
                    if (partial_move.direction == Direction::DOWN) {
                        square.row++;
                    } else {
                        square.column++;
                    }
                    // create copy of partial move
                    Move new_partial_move = partial_move;
                    // push back new tile to new partial move
                    new_partial_move.tiles.push_back(new_tile);
                    // remove new tile from player's hand
                    remaining_tiles.remove_tile(new_tile);
                    // call extend right with new partial word, new partial move, new node, and one less remaining tile
                    extend_right(
                            square,
                            partial_word + it->first,
                            new_partial_move,
                            it->second,
                            remaining_tiles,
                            legal_moves,
                            board);
                    // backtrack position of square
                    if (partial_move.direction == Direction::DOWN) {
                        square.row--;
                    } else {
                        square.column--;
                    }
                    // add tile back to player's hand to backtrack
                    remaining_tiles.add_tile(new_tile);
                } catch (std::out_of_range& e) {
                }
                // check if blank tile is available in player's hand
                try {
                    // get blank tile from player's hand
                    TileKind new_tile = remaining_tiles.lookup_tile(TileKind::BLANK_LETTER);
                    // set assigned to be char of node
                    new_tile.assigned = it->first;
                    // translate square right or down depending on direction of move
                    if (partial_move.direction == Direction::DOWN) {
                        square.row++;
                    } else {
                        square.column++;
                    }
                    // create copy of partial move
                    Move new_partial_move = partial_move;
                    // push back new tile to new partial move
                    new_partial_move.tiles.push_back(new_tile);
                    // remove new tile from player's hand
                    remaining_tiles.remove_tile(new_tile);
                    // call extend right with new partial word, new partial move, new node, and one less remaining tile
                    extend_right(
                            square,
                            partial_word + it->first,
                            new_partial_move,
                            it->second,
                            remaining_tiles,
                            legal_moves,
                            board);
                    // backtrack position of square
                    if (partial_move.direction == Direction::DOWN) {
                        square.row--;
                    } else {
                        square.column--;
                    }
                    // add tile back to player's hand to backtrack
                    remaining_tiles.add_tile(new_tile);
                } catch (std::out_of_range& e) {
                }
            }
        }
    }
}

Move ComputerPlayer::get_move(const Board& board, const Dictionary& dictionary) const {
    std::vector<Move> legal_moves;
    std::vector<Board::Anchor> anchors = board.get_anchors();
    // HW5: IMPLEMENT THIS
    // for loop iterates through vector of anchors
    for (size_t i = 0; i < anchors.size(); i++) {
        // create a copy of player's tiles
        TileCollection tiles_copy = this->tiles;
        // create a vector for move tiles
        std::vector<TileKind> move_tiles;
        // create new move with blank vector, position at anchor, and direction of anchor
        Move new_move = Move(move_tiles, anchors[i].position.row, anchors[i].position.column, anchors[i].direction);
        // create empty string
        std::string partial_word;
        // initialize node to be root of trie
        std::shared_ptr<Dictionary::TrieNode> node = dictionary.get_root();
        // call left part with anchor position, empty string, new move, root node, anchor limit, copy of tiles, empty
        // vector of moves, and board
        left_part(anchors[i].position, partial_word, new_move, node, anchors[i].limit, tiles_copy, legal_moves, board);
    }
    // return get best move
    return get_best_move(legal_moves, board, dictionary);
}

Move ComputerPlayer::get_best_move(
        std::vector<Move> legal_moves, const Board& board, const Dictionary& dictionary) const {
    Move best_move = Move();  // Pass if no move found
                              // HW5: IMPLEMENT THIS
    // if there are legal moves in vector
    if (legal_moves.size() > 0) {
        // keeps track of max points
        unsigned int points = 0;
        // iterate through legal moves vector
        for (size_t i = 0; i < legal_moves.size(); i++) {
            // call test place with move
            PlaceResult p_test_new = board.test_place(legal_moves[i]);
            // do not do comparisons if test place is invalid
            if (!p_test_new.valid) {
                continue;
            }
            // test for if all words created by move are valid
            bool d_test = true;
            // iterate through place result's word and check if is_word is false
            for (size_t k = 0; k < p_test_new.words.size(); k++) {
                // if is word is false, set d_test to false and break out of loop
                if (!dictionary.is_word(p_test_new.words[k])) {
                    d_test = false;
                    break;
                }
            }
            // only if d test is passed
            if (d_test) {
                // if move uses all tiles in player's hand
                if (legal_moves[i].tiles.size() == this->get_hand_size()) {
                    // compare current points to new placeresult's points with empty hand bonus
                    if ((p_test_new.points + 50) > points) {
                        // set best move to current move and points to placeresult's points plus 50
                        points = p_test_new.points + 50;
                        best_move = legal_moves[i];
                    }
                }
                // compare current points to new placeresult's points with empty hand bonus
                else if (p_test_new.points > points) {
                    // set best move to current move and points to placeresult's points
                    points = p_test_new.points;
                    best_move = legal_moves[i];
                }
            }
        }
    }
    // return best move
    return best_move;
}
