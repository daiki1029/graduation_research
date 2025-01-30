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
    const int PH = 480, PW = 720;

    const int hdlen1 = 57;

    unsigned char HD1[100] = {0};
    unsigned char HD2[100] = {0};
    unsigned char HD3[100] = {0};

    static int A1[PH + 16][PW + 16] = {0};
    static int R1[PH + 16][PW + 16] = {0};
    static int G1[PH + 16][PW + 16] = {0};
    static int B1[PH + 16][PW + 16] = {0};

    static int A2[PH + 16][PW + 16] = {0};
    static int R2[PH + 16][PW + 16] = {0};
    static int G2[PH + 16][PW + 16] = {0};
    static int B2[PH + 16][PW + 16] = {0};

    static int A4[PH + 16][PW + 16] = {0};
    static int R4[PH + 16][PW + 16] = {0};
    static int G4[PH + 16][PW + 16] = {0};
    static int B4[PH + 16][PW + 16] = {0};

    static int R3[PH][PW] = {0};
    static int G3[PH][PW] = {0};
    static int B3[PH][PW] = {0};

    static int temp_A5[PH + 16][PW + 16] = {0};
    static int temp_R5[PH + 16][PW + 16] = {0};
    static int temp_G5[PH + 16][PW + 16] = {0};
    static int temp_B5[PH + 16][PW + 16] = {0};

    static double Y1[PH + 64][PW + 64] = {0};
    static int Cb1[PH][PW] = {0};
    static int Cr1[PH][PW] = {0};
    static int Cb2[PH / 2][PW / 2] = {0};
    static int Cr2[PH / 2][PW / 2] = {0};

    static double Y3[PH + 64][PW + 64] = {0};
    static int Cb3[PH / 2][PW / 2] = {0};
    static int Cr3[PH / 2][PW / 2] = {0};

    static double Y4[PH][PW] = {0};
    static int Cb4[PH][PW] = {0};
    static int Cr4[PH][PW] = {0};
    static int Cb5[PH / 2][PW / 2] = {0};
    static int Cr5[PH / 2][PW / 2] = {0};

    static double F1[90][90] = {0};
    static double F2[90][90] = {0};

    int x = 0, y = 0, pic = 0;

    FILE *fp_in1 = fopen("ai_image.bmp", "rb");
    if (fp_in1 == NULL)
    {
        printf("インプットエラー1\n");
        return -1;
    }

    FILE *fp_out1 = fopen("ai_image_1block.yuv", "wb");
    if (fp_out1 == NULL)
    {
        printf("アウトプットエラー1\n");
        return -1;
    }

    //------------------背景画像入力----------------------------

    for (x = 0; x < hdlen1; x++)
    {
        HD1[x] = (unsigned char)fgetc(fp_in1);
    }

    for (y = 8; y < PH + 8; y++)
    {
        for (x = 8; x < PW + 8; x++)
        {
            B1[PH - 1 - y + 16][x] = (unsigned char)fgetc(fp_in1);
            G1[PH - 1 - y + 16][x] = (unsigned char)fgetc(fp_in1);
            R1[PH - 1 - y + 16][x] = (unsigned char)fgetc(fp_in1);
        }
    }

    //------------------オブジェクト画像入力--------------------

    for (y = 0; y < PH; y++)
    {
        for (x = 0; x < PW; x++)
        {
            Y1[y][x] = 299 * R1[y][x] + 587 * G1[y][x] + 114 * B1[y][x];
            Cb1[y][x] = -169 * R1[y][x] - 331 * G1[y][x] + 500 * B1[y][x];
            Cr1[y][x] = 500 * R1[y][x] - 419 * G1[y][x] - 81 * B1[y][x];
        }
    }

    for (y = 0; y < PH; y++)
    {
        for (x = 0; x < PW; x++)
        {
            Y1[y][x] = 219 * Y1[y][x] / 255000 + 16;
            Cb1[y][x] = 224 * Cb1[y][x] / 255000 + 128;
            Cr1[y][x] = 224 * Cr1[y][x] / 255000 + 128;
        }
    }

    for (y = 0; y < PH / 2; y++)
    {
        for (x = 0; x < PW / 2; x++)
        {
            Cb2[y][x] = (Cb1[2 * y][2 * x] + Cb1[2 * y + 1][2 * x] + Cb1[2 * y][2 * x + 1] + Cb1[2 * y + 1][2 * x + 1]) / 4;
            Cr2[y][x] = (Cr1[2 * y][2 * x] + Cr1[2 * y + 1][2 * x] + Cr1[2 * y][2 * x + 1] + Cr1[2 * y + 1][2 * x + 1]) / 4;
        }
    }

    //------------------スペクトル計算----------------------------

    const int blockSize = 64;
    const int normalization = 32;

    static double dct_result[PH + 64][PW + 64] = {0}; // 画像全体のDCT結果を保持
    double dct_filter[blockSize + 64][blockSize + 64] = {0};
    static double Y2[PH + 64][PW + 64] = {0};

    int i, j, u, v = 0;
    double coeff = 0;

    // cos変換の係数
    for (int u = 0; u < blockSize; u++)
    {
        for (int x = 0; x < blockSize; x++)
        {
            dct_filter[u][x] += cos(((2 * x + 1) * u * M_PI) / (2.0 * blockSize));
            if (u == 0)
            {
                dct_filter[u][x] *= sqrt(2.0) / 2.0;
            }
        }
    }

    // cos変換
    for (y = 0; y < PH; y += blockSize)
    {
        for (x = 0; x < PW; x += blockSize)
        {

            // 水平方向フィルター適応
            for (j = 0; j < blockSize; j++)
            {
                for (u = 0; u < blockSize; u++)
                {
                    coeff = 0;
                    for (i = 0; i < blockSize; i++)
                    {
                        coeff += dct_filter[u][i] * Y1[y + j][x + i];
                        Y2[y + j][x + u] = coeff * 1000;
                    }
                }
            }

            // 垂直方向フィルター適応
            for (i = 0; i < blockSize; i++)
            {
                for (v = 0; v < blockSize; v++)
                {
                    coeff = 0;
                    for (j = 0; j < blockSize; j++)
                    {
                        coeff += dct_filter[v][j] * Y2[y + j][x + i];
                        Y1[y + v][x + i] = coeff / 1000.0;
                        Y1[y + v][x + i] = std::abs(Y1[y + v][x + i]) / normalization;
                    }
                }
            }
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

    double degre = 0;
    double vertical_degre = 0;
    double valueRatio = 0.0;

    const int maskWidth = 3;
    const int maskHeight = 64;

    for (int by = 0; by < PH; by += blockSize)
    {
        for (int bx = 0; bx < PW; bx += blockSize)
        {

            // 64x64 ブロックごとに処理 (double 型)
            double block[blockSize][blockSize] = {0};
            double maxSum = 0;
            int bestAngle = 0;

            double eachSum[18] = {0};

            // ブロック内のデータを抽出
            for (int y = 0; y < blockSize; y++)
            {
                for (int x = 0; x < blockSize; x++)
                {
                    block[y][x] = Y1[by + y][bx + x];
                }
            }

            // 真ん中の 5×5 ブロックを 0 にする
            int center = blockSize / 2;
            for (int y = center - 2; y <= center + 2; y++)
            {
                for (int x = center - 2; x <= center + 2; x++)
                {
                    block[y][x] = 0; // 値を 0 に設定
                }
            }

            // 10度ごとに回転させて最もブロックの和が大きい角度を見つける
            for (int angle = 0; angle < 180; angle += 10)
            {
                double rotatedBlock[blockSize][blockSize] = {0};
                double sum = 0;

                // 回転処理
                double radians = angle * M_PI / 180.0;
                double cosA = cos(radians);
                double sinA = sin(radians);
                int cx = 0;
                int cy = 0;

                for (int y = 0; y < blockSize; y++)
                {
                    for (int x = 0; x < blockSize; x++)
                    {
                        int dx = x - cx;
                        int dy = y - cy;

                        // 新しい座標を計算
                        int newx = round(dx * cosA - dy * sinA) + cx;
                        int newy = round(dx * sinA + dy * cosA) + cy;

                        // 新しい座標が画像範囲内にあるかチェック
                        if (newx >= 0 && newx < blockSize && newy >= 0 && newy < blockSize)
                        {
                            rotatedBlock[newy][newx] = block[y][x];
                        }
                    }
                }
                // 特定の回転の画像を出力
                if (angle == 70)
                {
                    for (int y = 0; y < blockSize; y++)
                    {
                        for (int x = 0; x < blockSize; x++)
                        {
                            int pic = rotatedBlock[y][x];
                            if (pic < 0)
                                pic = 0;
                            else if (pic > 255)
                                pic = 255;
                            fputc((unsigned char)pic, fp_out1);
                        }
                    }

                    for (y = 0; y < blockSize / 2; y++)
                    {
                        for (x = 0; x < blockSize / 2; x++)
                        {
                            int pic = Cb1[y][x];
                            if (pic < 0)
                                pic = 0;
                            else if (pic > 255)
                                pic = 255;

                            fputc((unsigned char)128, fp_out1);
                        }
                    }

                    for (y = 0; y < blockSize / 2; y++)
                    {
                        for (x = 0; x < blockSize / 2; x++)
                        {
                            int pic = Cr1[y][x];
                            if (pic < 0)
                                pic = 0;
                            else if (pic > 255)
                                pic = 255;

                            fputc((unsigned char)128, fp_out1);
                        }
                    }
                }
            }
        }
    }

    fclose(fp_in1);

    fclose(fp_out1);
}
