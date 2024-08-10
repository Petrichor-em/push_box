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
class Cursor;

class Field {
friend void draw(const Field *field, const Pusher *pusher, std::vector<Box *> *boxes);
friend void draw(const Field *field, const Pusher *pusher, std::vector<Box *> *boxes, Cursor cursor);
public:
    Field(int r, int c);
    void set_bound(std::vector<Coord> bounds);
    bool is_bound(int row, int col); 
    bool is_out_of_bound(int row, int col);
    bool is_target(int row, int col);
    void add_target(int row, int col);
    void add_bound(int row, int col);
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

bool Field::is_target(int row, int col) {
    if (targets.find(Coord(row, col)) != targets.end()) {
        return true;
    }
    return false;
}

void Field::add_target(int row, int col) {
    targets.emplace(row, col);
}

void Field::add_bound(int row, int col) {
    bounds.emplace_back(row, col);
}

enum Motion {
    LEFT, DOWN, UP, RIGHT,
};

class Box {
friend void draw(const Field *field, const Pusher *pusher, std::vector<Box *> *boxes);
friend void draw(const Field *field, const Pusher *pusher, std::vector<Box *> *boxes, Cursor cursor);

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

void free_boxes(std::vector<Box *> *&boxes) {
    for (auto pbox : *boxes) {
        delete pbox;
    }
    delete boxes;
    boxes = nullptr;
}

class Pusher {
friend void draw(const Field *field, const Pusher *pusher, std::vector<Box *> *boxes);
friend void draw(const Field *field, const Pusher *pusher, std::vector<Box *> *boxes, Cursor cursor);

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

void draw(const Field *field, const Pusher *pusher, std::vector<Box *> *boxes) {
    system("cls");
    std::vector<std::vector<char>> frame (buf_height, std::vector<char>(buf_width, ' '));
    for (const auto &coord : field->bounds) {
        frame[coord.row + row_off][coord.col + col_off] = '*';
    }
    if (pusher) {
    frame[pusher->row + row_off][pusher->col + col_off] = '@';
    }
    for (const auto &target : field->targets) {
        frame[target.row + row_off][target.col + col_off] = '$';
    }
    if (boxes) {
        for (Box *box : *boxes) {
            frame[box->row + row_off][box->col + col_off] = '#';
        }
    }
    for (const auto &row : frame) {
        for (const auto &elem : row) {
            std::cout << elem;
        }
        std::cout << '\n';
    }
}

/*******************************************************************
 * Extension:
 *  Enable to set box, pusher and boundaries in the terminal
 *  ****************************************************************/

class Cursor {
public:
    void motion(Motion m);
    Cursor(int r, int c, int maxr, int maxc): row(r), col(c), max_rows(maxr), max_cols(maxc) { }
    Cursor(int maxr, int maxc): row(0), col(0), max_rows(maxr), max_cols(maxc) { }
    int get_row() { return row; }
    int get_col() { return col; }
    void reset() { row = 0; col = 0; }
//private:
    int row;
    int col;
    int max_rows;
    int max_cols;
};
 
void draw(const Field *field, const Pusher *pusher, std::vector<Box *> *boxes, Cursor cursor) {
    system("cls");
    std::vector<std::vector<char>> frame (buf_height, std::vector<char>(buf_width, ' '));
    if (field) {
        for (const auto &coord : field->bounds) {
            frame[coord.row + row_off][coord.col + col_off] = '*';
        }
    }
    if (pusher) {
    frame[pusher->row + row_off][pusher->col + col_off] = '@';
    }
    if (field) {
        for (const auto &target : field->targets) {
            frame[target.row + row_off][target.col + col_off] = '$';
        }
    }
    if (boxes) {
        for (Box *box : *boxes) {
            frame[box->row + row_off][box->col + col_off] = '#';
        }
    }
    frame[cursor.get_row() + row_off][cursor.get_col() + col_off] = '?';
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

void Cursor::motion(Motion m) {
    switch (m) {
        case UP:
            if (row > 0) {
                row -= 1;
            }
            break;
        case DOWN:
            if (row < max_rows - 2) {
                row += 1;
            }
            break;
        case LEFT:
            if (col > 0) {
                col -= 1;
            }
            break;
        case RIGHT:
            if (col < max_cols - 2) {
                col += 1;
            }
            break;
    }
}

// Step 1: set boundaries.
void draw_bounds(const std::vector<Coord> &bounds, Cursor cursor) {
    system("cls");
    std::vector<std::vector<char>> frame(buf_height, std::vector<char>(buf_width, ' '));
    for (const auto &coord : bounds) {
        frame[coord.row + row_off][coord.col + col_off] = '*';
    } 
    frame[cursor.get_row() + row_off][cursor.get_col() + col_off] = '?';
    for (const auto &row : frame) {
        for (const auto &elem : row) {
            std::cout << elem;
        }
        std::cout << '\n';
    }
    std::cout << cursor.get_row() << " < " << cursor.max_rows << " ?\n";
    std::cout << "frame height: " << frame.size() << "\n";
}

Field *input_field() {
    Cursor cursor(0, 0, buf_height, buf_width);
    Field *field = new Field(10, 10);
    while (1) {
        draw(field, nullptr, nullptr, cursor);                
        char ch;
        std::cin >> ch;
        if (ch == '!') {
            break;
        }
        switch (ch) {
            case 'h':
                cursor.motion(LEFT);
                break;
            case 'j':
                cursor.motion(DOWN);
                break;
            case 'k':
                cursor.motion(UP);
                break;
            case 'l':
                cursor.motion(RIGHT);
                break;
            case '*':
                field->add_bound(cursor.get_row(), cursor.get_col()); 
                break;
        }
    }
    std::vector<Coord> targets; 
    cursor.reset();
    while (1) {
        draw(field, nullptr, nullptr, cursor);                
        char ch;
        std::cin >> ch;
        if (ch == '!') {
            break;
        }
        switch (ch) {
            case 'h':
                cursor.motion(LEFT);
                break;
            case 'j':
                cursor.motion(DOWN);
                break;
            case 'k':
                cursor.motion(UP);
                break;
            case 'l':
                cursor.motion(RIGHT);
                break;
            case '$':
                field->add_target(cursor.get_row(), cursor.get_col()); 
                break;
        }
    }
    return field;
}

std::vector<Box *> *input_box(Field *field) {
    Cursor cursor(0, 0, buf_height, buf_width);
    std::vector<Box *> *boxes = new std::vector<Box *>();
    while(1) {
        draw(field, nullptr, boxes, cursor);
        char ch;
        std::cin >> ch;
        if (ch  == '!') {
            break;
        }
        switch (ch) {
            case 'h':
                cursor.motion(LEFT);
                break;
            case 'j':
                cursor.motion(DOWN);
                break;
            case 'k':
                cursor.motion(UP);
                break;
            case 'l':
                cursor.motion(RIGHT);
                break;
            case '#':
                if (!field->is_bound(cursor.get_row(), cursor.get_col())) {
                    Box *box = new Box(Coord(cursor.get_row(), cursor.get_col()), field);
                    boxes->push_back(box);
                }
                break;
        }
    }
    return boxes;
}

Pusher* input_pusher() {
    Cursor cursor(0, 0, buf_height, buf_width);
    char ch;
    while(1) {
        std::cin >> ch;
        switch (ch) {
            case 'h':
                cursor.motion(LEFT);
                break;
            case 'j':
                cursor.motion(DOWN);
                break;
            case 'k':
                cursor.motion(UP);
                break;
            case 'l':
                cursor.motion(RIGHT);
                break;
            case '@':
                Pusher *pusher = new Pusher(Coord(cursor.get_row(), cursor.get_col()));
                return pusher;
        }
    }
}

int main() {
    Field *field = input_field();
/*
    std::vector<Coord> bounds { {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0},
                                {5, 0}, {6, 0}, {7, 0}, {0, 1}, {0, 2},
                                {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, 
                                {7, 1}, {7, 2}, {7, 3}, {7, 4}, {7, 5}, 
                                {7, 6}, {7, 7}, {1, 7}, {2, 7}, {3, 7}, 
                                {4, 7}, {5, 7}, {6, 7}, };
*/
//    field->set_bound(bounds);
//    field->set_target(5, 6);
//    field->set_target(2, 2);
//    Box *box_1 = new Box(Coord{4, 4}, field);
//    Box *box_2 = new Box(Coord{3, 3}, field);
//
    std::vector<Box *> *boxes = input_box(field);
    Pusher *pusher = input_pusher();
//    std::vector<Box *> *boxes = new std::vector<Box *>{box_1, box_2};
    draw(field, pusher, boxes);
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
        draw(field, pusher, boxes);
//        std::cout << box->get_row() << ", " << box->get_col() << '\n';
//        std::cout << pusher->get_row() << ", " << pusher->get_col() << '\n';
    }
    free_boxes(boxes);
    delete field;
    field = nullptr;
    delete pusher;
    pusher = nullptr;
    return 0;
}
