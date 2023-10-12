#pragma once

const std::string PrecomputedMovePath = "move_file1.txt";

const uint64_t knight_magics[64] = {
    0x20400ULL,
    0x50800ULL,
    0xa1100ULL,
    0x142200ULL,
    0x284400ULL,
    0x508800ULL,
    0xa01000ULL,
    0x402000ULL,
    0x2040004ULL,
    0x5080008ULL,
    0xa110011ULL,
    0x14220022ULL,
    0x28440044ULL,
    0x50880088ULL,
    0xa0100010ULL,
    0x40200020ULL,
    0x204000402ULL,
    0x508000805ULL,
    0xa1100110aULL,
    0x1422002214ULL,
    0x2844004428ULL,
    0x5088008850ULL,
    0xa0100010a0ULL,
    0x4020002040ULL,
    0x20400040200ULL,
    0x50800080500ULL,
    0xa1100110a00ULL,
    0x142200221400ULL,
    0x284400442800ULL,
    0x508800885000ULL,
    0xa0100010a000ULL,
    0x402000204000ULL,
    0x2040004020000ULL,
    0x5080008050000ULL,
    0xa1100110a0000ULL,
    0x14220022140000ULL,
    0x28440044280000ULL,
    0x50880088500000ULL,
    0xa0100010a00000ULL,
    0x40200020400000ULL,
    0x204000402000000ULL,
    0x508000805000000ULL,
    0xa1100110a000000ULL,
    0x1422002214000000ULL,
    0x2844004428000000ULL,
    0x5088008850000000ULL,
    0xa0100010a0000000ULL,
    0x4020002040000000ULL,
    0x400040200000000ULL,
    0x800080500000000ULL,
    0x1100110a00000000ULL,
    0x2200221400000000ULL,
    0x4400442800000000ULL,
    0x8800885000000000ULL,
    0x100010a000000000ULL,
    0x2000204000000000ULL,
    0x4020000000000ULL,
    0x8050000000000ULL,
    0x110a0000000000ULL,
    0x22140000000000ULL,
    0x44280000000000ULL,
    0x88500000000000ULL,
    0x10a00000000000ULL,
    0x20400000000000ULL,
};
const uint64_t king_magics[64] = {
    0x302ULL,
    0x705ULL,
    0xe0aULL,
    0x1c14ULL,
    0x3828ULL,
    0x7050ULL,
    0xe0a0ULL,
    0xc040ULL,
    0x30203ULL,
    0x70507ULL,
    0xe0a0eULL,
    0x1c141cULL,
    0x382838ULL,
    0x705070ULL,
    0xe0a0e0ULL,
    0xc040c0ULL,
    0x3020300ULL,
    0x7050700ULL,
    0xe0a0e00ULL,
    0x1c141c00ULL,
    0x38283800ULL,
    0x70507000ULL,
    0xe0a0e000ULL,
    0xc040c000ULL,
    0x302030000ULL,
    0x705070000ULL,
    0xe0a0e0000ULL,
    0x1c141c0000ULL,
    0x3828380000ULL,
    0x7050700000ULL,
    0xe0a0e00000ULL,
    0xc040c00000ULL,
    0x30203000000ULL,
    0x70507000000ULL,
    0xe0a0e000000ULL,
    0x1c141c000000ULL,
    0x382838000000ULL,
    0x705070000000ULL,
    0xe0a0e0000000ULL,
    0xc040c0000000ULL,
    0x3020300000000ULL,
    0x7050700000000ULL,
    0xe0a0e00000000ULL,
    0x1c141c00000000ULL,
    0x38283800000000ULL,
    0x70507000000000ULL,
    0xe0a0e000000000ULL,
    0xc040c000000000ULL,
    0x302030000000000ULL,
    0x705070000000000ULL,
    0xe0a0e0000000000ULL,
    0x1c141c0000000000ULL,
    0x3828380000000000ULL,
    0x7050700000000000ULL,
    0xe0a0e00000000000ULL,
    0xc040c00000000000ULL,
    0x203000000000000ULL,
    0x507000000000000ULL,
    0xa0e000000000000ULL,
    0x141c000000000000ULL,
    0x2838000000000000ULL,
    0x5070000000000000ULL,
    0xa0e0000000000000ULL,
    0x40c0000000000000ULL,
};
const int rook_rellevant_bits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};
const int bishop_rellevant_bits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};
extern unsigned int state;

const enum {
    h1, g1, f1, e1, d1, c1, b1, a1,
    h2, g2, f2, e2, d2, c2, b2, a2,
    h3, g3, f3, e3, d3, c3, b3, a3,
    h4, g4, f4, e4, d4, c4, b4, a4,
    h5, g5, f5, e5, d5, c5, b5, a5,
    h6, g6, f6, e6, d6, c6, b6, a6,
    h7, g7, f7, e7, d7, c7, b7, a7,
    h8, g8, f8, e8, d8, c8, b8, a8, no_sq
};

const enum { rook, bishop };

const enum { p, b, n, r, q, k, rA, rH };

