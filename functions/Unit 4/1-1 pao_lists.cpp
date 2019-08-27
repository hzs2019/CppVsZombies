
// 此文件主要介绍用炮操作类建立多个炮列表以及简单实用
// 需要注意的是：一个炮至多只能被一个炮列表所拥有

#include "pvz.h"

////////////////////////// 使用示例

int main()
{
    // 注意：一个炮至多只能被一个炮列表所拥有，
    // 所以我们需要将 CvZ 自带的炮操作对象的炮列表清零
    UpdatePaoList({}); // 炮列表清零，必须做的一步，不然会报错

    // 创建(实例化)一个炮操作类的对象
    PaoOperator pao_list_1; //这里名字任取，你可以命名为 list_1 或者其他自己认为更舒服的名字

    // 设置 pao_list_1 中的炮列表
    // 使用下面语句后 pao_list_1 中炮列表被重置为 {2, 3}, {2, 4}, {2, 5}
    // 意思就是说，调用 pao_list_1 的成员函数(例如 pao roofPao 等)时，
    // 只会对此炮列表中的炮进行操作
    pao_list_1.resetPaoList({{2, 3}, {2, 4}, {2, 5}});

    //////////////////// 接下来我们再创建一个炮操作对象

    // 设置 pao_list_2 中的炮列表
    // 可以在创建对象的时候进行炮列表的初始化
    // 使用下面语句后 pao_list_1 中炮列表为 {3, 3}, {3, 4}, {3, 5}
    PaoOperator pao_list_2({{3, 3}, {3, 4}, {3, 5}});

    // 当然你也可以这么写
    // PaoOperator pao_list_2;
    // pao_list_2.resetPaoList({{3, 3}, {3, 4}, {3, 5}});

    // 以下语句是错误的！！！
    // PaoOperator pao_list_2({{2, 3}, {3, 4}, {3, 5}}); // It's not allowed !
    // 虽然上面的语句也创建了一个对象，但是 {2, 3} 已被 pao_list_1 占用

    /////////////////// 下面我们可以使用上面我们创建的两个对象

    // 使用 pao_list_1 中的炮
    pao_list_1.pao(2, 9);             // 发射炮的位置为 {2, 3}
    pao_list_1.pao({{3, 9}, {4, 9}}); //发射炮的位置为 {2, 4}, {2, 5}

    //使用 pao_list_2 中的炮
    pao_list_2.pao(2, 9);             // 发射炮的位置为 {3, 3}
    pao_list_2.pao({{3, 9}, {4, 9}}); //发射炮的位置为 {3, 4}, {3, 5}

    // 下面的语句与上面含义相同
    pao_list_1.skipPao(1); // 在 pao_list_1 跳过一门炮
    pao_list_2.skipPao(2); // 在 pao_list_2 跳过两门炮

    pao_list_1.setNextPao(2); // 将 pao_list_1 中的第二门炮设为下一门发射的炮
    pao_list_2.setNextPao(3, 5);// 将 pao_list_1 中位置为 (3, 5) 的炮设为下一门发射的炮

    return 0;
}