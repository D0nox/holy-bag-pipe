#include "MyLibraries.h"

uint64_t bishop_masks[64];
uint64_t rook_masks[64];
uint64_t bishop_attacks[64][512];
uint64_t rook_attacks[64][4096];

unsigned int state = 1804289383;

chessboard initialize_chessboard(std::string FenCode, bool& White, int& time) {
    chessboard board;
    int g = 0, i = 0, j = 0;
    char temp;
    while (FenCode[g] != ' ') {//main board
        temp = FenCode[g];
        if (FenCode[g] == '/') { i++; j = 0; }
        else if (temp < '9') {
            j += temp - '0';
        }
        else {
            if (temp == 'R')set_bit(board.white.rooks, 63 - i * 8 - j);
            else if (temp == 'N')set_bit(board.white.knights, 63 - i * 8 - j);
            else if (temp == 'Q')set_bit(board.white.queen, 63 - i * 8 - j);
            else if (temp == 'B')set_bit(board.white.bishops, 63 - i * 8 - j);
            else if (temp == 'K')set_bit(board.white.king, 63 - i * 8 - j);
            else if (temp == 'P')set_bit(board.white.pawns, 63 - i * 8 - j);
            else if (temp == 'r')set_bit(board.black.rooks, 63 - i * 8 - j);
            else if (temp == 'n')set_bit(board.black.knights, 63 - i * 8 - j);
            else if (temp == 'b')set_bit(board.black.bishops, 63 - i * 8 - j);
            else if (temp == 'k')set_bit(board.black.king, 63 - i * 8 - j);
            else if (temp == 'p')set_bit(board.black.pawns, 63 - i * 8 - j);
            else if (temp == 'q')set_bit(board.black.queen, 63 - i * 8 - j);
            j++;
        }
        g++;
    }
    if (FenCode[++g] == 'w')White = true;//kas eina
    else White = false;
    g += 2;
    while (FenCode[g] != ' ') {//castling
        if (FenCode[g] == '-') {
            g++;
            break;
        }
        else if (FenCode[g] == 'K')board.castling += 4;
        else if (FenCode[g] == 'Q')board.castling += 8;
        else if (FenCode[g] == 'k')board.castling += 1;
        else if (FenCode[g] == 'q')board.castling += 2;
        g++;
    }
    g++;
    if (FenCode[g] != '-') {//a peasant
        board.en_passant = (7 - int(FenCode[g] - 'a')) + (int(FenCode[g + 1] - '1') * 8);
        if (!White && ((get_bit(board.black.pawns, board.en_passant + 8 - 1) && board.en_passant > h3) || (get_bit(board.black.pawns, board.en_passant + 8 + 1) && board.en_passant < a3)))board.keep_en_passant = true;
        else if (White && ((get_bit(board.white.pawns, board.en_passant - 8 - 1) && board.en_passant > h6) || (get_bit(board.white.pawns, board.en_passant - 8 + 1) && board.en_passant < a6)))board.keep_en_passant = true;
        else board.keep_en_passant = false;
        g += 3;
    }
    else g += 2;
    while (FenCode[g] != ' ' && g < FenCode.length() - 1)g++; g++;
    while (FenCode[g] != ' ' && g < FenCode.length() - 1)g++; g++;
    if (g < FenCode.length() - 1) {
        g++;
        std::string timeString;
        if (White) while (FenCode[g] != ' ')
        {
            timeString += FenCode[g];
            g++;
        }
        else {
            while (FenCode[g] != ' ')g++;
            g++;
            while (g < FenCode.length())
            {
                timeString += FenCode[g];
                g++;
            }
        }
        time = stoi(timeString);
    }
    generate_hash_for_pos(board);
    return board;
}

bool MOVES_EMPTY(MOVES& moves) {
    return moves.move_c == 0 && moves.capture_c == 0;
}
int MOVE_COUNT(MOVES& moves) {
    return moves.move_c + moves.capture_c;
}
unsigned int generate_random_number()
{
    // XOR shift algorithm
    unsigned int x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state = x;
    return x;
}
uint64_t random_U64()
{
    // init numbers to randomize
    uint64_t u1, u2, u3, u4;

    // randomize numbers
    u1 = (uint64_t)(generate_random_number()) & 0xFFFF;
    u2 = (uint64_t)(generate_random_number()) & 0xFFFF;
    u3 = (uint64_t)(generate_random_number()) & 0xFFFF;
    u4 = (uint64_t)(generate_random_number()) & 0xFFFF;

    // shuffle bits and return
    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}
