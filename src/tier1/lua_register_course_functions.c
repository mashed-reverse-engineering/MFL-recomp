#include "mfl_tier2.h"
/*00405e40*/ extern int stub_return_zero();
/*0048b640*/ extern int lua_Occluder_Filename();
/*0048b670*/ extern int lua_Sky_Filename();
/*0048b710*/ extern int lua_Mts_Filename();
/*0048b7b0*/ extern int lua_SetCopter();
/*0048b830*/ extern int lua_Clump_Filename();
/*0048b8d0*/ extern int lua_UVA_Filename();
/*0048b930*/ extern int lua_Spline_Filename();
/*0048b9d0*/ extern int lua_AI_Bsp_Filename();
/*0048ba10*/ extern int lua_AI_Data_Filename();
/*0048ba40*/ extern int lua_World_Bsp_Filename();
/*0048ba70*/ extern int lua_Hoppy_Bsp_Filename();
/*0048baa0*/ extern int lua_Object_Bsp_Filename();
/*0048bb40*/ extern int lua_Collision_Bsp_Filename();
/*0048bb80*/ extern int lua_Texture_Dictionary_Filename();
/*0048bbb0*/ extern int lua_Lap_Variations();
/*0048bbe0*/ extern int lua_Split_Sector();
/*0048bc20*/ extern int lua_Lap_Line();
/*0048bc60*/ extern int lua_Lap_Line_End();
/*0048bc80*/ extern int lua_Lap_Line_Change();
/*0048bca0*/ extern int lua_Safe_Start_Lines();
/*0048bd10*/ extern int lua_Camera_Anim_Filename();
/*0048bd40*/ extern int lua_General_Anim_Filename();
/*0048bde0*/ extern int lua_Clump_Mts_Pair();
/*0048be40*/ extern int lua_Physics_Object();
/*0048beb0*/ extern int lua_Clump_Exclude_From_World();
/*0048bee0*/ extern int lua_Powerup_Filename();
/*0048bf30*/ extern int lua_Lights_Filename();
/*0048bf60*/ extern int lua_Ambient_RGB();
/*0048bfc0*/ extern int lua_Setup_Fog();
/*0048c060*/ extern int lua_Modify_Fog();
/*0048c110*/ extern int lua_Vehicle_Shininess_Range();
/*0048c160*/ extern int lua_Shine_Scale();
/*0048c190*/ extern int lua_Shadow_Scale();
/*0048c1c0*/ extern int lua_Vehicle_Body_Dirt_Texture__Vehicle_Wheel_Dirt_Texture();
/*0048c1d0*/ extern int lua_SetClumpAnim();
/*0048c270*/ extern int lua_RWP_Object();
/*0048c310*/ extern int lua_RWP_Object_Box();
/*0048c380*/ extern int lua_RWP_Object_Cylinder();
/*0048c3e0*/ extern int lua_RWP_Object_Sphere();
/*0048c430*/ extern int lua_RWP_Object_Capsule();
/*0048c490*/ extern int lua_RWP_Object_Properties();
/*0048c530*/ extern int lua_RWP_Object_CentreOfMassOffset();
/*0048c5a0*/ extern int lua_RWP_Object_BaseLabel();
/*0048c5f0*/ extern int lua_RWP_Object_ShatterVel();
/*0048c640*/ extern int lua_Set_Current_Position();
/*0048c690*/ extern int lua_Set_Current_Type();
/*0048c6c0*/ extern int lua_Set_Current_Respawn_Time();
/*0048c6e0*/ extern int lua_Place_Powerup();
/*0048c710*/ extern int lua_Set_Track_Powerups();
/*0048c7e0*/ extern int lua_Physics_Scenario_Create();
/*0048c850*/ extern int lua_Physics_Scenario_Set_Sequence();
/*0048c8b0*/ extern int lua_Physics_Scenario_Add_Objects();
/*0048c910*/ extern int lua_Physics_Scenario_Add_Animated_Object();
/*0048c990*/ extern int lua_Physics_Scenario_Create_Trigger();
/*0048ca40*/ extern int lua_Physics_Scenario_Trigger_Add_Objects();
/*0048cac0*/ extern int lua_Physics_Scenario_Get_Function();
/*0048cb40*/ extern int lua_Physics_Scenario_Trigger_Set_AI_Poly();
/*0048cbb0*/ extern int lua_KTC_NewCopter();
/*0048cc60*/ extern int lua_KTC_AddPickUp();
/*0048ccf0*/ extern int lua_ExpObj_Clump_Mts_Pair();
/*0048cd80*/ extern int lua_ClumpFade();
/*0048cf50*/ extern int lua_register_function();
/*004a0ac0*/ extern int lua_RainEnable();
/*004a0ae0*/ extern int lua_RainSetHeadColour();
/*004a0b80*/ extern int lua_RainSetTailColour();
/*004a0c20*/ extern int lua_RainSetPosition();
/*004a0cd0*/ extern int lua_RainSetScale();
/*004a0d30*/ extern int lua_RainSetCameraScale();
/*004a0d90*/ extern int lua_RainSetDirection();


/* lua_register_course_functions */

/* "registers the course script functions (Course_Id, AI_Bsp_Filename, World_Bsp_Filename ...)" */

