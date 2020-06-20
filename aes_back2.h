#ifndef AES_H
#define AES_H

#include <QObject>
#include <QFile>
class AES : public QObject {
    Q_OBJECT
public:
    explicit AES(QObject *parent = nullptr);
    /**
     * 参数 p: 明文的字符串数组。
     * 参数 plen: 明文的长度,长度必须为16的倍数。
     */
    void code(char *p, int plen);
    /**
     * 参数 c: 密文的字符串数组。
     * 参数 clen: 密文的长度,长度必须为16的倍数。
     */
    void decode(char *c, int clen);
    /**
     * 参数 key: 密钥的字符串数组，必须为16字节。
     */
    void setKey(const char *key);
private:
    QFile *file;
    //密钥对应的扩展数组
    int w[44];
    /**
     * S盒
     */
    static const unsigned char S[16][16];
    /**
     * 逆S盒
     */
    static const unsigned char S2[16][16];
    /**
     * 列混合要用到的矩阵
     */
    static const int colM[4][4];
    /**
     * 逆列混合用到的矩阵
     */
    static const int deColM[4][4];
    /**
     * 常量轮值表
     */
    static const int Rcon[10];
    /**
     * 有限域 GF(28) 上的乘法查表法
     */
    static const unsigned char Mul_02[256];
    static const unsigned char Mul_03[256];
    static const unsigned char Mul_09[256];
    static const unsigned char Mul_0b[256];
    static const unsigned char Mul_0d[256];
    static const unsigned char Mul_0e[256];
    /**
     * 检查密钥长度
     */
    int checkKeyLen(int len);
    /**
     * 扩展密钥，结果是把w[44]中的每个元素初始化
     */
    void extendKey(const char *key);
    /**
     * 把16个字符转变成4X4的数组，
     * 该矩阵中字节的排列顺序为从上到下，
     * 从左到右依次排列。
     */
    void convertToIntArray(char *str, int pa[4][4]);
    /**
     * 轮密钥加
     */
    void addRoundKey(int array[4][4], int round);
    /**
     * 字节代换
     */
    void subBytes(int array[4][4]);
    /**
     * 逆字节变换
     */
    void deSubBytes(int array[4][4]);
    /**
     * 行移位
     */
    void shiftRows(int array[4][4]);
    /**
     * 逆行移位
     */
    void deShiftRows(int array[4][4]);
    /**
     * 列混合
     */
    void mixColumns(int array[4][4]);
    /**
     * 逆列混合
     */
    void deMixColumns(int array[4][4]);
    /**
     * 把4X4数组转回字符串
     */
    static void convertArrayToStr(int array[4][4], char *str);
    /**
     * 从4个32位的密钥字中获得4X4数组，
     * 用于进行逆列混合
     */
    void getArrayFrom4W(int i, int array[4][4]);
    /**
     * 把连续的4个字符合并成一个4字节的整型
     */
    int getWordFromStr(char *str);
    /**
     * 把一个字符转变成整型
     */
    int getIntFromChar(char c);
    /**
     * 根据索引，从S盒中获得元素
     */
    int getNumFromSBox(int index);
    /**
     * 根据索引从逆S盒中获取值
     */
    int getNumFromS1Box(int index);
    /**
     * 将数组中的元素循环左移step位
     */
    void leftLoop4int(int array[4], int step);
    /**
     * 把4个元素的数组循环右移step位
     */
    void rightLoop4int(int array[4], int step);
    /**
     * 获取整形数据的低8位的左4个位
     */
    int getLeft4Bit(int num);
    /**
     * 获取整形数据的低8位的右4个位
     */
    int getRight4Bit(int num);
    /**
     * 把一个4字节的数的第一、二、三、四个字节取出，
     * 入进一个4个元素的整型数组里面。
     */
    void splitIntToArray(int num, int array[4]);
    /**
     * 把数组中的第一、二、三和四元素分别作为
     * 4字节整型的第一、二、三和四字节，合并成一个4字节整型
     */
    int mergeArrayToInt(int array[4]);
    /**
     * 把两个4X4数组进行异或
     */
    void addRoundTowArray(int aArray[4][4], int bArray[4][4]);
    /**
     * 密钥扩展中的T函数
     */
    int T(int num, int round);
    /**
     * GF上的二元运算
     */
    int GFMul(int n, int s);
    int GFMul2(int s);
    int GFMul3(int s);
    int GFMul4(int s);
    int GFMul8(int s);
    int GFMul9(int s);
    int GFMul11(int s);
    int GFMul12(int s);
    int GFMul13(int s);
    int GFMul14(int s);
};

#endif // AES_H