uint64_t random_fewbits() {
    return random_U64() & random_U64() & random_U64();
}
int Count_bits(uint64_t bitboard) {
    //Hamming weight algorithm or something
    bitboard = bitboard - ((bitboard >> 1) & 0x5555555555555555ULL);
    bitboard = (bitboard & 0x3333333333333333ULL) + ((bitboard >> 2) & 0x3333333333333333ULL);
    bitboard = (bitboard + (bitboard >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
    bitboard = bitboard + (bitboard >> 8);
    bitboard = bitboard + (bitboard >> 16);
    bitboard = bitboard + (bitboard >> 32);
    return static_cast<int>(bitboard & 0x7f);
}

int get_ls1b_index(unsigned long long bitboard) {
    // make sure bitboard is not empty
    if (bitboard != 0)
        // convert trailing zeros before LS1B to ones and count them
        return Count_bits((bitboard & (-1 * bitboard)) - 1);

    // otherwise
    else
        // return illegal index
        return -1;
}
uint64_t set_occupancy(int index, int bits_in_mask, uint64_t attack_mask)
{
    // occupancy map
    uint64_t occupancy = 0ULL;

    // loop over the range of bits within attack mask
    for (int count = 0; count < bits_in_mask; count++)
    {
        // get LS1B index of attacks mask
        int square = get_ls1b_index(attack_mask);

        // pop LS1B in attack map
        pop_bit(attack_mask, square);

        // make sure occupancy is on board
        if (index & (1 << count))
            // populate occupancy map
            occupancy |= (1ULL << square);
    }

    // return occupancy map
    return occupancy;
}
uint64_t mask_bishop_attacks(int square)
{
    // attack bitboard
    uint64_t attacks = 0ULL;

    // init files & ranks
    int f, r;

    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;

    // generate attacks
    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));

    // return attack map for bishop on a given square
    return attacks;
}
uint64_t mask_rook_attacks(int square)
{
    // attacks bitboard
    uint64_t attacks = 0ULL;

    // init files & ranks
    int f, r;

    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;

    // generate attacks
    for (r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf));
    for (r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
    for (f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
    for (f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));

    // return attack map for bishop on a given square
    return attacks;
}
uint64_t bishop_attacks_on_the_fly(int square, uint64_t block)
{
    // attack bitboard
    uint64_t attacks = 0ULL;

    // init files & ranks
    int f, r;

    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;

    // generate attacks
    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }

    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }

    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }

    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }

    // return attack map for bishop on a given square
    return attacks;
}
uint64_t rook_attacks_on_the_fly(int square, uint64_t block)
{
    // attacks bitboard
    uint64_t attacks = 0ULL;

    // init files & ranks
    int f, r;

    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;

    // generate attacks
    for (r = tr + 1; r <= 7; r++)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if (block & (1ULL << (r * 8 + tf))) break;
    }

    for (r = tr - 1; r >= 0; r--)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if (block & (1ULL << (r * 8 + tf))) break;
    }

    for (f = tf + 1; f <= 7; f++)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if (block & (1ULL << (tr * 8 + f))) break;
    }

    for (f = tf - 1; f >= 0; f--)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if (block & (1ULL << (tr * 8 + f))) break;
    }

    // return attack map for bishop on a given square
    return attacks;
}
uint64_t find_magic(int square, int relevant_bits, int bishop) {
    // define occupancies array
    uint64_t occupancies[4096];

    // define attacks array
    uint64_t attacks[4096];

    // define used indicies array
    uint64_t used_attacks[4096];

    // mask piece attack
    uint64_t mask_attack = bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);

    // occupancy variations
    int occupancy_variations = 1 << relevant_bits;

    // loop over the number of occupancy variations
    for (int count = 0; count < occupancy_variations; count++) {
        // init occupancies
        occupancies[count] = set_occupancy(count, relevant_bits, mask_attack);

        // init attacks
        attacks[count] = bishop ? bishop_attacks_on_the_fly(square, occupancies[count]) :
            rook_attacks_on_the_fly(square, occupancies[count]);
    }

    // test magic numbers
    for (int random_count = 0; random_count < 100000000; random_count++)
    {
        // init magic number candidate
        uint64_t magic = random_fewbits();

        // skip testing magic number if inappropriate
        if (Count_bits((mask_attack * magic) & 0xFF00000000000000ULL) < 6) continue;

        // reset used attacks array
        memset(used_attacks, 0ULL, sizeof(used_attacks));

        // init count & fail flag
        int count, fail;

        // test magic index
        for (count = 0, fail = 0; !fail && count < occupancy_variations; count++) {
            // generate magic index
            int magic_index = (int)((occupancies[count] * magic) >> (64 - relevant_bits));

            // if got free index
            if (used_attacks[magic_index] == 0ULL)
                // assign corresponding attack map
                used_attacks[magic_index] = attacks[count];

            // otherwise fail on  collision
            else if (used_attacks[magic_index] != attacks[count]) fail = 1;
        }

        // return magic if it works
        if (!fail) return magic;
    }

    // on fail
    printf("***Failed***\n");
    return 0ULL;
}
void init_magics()
{
    printf("const U64 rook_magics[64] = {\n");

    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
        printf("    0x%llxULL,\n", find_magic(square, rook_rellevant_bits[square], 0));

    printf("};\n\nconst U64 bishop_magics[64] = {\n");

    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
        printf("    0x%llxULL,\n", find_magic(square, bishop_rellevant_bits[square], 1));

    printf("};\n\n");
}
void generate_knight_attacks() {
    uint64_t knight_attacks[64];
    int ij, temp1, temp2;
    printf("const U64 knight_magics[64] = {\n");
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            knight_attacks[(ij = i * 8 + j)] = 0;
            if ((temp1 = i - 2) > -1) {
                if ((temp2 = j - 1) > -1)
                    set_bit(knight_attacks[ij], temp1 * 8 + temp2);
                if ((temp2 = j + 1) < 8)
                    set_bit(knight_attacks[ij], temp1 * 8 + temp2);
            }
            if ((temp1 = i + 2) < 8) {
                if ((temp2 = j - 1) > -1)
                    set_bit(knight_attacks[ij], temp1 * 8 + temp2);
                if ((temp2 = j + 1) < 8)
                    set_bit(knight_attacks[ij], temp1 * 8 + temp2);
            }
            if ((temp1 = j - 2) > -1) {
                if ((temp2 = i - 1) > -1)
                    set_bit(knight_attacks[ij], temp2 * 8 + temp1);
                if ((temp2 = i + 1) < 8)
                    set_bit(knight_attacks[ij], temp2 * 8 + temp1);
            }
            if ((temp1 = j + 2) < 8) {
                if ((temp2 = i - 1) > -1)
                    set_bit(knight_attacks[ij], temp2 * 8 + temp1);
                if ((temp2 = i + 1) < 8)
                    set_bit(knight_attacks[ij], temp2 * 8 + temp1);
            }
            printf("\t0x%llxULL,\n", knight_attacks[ij]);
        }
    }
    printf("};\n\n");
}
void generate_king_attacks() {
    uint64_t king_attacks[64];
    int ij, temp1, temp2;
    printf("const U64 king_magics[64] = {\n");
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            king_attacks[(ij = i * 8 + j)] = 0;
            if ((temp1 = i - 1) > -1) {
                set_bit(king_attacks[ij], temp1 * 8 + j);
                if ((temp2 = j - 1) > -1)
                    set_bit(king_attacks[ij], temp1 * 8 + temp2);
                if ((temp2 = j + 1) < 8)
                    set_bit(king_attacks[ij], temp1 * 8 + temp2);
            }
            if ((temp1 = i + 1) < 8) {
                set_bit(king_attacks[ij], temp1 * 8 + j);
                if ((temp2 = j - 1) > -1)
                    set_bit(king_attacks[ij], temp1 * 8 + temp2);
                if ((temp2 = j + 1) < 8)
                    set_bit(king_attacks[ij], temp1 * 8 + temp2);
            }
            if ((temp1 = j - 1) > -1) {
                set_bit(king_attacks[ij], i * 8 + temp1);
                if ((temp2 = i - 1) > -1)
                    set_bit(king_attacks[ij], temp2 * 8 + temp1);
                if ((temp2 = i + 1) < 8)
                    set_bit(king_attacks[ij], temp2 * 8 + temp1);
            }
            if ((temp1 = j + 1) < 8) {
                set_bit(king_attacks[ij], i * 8 + temp1);
                if ((temp2 = i - 1) > -1)
                    set_bit(king_attacks[ij], temp2 * 8 + temp1);
                if ((temp2 = i + 1) < 8)
                    set_bit(king_attacks[ij], temp2 * 8 + temp1);
            }
            printf("\t0x%llxULL,\n", king_attacks[ij]);
        }
    }
    printf("};\n\n");
}
void init_sliders_attacks(int is_bishop)
{
    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
    {
        // init bishop & rook masks
        bishop_masks[square] = mask_bishop_attacks(square);
        rook_masks[square] = mask_rook_attacks(square);

        // init current mask
        uint64_t mask = is_bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);

        // count attack mask bits
        int bit_count = Count_bits(mask);

        // occupancy variations count
        int occupancy_variations = 1 << bit_count;

        // loop over occupancy variations
        for (int count = 0; count < occupancy_variations; count++)
        {
            // bishop
            if (is_bishop)
            {
                // init occupancies, magic index & attacks
                uint64_t occupancy = set_occupancy(count, bit_count, mask);
                uint64_t magic_index = occupancy * bishop_magics[square] >> 64 - bishop_rellevant_bits[square];
                bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
            }

            // rook
            else
            {
                // init occupancies, magic index & attacks
                uint64_t occupancy = set_occupancy(count, bit_count, mask);
                uint64_t magic_index = occupancy * rook_magics[square] >> 64 - rook_rellevant_bits[square];
                rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);
            }
        }
    }
}
uint64_t get_bishop_attacks(int square, uint64_t occupancy) {
    // calculate magic index
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magics[square];
    occupancy >>= 64 - bishop_rellevant_bits[square];

    // return rellevant attacks
    return bishop_attacks[square][occupancy];

}
uint64_t get_rook_attacks(int square, uint64_t occupancy) {

    // calculate magic index
    occupancy &= rook_masks[square];
    occupancy *= rook_magics[square];
    occupancy >>= 64 - rook_rellevant_bits[square];

    // return rellevant attacks
    return rook_attacks[square][occupancy];
}

