//          Model-managment-functions.        Franz Reiter.  10.Dez.2003
/*
 *
 * Copyright (C) 2015 CADCAM-Services Franz Reiter (franz.reiter@cadcam.co.at)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
-----------------------------------------------------
TODO:
  ..

-----------------------------------------------------
Modifications:
2010-07-12 Created from xa_mod.c   RF

-----------------------------------------------------
*/
/*!
\file  ../xa/xa_mod_gui.c
\brief GUI Model-managment-functions 
\code
=====================================================
List_functions - see ../xa/xa_mod.c
=====================================================

\endcode *//*----------------------------------------



*/

#ifdef _MSC_VER
#include "MS_Def0.h"
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>                    // isdigit



#include "../ut/ut_geo.h"              // Point ..
// #include "../ut/func_types.h"          // UI_FuncSet
// #include "../ut/ut_umem.h"
#include "../ut/ut_cast.h"             // INT_PTR
#include "../ut/ut_os.h"               // OS_get_bas_dir ..

#include "../xa/xa_uid.h"              // UID_WinEdit
#include "../xa/xa.h"                  // MDL_IS_MAIN
#include "../xa/xa_msg.h"              // MSG_cancel,


#include "../gui/gui__.h"              // Gtk


// EXTERNALS:
// // from ../xa/xa.h
// extern  char     AP_modact_nam[128];    // name of the active submodel; ""=(main)



// PROTOS:
  // int Mod_cre_CB (void *parent, void *data);
  // int Mod_m2s_CB (void *parent, void *data);
  void Mod_chg_CB (char *modNam);
  // int Mod_ren_CB (void *parent, void *data);
  // int Mod_del_CB (MemObj *mo, void **data);



//================================================================
  int Mod_smNam_get (char *smNam) {
//================================================================
/// Mod_smNam_get       get new subModelname from user
//// Output smNam       size must be >= 64
///        retCod      -1=cancel;    0=OK

  int    irc;
  char   s1[256], newNam[256];


  L_name:
  // get new subModelname
  strcpy(s1, "submodel_1");
  irc = GUI_dlg_e2b (s1, 250, "name for new submodel:", "OK", "Cancel");
  if(irc != 0) return -1;


  // make safeName from new subModelname
  // Mod_safeName (newNam, fnam);
  UTX_safeName (s1, 1); // change '. /\\'


  // check maxLen; max 62 chars
  if(strlen(s1) > 62) {
    TX_Print("**** max length of subModelname = 62 chars - reduced");
    s1[62] = '\0';
  }
  strcpy(smNam, s1);


  // test if subModel mNam already exists 
  sprintf(newNam,"%sModel_%s",OS_get_tmp_dir(),smNam);
  if(OS_checkFilExist(newNam,1) == 1) {
    // sprintf(newNam,"  overwrite submodel %s ?  ",mNam);
    MSG_get_1 (newNam, 256, "OVER_mdl", "%s", smNam);
    irc = GUI_dlg_2b (newNam, "OK", "Cancel");
    if(irc != 0) goto L_name;
  }


  return 0;

}


//====================================================================
  int Mod_cre__ () {
//====================================================================
/// create new SubModel:
/// save new submodelname;
/// provide new (empty) Modelspace

  int    irc;
  char   cbuf1[256], newNam[256], mNam[64];


  // save the active Submodel AP_modact_nam -> TempFile
  Mod_sav_tmp ();


  // get smNam = new subModelname
  irc = Mod_smNam_get (mNam);
  if(irc < 0) return -1;


  // provide new (empty) Modelspace
  UTF_clear_ ();                    // Clear Mem

  // create 1. Line
  sprintf(cbuf1, "# Submodel %s", mNam);
  UTF_add_line (cbuf1);
  ED_load__ ();  // Memory > Editfenster
  UNDO_clear ();                 // clear Undo-List
  AP_Init1 ();   // init Displist, WC_main ..


  // save active submodelname
  strcpy(AP_modact_nam, mNam);

  // display submodelname im Titlebar
  Mod_chg_tit ();

  // activate model in browser
  Brw_sMdl_act (mNam);

  return irc;

}


//====================================================================
  int Mod_m2s__ () {
//====================================================================
/// move Mainmodel > Submodel

// TODO: use Mod_SM_add_file


  int   irc;
  char  cbuf1[256], newNam[256], mNam[64];

  // save the active Submodel AP_modact_nam -> TempFile
  Mod_sav_tmp ();

  // get smNam = new subModelname
  irc = Mod_smNam_get (mNam);
  if(irc < 0) return -1;


  // rename
  sprintf(cbuf1,"%sModel_%s",OS_get_tmp_dir(),AP_modact_nam);
  sprintf(newNam,"%sModel_%s",OS_get_tmp_dir(),mNam);
  rename (cbuf1, newNam);

  // disp msg
  TX_Print(" mainmodel moved to submodel %s",mNam);

  // clear mainModelspace
  UTF_clear_ ();                    // Clear Mem

  // init Modelspace with datum
  sprintf (cbuf1 , "# %s",OS_date1());
  UTF_add_line (cbuf1);

  ED_load__ ();  // Memory > Editfenster

  UNDO_clear ();                 // clear Undo-List

  GA_hide__ (-1, 0L, 0);           // clear Attribute-Table

  AP_Init1 ();                      // init Displist, WC_main ..

  // update Mod.lst
  Mod_mkList (0);

  // add new subModel in browserwindow
  Brw_Mod_add (mNam);


  // reprocess whole model
  UI_but_END ();  

  return 0;

}


