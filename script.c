#include <stdlib.h>

int main() {
    // Run another program with gdb
    system("./TCL_Shell");
    //system("r");
    system("read_design pid.txt");
    system("list_components");
    system("list_component_CCS PID/U182");
    system("report_component_type PID/U182");
    system("report_component_function PID/U182");
    system("list_IOs");
    system("list_IO_CCS o_un|28");
    system("quit");

    return 0;
}
