#pragma once

const char MAX_DEPTH = 6;

int NegaMaxSearch(Board *board, char isPassed, char color, char depth, char *PutPos);

int Evaluation(Board *board, char color);