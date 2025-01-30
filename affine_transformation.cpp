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

    static double Y1[PH][PW] = {128};
    static int Cb1[PH / 2][PW / 2] = {0};
    static int Cr1[PH / 2][PW / 2] = {0};

    int x = 0, y = 0, pic = 0;

    FILE *fp_in1 = fopen("Boy_and_Toys30p.yuv", "rb");
    if (fp_in1 == NULL)
    {
        printf("インプットエラー1\n");
        return -1;
    }

    FILE *fp_out1 = fopen("回転60度_boy_and_toys.yuv", "wb");
    if (fp_out1 == NULL)
    {
        printf("アウトプットエラー1\n");
        return -1;
    }

    for (int y = 0; y < PH; y++)
    {
        for (int x = 0; x < PW; x++)
        {
            Y1[y][x] = (unsigned char)fgetc(fp_in1); // 元画像のデータをコピー
        }
    }

    for (y = 0; y < PH / 2; y++)
    {
        for (x = 0; x < PW / 2; x++)
        {
            Cb1[y][x] = (unsigned char)fgetc(fp_in1);
        }
    }

    for (y = 0; y < PH / 2; y++)
    {
        for (x = 0; x < PW / 2; x++)
        {
            Cr1[y][x] = (unsigned char)fgetc(fp_in1);
        }
    }

    // 回転処理
    int cx = PW / 2;
    int cy = PH / 2;
    static double Ynew[PH + 64][PW + 64] = {0};

    // 30度回転（ラジアンに変換）
    double angle = -M_PI / 3;
    double cosA = cos(angle);
    double sinA = sin(angle);
    for (int ny = 0; ny < PH; ny++)
    {
        for (int nx = 0; nx < PW; nx++)
        {
            // 中心を原点とした座標に変換
            int dx = nx - cx;
            int dy = ny - cy;

            // 逆回転を適用
            int srcx = round(dx * cosA + dy * sinA) + cx;
            int srcy = round(-dx * sinA + dy * cosA) + cy;

            // 元画像の範囲内かチェック
            if (srcx >= 0 && srcx < PW && srcy >= 0 && srcy < PH)
            {
                // バイリニア補間を使用
                int x0 = floor(srcx);
                int y0 = floor(srcy);
                int x1 = min(x0 + 1, PW - 1);
                int y1 = min(y0 + 1, PH - 1);
                
                double fx = srcx - x0;
                double fy = srcy - y0;

                Ynew[ny][nx] = 
                    Y1[y0][x0] * (1 - fx) * (1 - fy) +
                    Y1[y0][x1] * fx * (1 - fy) +
                    Y1[y1][x0] * (1 - fx) * fy +
                    Y1[y1][x1] * fx * fy;
            }
        }
    }

    for (y = 0; y < PH; y++)
    {
        for (x = 0; x < PW; x++)
        {
            double pic = Ynew[y][x];
            if (pic < 0)
                pic = 0;
            else if (pic > 255)
                pic = 255;
            fputc((unsigned char)pic, fp_out1);
        }
    }

    for (y = 0; y < PH / 2; y++)
    {
        for (x = 0; x < PW / 2; x++)
        {
            int pic = Cb1[y][x];
            fputc((unsigned char)128, fp_out1);
        }
    }

    for (y = 0; y < PH / 2; y++)
    {
        for (x = 0; x < PW / 2; x++)
        {
            int pic = Cr1[y][x];
            fputc((unsigned char)128, fp_out1);
        }
    }

    fclose(fp_in1);
    fclose(fp_out1);
}