#include "player.h"

using namespace std;

// TODO: implement member functions
// add the amount of points passed in to player's total points
void Player::add_points(size_t points) { this->points += points; }
// subtract the amount of points passed in from player's total points
void Player::subtract_points(size_t points) { this->points -= points; }
// get the total points the player has
size_t Player::get_points() const { return this->points; }
// get the name of the player
const std::string& Player::get_name() const { return this->name; }
// get the number of tiles in the player's hand
size_t Player::count_tiles() const { return tiles.count_tiles(); }
// remove the tiles of the vector of TileKinds passed in from the player's hand
void Player::remove_tiles(const std::vector<TileKind>& tiles) {
    // iterate through the vector of tiles passed in
    for (size_t i = 0; i < tiles.size(); i++) {
        // catch any exceptions from the remove_tile/remove_tiles functions in tile_collection
        try {
            this->tiles.remove_tile(tiles[i]);
        } catch (out_of_range& e) {
            throw;
        }
    }
}
// add the tiles of the vector of TileKinds passed in to the player's hand
void Player::add_tiles(const std::vector<TileKind>& tiles) {
    for (size_t i = 0; i < tiles.size(); i++) {
        this->tiles.add_tile(tiles[i]);
    }
}
// check if player has a specific tile
bool Player::has_tile(TileKind tile) {
    bool found = false;
    // catch any exceptions from the look_up tile function, and return bool accordingly
    try {
        tile = tiles.lookup_tile(tile.letter);
        found = true;
    } catch (out_of_range& e) {
        return found;
    }
    return found;
}
// get the total value of the tiles in the player's hand
unsigned int Player::get_hand_value() const { return tiles.total_points(); }
// get the size of the player's hand
size_t Player::get_hand_size() const { return this->hand_size; }