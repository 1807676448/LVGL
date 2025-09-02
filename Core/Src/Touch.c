#include "Touch.h" // 包含触屏相关头文件（定义实际项目路径调整）
#include "My_Debug.h"
// 触屏设备结构体初始化（包含函数指针和各种状态变量）
_m_tp_dev tp_dev =
    {
        TP_Init, // 初始化函数指针
        TP_Scan, // 扫描函数指针
        0,       // x：当前X坐标
        0,       // y：当前Y坐标
        0,       // x0：首次按下X坐标
        0,       // y0：首次按下Y坐标
        0,       // sta：触屏状态标志
        1,       // xfac：X轴校准系数
        1,       // yfac：Y轴校准系数
        0,       // xoff：X轴偏移量
        0,       // yoff：Y轴偏移量
        0,       // touchtype：触屏类型（0：X/Y与屏幕同向，1：反向）
};

// 触屏读取命令（默认值，根据触屏类型可能动态调整）
// touchtype=0时：X轴读取命令为0XD0，Y轴读取命令为0X90
uint8_t CMD_RDX = 0XD0; // 读X坐标命令
uint8_t CMD_RDY = 0x90; // 读Y坐标命令

/**
 * @brief  通过类SPI总线向触屏IC写入一个字节数据
 * @param  num: 要写入的8位数据
 * @retval 无
 * @note   采用软件模拟SPI时序，通过TDIN引脚发送数据，TCLK引脚提供时钟（上升沿有效）
 */
void TP_Write_Byte(uint8_t num)
{
    uint8_t count = 0; // 循环计数变量（用于发送8位数据）

    // 循环8次，逐位发送数据（从最高位到最低位）
    for (count = 0; count < 8; count++)
    {
        // 判断当前最高位是否为1，设置TDIN引脚电平
        if (num & 0x80) // 最高位为1
        {
            TDIN(1); // 数据线置高
            HAL_Delay_Us(1);
        }
        else // 最高位为0
        {
            TDIN(0); // 数据线置低
            HAL_Delay_Us(1);
        }

        num <<= 1; // 数据左移一位，将次高位移到最高位

        TCLK(0); // 拉低时钟线，准备产生上升沿
        HAL_Delay_Us(1);
        TCLK(1); // 拉高时钟线，产生上升沿（触屏IC在上升沿采样数据）
        HAL_Delay_Us(1);
    }
}

/**
 * @brief  读取触屏AD转换值
 * @param  CMD: AD转换命令（读取X坐标或Y坐标的命令，如CMD_RDX/CMD_RDY）
 * @retval 16位AD转换结果（仅高12位有效，低4位无用，最终会右移4位处理）
 * @note   操作对象为ADS7846触屏IC，通过SPI类似的时序（TCLK/TDIN/DOUT）实现数据交互
 */
uint16_t TP_Read_AD(uint8_t CMD)
{
    uint8_t count = 0; // 循环计数变量，用于读取16位数据
    uint16_t Num = 0;  // 存储AD转换结果

    TCLK(0); // 拉低时钟线，准备发送命令
    TDIN(0); // 拉低数据线，初始化数据状态
    TCS(0);  // 拉低片选线，选中ADS7846触屏IC（开始通信）

    TP_Write_Byte(CMD); // 向ADS7846发送AD转换命令（如读X或读Y）

    HAL_Delay_Us(8); // 等待AD转换完成，ADS7846最大转换时间为6微秒（此处用毫秒延迟兼容，实际可优化为微秒级）

    TCLK(0);         // 拉低时钟，准备清除BUSY状态
    HAL_Delay_Us(2); // 短暂延时，确保电平稳定
    TCLK(1);         // 拉高时钟，产生一个时钟脉冲，清除ADS7846的BUSY标志
    HAL_Delay_Us(1);
    TCLK(0); // 拉低时钟，准备读取AD转换结果

    // 循环16次，逐位读取AD转换结果（ADS7846输出16位数据，高12位有效）
    for (count = 0; count < 16; count++)
    {
        Num <<= 1; // 结果左移1位，腾出最低位用于接收新数据
        TCLK(0);   // 拉低时钟（下降沿有效，ADS7846在下降沿更新DOUT数据）
        HAL_Delay_Us(2);
        TCLK(1); // 拉高时钟，完成一次数据采样
        HAL_Delay_Us(1);
        if (DOUT)  // 读取DOUT引脚状态（1或0），若为1则当前位为1
            Num++; // 最低位置1（Num初始为0，加1等价于最低位置1）
    }

    Num >>= 4; // 右移4位，丢弃低4位无效数据，保留高12位有效AD值
    HAL_Delay_Us(1);
    TCS(1); // 拉高片选线，释放ADS7846（结束通信）

    return Num; // 返回处理后的AD值（仅高12位有效）
}

