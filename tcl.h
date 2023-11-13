#ifndef TCL_H
#define TCL_H

extern Tcl_Interp *interp_nef;

int init_interpreter();
int ls(ClientData clientdata, Tcl_Interp *interp_nef, int argc, Tcl_Obj *const argv[]);
int less(ClientData clientdata, Tcl_Interp *interp_nef, int argc, Tcl_Obj *const argv[]);
int del_interpreter();

#endif
