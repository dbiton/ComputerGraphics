#pragma once

#include "Resource.h"

void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void initMenu();

// intentional indents to visualize the structure of the menu
void mainMenu(int id);
/**/void modelsMenu(int id);
/*    */void newModelMenu(int id);
/*    */void materialMenu(int id);
/**/void camerasMenu(int id);
/*    */void newCameraMenu(int id);
/*    */void shadingMenu(int id);
/**/void lightsMenu(int id);
/*    */void newLightMenu(int id);
/**/void togglesMenu(int id);
/**/void controlMenu(int id);
/**/void resetMenu(int id);
/**/void projectionMenu(int id);
/**/void sensitivityMenu(int id);
/**/void advancedMenu(int id);
/*    */void supersamplingMenu(int id);
/*    */void fogMenu(int id);
/*    */void bloomMenu(int id);

void makeModelsSubMenu();
void makeCamerasSubMenu();
void makeLightsSubMenu();