void restore_opponents_piece(chessboard& BOARD, int square, bool white, int what_to_restore) {
    if (white) {
        if (what_to_restore == p)set_bit(BOARD.black.pawns, square);
        else if (what_to_restore == r)set_bit(BOARD.black.rooks, square);
        else if (what_to_restore == q)set_bit(BOARD.black.queen, square);
        else if (what_to_restore == b)set_bit(BOARD.black.bishops, square);
        else if (what_to_restore == n)set_bit(BOARD.black.knights, square);
        else if (what_to_restore == rA) { set_bit(BOARD.black.rooks, square); BOARD.castling ^= 2; }
        else if (what_to_restore == rH) { set_bit(BOARD.black.rooks, square); BOARD.castling ^= 1; }
    }
    else {
        if (what_to_restore == p)set_bit(BOARD.white.pawns, square);
        else if (what_to_restore == r)set_bit(BOARD.white.rooks, square);
        else if (what_to_restore == q)set_bit(BOARD.white.queen, square);
        else if (what_to_restore == b)set_bit(BOARD.white.bishops, square);
        else if (what_to_restore == n)set_bit(BOARD.white.knights, square);
        else if (what_to_restore == rA) { set_bit(BOARD.white.rooks, square); BOARD.castling ^= 8; }
        else if (what_to_restore == rH) { set_bit(BOARD.white.rooks, square); BOARD.castling ^= 4; }
    }
}
void delete_opponents_piece(chessboard& BOARD, int square, bool white, int& what_to_restore) {
    if (white) {
        if (get_bit(BOARD.black.bishops, square) != 0) { what_to_restore = b; pop_bit(BOARD.black.bishops, square); }
        else if (get_bit(BOARD.black.pawns, square) != 0) { what_to_restore = p; pop_bit(BOARD.black.pawns, square); }
        else if (get_bit(BOARD.black.knights, square) != 0) { what_to_restore = n; pop_bit(BOARD.black.knights, square); }
        else if (get_bit(BOARD.black.queen, square) != 0) { what_to_restore = q; pop_bit(BOARD.black.queen, square); }
        else if (get_bit(BOARD.black.rooks, square) != 0) {
            pop_bit(BOARD.black.rooks, square);

            if (square == a8 && (BOARD.castling & 2) != 0) { BOARD.castling ^= 2; what_to_restore = rA; }
            else if (square == h8 && (BOARD.castling & 1) != 0) { BOARD.castling ^= 1; what_to_restore = rH; }
            else what_to_restore = r;
        }
    }
    else {
        if (get_bit(BOARD.white.bishops, square) != 0) { what_to_restore = b; pop_bit(BOARD.white.bishops, square); }
        else if (get_bit(BOARD.white.knights, square) != 0) { what_to_restore = n; pop_bit(BOARD.white.knights, square); }
        else if (get_bit(BOARD.white.queen, square) != 0) { what_to_restore = q; pop_bit(BOARD.white.queen, square); }
        else if (get_bit(BOARD.white.pawns, square) != 0) { what_to_restore = p; pop_bit(BOARD.white.pawns, square); }
        else if (get_bit(BOARD.white.rooks, square) != 0) {
            pop_bit(BOARD.white.rooks, square);

            if (square == a1 && (BOARD.castling & 8) != 0) { BOARD.castling ^= 8; what_to_restore = rA; }
            else if (square == h1 && (BOARD.castling & 4) != 0) { BOARD.castling ^= 4; what_to_restore = rH; }
            else what_to_restore = r;
        }
    }
}
void delete_piece(chessboard& BOARD, int square, bool white) {
    if (!white) {
        if (get_bit(BOARD.black.bishops, square) != 0) { pop_bit(BOARD.black.bishops, square); }
        else if (get_bit(BOARD.black.pawns, square) != 0) { pop_bit(BOARD.black.pawns, square); }
        else if (get_bit(BOARD.black.knights, square) != 0) { pop_bit(BOARD.black.knights, square); }
        else if (get_bit(BOARD.black.queen, square) != 0) { pop_bit(BOARD.black.queen, square); }
        else if (get_bit(BOARD.black.rooks, square) != 0) { pop_bit(BOARD.black.rooks, square); }
    }
    else {
        if (get_bit(BOARD.white.bishops, square) != 0) { pop_bit(BOARD.white.bishops, square); }
        else if (get_bit(BOARD.white.knights, square) != 0) { pop_bit(BOARD.white.knights, square); }
        else if (get_bit(BOARD.white.queen, square) != 0) { pop_bit(BOARD.white.queen, square); }
        else if (get_bit(BOARD.white.pawns, square) != 0) { pop_bit(BOARD.white.pawns, square); }
        else if (get_bit(BOARD.white.rooks, square) != 0) { pop_bit(BOARD.white.rooks, square); }
    }
}

uint64_t& getPawn(chessboard& BOARD, bool white) {
    if (white)return BOARD.white.pawns;
    else return BOARD.black.pawns;
}
uint64_t& getBishop(chessboard& BOARD, bool white) {
    if (white)return BOARD.white.bishops;
    else return BOARD.black.bishops;
}
uint64_t& getKnight(chessboard& BOARD, bool white) {
    if (white)return BOARD.white.knights;
    else return BOARD.black.knights;
}
uint64_t& getRook(chessboard& BOARD, bool white) {
    if (white)return BOARD.white.rooks;
    else return BOARD.black.rooks;
}
uint64_t& getKing(chessboard& BOARD, bool white) {
    if (white)return BOARD.white.king;
    else return BOARD.black.king;
}
uint64_t& getQueen(chessboard& BOARD, bool white) {
    if (white)return BOARD.white.queen;
    else return BOARD.black.queen;
}
bitboards& getColour(chessboard& BOARD, bool white) {
    if (white)return BOARD.white;
    else return BOARD.black;
}

uint64_t& moveAnalyseOrder(chessboard& BOARD, bool White, int groupNum) {
    if (groupNum == 0)return getPawn(BOARD, White);
    else if (groupNum == 1)return getKnight(BOARD, White);
    else if (groupNum == 2)return getBishop(BOARD, White);
    else if (groupNum == 3)return getQueen(BOARD, White);
    else if (groupNum == 4)return getRook(BOARD, White);
    else return getKing(BOARD, White);
}

void read_precomputed_moves(std::unordered_map<std::string, std::string>& precomputed_moves, std::string file_name) {
    std::ifstream input_file(file_name);
    if (input_file.is_open()) {
        std::string line;

        while (std::getline(input_file, line)) {
            if (line[0] != '#' && line != "") {
                std::vector<std::string> split_line = split(line, ';');
                precomputed_moves[split_line[0]] = split_line[1];
            }
        }

        input_file.close();
    }
    else {
        std::cout << "ZE \"" << file_name << "\" FILE is MISSING\n";
    }
}
std::vector<std::string> split(const std::string line, char split_symbol) {
    int i = 0;
    std::vector<std::string> words;
    std::string word = "";
    while (i != line.length()) {
        if (line[i] != split_symbol)
            word += line[i];
        else if (word != "") { words.push_back(word); word = ""; }
        i++;
    }
    if (word[word.length() - 1] != split_symbol && word != "")words.push_back(word);
    return words;
}
bool are_chessboards_equal(chessboard a, chessboard b) {
    return a.white.bishops == b.white.bishops && a.white.pawns == b.white.pawns && a.white.knights == b.white.knights && a.white.king == b.white.king &&
        a.white.queen == b.white.queen && a.white.rooks == b.white.rooks && a.castling == b.castling &&
        a.black.bishops == b.black.bishops && a.black.pawns == b.black.pawns && a.black.knights == b.black.knights && a.black.king == b.black.king &&
        a.black.queen == b.black.queen && a.black.rooks == b.black.rooks && a.hash == b.hash;
}
bool are_chessboards_equal1(chessboard a, chessboard b) {
    return a.en_passant == b.en_passant && a.white.bishops == b.white.bishops && a.white.pawns == b.white.pawns && a.white.knights == b.white.knights && a.white.king == b.white.king &&
        a.white.queen == b.white.queen && a.white.rooks == b.white.rooks && a.castling == b.castling && a.keep_en_passant == b.keep_en_passant &&
        a.black.bishops == b.black.bishops && a.black.pawns == b.black.pawns && a.black.knights == b.black.knights && a.black.king == b.black.king &&
        a.black.queen == b.black.queen && a.black.rooks == b.black.rooks && a.hash == b.hash;
}