const uint64_t rook_magics[64] = {
    0xa8002c000108020ULL,
    0x6c00049b0002001ULL,
    0x100200010090040ULL,
    0x2480041000800801ULL,
    0x280028004000800ULL,
    0x900410008040022ULL,
    0x280020001001080ULL,
    0x2880002041000080ULL,
    0xa000800080400034ULL,
    0x4808020004000ULL,
    0x2290802004801000ULL,
    0x411000d00100020ULL,
    0x402800800040080ULL,
    0xb000401004208ULL,
    0x2409000100040200ULL,
    0x1002100004082ULL,
    0x22878001e24000ULL,
    0x1090810021004010ULL,
    0x801030040200012ULL,
    0x500808008001000ULL,
    0xa08018014000880ULL,
    0x8000808004000200ULL,
    0x201008080010200ULL,
    0x801020000441091ULL,
    0x800080204005ULL,
    0x1040200040100048ULL,
    0x120200402082ULL,
    0xd14880480100080ULL,
    0x12040280080080ULL,
    0x100040080020080ULL,
    0x9020010080800200ULL,
    0x813241200148449ULL,
    0x491604001800080ULL,
    0x100401000402001ULL,
    0x4820010021001040ULL,
    0x400402202000812ULL,
    0x209009005000802ULL,
    0x810800601800400ULL,
    0x4301083214000150ULL,
    0x204026458e001401ULL,
    0x40204000808000ULL,
    0x8001008040010020ULL,
    0x8410820820420010ULL,
    0x1003001000090020ULL,
    0x804040008008080ULL,
    0x12000810020004ULL,
    0x1000100200040208ULL,
    0x430000a044020001ULL,
    0x280009023410300ULL,
    0xe0100040002240ULL,
    0x200100401700ULL,
    0x2244100408008080ULL,
    0x8000400801980ULL,
    0x2000810040200ULL,
    0x8010100228810400ULL,
    0x2000009044210200ULL,
    0x4080008040102101ULL,
    0x40002080411d01ULL,
    0x2005524060000901ULL,
    0x502001008400422ULL,
    0x489a000810200402ULL,
    0x1004400080a13ULL,
    0x4000011008020084ULL,
    0x26002114058042ULL,
};
const uint64_t bishop_magics[64] = {
    0x89a1121896040240ULL,
    0x2004844802002010ULL,
    0x2068080051921000ULL,
    0x62880a0220200808ULL,
    0x4042004000000ULL,
    0x100822020200011ULL,
    0xc00444222012000aULL,
    0x28808801216001ULL,
    0x400492088408100ULL,
    0x201c401040c0084ULL,
    0x840800910a0010ULL,
    0x82080240060ULL,
    0x2000840504006000ULL,
    0x30010c4108405004ULL,
    0x1008005410080802ULL,
    0x8144042209100900ULL,
    0x208081020014400ULL,
    0x4800201208ca00ULL,
    0xf18140408012008ULL,
    0x1004002802102001ULL,
    0x841000820080811ULL,
    0x40200200a42008ULL,
    0x800054042000ULL,
    0x88010400410c9000ULL,
    0x520040470104290ULL,
    0x1004040051500081ULL,
    0x2002081833080021ULL,
    0x400c00c010142ULL,
    0x941408200c002000ULL,
    0x658810000806011ULL,
    0x188071040440a00ULL,
    0x4800404002011c00ULL,
    0x104442040404200ULL,
    0x511080202091021ULL,
    0x4022401120400ULL,
    0x80c0040400080120ULL,
    0x8040010040820802ULL,
    0x480810700020090ULL,
    0x102008e00040242ULL,
    0x809005202050100ULL,
    0x8002024220104080ULL,
    0x431008804142000ULL,
    0x19001802081400ULL,
    0x200014208040080ULL,
    0x3308082008200100ULL,
    0x41010500040c020ULL,
    0x4012020c04210308ULL,
    0x208220a202004080ULL,
    0x111040120082000ULL,
    0x6803040141280a00ULL,
    0x2101004202410000ULL,
    0x8200000041108022ULL,
    0x21082088000ULL,
    0x2410204010040ULL,
    0x40100400809000ULL,
    0x822088220820214ULL,
    0x40808090012004ULL,
    0x910224040218c9ULL,
    0x402814422015008ULL,
    0x90014004842410ULL,
    0x1000042304105ULL,
    0x10008830412a00ULL,
    0x2520081090008908ULL,
    0x40102000a0a60140ULL,
};

const int BOARD_WIDTH = 8;
const int BOARD_HEIGHT = 8;

const uint64_t EMPTY = 0;
const uint64_t PAWNS = 0xFF00;
const uint64_t KNIGHTS = 0x42;
const uint64_t BISHOPS = 0x24;
const uint64_t KING = 0x8;
const uint64_t QUEEN = 0x10;
const uint64_t ROOKS = 0x81;
const int CASTLING = 0xF;
const int EN_PASSANT = 0;
const uint64_t trim_left = 0x8080808080808080;
const uint64_t trim_right = 0x101010101010101;
const int both_king_locations = e1 * 100 + e8;
const int max_saved_board_n = 90;

const enum moveTypes{normal, capture, promotion};//move types


const enum EvalValues{infinity = 100000, checkmate = -99999};


const int DEPTH = 6;


extern uint64_t bishop_masks[64];
extern uint64_t rook_masks[64];
extern uint64_t bishop_attacks[64][512];
extern uint64_t rook_attacks[64][4096];

const std::string normal_notation[64] = {
    "h1","g1","f1","e1","d1","c1","b1","a1",
    "h2","g2","f2","e2","d2","c2","b2","a2",
    "h3","g3","f3","e3","d3","c3","b3","a3",
    "h4","g4","f4","e4","d4","c4","b4","a4",
    "h5","g5","f5","e5","d5","c5","b5","a5",
    "h6","g6","f6","e6","d6","c6","b6","a6",
    "h7","g7","f7","e7","d7","c7","b7","a7",
    "h8","g8","f8","e8","d8","c8","b8","a8"
};