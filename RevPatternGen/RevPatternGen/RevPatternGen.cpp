// RevPatternGen.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#define POW2_8 256

typedef unsigned long long uint64;

int main()
{

	char PAT[POW2_8][POW2_8][8] = {0};
	char me, enm;
	char pos;
	char mask;
	char result = 0;
	//8マスの全パターンループ
	for (me = 0; me < POW2_8; me++) {
		for (enm = 0; enm < POW2_8; enm++) {
			//もし同じ場所にビットが立っていたら不正な値なので0を代入
			//(白と黒が同時にあるマスは存在しない)
			if ((me&enm) != 0) {
				for (pos = 0; pos < 8; pos++) {
					PAT[me][enm][pos] = 0;
				}
			}
			else {
				//8マスすべてに対して
				for (pos = 0; pos < 8; pos++) {
					result = 0;
					//着手場所が空いていないといけない
					if (((me | enm) & pos) != 0) {

						//右方向に対して
						mask = (pos >> 1) & 0x7E;
						//敵石が連続する間
						while (mask != 0 && (mask & enm) != 0) {
							result |= mask;
							mask = (mask >> 1) & 0x7E;
						}
						//自分の石が無かったら着手不可=0
						if ((mask & me) == 0) {
							result = 0;
						}
						else {
							PAT[me][enm][pos] = result;
						}

						//左方向に対して
						mask = (pos << 1) & 0x7E;
						//敵石が連続する間
						while (mask != 0 && (mask & enm) != 0) {
							result |= mask;
							mask = (mask << 1) & 0x7E;
						}
						//自分の石が無かったら着手不可=0
						if ((mask & me) == 0) {
							result = 0;
						}
						else {
							PAT[me][enm][pos] = result;
						}

					}
					else {
						PAT[me][enm][pos] = 0;
					}
				}
			}
		}
	}

    return 0;
}