//====================================================================
  int Mod_chg__ (int mode) {
//====================================================================
/// \code
/// activate other Submodel
/// - save active Model (AP_modact_nam) -> tmp/Model_<submodelname>
/// - provide List of Submodelnames
/// - Selection -> Mod_chg_CB
///   mode   0=save actice Submodel;  1=do not save active Submodel
/// \endcode

  int   i1;
  char  fnam[256], s1[256];

  printf("Mod_chg__\n");


  // save the active Submodel AP_modact_nam -> TempFile
  if(mode == 0) Mod_sav_tmp ();


  //--------------------------------------------------------------
  // provide List of Submodelnames (<basedir>/tmp/Model_*) -> fnam
  Mod_mkList (0);


  //--------------------------------------------------------------
  // let user select from list of Submodelnames
  sprintf(fnam,"%sMod.lst",OS_get_tmp_dir());
  i1 = GUI_listf1__ (s1, sizeof(s1), fnam, "\"select model\"", "\"x40,y40\"");
  if(i1 < 0) return -1;

  // change subModel
  Mod_chg_CB (s1);

  return 0;

}


///=================================================================
  int Mod_ren__ () {
///=================================================================
/// rename active subModel in browser
/// the callback if OK selected.

  int   irc;
  char  s1[320], newNam[256], mNam[64];


  if(MDL_IS_MAIN) {
    TX_Error("es ist kein Submodel aktiv ..");
    return -1;
  }


  printf("Mod_ren_CB |%s|\n",AP_modact_nam);


  sprintf(s1, "rename Submodel %s to:", AP_modact_nam);
  strcpy(newNam, AP_modact_nam);
  irc = GUI_dlg_e2b (newNam, 250, s1, "OK", "Cancel");
  if(irc != 0) return -1;             // error
  if(strlen(newNam) < 1) return -1;   // cancel


  // make safeName from new subModelname
  // Mod_safeName (newNam, fnam);
  UTX_safeName (newNam, 1); // change '. /\\'

  // check maxLen; max 62 chars
  if(strlen(newNam) > 62) {
    TX_Print("max Laenge = 62 chars; gekuerzt !!");
    newNam[62] = '\0';
  }
  strcpy(mNam, newNam);
    printf(" Mod_ren__-L1 |%s|\n",mNam);


  // rename <tmp/Model_<AP_modact_nam>> -> tmp/Model_<mNam>>
  sprintf(s1, "%sModel_%s", OS_get_tmp_dir(), AP_modact_nam);
  sprintf(newNam, "%sModel_%s", OS_get_tmp_dir(), mNam);
  OS_file_rename (s1, newNam);

  // update sm-List <tmpDir>/Mod.lst
  Mod_mkList (1);

  // set name
  strcpy(AP_modact_nam, mNam);

  // fix title
  Mod_chg_tit ();

  // disp msg
  TX_Print(" subModel renamed to %s",mNam);

  // rename active subModel in browser
  Brw_sMdl_ren_CB (mNam);
  // rename the active mdlNod to <mNam>
  // GUI_tree1_row_set (&winBrw, &mdlNod, -1, mNam, 0);

  return 0;

}


//====================================================================
  int Mod_del__ () {
//====================================================================
// Mod_del__           delete Submodel AP_modact_nam

  int      i1;
  char     s1[256];


  printf("Mod_del__\n");

  if(MDL_IS_MAIN) {
    TX_Error("no Submodel is active ..");
    return -1;
  }

  // ask user - delete ? yes / no
  sprintf(s1, "  delete Submodel %s  ",AP_modact_nam);
  i1 = GUI_dlg_2b (s1, MSG_const__(MSG_ok), MSG_const__(MSG_no));
    // printf(" PRG_Del__-L2 %d\n",i1);
  if(i1 != 0) return -1;            // error or cancel


  // del <tmp/Model_<AP_modact_nam>>
  strcpy (s1, AP_modact_nam);

  Mod_chg__ (1); // do not save active Submodel

  Brw_Mdl_del_sm (s1);  // delete submodel in browser

  Mod_del1__ (s1);

  return 0;

}

/*
//=====================================================================
  int Mod_del_CB (MemObj *mo, void **data) {
//=====================================================================

  int   idat;
  char  cbuf[256];


  idat = GUI_DATA_EVENT;
  printf("Mod_del_CB %d\n",idat);
  if(idat != UI_FuncOK) return -1;

  // del <tmp/Model_<AP_modact_nam>>
  strcpy (cbuf, AP_modact_nam);


  Mod_chg__ (1); // do not save active Submodel

  Brw_Mdl_del_sm (cbuf);  // delete submodel in browser

  Mod_del1__ (cbuf);

  return 0;

}
*/

// EOF