void lua_register_course_functions(void)
{
  lua_register_function("Course_Id",stub_return_zero);
  lua_register_function("AI_Bsp_Filename",lua_AI_Bsp_Filename);
  lua_register_function("AI_Data_Filename",lua_AI_Data_Filename);
  lua_register_function("World_Bsp_Filename",lua_World_Bsp_Filename);
  lua_register_function("Hoppy_Bsp_Filename",lua_Hoppy_Bsp_Filename);
  lua_register_function("Object_Bsp_Filename",lua_Object_Bsp_Filename);
  lua_register_function("Collision_Bsp_Filename",lua_Collision_Bsp_Filename);
  lua_register_function("Texture_Dictionary_Filename",lua_Texture_Dictionary_Filename);
  lua_register_function("Occluder_Filename",lua_Occluder_Filename);
  lua_register_function("Sky_Filename",lua_Sky_Filename);
  lua_register_function("Mts_Filename",lua_Mts_Filename);
  lua_register_function("Clump_Filename",lua_Clump_Filename);
  lua_register_function("UVA_Filename",lua_UVA_Filename);
  lua_register_function("Spline_Filename",lua_Spline_Filename);
  lua_register_function("Camera_Anim_Filename",lua_Camera_Anim_Filename);
  lua_register_function("General_Anim_Filename",lua_General_Anim_Filename);
  lua_register_function("Clump_Mts_Pair",lua_Clump_Mts_Pair);
  lua_register_function("Physics_Object",lua_Physics_Object);
  lua_register_function("Course_Name",stub_return_zero);
  lua_register_function("Clump_Exclude_From_World",lua_Clump_Exclude_From_World);
  lua_register_function("Powerup_Filename",lua_Powerup_Filename);
  lua_register_function("Lights_Filename",lua_Lights_Filename);
  lua_register_function("Ambient_RGB",lua_Ambient_RGB);
  lua_register_function("Setup_Fog",lua_Setup_Fog);
  lua_register_function("Modify_Fog",lua_Modify_Fog);
  lua_register_function("Vehicle_Shininess_Range",lua_Vehicle_Shininess_Range);
  lua_register_function("Shine_Scale",lua_Shine_Scale);
  lua_register_function("Shadow_Scale",lua_Shadow_Scale);
  lua_register_function
            ("Vehicle_Body_Dirt_Texture",lua_Vehicle_Body_Dirt_Texture__Vehicle_Wheel_Dirt_Texture);
  lua_register_function
            ("Vehicle_Wheel_Dirt_Texture",lua_Vehicle_Body_Dirt_Texture__Vehicle_Wheel_Dirt_Texture)
  ;
  lua_register_function("SetClumpAnim",lua_SetClumpAnim);
  lua_register_function("Lap_Variations",lua_Lap_Variations);
  lua_register_function("Lap_Line",lua_Lap_Line);
  lua_register_function("Lap_Line_End",lua_Lap_Line_End);
  lua_register_function("Lap_Line_Change",lua_Lap_Line_Change);
  lua_register_function("Safe_Start_Lines",lua_Safe_Start_Lines);
  lua_register_function("Split_Sector",lua_Split_Sector);
  lua_register_function("RWP_Object",lua_RWP_Object);
  lua_register_function("RWP_Object_Box",lua_RWP_Object_Box);
  lua_register_function("RWP_Object_Cylinder",lua_RWP_Object_Cylinder);
  lua_register_function("RWP_Object_Sphere",lua_RWP_Object_Sphere);
  lua_register_function("RWP_Object_Capsule",lua_RWP_Object_Capsule);
  lua_register_function("RWP_Object_Properties",lua_RWP_Object_Properties);
  lua_register_function("RWP_Object_CentreOfMassOffset",lua_RWP_Object_CentreOfMassOffset);
  lua_register_function("RWP_Object_BaseLabel",lua_RWP_Object_BaseLabel);
  lua_register_function("RWP_Object_ShatterVel",lua_RWP_Object_ShatterVel);
  lua_register_function("Physics_Scenario_Create",lua_Physics_Scenario_Create);
  lua_register_function("Physics_Scenario_Set_Sequence",lua_Physics_Scenario_Set_Sequence);
  lua_register_function("Physics_Scenario_Add_Objects",lua_Physics_Scenario_Add_Objects);
  lua_register_function
            ("Physics_Scenario_Add_Animated_Object",lua_Physics_Scenario_Add_Animated_Object);
  lua_register_function("Physics_Scenario_Create_Trigger",lua_Physics_Scenario_Create_Trigger);
  lua_register_function
            ("Physics_Scenario_Trigger_Add_Objects",lua_Physics_Scenario_Trigger_Add_Objects);
  lua_register_function("Physics_Scenario_Get_Function",lua_Physics_Scenario_Get_Function);
  lua_register_function
            ("Physics_Scenario_Trigger_Set_AI_Poly",lua_Physics_Scenario_Trigger_Set_AI_Poly);
  lua_register_function("Set_Current_Position",lua_Set_Current_Position);
  lua_register_function("Set_Current_Type",lua_Set_Current_Type);
  lua_register_function("Set_Current_Respawn_Time",lua_Set_Current_Respawn_Time);
  lua_register_function("Place_Powerup",lua_Place_Powerup);
  lua_register_function("Set_Track_Powerups",lua_Set_Track_Powerups);
  lua_register_function("RainEnable",lua_RainEnable);
  lua_register_function("RainSetHeadColour",lua_RainSetHeadColour);
  lua_register_function("RainSetTailColour",lua_RainSetTailColour);
  lua_register_function("RainSetPosition",lua_RainSetPosition);
  lua_register_function("RainSetScale",lua_RainSetScale);
  lua_register_function("RainSetCameraScale",lua_RainSetCameraScale);
  lua_register_function("RainSetDirection",lua_RainSetDirection);
  lua_register_function("SetCopter",lua_SetCopter);
  lua_register_function("KTC_NewCopter",lua_KTC_NewCopter);
  lua_register_function("KTC_AddPickUp",lua_KTC_AddPickUp);
  lua_register_function("ExpObj_Clump_Mts_Pair",lua_ExpObj_Clump_Mts_Pair);
  lua_register_function("ClumpFade",lua_ClumpFade);
  return;
}
