// RevPatternGen.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#define POW2_8 256

typedef unsigned long long uint64;

int main()
{

	uint64 PAT[POW2_8][POW2_8][8] = {0};
	char black, white;
	char pos;

	for (black = 0; black < POW2_8; black++) {
		for (white = 0; white < POW2_8; white++) {
			//もし同じ場所にビットが立っていたら不正な値なので0を代入
			//(白と黒が同時にあるマスは存在しない)
			if ((black&white) != 0) {
				for (pos = 0; pos < 8; pos++) {
					PAT[black][white][pos] = 0;
				}
			}
			else {
				for (pos = 0; pos < 8; pos++) {
					//着手場所が空いていないといけない
					if (((black | white) & pos) != 0) {

					}
					else {
						PAT[black][white][pos] = 0;
					}
				}
			}
		}
	}

    return 0;
}

