//深圳市鸿讯电子有限公司
//创建日期:2023/3/30
//版本：V1.0
//单片机型号：STM32F103C8T6  3.3V

//开发板SYSTEM文件夹，采用V3.5库的头文件(stm32f10x.h、system_stm32f10x.h和
//core_cm3.h等几个文件放在:SYSTEM\sys 文件夹里面)，使得代码可以完美移植到库函数里面使用，
//不过需要注意，在新建工程的时候，请在：Options for Target-->C/C++ 选项卡里面 Preprocessor
// Symbols 栏定义你的STM32芯片容量。比如:STM32F103C8T6开发板用户，使用的是中等容量的STM32
//芯片，则在Define栏输入：STM32F10X_MD 

//对于STM32F103系列芯片，设置原则如下：
//16KB≤FLASH≤32KB       选择：STM32F10X_LD
//64KB≤FLASH≤128KB      选择：STM32F10X_MD
//256KB≤FLASH≤512KB     选择：STM32F10X_HD