uint64_t get_pawn_moves_and_attacks(uint64_t occupancy, int square, bool is_white, uint64_t opponent_occupancy) {
    uint64_t pawn_moves = 0, pawn = 0;
    set_bit(pawn, square);
    if (is_white) {
        pawn_moves = (~occupancy & (pawn << BOARD_WIDTH));//move one forward
        pawn_moves |= ((pawn_moves & (PAWNS << BOARD_WIDTH)) << BOARD_WIDTH) & ~occupancy;//move 2 forward
        pawn_moves |= ((pawn & ~trim_left) << (BOARD_WIDTH + 1)) & opponent_occupancy;
        pawn_moves |= ((pawn & ~trim_right) << (BOARD_WIDTH - 1)) & opponent_occupancy;
    }
    else {//reverse
        pawn_moves = (~occupancy & (pawn >> BOARD_WIDTH));
        pawn_moves |= ((pawn_moves & (PAWNS << (4 * BOARD_WIDTH))) >> BOARD_WIDTH) & ~occupancy;
        pawn_moves |= ((pawn & ~trim_right) >> (BOARD_WIDTH + 1)) & opponent_occupancy;
        pawn_moves |= ((pawn & ~trim_left) >> (BOARD_WIDTH - 1)) & opponent_occupancy;
    }
    return pawn_moves;
}
uint64_t getPawnMoves(uint64_t occupancy, int square, bool is_white) {
    uint64_t pawn_moves = 0, pawn = 0;
    set_bit(pawn, square);
    if (is_white) {
        pawn_moves = (~occupancy & (pawn << BOARD_WIDTH));
        pawn_moves |= ((pawn_moves & (PAWNS << BOARD_WIDTH)) << BOARD_WIDTH) & ~occupancy;
    }
    else {//reverse
        pawn_moves = (~occupancy & (pawn >> BOARD_WIDTH));
        pawn_moves |= ((pawn_moves & (PAWNS << (4 * BOARD_WIDTH))) >> BOARD_WIDTH) & ~occupancy;
    }
    return pawn_moves;
}
uint64_t get_pawn_attacks(int square, bool white) {
    uint64_t pawn_attacks = 0, temp = 0;
    set_bit(temp, square);
    if (white) {
        pawn_attacks |= (temp & ~trim_right) << 7;
        pawn_attacks |= (temp & ~trim_left) << 9;
    }
    else {
        pawn_attacks |= (temp & ~trim_right) >> 7;
        pawn_attacks |= (temp & ~trim_left) >> 9;
    }
    return pawn_attacks;
}
uint64_t get_all_pawn_attacks(uint64_t pawns, bool is_white) {
    uint64_t pawn_attacks = 0;
    if (is_white) {
        pawn_attacks = ((pawns & ~trim_left) << BOARD_WIDTH + 1);
        pawn_attacks |= ((pawns & ~trim_right) << BOARD_WIDTH - 1);
    }
    else {//reverse
        pawn_attacks = ((pawns & ~trim_right) >> BOARD_WIDTH + 1);
        pawn_attacks |= ((pawns & ~trim_left) >> BOARD_WIDTH - 1);
    }
    return pawn_attacks;
}
uint64_t get_all_pawn_moves(uint64_t pawns, uint64_t occupancy, bool white) {
    uint64_t pawn_moves = 0;
    if (white) {
        pawn_moves = (~occupancy & (pawns << BOARD_WIDTH));//move one forward
        pawn_moves |= ((pawn_moves & (PAWNS << BOARD_WIDTH)) << BOARD_WIDTH) & ~occupancy;//move 2 forward
    }
    else {//reverse
        pawn_moves = (~occupancy & (pawns >> BOARD_WIDTH));
        pawn_moves |= ((pawn_moves & (PAWNS << (4 * BOARD_WIDTH))) >> BOARD_WIDTH) & ~occupancy;
    }
    return pawn_moves;
}
uint64_t get_knight_attacks(int square) {
    return knight_magics[square];
}
uint64_t get_king_attacks(int square) {
    return king_magics[square];
}
uint64_t get_occupancy(bitboards a) {
    return a.pawns | a.knights | a.bishops | a.king | a.queen | a.rooks;
}
uint64_t get_occupancy(chessboard a) {
    return get_occupancy(a.black) | get_occupancy(a.white);
}

void move_piece(bitboards& a, MOVE move) {
    if (get_bit(a.pawns, move.from)) {
        pop_bit(a.pawns, move.from);
        set_bit(a.pawns, move.to);
    }
    else if (get_bit(a.knights, move.from)) {
        pop_bit(a.knights, move.from);
        set_bit(a.knights, move.to);
    }
    else if (get_bit(a.bishops, move.from)) {
        pop_bit(a.bishops, move.from);
        set_bit(a.bishops, move.to);
    }
    else if (get_bit(a.queen, move.from)) {
        pop_bit(a.queen, move.from);
        set_bit(a.queen, move.to);
    }
    else if (get_bit(a.rooks, move.from)) {
        pop_bit(a.rooks, move.from);
        set_bit(a.rooks, move.to);
    }
    else if (get_bit(a.king, move.from)) {
        pop_bit(a.king, move.from);
        set_bit(a.king, move.to);
    }
}

uint64_t get_queen_attacks(int square, uint64_t occupancy) {
    return get_rook_attacks(square, occupancy) | get_bishop_attacks(square, occupancy);
}

unsigned int find_first_set_bit(uint64_t x)
{
    static const int debruijn64[64] = {
        0, 1, 48, 2, 57, 49, 28, 3, 61, 58, 50, 42, 38, 29, 17, 4,
        62, 55, 59, 36, 53, 51, 43, 22, 45, 39, 33, 30, 24, 18, 12, 5,
        63, 47, 56, 27, 60, 41, 37, 16, 54, 35, 52, 21, 44, 32, 23, 11,
        46, 26, 40, 15, 34, 20, 31, 10, 25, 14, 19, 9, 13, 8, 7, 6
    };
    return debruijn64[((uint64_t)((x & (-1 * x)) * 0x03f79d71b4cb0a89)) >> 58];
}

