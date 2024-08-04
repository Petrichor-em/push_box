#include <iostream>
#include <vector>
#include <set>

constexpr unsigned buf_width = 80;
constexpr unsigned buf_height = 30;
constexpr unsigned row_off = 5;
constexpr unsigned col_off = 5;

struct Coord {
    int row;
    int col;
    bool operator<(const Coord &rhs) const {
        if (row < rhs.row) return true;
        if (row == rhs.row && col < rhs.col) {
            return true;
        }
        return false;
    }
    Coord(int r, int c): row(r), col(c) { }
};

/*********************************
 *  # -> box
 *  * -> boundary
 *  @ -> puser
 *  $ -> target
 *********************************/
class Pusher;
class Box;
class Field {
friend void draw(const Field *field, const Pusher *pusher, std::vector<Box *> &boxes);

public:
    Field(int r, int c);
    void set_bound(std::vector<Coord> bounds);
    bool is_bound(int row, int col); 
    bool is_out_of_bound(int row, int col);
    void set_target(int row, int col);
    bool is_target(int row, int col);
private:
    int rows;
    int cols;
    std::vector<Coord> bounds;
    std::set<Coord> targets;
};

Field::Field(int r, int c): rows(r), cols(c), bounds(), targets() { }

void Field::set_bound(std::vector<Coord> b) {
    bounds = b;
}

bool Field::is_bound(int row, int col) {
   for (const auto &coord : bounds) {
       if (coord.row == row && coord.col == col) {
           return true;
       }
   }   
   return false;
}

bool Field::is_out_of_bound(int row, int col) {
    if (row < 0 || col < 0 || row >= rows || col >= cols) {
        return true;
    }
    return false;
}

void Field::set_target(int row, int col) {
   targets.emplace(row, col); 
}

bool Field::is_target(int row, int col) {
    if (targets.find(Coord(row, col)) != targets.end()) {
        return true;
    }
    return false;
}

enum Motion {
    LEFT, DOWN, UP, RIGHT,
};

class Box {
friend void draw(const Field *field, const Pusher *pusher, std::vector<Box *> &boxes);

public:
    Box(Coord start_coord);
    Box(Coord start_coord, Field *field);
    bool motion(Motion m, std::vector<Box *> &boxes); 
    int get_row() const;
    int get_col() const;
private:
    int row;
    int col;
    Field *field;
};

int Box::get_row() const { return row; }

int Box::get_col() const { return col; }

Box::Box(Coord start_coord): row(start_coord.row), col(start_coord.col), field(nullptr) { }

Box::Box(Coord start_coord, Field *f): row(start_coord.row), col(start_coord.col), field(f) { }

bool Box::motion(Motion m, std::vector<Box *> &boxes) {
    int next_row = 0, next_col = 0;
    switch (m) {
        case UP:
           next_row = row - 1; 
           next_col = col;
           break;
        case DOWN:
            next_row = row + 1;
            next_col = col;
            break;
        case LEFT:
            next_row = row;
            next_col = col - 1;
            break;
        case RIGHT:
            next_row = row;
            next_col = col + 1;
            break;
    }
    if (!field->is_out_of_bound(next_row, next_col) && !field->is_bound(next_row, next_col)) {
        for (Box *box : boxes) {
            if (box->row == next_row && box->col == next_col) {
                if (box->motion(m, boxes)) {
                    row = next_row;
                    col = next_col;
                    return true;
                }
                return false;
            }
        } 
        row = next_row;
        col = next_col;
        return true;
    }
    return false;
}

class Pusher {
friend void draw(const Field *field, const Pusher *pusher, std::vector<Box *> &boxes);

public:
    Pusher(Coord start_coord);
    Pusher(Coord start_coord, Field *f);
    bool motion(Motion m, std::vector<Box *> &boxes);
    int get_row();
    int get_col();
private:
    int row;
    int col;
    Field *field;
};

int Pusher::get_row() { return row; }

int Pusher::get_col() { return col; }

Pusher::Pusher(Coord start_coord): row(start_coord.row), col(start_coord.col) { }

Pusher::Pusher(Coord start_coord, Field *f): row(start_coord.row), col(start_coord.col), field(f) { }

bool Pusher::motion(Motion m, std::vector<Box *> &boxes) {
    int next_row = 0, next_col = 0;
    switch (m) {
        case UP:
           next_row = row - 1; 
           next_col = col;
           break;
        case DOWN:
            next_row = row + 1;
            next_col = col;
            break;
        case LEFT:
            next_row = row;
            next_col = col - 1;
            break;
        case RIGHT:
            next_row = row;
            next_col = col + 1;
            break;
    }
    if (!field->is_out_of_bound(next_row, next_col) && !field->is_bound(next_row, next_col)) {
        for (Box *box : boxes) {
            if (box->get_row() == next_row && box->get_col() == next_col) {
                if (box->motion(m, boxes)) {
                    row = next_row;
                    col = next_col;
                    return true;
                }
                return false;
            } 
        } 
        row = next_row;
        col = next_col;
        return true;
    }
    return false;
}

void draw(const Field *field, const Pusher *pusher, std::vector<Box *> &boxes) {
    system("cls");
    std::vector<std::vector<char>> frame = std::vector<std::vector<char>>(buf_height, std::vector<char>(buf_width, ' '));
    for (const auto &coord : field->bounds) {
        frame[coord.row + row_off][coord.col + col_off] = '*';
    }
    frame[pusher->row + row_off][pusher->col + col_off] = '@';
    for (const auto &target : field->targets) {
        frame[target.row + row_off][target.col + col_off] = '$';
    }
    for (Box *box : boxes) {
        frame[box->row + row_off][box->col + col_off] = '#';
    }
    for (const auto &row : frame) {
        for (const auto &elem : row) {
            std::cout << elem;
        }
        std::cout << '\n';
    }
}

bool check(Field *field, const std::vector<Box *> &boxes) {
    for (const auto box : boxes) {
        if (!field->is_target(box->get_row(), box->get_col())) {
            return false;
        }
    }
    return true;
}

int main() {
    Field *field = new Field(8, 8);
    std::vector<Coord> bounds { {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0},
                                {5, 0}, {6, 0}, {7, 0}, {0, 1}, {0, 2},
                                {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, 
                                {7, 1}, {7, 2}, {7, 3}, {7, 4}, {7, 5}, 
                                {7, 6}, {7, 7}, {1, 7}, {2, 7}, {3, 7}, 
                                {4, 7}, {5, 7}, {6, 7}, };
    field->set_bound(bounds);
    field->set_target(5, 6);
    field->set_target(2, 2);
    Box *box_1 = new Box(Coord{4, 4}, field);
    Box *box_2 = new Box(Coord{3, 3}, field);
    Pusher *pusher = new Pusher(Coord{1, 1}, field);
    std::vector<Box *> *boxes = new std::vector<Box *>{box_1, box_2};
    draw(field, pusher, *boxes);
    Motion m;
    while(1) {
        if(check(field, *boxes)) {
            std::cout << "You win\n";
            break;
        }
        char ch;
        std::cin >> ch;        
        switch (ch) {
            case 'j':
                m = DOWN; 
                break;
            case 'k':
                m = UP; 
                break;
            case 'h':
                m = LEFT; 
                break;
            case 'l':
                m = RIGHT; 
                break;
        }
        pusher->motion(m, *boxes);
        draw(field, pusher, *boxes);
//        std::cout << box->get_row() << ", " << box->get_col() << '\n';
//        std::cout << pusher->get_row() << ", " << pusher->get_col() << '\n';
    }
    return 0;
}
