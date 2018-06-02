#include "system.h"


static volatile uint8_t s_criticalNesting = 0;  ///< 进入临界资源嵌套数量,会有多次进入临界资源

//////////////////////////////////////////////////////////////////////////////////
/// @brief 	进入临界资源区
///
/// @param[in]  无
/// @reture     无
///
//////////////////////////////////////////////////////////////////////////////////
void EnterCritical(void)
{
    if (s_criticalNesting == 0)   //第一级临界态嵌套，关闭中断
    {
        __disable_irq(); // 屏蔽中断
    }
    s_criticalNesting++;
}

//////////////////////////////////////////////////////////////////////////////////
/// @brief 	退出临界资源区
///
/// @param[in]  无
/// @reture     无
///
//////////////////////////////////////////////////////////////////////////////////
void ExitCritical(void)
{
    if (s_criticalNesting > 0)
    {
        s_criticalNesting--;
        if (s_criticalNesting == 0)//第一级临界态嵌套，开中断
        {
			__enable_irq(); // 使能中断
		}
    }
}