// 触屏坐标读取配置参数
#define READ_TIMES 5 // 单次坐标读取的采样次数（连续读5次，用于后续去极值滤波）
#define LOST_VAL 1   // 滤波时丢弃的极值个数（丢弃1个最大值和1个最小值，保留中间3个值）

/**
 * @brief  读取触屏X或Y坐标（带滤波处理，提高稳定性）
 * @param  xy: 坐标读取命令（CMD_RDX：读X坐标，CMD_RDY：读Y坐标）
 * @retval 滤波后的坐标值（多次采样→排序→去极值→求平均，降低随机误差）
 * @note   核心是通过"多次采样+排序去极值+平均"的滤波算法，减少触屏采样的抖动
 */
uint16_t TP_Read_XOY(uint8_t xy)
{
    uint16_t i, j;            // 循环变量
    uint16_t buf[READ_TIMES]; // 存储多次采样的AD值（大小为READ_TIMES=5）
    uint16_t sum = 0;         // 存储有效采样值的总和（用于求平均）
    uint16_t temp;            // 临时变量，用于排序时交换数据

    // 第一步：连续读取READ_TIMES次AD值，存入缓冲区
    for (i = 0; i < READ_TIMES; i++)
    {
        buf[i] = TP_Read_AD(xy); // 每次采样调用TP_Read_AD获取原始AD值
    }

    // 第二步：对缓冲区的AD值进行升序排序（冒泡排序）
    for (i = 0; i < READ_TIMES - 1; i++)
    {
        for (j = i + 1; j < READ_TIMES; j++)
        {
            if (buf[i] > buf[j]) // 若前一个值大于后一个值，交换位置（确保从小到大排序）
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }

    // 第三步：丢弃LOST_VAL个最大值和LOST_VAL个最小值，计算中间值的总和
    sum = 0;
    for (i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++)
        sum += buf[i]; // 累加中间3个值（5次采样→丢弃1个最大+1个最小→剩3个）

    // 第四步：求平均值，得到滤波后的坐标值
    temp = sum / (READ_TIMES - 2 * LOST_VAL); // 除以有效数据个数（3）
    return temp;
}

/**
 * @brief  读取触屏的X和Y坐标（基础读取函数）
 * @param  x: 指向存储X坐标的变量指针（输出参数，存储读取到的X坐标）
 * @param  y: 指向存储Y坐标的变量指针（输出参数，存储读取到的Y坐标）
 * @retval 0：读数失败（原代码中判断条件被注释，当前默认返回成功）；1：读数成功
 * @note   内部调用TP_Read_XOY分别读取X和Y坐标，原代码中"坐标值<100则失败"的判断被注释，需根据实际需求启用
 */
uint8_t TP_Read_XY(uint16_t *x, uint16_t *y)
{
    uint16_t xtemp, ytemp; // 临时存储X和Y坐标的AD值

    xtemp = TP_Read_XOY(CMD_RDX); // 读取滤波后的X坐标AD值
    ytemp = TP_Read_XOY(CMD_RDY); // 读取滤波后的Y坐标AD值

    // if(xtemp<100||ytemp<100)return 0;// 注释：若坐标值过小（小于100），判定为读数失败（可根据实际校准范围调整）
    *x = xtemp; // 将读取到的X坐标赋值给输出参数
    *y = ytemp; // 将读取到的Y坐标赋值给输出参数
    return 1;   // 返回读数成功
}

// 坐标验证配置参数
#define ERR_RANGE 50 // 两次采样的最大允许误差范围（前后两次X/Y坐标差值需小于50，否则判定为无效）

/**
 * @brief  读取触屏坐标（双重验证，提高准确性）
 * @param  x: 指向存储最终X坐标的变量指针（输出参数，为两次有效采样的平均值）
 * @param  y: 指向存储最终Y坐标的变量指针（输出参数，为两次有效采样的平均值）
 * @retval 0：读数失败（两次采样误差超范围或单次采样失败）；1：读数成功
 * @note   连续读取两次坐标，若两次差值在ERR_RANGE内，取平均值作为结果，进一步降低采样误差
 */
uint8_t TP_Read_XY2(uint16_t *x, uint16_t *y)
{
    uint16_t x1, y1; // 第一次采样的X/Y坐标
    uint16_t x2, y2; // 第二次采样的X/Y坐标
    uint8_t flag;    // 采样结果标志（0：失败，1：成功）

    // 第一次读取坐标
    flag = TP_Read_XY(&x1, &y1);
    if (flag == 0)
        return (0); // 第一次采样失败，直接返回失败

    // 第二次读取坐标
    flag = TP_Read_XY(&x2, &y2);
    if (flag == 0)
        return (0); // 第二次采样失败，直接返回失败

    // 验证两次采样的X坐标差值是否在允许范围内（ERR_RANGE=50）
    // 验证两次采样的Y坐标差值是否在允许范围内
    if (((x2 <= x1 && x1 < x2 + ERR_RANGE) || (x1 <= x2 && x2 < x1 + ERR_RANGE)) && ((y2 <= y1 && y1 < y2 + ERR_RANGE) || (y1 <= y2 && y2 < y1 + ERR_RANGE)))
    {
        *x = (x1 + x2) / 2; // 两次X坐标取平均，作为最终X坐标
        *y = (y1 + y2) / 2; // 两次Y坐标取平均，作为最终Y坐标
        return 1;           // 两次采样有效，返回成功
    }
    else
        return 0; // 两次采样误差超范围，返回失败
}

/**
 * @brief  扫描触屏状态（检测是否有触摸，并读取坐标）
 * @param  tp: 坐标类型选择（0：读取屏幕坐标（已校准）；1：读取物理坐标（原始AD值，用于校准））
 * @retval 0：无触摸；1：有触摸
 * @note   依赖全局触屏设备结构体tp_dev，存储当前触摸状态、坐标等信息；PEN引脚为触屏触摸检测引脚（低电平表示有触摸）
 */
uint8_t TP_Scan(uint8_t tp)
{
    // 检测PEN引脚（低电平表示有触摸按下）
    if (PEN == 0)
    {
        // 根据tp选择读取物理坐标或屏幕坐标
        if (tp)
        {
            // tp=1：读取物理坐标（原始AD值，用于校准流程）
            TP_Read_XY2(&tp_dev.x, &tp_dev.y);
        }
        else if (TP_Read_XY2(&tp_dev.x, &tp_dev.y))
        {
            // tp=0：读取屏幕坐标（通过校准参数转换为LCD屏幕坐标）
            tp_dev.x = (int)(tp_dev.xfac * tp_dev.x + tp_dev.xoff); // X坐标校准转换（系数+偏移）
            tp_dev.y = (int)(tp_dev.yfac * tp_dev.y + tp_dev.yoff); // Y坐标校准转换（系数+偏移）
        }

        // 检查之前是否无触摸（首次检测到触摸）
        if ((tp_dev.sta & TP_PRES_DOWN) == 0)
        {
            tp_dev.sta = TP_PRES_DOWN | TP_CATH_PRES; // 更新状态：触摸按下+首次捕获
            tp_dev.x0 = tp_dev.x;                     // 记录首次按下时的X坐标（用于判断滑动等操作）
            tp_dev.y0 = tp_dev.y;                     // 记录首次按下时的Y坐标
        }
    }
    else
    {
        // 无触摸（PEN引脚高电平）
        // 检查之前是否有触摸（当前为触摸松开）
        if (tp_dev.sta & TP_PRES_DOWN)
        {
            tp_dev.sta &= ~(1 << 7); // 清除"触摸按下"标志（TP_PRES_DOWN为bit7，置0表示松开）
        }
        else
        {
            // 之前就无触摸，初始化坐标（标记为无效）
            tp_dev.x0 = 0;
            tp_dev.y0 = 0;
            tp_dev.x = 0xffff; // 用0xffff表示X坐标无效
            tp_dev.y = 0xffff; // 用0xffff表示Y坐标无效
        }
    }

    // 返回当前触摸状态（仅保留TP_PRES_DOWN位，1表示有触摸，0表示无触摸）
    return tp_dev.sta & TP_PRES_DOWN;
}

/**
 * @brief  初始化触屏模块（主入口函数）
 * @param  无
 * @retval 0：初始化成功且已加载历史校准参数；1：初始化成功但执行了新校准
 * @note   流程：读取一次坐标初始化→初始化EEPROM→尝试加载历史校准参数→若失败则执行校准→保存参数
 */
uint8_t TP_Init(void)
{
    TP_Read_XY(&tp_dev.x, &tp_dev.y); // 第一次读取坐标，初始化硬件状态

    return 1; // 返回1，表示执行了新校准
}