volatile int g_data = 0x1234;   /* .data: needs the flash -> RAM copy */
volatile int g_bss;             /* .bss: must end up zero */

int main(void)
{
    volatile int x = g_data + g_bss;   /* reference both, see note below */
    (void)x;
    while (1) { }
}