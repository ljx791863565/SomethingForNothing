#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// 使用va_start, va_arg, va_end, va_list实现printf的格式字符串和参数解析
// 格式转换还是调用printf实现


// TODO
// bug1: %s解析时段错误
// bug2: 当%c和%d同时存在时，%d解析还是%c的内容
void minprintf(char *fmt, ...)
{
    // va_list类型用于声明一个变量，该变量将依次引用各参数
    // argument pointer
    va_list ap;

    char *p = NULL;
    int ival = 0;
    double dval = 0.0;
    char *sval;
    char cval = 0;
    for (p = fmt; *p; p++) {
        if (*p != '%') {
            putchar(*p);
            continue;
        }
        switch (*++p) {
        case 'd':
            //每次调用va_arg都将返回一个参数，并使ap指向下一个参数  使用type来指定返回类型和ap移动步长
            ival = va_arg(ap, int);
            printf("%d", ival);
            break;
        case 'f':
            dval = va_arg(ap, double);
            printf("%f", dval);
            break;
        case 's':
            for (sval = va_arg(ap, char*); *sval; sval++) {
                putchar(*sval);
            }
            break;
        case 'c':
            cval = (char)va_arg(ap, int);
            printf("%c", cval);
            break;
        default:
            putchar(*p);
            break;
        }
    }

    // 清理ap_list资源
    va_end(ap);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage %s <fmt>\n", argv[0]);
    }

    printf("fmt: [%s]\n", argv[1]);

    minprintf(argv[1], 65, 123);

    exit(0);
}