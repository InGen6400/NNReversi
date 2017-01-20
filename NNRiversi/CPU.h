#pragma once

const char MAX_DEPTH = 6;

const int VALUE_MAX = 100000000;

int NegaMaxSearch(Board *board, char isPassed, char color, char depth, char *PutPos);

int Evaluation(Board *board, char color);