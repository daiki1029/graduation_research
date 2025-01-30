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

    const int hdlen1 = 53;
    const int hdlen2 = 57;
    const int hdlen3 = 57;

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

    FILE *fp_in1 = fopen("背景画像.bmp", "rb");
    if (fp_in1 == NULL)
    {
        printf("インプットエラー1\n");
        return -1;
    }

    FILE *fp_out1 = fopen("消す.yuv", "wb");
    if (fp_out1 == NULL)
    {
        printf("アウトプットエラー1\n");
        return -1;
    }
    //------------------背景画像入力----------------------------

    for (y = 0; y < PH; y++)
    {
        for (x = 0; x < PW; x++)
        {
            Y1[y][x] = (unsigned char)fgetc(fp_in1);
        }
    }

    for (y = 0; y < PH / 2; y++)
    {
        for (x = 0; x < PW / 2; x++)
        {
            Cb2[y][x] = (unsigned char)fgetc(fp_in1);
        }
    }

    for (y = 0; y < PH / 2; y++)
    {
        for (x = 0; x < PW / 2; x++)
        {
            Cr2[y][x] = (unsigned char)fgetc(fp_in1);
        }
    }

    //------------------画像合成----------------------------

    // 一次元水平フィルター作成
    int length = 5;
    double degre = 10;

    for (int i = 0; i < length; i++)
    {
        F1[22][i + 15] = 1.0 / length;
    }

    double ds = sin(degre * M_PI / -180.0);
    double dc = cos(degre * M_PI / -180.0);

    // 係数を回転で間引く
    for (y = 0; y < 45; y++)
    {
        for (x = 0; x < 45; x++)
        {
            double dx = dc * (x - 22) - ds * (y - 22) + 22;
            double dy = ds * (x - 22) + dc * (y - 22) + 22;
            if (0 <= dx && dx < 45 && 0 <= dy && dy < 45)
            {

                int ix = int(dx);
                int iy = int(dy);
                double sx = dx - ix;
                double sy = dy - iy;
                double y1dxdy = 0;
                // xy double型で厳密にやる
                for (int i = 0; i < 15; i++)
                {
                    for (int j = 0; j < 15; j++)
                    {
                        double mvx = M_PI * (j - 7 - sx) + 0.00000001;
                        double sincx = sin(mvx) / (mvx);

                        double mvy = M_PI * (i - 7 - sy) + 0.00000001;
                        double sincy = sin(mvy) / (mvy);

                        y1dxdy += sincy * sincx * F1[iy - 7 + i][ix - 7 + j];
                    }
                }
                F2[y][x] = y1dxdy;
            }
        }
    }

    // 係数正規化
    double ltotal = 0;
    double gototal = 0;
    for (y = 15; y < 30; y++)
    {
        for (x = 15; x < 30; x++)
        {
            ltotal += F2[y][x];
        }
    }
    std::cout << ltotal << std::endl;

    for (y = 15; y < 30; y++)
    {
        for (x = 15; x < 30; x++)
        {
            F2[y][x] = F2[y][x] / ltotal;
            std::cout << F2[y][x] << " ";
        }
        std::cout << std::endl;
    }

    //------------------合成画像出力----------------------------

    for (y = 0; y < PH; y++)
    {
        for (x = 0; x < PW; x++)
        {
            int pic = Y1[y][x];
            fputc((unsigned char)pic, fp_out1);
        }
    }

    for (y = 0; y < PH / 2; y++)
    {
        for (x = 0; x < PW / 2; x++)
        {
            int pic = Cb2[y][x];
            fputc((unsigned char)pic, fp_out1);
        }
    }

    for (y = 0; y < PH / 2; y++)
    {
        for (x = 0; x < PW / 2; x++)
        {
            int pic = Cr2[y][x];
            fputc((unsigned char)pic, fp_out1);
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