void generateCapturesAndProm(chessboard& BOARD, bool White, MOVE captures[], int& capture_c)
{
    uint64_t white_occupancy = get_occupancy(BOARD.white), all_occupancy = get_occupancy(BOARD),
        black_occupancy = all_occupancy ^ white_occupancy, temp_move, temp_capture,
        temp_white_occupancy = white_occupancy, temp_black_occupancy = black_occupancy;
    int square = 0, move_to_square, restore_to;
    if (White) {
        for (int i = 0; i < 6; i++) {
            temp_white_occupancy = moveAnalyseOrder(BOARD, White, i);
            while (temp_white_occupancy != 0) {
                square = find_first_set_bit(temp_white_occupancy);
                if (i == 0 && (((temp_move = get_pawn_moves_and_attacks(all_occupancy, square, White, black_occupancy)) & black_occupancy) != 0 || BOARD.en_passant != 0 || (temp_move != 0 && square > a6))) {
                    pop_bit(BOARD.white.pawns, square);
                    if (square > a6) {
                        temp_capture = temp_move & black_occupancy;
                        temp_move ^= temp_capture;
                        while (temp_capture != 0) {
                            set_bit(BOARD.white.queen, (move_to_square = find_first_set_bit(temp_capture)));
                            delete_opponents_piece(BOARD, move_to_square, White, restore_to);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square + 8;
                            }
                            pop_bit(BOARD.white.queen, move_to_square);
                            /*set_bit(BOARD.white.knights, move_to_square);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square + 16;
                            }
                            pop_bit(BOARD.white.knights, move_to_square);
                            set_bit(BOARD.white.bishops, move_to_square);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square + 24;
                            }
                            pop_bit(BOARD.white.bishops, move_to_square);
                            set_bit(BOARD.white.rooks, move_to_square);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square + 32;
                            }
                            pop_bit(BOARD.white.rooks, move_to_square);*/
                            restore_opponents_piece(BOARD, move_to_square, White, restore_to);
                            pop_bit(temp_capture, move_to_square);
                        }
                        if (temp_move != 0) {
                            set_bit(BOARD.white.queen, (move_to_square = square + BOARD_WIDTH));
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square + 8;
                            }
                            pop_bit(BOARD.white.queen, move_to_square);
                            /*set_bit(BOARD.white.knights, move_to_square);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square + 16;
                            }
                            pop_bit(BOARD.white.knights, move_to_square);
                            set_bit(BOARD.white.bishops, move_to_square);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square + 24;
                            }
                            pop_bit(BOARD.white.bishops, move_to_square);
                            set_bit(BOARD.white.rooks, move_to_square);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square + 32;
                            }
                            pop_bit(BOARD.white.rooks, move_to_square);*/
                            pop_bit(temp_capture, move_to_square);
                        }
                    }
                    else {
                        temp_capture = temp_move & black_occupancy;
                        if (BOARD.en_passant != 0 && ((BOARD.en_passant - 8 + 1 == square) && BOARD.en_passant < a6) || (BOARD.en_passant - 8 - 1 == square && BOARD.en_passant > h6)) {
                            set_bit(BOARD.white.pawns, BOARD.en_passant);
                            pop_bit(BOARD.black.pawns, BOARD.en_passant - 8);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = BOARD.en_passant;
                            }
                            set_bit(BOARD.black.pawns, BOARD.en_passant - 8);
                            pop_bit(BOARD.white.pawns, BOARD.en_passant);
                        }
                        while (temp_capture != 0) {
                            set_bit(BOARD.white.pawns, (move_to_square = find_first_set_bit(temp_capture)));
                            delete_opponents_piece(BOARD, move_to_square, White, restore_to);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square;
                            }
                            restore_opponents_piece(BOARD, move_to_square, White, restore_to);
                            pop_bit(BOARD.white.pawns, move_to_square);
                            pop_bit(temp_capture, move_to_square);
                        }
                    }
                    set_bit(BOARD.white.pawns, square);
                }
                else if (i == 1 && ((temp_capture = (get_knight_attacks(square) & ~white_occupancy & black_occupancy)) != 0)) {
                    pop_bit(BOARD.white.knights, square);
                    while (temp_capture != 0) {
                        set_bit(BOARD.white.knights, (move_to_square = find_first_set_bit(temp_capture)));
                        delete_opponents_piece(BOARD, move_to_square, White, restore_to);
                        if (!king_in_check(BOARD, White)) {
                            captures[capture_c].from = square;
                            captures[capture_c++].to = move_to_square;
                        }
                        restore_opponents_piece(BOARD, move_to_square, White, restore_to);
                        pop_bit(BOARD.white.knights, move_to_square);
                        pop_bit(temp_capture, move_to_square);
                    }
                    set_bit(BOARD.white.knights, square);
                }
                else if (i == 2 && ((temp_capture = (get_bishop_attacks(square, all_occupancy) & ~white_occupancy & black_occupancy)) != 0)) {
                    pop_bit(BOARD.white.bishops, square);
                    while (temp_capture != 0) {
                        set_bit(BOARD.white.bishops, (move_to_square = find_first_set_bit(temp_capture)));
                        delete_opponents_piece(BOARD, move_to_square, White, restore_to);
                        if (!king_in_check(BOARD, White)) {
                            captures[capture_c].from = square;
                            captures[capture_c++].to = move_to_square;
                        }
                        restore_opponents_piece(BOARD, move_to_square, White, restore_to);
                        pop_bit(BOARD.white.bishops, move_to_square);
                        pop_bit(temp_capture, move_to_square);
                    }
                    set_bit(BOARD.white.bishops, square);
                }
                else if (i == 3 && ((temp_capture = (get_queen_attacks(square, all_occupancy) & ~white_occupancy & black_occupancy)) != 0)) {
                    pop_bit(BOARD.white.queen, square);
                    while (temp_capture != 0) {
                        set_bit(BOARD.white.queen, (move_to_square = find_first_set_bit(temp_capture)));
                        delete_opponents_piece(BOARD, move_to_square, White, restore_to);
                        if (!king_in_check(BOARD, White)) {
                            captures[capture_c].from = square;
                            captures[capture_c++].to = move_to_square;
                        }
                        restore_opponents_piece(BOARD, move_to_square, White, restore_to);
                        pop_bit(BOARD.white.queen, move_to_square);
                        pop_bit(temp_capture, move_to_square);
                    }
                    set_bit(BOARD.white.queen, square);
                }
                else if (i == 4 && ((temp_capture = (get_rook_attacks(square, all_occupancy) & ~white_occupancy & black_occupancy)) != 0)) {
                    pop_bit(BOARD.white.rooks, square);
                    while (temp_capture != 0) {
                        set_bit(BOARD.white.rooks, (move_to_square = find_first_set_bit(temp_capture)));
                        delete_opponents_piece(BOARD, move_to_square, White, restore_to);
                        if (!king_in_check(BOARD, White)) {
                            captures[capture_c].from = square;
                            captures[capture_c++].to = move_to_square;
                        }
                        restore_opponents_piece(BOARD, move_to_square, White, restore_to);
                        pop_bit(BOARD.white.rooks, move_to_square);
                        pop_bit(temp_capture, move_to_square);
                    }
                    set_bit(BOARD.white.rooks, square);
                }
                else if (i == 5 && ((temp_capture = (get_king_attacks(square) & ~white_occupancy & black_occupancy)) != 0)) {
                    pop_bit(BOARD.white.king, square);
                    while (temp_capture != 0) {
                        set_bit(BOARD.white.king, (move_to_square = find_first_set_bit(temp_capture)));
                        delete_opponents_piece(BOARD, move_to_square, White, restore_to);
                        if (!king_in_check(BOARD, White)) {
                            captures[capture_c].from = square;
                            captures[capture_c++].to = move_to_square;
                        }
                        restore_opponents_piece(BOARD, move_to_square, White, restore_to);
                        pop_bit(BOARD.white.king, move_to_square);
                        pop_bit(temp_capture, move_to_square);
                    }
                    set_bit(BOARD.white.king, square);
                }
                pop_bit(temp_white_occupancy, square);
            }
        }
    }
    else {
        for (int i = 0; i < 6; i++) {
            temp_black_occupancy = moveAnalyseOrder(BOARD, White, i);
            while (temp_black_occupancy != 0) {
                square = find_first_set_bit(temp_black_occupancy);
                if (i == 0 && (((temp_move = get_pawn_moves_and_attacks(all_occupancy, square, White, white_occupancy)) & white_occupancy) != 0 || BOARD.en_passant != 0 || (temp_move != 0 && square < h3))) {
                    pop_bit(BOARD.black.pawns, square);
                    if (square < h3) {
                        temp_capture = temp_move & white_occupancy;
                        temp_move ^= temp_capture;
                        while (temp_capture != 0) {
                            set_bit(BOARD.black.queen, (move_to_square = find_first_set_bit(temp_capture)));
                            delete_opponents_piece(BOARD, move_to_square, White, restore_to);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square + 64;
                            }
                            pop_bit(BOARD.black.queen, move_to_square);
                            /*set_bit(BOARD.black.knights, move_to_square);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square + 72;
                            }
                            pop_bit(BOARD.black.knights, move_to_square);
                            set_bit(BOARD.black.bishops, move_to_square);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square + 80;
                            }
                            pop_bit(BOARD.black.bishops, move_to_square);
                            set_bit(BOARD.black.rooks, move_to_square);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square + 88;
                            }
                            pop_bit(BOARD.black.rooks, move_to_square);*/
                            restore_opponents_piece(BOARD, move_to_square, White, restore_to);
                            pop_bit(temp_capture, move_to_square);
                        }
                        if (temp_move != 0) {
                            set_bit(BOARD.black.queen, (move_to_square = square - BOARD_WIDTH));
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square + 64;
                            }
                            pop_bit(BOARD.black.queen, move_to_square);
                            /*set_bit(BOARD.black.knights, move_to_square);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square + 72;
                            }
                            pop_bit(BOARD.black.knights, move_to_square);
                            set_bit(BOARD.black.bishops, move_to_square);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square + 80;
                            }
                            pop_bit(BOARD.black.bishops, move_to_square);
                            set_bit(BOARD.black.rooks, move_to_square);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square + 88;
                            }
                            pop_bit(BOARD.black.rooks, move_to_square);*/
                            pop_bit(temp_capture, move_to_square);
                        }
                    }
                    else {
                        temp_capture = temp_move & white_occupancy;
                        if (BOARD.en_passant != 0 && ((BOARD.en_passant + 8 + 1 == square && BOARD.en_passant < a3) || (BOARD.en_passant + 8 - 1 == square && BOARD.en_passant > h3))) {
                            set_bit(BOARD.black.pawns, BOARD.en_passant);
                            pop_bit(BOARD.white.pawns, BOARD.en_passant + 8);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = BOARD.en_passant;
                            }
                            set_bit(BOARD.white.pawns, BOARD.en_passant + 8);
                            pop_bit(BOARD.black.pawns, BOARD.en_passant);
                        }
                        while (temp_capture != 0) {
                            set_bit(BOARD.black.pawns, (move_to_square = find_first_set_bit(temp_capture)));
                            delete_opponents_piece(BOARD, move_to_square, White, restore_to);
                            if (!king_in_check(BOARD, White)) {
                                captures[capture_c].from = square;
                                captures[capture_c++].to = move_to_square;
                            }
                            restore_opponents_piece(BOARD, move_to_square, White, restore_to);
                            pop_bit(BOARD.black.pawns, move_to_square);
                            pop_bit(temp_capture, move_to_square);
                        }
                    }
                    set_bit(BOARD.black.pawns, square);
                }
                else if (i == 1 && ((temp_capture = (get_knight_attacks(square) & ~black_occupancy & white_occupancy)) != 0)) {
                    pop_bit(BOARD.black.knights, square);
                    while (temp_capture != 0) {
                        set_bit(BOARD.black.knights, (move_to_square = find_first_set_bit(temp_capture)));
                        delete_opponents_piece(BOARD, move_to_square, White, restore_to);
                        if (!king_in_check(BOARD, White)) {
                            captures[capture_c].from = square;
                            captures[capture_c++].to = move_to_square;
                        }
                        restore_opponents_piece(BOARD, move_to_square, White, restore_to);
                        pop_bit(BOARD.black.knights, move_to_square);
                        pop_bit(temp_capture, move_to_square);
                    }
                    set_bit(BOARD.black.knights, square);
                }
                else if (i == 2 && ((temp_capture = (get_bishop_attacks(square, all_occupancy) & ~black_occupancy & white_occupancy)) != 0)) {
                    pop_bit(BOARD.black.bishops, square);
                    while (temp_capture != 0) {
                        set_bit(BOARD.black.bishops, (move_to_square = find_first_set_bit(temp_capture)));
                        delete_opponents_piece(BOARD, move_to_square, White, restore_to);
                        if (!king_in_check(BOARD, White)) {
                            captures[capture_c].from = square;
                            captures[capture_c++].to = move_to_square;
                        }
                        restore_opponents_piece(BOARD, move_to_square, White, restore_to);
                        pop_bit(BOARD.black.bishops, move_to_square);
                        pop_bit(temp_capture, move_to_square);
                    }
                    set_bit(BOARD.black.bishops, square);
                }
                else if (i == 3 && ((temp_capture = (get_queen_attacks(square, all_occupancy) & ~black_occupancy & white_occupancy)) != 0)) {
                    pop_bit(BOARD.black.queen, square);
                    while (temp_capture != 0) {
                        set_bit(BOARD.black.queen, (move_to_square = find_first_set_bit(temp_capture)));
                        delete_opponents_piece(BOARD, move_to_square, White, restore_to);
                        if (!king_in_check(BOARD, White)) {
                            captures[capture_c].from = square;
                            captures[capture_c++].to = move_to_square;
                        }
                        restore_opponents_piece(BOARD, move_to_square, White, restore_to);
                        pop_bit(BOARD.black.queen, move_to_square);
                        pop_bit(temp_capture, move_to_square);
                    }
                    set_bit(BOARD.black.queen, square);
                }
                else if (i == 4 && ((temp_capture = (get_rook_attacks(square, all_occupancy) & ~black_occupancy & white_occupancy)) != 0)) {
                    pop_bit(BOARD.black.rooks, square);
                    while (temp_capture != 0) {
                        set_bit(BOARD.black.rooks, (move_to_square = find_first_set_bit(temp_capture)));
                        delete_opponents_piece(BOARD, move_to_square, White, restore_to);
                        if (!king_in_check(BOARD, White)) {
                            captures[capture_c].from = square;
                            captures[capture_c++].to = move_to_square;
                        }
                        restore_opponents_piece(BOARD, move_to_square, White, restore_to);
                        pop_bit(BOARD.black.rooks, move_to_square);
                        pop_bit(temp_capture, move_to_square);
                    }
                    set_bit(BOARD.black.rooks, square);
                }
                else if (i == 5 && (temp_capture = (get_king_attacks(square) & ~black_occupancy & white_occupancy)) != 0) {
                    pop_bit(BOARD.black.king, square);
                    while (temp_capture != 0) {
                        set_bit(BOARD.black.king, (move_to_square = find_first_set_bit(temp_capture)));
                        delete_opponents_piece(BOARD, move_to_square, White, restore_to);
                        if (!king_in_check(BOARD, White)) {
                            captures[capture_c].from = square;
                            captures[capture_c++].to = move_to_square;
                        }
                        restore_opponents_piece(BOARD, move_to_square, White, restore_to);
                        pop_bit(BOARD.black.king, move_to_square);
                        pop_bit(temp_capture, move_to_square);
                    }
                    set_bit(BOARD.black.king, square);
                }
                pop_bit(temp_black_occupancy, square);
            }
        }
    }
}
void generateMoves(chessboard& BOARD, bool White, MOVES& moves)
{
    uint64_t white_occupancy = get_occupancy(BOARD.white), all_occupancy = get_occupancy(BOARD),
        black_occupancy = all_occupancy ^ white_occupancy, temp_move,
        temp_white_occupancy = white_occupancy, temp_black_occupancy = black_occupancy;
    int square = 0, move_to_square;
    if (White) {
        for (int i = 0; i < 6; i++) {
            temp_white_occupancy = moveAnalyseOrder(BOARD, White, i);
            while (temp_white_occupancy != 0) {
                square = find_first_set_bit(temp_white_occupancy);
                if (i == 0 && (temp_move = getPawnMoves(all_occupancy, square, White)) != 0) {
                    pop_bit(BOARD.white.pawns, square);
                    if (square <= a6)
                        while (temp_move != 0) {
                            set_bit(BOARD.white.pawns, (move_to_square = find_first_set_bit(temp_move)));
                            if (!king_in_check(BOARD, White)) {
                                moves.moves[moves.move_c].from = square;
                                moves.moves[moves.move_c++].to = move_to_square;
                            }
                            pop_bit(BOARD.white.pawns, move_to_square);
                            pop_bit(temp_move, move_to_square);
                        }
                    set_bit(BOARD.white.pawns, square);
                }
                else if (i == 1 && ((temp_move = (get_knight_attacks(square) & ~all_occupancy)) != 0)) {
                    pop_bit(BOARD.white.knights, square);
                    while (temp_move != 0) {
                        set_bit(BOARD.white.knights, (move_to_square = find_first_set_bit(temp_move)));
                        if (!king_in_check(BOARD, White)) {
                            moves.moves[moves.move_c].from = square;
                            moves.moves[moves.move_c++].to = move_to_square;
                        }
                        pop_bit(BOARD.white.knights, move_to_square);
                        pop_bit(temp_move, move_to_square);
                    }
                    set_bit(BOARD.white.knights, square);
                }
                else if (i == 2 && ((temp_move = (get_bishop_attacks(square, all_occupancy) & ~all_occupancy)) != 0)) {
                    pop_bit(BOARD.white.bishops, square);
                    while (temp_move != 0) {
                        set_bit(BOARD.white.bishops, (move_to_square = find_first_set_bit(temp_move)));
                        if (!king_in_check(BOARD, White)) {
                            moves.moves[moves.move_c].from = square;
                            moves.moves[moves.move_c++].to = move_to_square;
                        }
                        pop_bit(BOARD.white.bishops, move_to_square);
                        pop_bit(temp_move, move_to_square);
                    }
                    set_bit(BOARD.white.bishops, square);
                }
                else if (i == 3 && ((temp_move = (get_queen_attacks(square, all_occupancy) & ~all_occupancy)) != 0)) {
                    pop_bit(BOARD.white.queen, square);
                    while (temp_move != 0) {
                        set_bit(BOARD.white.queen, (move_to_square = find_first_set_bit(temp_move)));
                        if (!king_in_check(BOARD, White)) {
                            moves.moves[moves.move_c].from = square;
                            moves.moves[moves.move_c++].to = move_to_square;
                        }
                        pop_bit(BOARD.white.queen, move_to_square);
                        pop_bit(temp_move, move_to_square);
                    }
                    set_bit(BOARD.white.queen, square);
                }
                else if (i == 4 && ((temp_move = (get_rook_attacks(square, all_occupancy) & ~all_occupancy)) != 0)) {
                    pop_bit(BOARD.white.rooks, square);
                    while (temp_move != 0) {
                        set_bit(BOARD.white.rooks, (move_to_square = find_first_set_bit(temp_move)));
                        if (!king_in_check(BOARD, White)) {
                            moves.moves[moves.move_c].from = square;
                            moves.moves[moves.move_c++].to = move_to_square;
                        }
                        pop_bit(BOARD.white.rooks, move_to_square);
                        pop_bit(temp_move, move_to_square);
                    }
                    set_bit(BOARD.white.rooks, square);
                }
                else if (i == 5 && ((temp_move = (get_king_attacks(square) & ~all_occupancy)) != 0)) {
                    if ((BOARD.castling & 12) != 0 && !king_in_check(BOARD, White)) {
                        int temp_check;
                        pop_bit(BOARD.white.king, square);
                        if ((BOARD.castling & 8) != 0 && get_bit(all_occupancy, square + 1) == 0 && get_bit(all_occupancy, square + 2) == 0 && get_bit(all_occupancy, square + 3) == 0) {
                            set_bit(BOARD.white.king, d1);
                            temp_check = king_in_check(BOARD, White);
                            pop_bit(BOARD.white.king, d1);
                            if (!temp_check) {
                                set_bit(BOARD.white.king, c1);
                                temp_check = king_in_check(BOARD, White);
                                pop_bit(BOARD.white.king, c1);
                                if (!temp_check) {
                                    set_bit(BOARD.white.king, c1);
                                    pop_bit(BOARD.white.rooks, a1);
                                    set_bit(BOARD.white.rooks, d1);
                                    if (!king_in_check(BOARD, White)) {
                                        moves.moves[moves.move_c].from = square;
                                        moves.moves[moves.move_c++].to = c1;
                                    }
                                    set_bit(BOARD.white.rooks, a1);
                                    pop_bit(BOARD.white.rooks, d1);
                                    pop_bit(BOARD.white.king, c1);
                                }
                            }
                        }
                        if ((BOARD.castling & 4) != 0 && get_bit(all_occupancy, square - 1) == 0 && get_bit(all_occupancy, square - 2) == 0) {
                            set_bit(BOARD.white.king, f1);
                            temp_check = king_in_check(BOARD, White);
                            pop_bit(BOARD.white.king, f1);
                            if (!temp_check) {
                                set_bit(BOARD.white.king, g1);
                                temp_check = king_in_check(BOARD, White);
                                pop_bit(BOARD.white.king, g1);
                                if (!temp_check) {
                                    set_bit(BOARD.white.king, g1);
                                    pop_bit(BOARD.white.rooks, h1);
                                    set_bit(BOARD.white.rooks, f1);
                                    if (!king_in_check(BOARD, White)) {
                                        moves.moves[moves.move_c].from = square;
                                        moves.moves[moves.move_c++].to = g1;
                                    }
                                    set_bit(BOARD.white.rooks, h1);
                                    pop_bit(BOARD.white.rooks, f1);
                                    pop_bit(BOARD.white.king, g1);
                                }
                            }
                        }
                        set_bit(BOARD.white.king, square);
                    }
                    pop_bit(BOARD.white.king, square);
                    while (temp_move != 0) {
                        set_bit(BOARD.white.king, (move_to_square = find_first_set_bit(temp_move)));
                        if (!king_in_check(BOARD, White)) {
                            moves.moves[moves.move_c].from = square;
                            moves.moves[moves.move_c++].to = move_to_square;
                        }
                        pop_bit(BOARD.white.king, move_to_square);
                        pop_bit(temp_move, move_to_square);
                    }
                    set_bit(BOARD.white.king, square);
                }
                pop_bit(temp_white_occupancy, square);
            }
        }
    }
    else {
        for (int i = 0; i < 6; i++) {
            temp_black_occupancy = moveAnalyseOrder(BOARD, White, i);
            while (temp_black_occupancy != 0) {
                square = find_first_set_bit(temp_black_occupancy);
                if (i == 0 && (temp_move = getPawnMoves(all_occupancy, square, White)) != 0) {
                    pop_bit(BOARD.black.pawns, square);
                    if (square >= h3)
                        while (temp_move != 0) {
                            set_bit(BOARD.black.pawns, (move_to_square = find_first_set_bit(temp_move)));
                            if (!king_in_check(BOARD, White)) {
                                moves.moves[moves.move_c].from = square;
                                moves.moves[moves.move_c++].to = move_to_square;
                            }
                            pop_bit(BOARD.black.pawns, move_to_square);
                            pop_bit(temp_move, move_to_square);
                        }
                    set_bit(BOARD.black.pawns, square);
                }
                else if (i == 1 && ((temp_move = (get_knight_attacks(square) & ~all_occupancy)) != 0)) {
                    pop_bit(BOARD.black.knights, square);
                    while (temp_move != 0) {
                        set_bit(BOARD.black.knights, (move_to_square = find_first_set_bit(temp_move)));
                        if (!king_in_check(BOARD, White)) {
                            moves.moves[moves.move_c].from = square;
                            moves.moves[moves.move_c++].to = move_to_square;
                        }
                        pop_bit(BOARD.black.knights, move_to_square);
                        pop_bit(temp_move, move_to_square);
                    }
                    set_bit(BOARD.black.knights, square);
                }
                else if (i == 2 && ((temp_move = (get_bishop_attacks(square, all_occupancy) & ~all_occupancy)) != 0)) {
                    pop_bit(BOARD.black.bishops, square);
                    while (temp_move != 0) {
                        set_bit(BOARD.black.bishops, (move_to_square = find_first_set_bit(temp_move)));
                        if (!king_in_check(BOARD, White)) {
                            moves.moves[moves.move_c].from = square;
                            moves.moves[moves.move_c++].to = move_to_square;
                        }
                        pop_bit(BOARD.black.bishops, move_to_square);
                        pop_bit(temp_move, move_to_square);
                    }
                    set_bit(BOARD.black.bishops, square);
                }
                else if (i == 3 && ((temp_move = (get_queen_attacks(square, all_occupancy) & ~all_occupancy)) != 0)) {
                    pop_bit(BOARD.black.queen, square);
                    while (temp_move != 0) {
                        set_bit(BOARD.black.queen, (move_to_square = find_first_set_bit(temp_move)));
                        if (!king_in_check(BOARD, White)) {
                            moves.moves[moves.move_c].from = square;
                            moves.moves[moves.move_c++].to = move_to_square;
                        }
                        pop_bit(BOARD.black.queen, move_to_square);
                        pop_bit(temp_move, move_to_square);
                    }
                    set_bit(BOARD.black.queen, square);
                }
                else if (i == 4 && ((temp_move = (get_rook_attacks(square, all_occupancy) & ~all_occupancy)) != 0)) {
                    pop_bit(BOARD.black.rooks, square);
                    while (temp_move != 0) {
                        set_bit(BOARD.black.rooks, (move_to_square = find_first_set_bit(temp_move)));
                        if (!king_in_check(BOARD, White)) {
                            moves.moves[moves.move_c].from = square;
                            moves.moves[moves.move_c++].to = move_to_square;
                        }
                        pop_bit(BOARD.black.rooks, move_to_square);
                        pop_bit(temp_move, move_to_square);
                    }
                    set_bit(BOARD.black.rooks, square);
                }
                else if (i == 5 && ((temp_move = (get_king_attacks(square) & ~all_occupancy)) != 0)) {
                    if ((BOARD.castling & 3) != 0 && !king_in_check(BOARD, White)) {
                        int temp_check;
                        pop_bit(BOARD.black.king, square);
                        if ((BOARD.castling & 2) != 0 && get_bit(all_occupancy, square + 1) == 0 && get_bit(all_occupancy, square + 2) == 0 && get_bit(all_occupancy, square + 3) == 0) {
                            set_bit(BOARD.black.king, d8);
                            temp_check = king_in_check(BOARD, White);
                            pop_bit(BOARD.black.king, d8);
                            if (!temp_check) {
                                set_bit(BOARD.black.king, c8);
                                temp_check = king_in_check(BOARD, White);
                                pop_bit(BOARD.black.king, c8);
                                if (!temp_check) {
                                    set_bit(BOARD.black.king, c8);
                                    pop_bit(BOARD.black.rooks, a8);
                                    set_bit(BOARD.black.rooks, d8);
                                    if (!king_in_check(BOARD, White)) {
                                        moves.moves[moves.move_c].from = square;
                                        moves.moves[moves.move_c++].to = c8;
                                    }
                                    set_bit(BOARD.black.rooks, a8);
                                    pop_bit(BOARD.black.rooks, d8);
                                    pop_bit(BOARD.black.king, c8);
                                }
                            }
                        }
                        if ((BOARD.castling & 1) != 0 && get_bit(all_occupancy, square - 1) == 0 && get_bit(all_occupancy, square - 2) == 0) {
                            set_bit(BOARD.black.king, f8);
                            temp_check = king_in_check(BOARD, White);
                            pop_bit(BOARD.black.king, f8);
                            if (!temp_check) {
                                set_bit(BOARD.black.king, g8);
                                temp_check = king_in_check(BOARD, White);
                                pop_bit(BOARD.black.king, g8);
                                if (!temp_check) {
                                    set_bit(BOARD.black.king, g8);
                                    pop_bit(BOARD.black.rooks, h8);
                                    set_bit(BOARD.black.rooks, f8);
                                    if (!king_in_check(BOARD, White)) {
                                        moves.moves[moves.move_c].from = square;
                                        moves.moves[moves.move_c++].to = g8;
                                    }
                                    set_bit(BOARD.black.rooks, h8);
                                    pop_bit(BOARD.black.rooks, f8);
                                    pop_bit(BOARD.black.king, g8);
                                }
                            }
                        }
                        set_bit(BOARD.black.king, square);
                    }
                    pop_bit(BOARD.black.king, square);
                    while (temp_move != 0) {
                        set_bit(BOARD.black.king, (move_to_square = find_first_set_bit(temp_move)));
                        if (!king_in_check(BOARD, White)) {
                            moves.moves[moves.move_c].from = square;
                            moves.moves[moves.move_c++].to = move_to_square;
                        }
                        pop_bit(BOARD.black.king, move_to_square);
                        pop_bit(temp_move, move_to_square);
                    }
                    set_bit(BOARD.black.king, square);
                }
                pop_bit(temp_black_occupancy, square);
            }
        }
    }
}
void generateAllMoves(chessboard& BOARD, bool White, MOVES& moves) {
    generateCapturesAndProm(BOARD, White, moves.captures, moves.capture_c);
    generateMoves(BOARD, White, moves);
}
bool king_in_check(chessboard& BOARD, bool white) {
    int kingsquare;
    uint64_t temp_attack, all_occupancy = get_occupancy(BOARD);
    if (white) {
        kingsquare = find_first_set_bit(BOARD.white.king);
        if ((((temp_attack = get_bishop_attacks(kingsquare, all_occupancy)) & BOARD.black.bishops) != 0) || ((temp_attack & BOARD.black.queen) != 0))return true;
        if ((((temp_attack = get_rook_attacks(kingsquare, all_occupancy)) & BOARD.black.rooks) != 0) || ((temp_attack & BOARD.black.queen) != 0))return true;
        if ((get_knight_attacks(kingsquare) & BOARD.black.knights) != 0)return true;
        if ((get_king_attacks(kingsquare) & BOARD.black.king) != 0)return true;
        if (((((BOARD.white.king & ~trim_left) << (BOARD_WIDTH + 1)) & BOARD.black.pawns) != 0) || ((((BOARD.white.king & ~trim_right) << (BOARD_WIDTH - 1)) & BOARD.black.pawns) != 0)) return true;
    }
    else {
        kingsquare = find_first_set_bit(BOARD.black.king);
        if ((((temp_attack = get_bishop_attacks(kingsquare, all_occupancy)) & BOARD.white.bishops) != 0) || ((temp_attack & BOARD.white.queen) != 0))return true;
        if ((((temp_attack = get_rook_attacks(kingsquare, all_occupancy)) & BOARD.white.rooks) != 0) || ((temp_attack & BOARD.white.queen) != 0))return true;
        if ((get_knight_attacks(kingsquare) & BOARD.white.knights) != 0)return true;
        if ((get_king_attacks(kingsquare) & BOARD.white.king) != 0)return true;
        if (((((BOARD.black.king & ~trim_left) >> (BOARD_WIDTH - 1)) & BOARD.white.pawns) != 0) || ((((BOARD.black.king & ~trim_right) >> (BOARD_WIDTH + 1)) & BOARD.white.pawns) != 0)) return true;
    }
    return false;
}