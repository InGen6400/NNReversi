#pragma once

const char MAX_DEPTH = 6;

unsigned int NegaMaxSearch(Board *board, char color, char depth, char *PutPos);

unsigned int Evaluation(Board *board, char color);