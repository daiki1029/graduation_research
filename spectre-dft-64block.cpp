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

    FILE *fp_in1 = fopen("slant_line_ai_image.bmp", "rb");
    if (fp_in1 == NULL)
    {
        printf("インプットエラー1\n");
        return -1;
    }

    FILE *fp_in2 = fopen("バイク画像.bmp", "rb");
    if (fp_in2 == NULL)
    {
        printf("インプットエラー2\n");
        return -1;
    }

    FILE *fp_in3 = fopen("slant_line_ai_image.bmp", "rb");
    if (fp_in3 == NULL)
    {
        printf("インプットエラー3\n");
        return -1;
    }

    FILE *fp_out1 = fopen("消す予定1.yuv", "wb");
    if (fp_out1 == NULL)
    {
        printf("アウトプットエラー1\n");
        return -1;
    }

    FILE *fp_out2 = fopen("slant_line_ai_image_64block.yuv", "wb");
    if (fp_out2 == NULL)
    {
        printf("アウトプットエラー2\n");
        return -1;
    }

    FILE *fp_out3 = fopen("slant_line_ai_image_dft_result_64block.yuv", "wb");
    if (fp_out3 == NULL)
    {
        printf("アウトプットエラー3\n");
        return -1;
    }

    // fp_out4はRGBのままのためYUViewで表示できない
    FILE *fp_out4 = fopen("消す予定3.yuv", "wb");
    if (fp_out4 == NULL)
    {
        printf("アウトプットエラー4\n");
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
            A1[PH - 1 - y + 16][x] = (unsigned char)fgetc(fp_in1);
            B1[PH - 1 - y + 16][x] = (unsigned char)fgetc(fp_in1);
            G1[PH - 1 - y + 16][x] = (unsigned char)fgetc(fp_in1);
            R1[PH - 1 - y + 16][x] = (unsigned char)fgetc(fp_in1);
        }
    }

    //------------------オブジェクト画像入力--------------------

    for (x = 0; x < hdlen2; x++)
    {
        HD2[x] = (unsigned char)fgetc(fp_in2);
    }

    for (y = 8; y < PH + 8; y++)
    {
        for (x = 8; x < PW + 8; x++)
        {
            A2[PH - 1 - y + 16][x] = (unsigned char)fgetc(fp_in2);
            B2[PH - 1 - y + 16][x] = (unsigned char)fgetc(fp_in2);
            G2[PH - 1 - y + 16][x] = (unsigned char)fgetc(fp_in2);
            R2[PH - 1 - y + 16][x] = (unsigned char)fgetc(fp_in2);
        }
    }

    //------------------オブジェクト拡大画像入力----------------

    for (x = 0; x < hdlen3; x++)
    {
        HD3[x] = (unsigned char)fgetc(fp_in3);
    }

    for (y = 0; y < PH; y++)
    {
        for (x = 0; x < PW; x++)
        {
            B3[PH - 1 - y][x] = (unsigned char)fgetc(fp_in3);
            G3[PH - 1 - y][x] = (unsigned char)fgetc(fp_in3);
            R3[PH - 1 - y][x] = (unsigned char)fgetc(fp_in3);
        }
    }

    for (y = 0; y < PH; y++)
    {
        for (x = 0; x < PW; x++)
        {
            Y1[y][x] = 299 * R3[y][x] + 587 * G3[y][x] + 114 * B3[y][x];
            Cb1[y][x] = -169 * R3[y][x] - 331 * G3[y][x] + 500 * B3[y][x];
            Cr1[y][x] = 500 * R3[y][x] - 419 * G3[y][x] - 81 * B3[y][x];
        }
    }

    for (y = 0; y < PH; y++)
    {
        for (x = 0; x < PW; x++)
        {
            Y1[y][x] = 219 * Y1[y][x] / 255000 + 16;
            Cb1[y][x] = 224 * Cb1[y][x] / 255000 + 128;
            Cr1[y][x] = 224 * Cr1[y][x] / 255000 + 128;

            Y3[y][x] = Y1[y][x];
        }
    }

    for (y = 0; y < PH / 2; y++)
    {
        for (x = 0; x < PW / 2; x++)
        {
            Cb2[y][x] = (Cb1[2 * y][2 * x] + Cb1[2 * y + 1][2 * x] + Cb1[2 * y][2 * x + 1] + Cb1[2 * y + 1][2 * x + 1]) / 4;
            Cr2[y][x] = (Cr1[2 * y][2 * x] + Cr1[2 * y + 1][2 * x] + Cr1[2 * y][2 * x + 1] + Cr1[2 * y + 1][2 * x + 1]) / 4;

            Cb3[y][x] = Cb2[y][x];
            Cr3[y][x] = Cr2[y][x];
        }
    }

    //------------------スペクトル計算----------------------------

    const int blockSize = 64;
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

    // ファイル出力用の象限操作
    for (int by = 0; by < PH; by += blockSize)
    {
        for (int bx = 0; bx < PW; bx += blockSize)
        {

            double block[blockSize][blockSize];

            for (int y = 0; y < blockSize; y++)
            {
                for (int x = 0; x < blockSize; x++)
                {
                    block[y][x] = Y1[by + y][bx + x];
                }
            }

            // 第1象限 <-> 第3象限
            for (int y = 0; y < 32; y++)
            {
                for (int x = 32; x < 64; x++)
                {
                    swap(block[y][x], block[y + 32][x - 32]);
                }
            }

            // 第2象限と第4象限を入れ替え
            for (int y = 0; y < 32; y++)
            {
                for (int x = 0; x < 32; x++)
                {
                    swap(block[y][x], block[y + 32][x + 32]);
                }
            }

            for (int y = 0; y < blockSize; y++)
            {
                for (int x = 0; x < blockSize; x++)
                {
                    Y1[by + y][bx + x] = block[y][x];
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

    int ref_y = 0;
    int ref_x = 0;

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
                int cx = blockSize / 2;
                int cy = blockSize / 2;

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

                // マスクと重なっている部分の値を取得して和を計算
                for (int y = 0; y < maskHeight; ++y)
                {
                    for (int x = -maskWidth / 2; x <= maskWidth / 2; ++x)
                    {
                        int maskX = blockSize / 2 + x; // マスクの中心を基準にx座標を取得
                        int maskY = y;
                        if (maskX >= 0 && maskX < blockSize && maskY >= 0 && maskY < blockSize)
                        {
                            sum += rotatedBlock[maskY][maskX];
                            eachSum[angle / 10] += rotatedBlock[maskY][maskX];
                        }
                    }
                }
                cout << angle << "度の時、" << sum << endl;
            }

            for (int i = 0; i < 18; ++i)
            {
                if (eachSum[i] > maxSum)
                {
                    maxSum = eachSum[i];
                    bestAngle = i * 10;
                }
            }

            std::cout << endl
                      << "Best angle for block at (" << by / blockSize << ", " << bx / blockSize << "): "
                      << bestAngle << " degrees with sum: " << maxSum << std::endl
                      << "よって、オブジェクトを" << bestAngle << "度方向にブラします" << std::endl
                      << "---------------------------------------------------" << std::endl;
        }
    }

    //------------------画像合成----------------------------

    // 一次元水平フィルター作成
    int length = valueRatio;
    // valueRatio * 30000000;

    for (int i = 0; i < length; i++)
    {
        F1[22][i + 15] = 1.0 / length;
    }

    double ds = sin(degre * M_PI / 180.0);
    double dc = cos(degre * M_PI / 180.0);

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

    for (y = 15; y < 30; y++)
    {
        for (x = 15; x < 30; x++)
        {
            F2[y][x] = F2[y][x] / ltotal;
        }
    }

    int max_value;
    for (y = 8; y < PH + 8; y++)
    {
        for (x = 8; x < PW + 8; x++)
        {
            max_value = std::max(std::max(R2[y][x], G2[y][x]), B2[y][x]);
            if (max_value != 0)
            {
                A2[y][x] = 255;
            }
            else
            {
                A2[y][x] = 0;
            }
        }
    }

    // F2はフィルター係数が入っている
    for (int y = 8; y < PH + 8; y++)
    {
        for (int x = 8; x < PW + 8; x++)
        {
            double AT = 0.0;
            double RT = 0.0;
            double GT = 0.0;
            double BT = 0.0;
            for (int k = 15; k < 30; k++)
            {
                for (int l = 15; l < 30; l++)
                {
                    // R5の範囲をy-7からy+7, x-7からx+7に調整
                    AT += A2[y + (k - 22)][x + (l - 22)] * F2[k][l];
                    RT += R2[y + (k - 22)][x + (l - 22)] * F2[k][l];
                    GT += G2[y + (k - 22)][x + (l - 22)] * F2[k][l];
                    BT += B2[y + (k - 22)][x + (l - 22)] * F2[k][l];
                }
            }
            A2[y][x] = int(AT + 0.5);
            R2[y][x] = int(RT + 0.5);
            G2[y][x] = int(GT + 0.5);
            B2[y][x] = int(BT + 0.5);
        }
    }

    // aブレンド
    for (y = 8; y < PH + 8; y++)
    {
        for (x = 8; x < PW + 8; x++)
        {
            R4[y][x] = (R2[y][x] * 255 + R1[y][x] * (255 - A2[y][x]) + 128) / 255;
            G4[y][x] = (G2[y][x] * 255 + G1[y][x] * (255 - A2[y][x]) + 128) / 255;
            B4[y][x] = (B2[y][x] * 255 + B1[y][x] * (255 - A2[y][x]) + 128) / 255;
        }
    }

    // RGBからYUVへの変換
    for (y = 8; y < PH + 8; y++)
    {
        for (x = 8; x < PW + 8; x++)
        {
            Y4[y - 8][x - 8] = 299 * R4[y][x] + 587 * G4[y][x] + 114 * B4[y][x];
            Cb4[y - 8][x - 8] = -169 * R4[y][x] - 331 * G4[y][x] + 500 * B4[y][x];
            Cr4[y - 8][x - 8] = 500 * R4[y][x] - 419 * G4[y][x] - 81 * B4[y][x];
        }
    }

    // YUVの正規化
    for (y = 0; y < PH; y++)
    {
        for (x = 0; x < PW; x++)
        {
            Y4[y][x] = 219 * Y4[y][x] / 255000 + 16;
            Cb4[y][x] = 224 * Cb4[y][x] / 255000 + 128;
            Cr4[y][x] = 224 * Cr4[y][x] / 255000 + 128;
        }
    }

    // クロマサブサンプリング（4:2:0）
    for (y = 0; y < PH / 2; y++)
    {
        for (x = 0; x < PW / 2; x++)
        {
            Cb5[y][x] = (Cb4[2 * y][2 * x] + Cb4[2 * y + 1][2 * x] + Cb4[2 * y][2 * x + 1] + Cb4[2 * y + 1][2 * x + 1]) / 4;
            Cr5[y][x] = (Cr4[2 * y][2 * x] + Cr4[2 * y + 1][2 * x] + Cr4[2 * y][2 * x + 1] + Cr4[2 * y + 1][2 * x + 1]) / 4;
        }
    }

    //------------------合成画像出力----------------------------

    for (y = 0; y < PH; y++)
    {
        for (x = 0; x < PW; x++)
        {
            int pic = Y4[y][x];
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
            int pic = Cb5[y][x];
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
            int pic = Cr5[y][x];
            if (pic < 0)
                pic = 0;
            else if (pic > 255)
                pic = 255;

            fputc((unsigned char)pic, fp_out1);
        }
    }

    //------------------バイク拡大画像(線付き)----------------------------

    for (y = 0; y < PH; y++)
    {
        for (x = 0; x < PW; x++)
        {
            double pic = Y3[y][x];
            if (pic < 0)
                pic = 0;
            else if (pic > 255)
                pic = 255;
            fputc((unsigned char)pic, fp_out2);
        }
    }

    for (y = 0; y < PH / 2; y++)
    {
        for (x = 0; x < PW / 2; x++)
        {
            int pic = Cb3[y][x];
            if (pic < 0)
                pic = 0;
            else if (pic > 255)
                pic = 255;

            // xまたはyが64の倍数のときに線を書きたい
            if (x % 32 == 0 || y % 32 == 0)
            {
                fputc((unsigned char)0, fp_out2); // 例えば0で線を書く場合
            }
            else
            {
                fputc((unsigned char)pic, fp_out2);
            }
        }
    }

    for (y = 0; y < PH / 2; y++)
    {
        for (x = 0; x < PW / 2; x++)
        {
            int pic = Cr3[y][x];
            if (pic < 0)
                pic = 0;
            else if (pic > 255)
                pic = 255;

            // xまたはyが64の倍数のときに線を書きたい
            if (x % 32 == 0 || y % 32 == 0)
            {
                fputc((unsigned char)0, fp_out2); // 例えば0で線を書く場合
            }
            else
            {
                fputc((unsigned char)pic, fp_out2);
            }
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
            fputc((unsigned char)pic, fp_out3);
        }
    }

    for (y = 0; y < PH / 2; y++)
    {
        for (x = 0; x < PW / 2; x++)
        {
            int pic = Cb2[y][x];
            if (pic < 0)
                pic = 0;
            else if (pic > 255)
                pic = 255;
            fputc((unsigned char)128, fp_out3);
        }
    }

    for (y = 0; y < PH / 2; y++)
    {
        for (x = 0; x < PW / 2; x++)
        {
            int pic = Cr2[y][x];
            if (pic < 0)
                pic = 0;
            else if (pic > 255)
                pic = 255;
            fputc((unsigned char)128, fp_out3);
        }
    }

    //------------------元画像チェック----------------------------

    for (y = 8; y < PH + 8; y++)
    {
        for (x = 8; x < PW + 8; x++)
        {
            int pic = R1[y][x];
            if (pic < 0)
                pic = 0;
            else if (pic > 255)
                pic = 255;

            fputc((unsigned char)pic, fp_out4);
        }
    }

    for (y = 8; y < PH + 8; y++)
    {
        for (x = 8; x < PW + 8; x++)
        {
            int pic = G1[y][x];
            if (pic < 0)
                pic = 0;
            else if (pic > 255)
                pic = 255;

            fputc((unsigned char)pic, fp_out4);
        }
    }

    for (y = 8; y < PH + 8; y++)
    {
        for (x = 8; x < PW + 8; x++)
        {
            int pic = B1[y][x];
            if (pic < 0)
                pic = 0;
            else if (pic > 255)
                pic = 255;

            fputc((unsigned char)pic, fp_out4);
        }
    }

    fclose(fp_in1);
    fclose(fp_in2);
    fclose(fp_in3);

    fclose(fp_out1);
    fclose(fp_out2);
    fclose(fp_out3);
    fclose(fp_out4);
}
