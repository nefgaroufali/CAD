#ifndef TCL_H
#define TCL_H

extern Tcl_Interp *interp_nef;

int init_interpreter();
int ls(ClientData clientdata, Tcl_Interp *interp_nef, int argc, Tcl_Obj *const argv[]);
int less(ClientData clientdata, Tcl_Interp *interp_nef, int argc, Tcl_Obj *const argv[]);
int del_interpreter();
int read_design(ClientData clientdata, Tcl_Interp *interp, int argc, Tcl_Obj *const argv[]);
int list_components(ClientData clientdata, Tcl_Interp *interp, int argc, Tcl_Obj *const argv[]);
int report_component_type(ClientData clientdata, Tcl_Interp *interp, int argc, Tcl_Obj *const argv[]);
int report_component_function(ClientData clientdata, Tcl_Interp *interp, int argc, Tcl_Obj *const argv[]);
int list_component_CCS(ClientData clientdata, Tcl_Interp *interp, int argc, Tcl_Obj *const argv[]);

#endif
