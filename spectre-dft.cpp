#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <cmath>
#include <limits>
#include <complex>
#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;

int main()
{
    const int PH = 720, PW = 720;

    static double paddedImage[480][720] = {0};

    static double Y1[PH][PW] = {128};
    static int Cb1[PH][PW] = {0};
    static int Cr1[PH][PW] = {0};
    static int Cb2[PH / 2][PW / 2] = {0};
    static int Cr2[PH / 2][PW / 2] = {0};

    int x = 0, y = 0, pic = 0;

    FILE *fp_in1 = fopen("Boy_and_Toys30p.yuv", "rb");
    if (fp_in1 == NULL)
    {
        printf("インプットエラー1\n");
        return -1;
    }

    FILE *fp_out1 = fopen("boy_and_toys_dft_result.yuv", "wb");
    if (fp_out1 == NULL)
    {
        printf("アウトプットエラー1\n");
        return -1;
    }

    for (y = 0; y < 480; y++)
    {
        for (x = 0; x < 720; x++)
        {
            paddedImage[y][x] = (unsigned char)fgetc(fp_in1);
        }
    }

    for (y = 0; y < 480 / 2; y++)
    {
        for (x = 0; x < 720 / 2; x++)
        {
            Cb1[y][x] = (unsigned char)fgetc(fp_in1);
        }
    }

    for (y = 0; y < 480 / 2; y++)
    {
        for (x = 0; x < 720 / 2; x++)
        {
            Cr1[y][x] = (unsigned char)fgetc(fp_in1);
        }
    }

    for (int y = 0; y < PH; y++)
    {
        for (int x = 0; x < PW; x++)
        {
            Y1[y][x] = paddedImage[y][x]; // 元画像のデータをコピー
        }
    }

    //------------------スペクトル計算----------------------------

    const int blockSize = 720;
    const int normalization = 32;

    static complex<double> dft_filter[blockSize + 64][blockSize + 64] = {{0, 0}};
    static complex<double> Y2[PH + 64][PW + 64] = {{0, 0}};

    int i, j, u, v = 0;
    static complex<double> coeff = 0;

    // 離散フーリエ変換のフィルタ係数計算
    for (int u = 0; u < blockSize; u++)
    {
        for (int x = 0; x < blockSize; x++)
        {
            double angle = 2 * M_PI * u * x / blockSize;
            dft_filter[u][x] = std::complex<double>(cos(angle), sin(angle));
        }
    }

    // フーリエ変換の適用
    for (int y = 0; y < PH; y += blockSize)
    {
        for (int x = 0; x < PW; x += blockSize)
        {

            // 水平方向フィルター適用
            for (j = 0; j < blockSize; j++)
            {
                for (u = 0; u < blockSize; u++)
                {
                    coeff = 0;
                    for (i = 0; i < blockSize; i++)
                    {
                        coeff += dft_filter[u][i] * std::complex<double>(Y1[y + j][x + i], 0);
                        Y2[y + j][x + u] = coeff;
                    }
                }
            }

            // 垂直方向フィルター適用
            for (i = 0; i < blockSize; i++)
            {
                for (v = 0; v < blockSize; v++)
                {
                    coeff = 0;
                    for (j = 0; j < blockSize; j++)
                    {
                        coeff += dft_filter[v][j] * Y2[y + j][x + i];
                        Y1[y + v][x + i] = std::abs(coeff) / normalization;
                    }
                }
            }
        }
    }
     // 象限の入れ替え処理
    for (int y = 0; y < 720 / 2; y++)
    {
        for (int x = 0; x < 720 / 2; x++)
        {
            // 第1象限 (左上) と第3象限 (右下) の入れ替え
            swap(Y1[y][x], Y1[y + 720 / 2][x + 720 / 2]);

            // 第2象限 (右上) と第4象限 (左下) の入れ替え
            swap(Y1[y][x + 720 / 2], Y1[y + 720 / 2][x]);
        }
    }

    // ノルムにする
    for (y = 0; y < PH; y++)
    {
        for (x = 0; x < PW; x++)
        {
            Y1[y][x] = std::abs(Y1[y][x]);
        }
    }

    //------------------スペクトル出力(線付き)----------------------------

    for (y = 0; y < PH; y++)
    {
        for (x = 0; x < PW; x++)
        {
            double pic = Y1[y][x];
            if (pic < 0)
                pic = 0;
            else if (pic > 255)
                pic = 255;
            fputc((unsigned char)pic, fp_out1);
        }
    }

    for (y = 0; y < PH ; y++)
    {
        for (x = 0; x < PW; x++)
        {
            int pic = Cb1[y][x];
            if (pic < 0)
                pic = 0;
            else if (pic > 255)
                pic = 255;
            fputc((unsigned char)128, fp_out1);
        }
    }

    for (y = 0; y < PH; y++)
    {
        for (x = 0; x < PW; x++)
        {
            int pic = Cr1[y][x];
            if (pic < 0)
                pic = 0;
            else if (pic > 255)
                pic = 255;
            fputc((unsigned char)128, fp_out1);
        }
    }

    fclose(fp_in1);
    fclose(fp_out1);
}

/*
satoudaiki@satoudaikinoMacBook-Pro graduation_thesis % cd "/Users/satoudaiki/source/graduation_thesis/" && g++ main.cpp -o main && "/Users/satoudaiki/source/graduation_thesis/"main > output.txt
ディレクトリ移動 & main.cppをコンパイルしてmainファイルを作成 & mainを実行 & コンソールの中身をテキストファイルに書き込み
*/

/*
YUView と Code Runnerをインストール
YCbCrのYUVファイルはYUV Viewで開ける。
設定方法は
>set height 480
>set wide 720
>set chr~~ 420
*/

/*
shift + option + f　でコード整理
*/