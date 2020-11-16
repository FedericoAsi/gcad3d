/* ../xa/xa_sele.h

*//*!
\file ../xa/xa_sele.h
\ingroup grp_ap

See INF_workflow_select

TODO:
- Typ_goAxis unused ...


*/



// selectionGroups (cannot be used as chars):      TYP_IS_SELGRP()
#define Typ_goGeom      1000  ///< all       (USED AS CHAR U)
#define Typ_goPrim      1001  ///< PT/LN/CI/Spline; nicht Plg,CCV
#define Typ_goGeo1      1002  ///< LN/CI/CV/PL/Sur/Sol (nicht PT,VC)
#define Typ_goGeo2      1003  ///< CI/ELL (prj -> PLN)
#define Typ_go_PD       1004  ///< PT|VC            
#define Typ_goGeo4      1005  ///< UNUSED VC/LN/PLN
#define Typ_goGeo5      1006  ///< CIR/ELL/CCV      (closed cont.)
#define Typ_goGeoSUSU   1007  ///< Sur|Sol          (Supporting Surface)
#define Typ_goGeo6      1008  ///< PT|LN|AC
#define Typ_goGeo7      1009  ///< Val|V|PT|VC      (angle ..)
#define Typ_goGeo8      1010  ///< Val|V|PT         (parameter ..)
#define Typ_goAxis      1011  ///< RotAxis LN/PT+PT/PT+LN/PT+VC/PLN
#define Typ_go_LCS      1012  ///< Line,Circ,Curv
#define Typ_go_LR       1013  ///< Line,Plane
#define Typ_go_RA       1014  ///< plane|surface
#define Typ_go_lf1      1015  ///< all curves, not contour
#define Typ_go_lf2      1016  ///< all curves, not trimmed-curve
#define Typ_go_JNT      1017  ///< Joint

// selectionModifiers
#define Typ_FncVAR1     1020  ///< next DB-var
#define Typ_FncVAR2     1021  ///< previous DB-var
#define Typ_FncVC1      1022  ///< next DB-vector
#define Typ_FncVC2      1023  ///< previous DB-vector
#define Typ_FncNxt      1024  ///< next version/solution (Typ_modCoord)
#define Typ_FncPrv      1025  ///< previous version/solution (Typ_modCoord)
#define Typ_FncNxt      1024  ///< next version/solution (Typ_modCoord)
#define Typ_FncPrv      1025  ///< previous version/solution (Typ_modCoord)
#define Typ_FncDirX     1026  ///< parallel (U) or across (V)
#define Typ_FncPtOnObj  1027  ///< parametric point on obj
#define Typ_FncPtOnCP   1028  ///< position on constrPln

// aux
#define Typ_EOT         1050  ///< last typ
// #define Typ_Val_symTyp  1100
// #define Typ_Str_Dir1    1101
// #define Typ_Obj_0       1102  


  typedef struct {int typ; char oid[128];}            subCurv;
// S(baseCv v1)        baseCv=contour; v1=segNr; subCurv = L|C in CCV
// S(baseCv v1 v2)     baseCv=contour; v1=segNr; subCurv=contour|plg; v2=segNr
// P|D(baseCv v2)      typ=P; v1<1.; v2=parameter;     eg P on plg
// P|D(baseCv v1 v2)   typ=P; v1=segNr(>=1); v2=parameter; eg P on L in CCV
                          // segmentNumbers: 1 = first segment.


#define SUBCVTABSIZ 20
  typedef struct {int iNr; subCurv *cva;}            subCvTab;


#define SELE_SRC_CNVT_NXT(xTyp) sele_src_cnvt_test(sCva,xTyp,selTyp,dbi,selPos)




// EOF
